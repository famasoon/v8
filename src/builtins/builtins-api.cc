// Copyright 2016 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/api/api-arguments-inl.h"
#include "src/api/api-natives.h"
#include "src/builtins/builtins-utils-inl.h"
#include "src/builtins/builtins.h"
#include "src/logging/log.h"
#include "src/logging/runtime-call-stats-scope.h"
#include "src/objects/objects-inl.h"
#include "src/objects/prototype.h"
#include "src/objects/templates.h"
#include "src/objects/visitors.h"

namespace v8 {
namespace internal {

namespace {

// Returns the holder JSObject if the function can legally be called with this
// receiver.  Returns nullptr if the call is illegal.
// TODO(dcarney): CallOptimization duplicates this logic, merge.
JSReceiver GetCompatibleReceiver(Isolate* isolate, FunctionTemplateInfo info,
                                 JSReceiver receiver) {
  RCS_SCOPE(isolate, RuntimeCallCounterId::kGetCompatibleReceiver);
  Object recv_type = info.signature();
  // No signature, return holder.
  if (!recv_type.IsFunctionTemplateInfo()) return receiver;
  // A Proxy cannot have been created from the signature template.
  if (!receiver.IsJSObject()) return JSReceiver();

  JSObject js_obj_receiver = JSObject::cast(receiver);
  FunctionTemplateInfo signature = FunctionTemplateInfo::cast(recv_type);

  // Check the receiver.
  if (signature.IsTemplateFor(js_obj_receiver)) return receiver;

  // The JSGlobalProxy might have a hidden prototype.
  if (V8_UNLIKELY(js_obj_receiver.IsJSGlobalProxy())) {
    HeapObject prototype = js_obj_receiver.map().prototype();
    if (!prototype.IsNull(isolate)) {
      JSObject js_obj_prototype = JSObject::cast(prototype);
      if (signature.IsTemplateFor(js_obj_prototype)) return js_obj_prototype;
    }
  }
  return JSReceiver();
}

template <bool is_construct>
V8_WARN_UNUSED_RESULT MaybeHandle<Object> HandleApiCallHelper(
    Isolate* isolate, Handle<HeapObject> new_target,
    Handle<FunctionTemplateInfo> fun_data, Handle<Object> receiver,
    BuiltinArguments args) {
  Handle<JSReceiver> js_receiver;
  JSReceiver raw_holder;
  base::Optional<BuiltinArguments::ChangeValueScope> set_receiver_value_scope;
  if (is_construct) {
    DCHECK(args.receiver()->IsTheHole(isolate));
    if (fun_data->GetInstanceTemplate().IsUndefined(isolate)) {
      v8::Local<ObjectTemplate> templ =
          ObjectTemplate::New(reinterpret_cast<v8::Isolate*>(isolate),
                              ToApiHandle<v8::FunctionTemplate>(fun_data));
      FunctionTemplateInfo::SetInstanceTemplate(isolate, fun_data,
                                                Utils::OpenHandle(*templ));
    }
    Handle<ObjectTemplateInfo> instance_template(
        ObjectTemplateInfo::cast(fun_data->GetInstanceTemplate()), isolate);
    ASSIGN_RETURN_ON_EXCEPTION(
        isolate, js_receiver,
        ApiNatives::InstantiateObject(isolate, instance_template,
                                      Handle<JSReceiver>::cast(new_target)),
        Object);
    set_receiver_value_scope.emplace(
        isolate, &args, BuiltinArguments::kReceiverOffset, *js_receiver);
    DCHECK_EQ(*js_receiver, *args.receiver());

    raw_holder = *js_receiver;
  } else {
    DCHECK(receiver->IsJSReceiver());
    js_receiver = Handle<JSReceiver>::cast(receiver);

    if (!fun_data->accept_any_receiver() &&
        js_receiver->IsAccessCheckNeeded()) {
      // Proxies never need access checks.
      DCHECK(js_receiver->IsJSObject());
      Handle<JSObject> js_obj_receiver = Handle<JSObject>::cast(js_receiver);
      if (!isolate->MayAccess(handle(isolate->context(), isolate),
                              js_obj_receiver)) {
        isolate->ReportFailedAccessCheck(js_obj_receiver);
        RETURN_EXCEPTION_IF_SCHEDULED_EXCEPTION(isolate, Object);
        return isolate->factory()->undefined_value();
      }
    }

    raw_holder = GetCompatibleReceiver(isolate, *fun_data, *js_receiver);

    if (raw_holder.is_null()) {
      // This function cannot be called with the given receiver.  Abort!
      THROW_NEW_ERROR(
          isolate, NewTypeError(MessageTemplate::kIllegalInvocation), Object);
    }
  }

  Object raw_call_data = fun_data->call_code(kAcquireLoad);
  if (!raw_call_data.IsUndefined(isolate)) {
    DCHECK(raw_call_data.IsCallHandlerInfo());
    CallHandlerInfo call_data = CallHandlerInfo::cast(raw_call_data);
    Object data_obj = call_data.data();

    FunctionCallbackArguments custom(isolate, data_obj, raw_holder, *new_target,
                                     args.address_of_first_argument(),
                                     args.length() - 1);
    Handle<Object> result = custom.Call(call_data);

    RETURN_EXCEPTION_IF_SCHEDULED_EXCEPTION(isolate, Object);
    if (result.is_null()) {
      if (is_construct) return js_receiver;
      return isolate->factory()->undefined_value();
    }
    // Rebox the result.
    result->VerifyApiCallResultType();
    if (!is_construct || result->IsJSReceiver())
      return handle(*result, isolate);
  }

  return js_receiver;
}

}  // anonymous namespace

BUILTIN(HandleApiCall) {
  HandleScope scope(isolate);
  Handle<Object> receiver = args.receiver();
  Handle<HeapObject> new_target = args.new_target();
  Handle<FunctionTemplateInfo> fun_data(
      args.target()->shared().get_api_func_data(), isolate);
  if (new_target->IsJSReceiver()) {
    RETURN_RESULT_OR_FAILURE(
        isolate, HandleApiCallHelper<true>(isolate, new_target, fun_data,
                                           receiver, args));
  } else {
    RETURN_RESULT_OR_FAILURE(
        isolate, HandleApiCallHelper<false>(isolate, new_target, fun_data,
                                            receiver, args));
  }
}

namespace {

class RelocatableArguments : public BuiltinArguments, public Relocatable {
 public:
  RelocatableArguments(Isolate* isolate, int length, Address* arguments)
      : BuiltinArguments(length, arguments), Relocatable(isolate) {}

  RelocatableArguments(const RelocatableArguments&) = delete;
  RelocatableArguments& operator=(const RelocatableArguments&) = delete;

  inline void IterateInstance(RootVisitor* v) override {
    if (length() == 0) return;
    v->VisitRootPointers(Root::kRelocatable, nullptr, first_slot(),
                         last_slot() + 1);
  }
};

}  // namespace

MaybeHandle<Object> Builtins::InvokeApiFunction(
    Isolate* isolate, bool is_construct, Handle<FunctionTemplateInfo> function,
    Handle<Object> receiver, int argc, Handle<Object> args[],
    Handle<HeapObject> new_target) {
  RCS_SCOPE(isolate, RuntimeCallCounterId::kInvokeApiFunction);

  // Do proper receiver conversion for non-strict mode api functions.
  if (!is_construct && !receiver->IsJSReceiver()) {
    ASSIGN_RETURN_ON_EXCEPTION(
        isolate, receiver, Object::ConvertReceiver(isolate, receiver), Object);
  }

  // We assume that all lazy accessor pairs have been instantiated when setting
  // a break point on any API function.
  DCHECK(!Handle<FunctionTemplateInfo>::cast(function)->BreakAtEntry());

  // Construct BuiltinArguments object:
  // new target, function, arguments reversed, receiver.
  const int kBufferSize = 32;
  Address small_argv[kBufferSize];
  Address* argv;
  const int frame_argc = argc + BuiltinArguments::kNumExtraArgsWithReceiver;
  if (frame_argc <= kBufferSize) {
    argv = small_argv;
  } else {
    argv = new Address[frame_argc];
  }
  argv[BuiltinArguments::kNewTargetOffset] = new_target->ptr();
  argv[BuiltinArguments::kTargetOffset] = Smi::FromInt(0).ptr();
  argv[BuiltinArguments::kArgcOffset] = Smi::FromInt(frame_argc).ptr();
  argv[BuiltinArguments::kPaddingOffset] =
      ReadOnlyRoots(isolate).the_hole_value().ptr();
  argv[BuiltinArguments::kReceiverOffset] = receiver->ptr();
  int cursor = BuiltinArguments::kNumExtraArgsWithReceiver;
  for (int i = 0; i < argc; ++i) {
    argv[cursor++] = args[i]->ptr();
  }
  MaybeHandle<Object> result;
  {
    RelocatableArguments arguments(isolate, frame_argc, &argv[frame_argc - 1]);
    if (is_construct) {
      result = HandleApiCallHelper<true>(isolate, new_target, function,
                                         receiver, arguments);
    } else {
      result = HandleApiCallHelper<false>(isolate, new_target, function,
                                          receiver, arguments);
    }
  }
  if (argv != small_argv) delete[] argv;
  return result;
}

// Helper function to handle calls to non-function objects created through the
// API. The object can be called as either a constructor (using new) or just as
// a function (without new).
V8_WARN_UNUSED_RESULT static Object HandleApiCallAsFunctionOrConstructor(
    Isolate* isolate, bool is_construct_call, BuiltinArguments args) {
  Handle<Object> receiver = args.receiver();

  // Get the object called.
  JSObject obj = JSObject::cast(*receiver);

  // Set the new target.
  HeapObject new_target;
  if (is_construct_call) {
    // TODO(adamk): This should be passed through in args instead of
    // being patched in here. We need to set a non-undefined value
    // for v8::FunctionCallbackInfo::IsConstructCall() to get the
    // right answer.
    new_target = obj;
  } else {
    new_target = ReadOnlyRoots(isolate).undefined_value();
  }

  // Get the invocation callback from the function descriptor that was
  // used to create the called object.
  DCHECK(obj.map().is_callable());
  JSFunction constructor = JSFunction::cast(obj.map().GetConstructor());
  DCHECK(constructor.shared().IsApiFunction());
  Object handler =
      constructor.shared().get_api_func_data().GetInstanceCallHandler();
  DCHECK(!handler.IsUndefined(isolate));
  CallHandlerInfo call_data = CallHandlerInfo::cast(handler);

  // Get the data for the call and perform the callback.
  Object result;
  {
    HandleScope scope(isolate);
    FunctionCallbackArguments custom(isolate, call_data.data(), obj, new_target,
                                     args.address_of_first_argument(),
                                     args.length() - 1);
    Handle<Object> result_handle = custom.Call(call_data);
    if (result_handle.is_null()) {
      result = ReadOnlyRoots(isolate).undefined_value();
    } else {
      result = *result_handle;
    }
  }
  // Check for exceptions and return result.
  RETURN_FAILURE_IF_SCHEDULED_EXCEPTION(isolate);
  return result;
}

// Handle calls to non-function objects created through the API. This delegate
// function is used when the call is a normal function call.
BUILTIN(HandleApiCallAsFunction) {
  return HandleApiCallAsFunctionOrConstructor(isolate, false, args);
}

// Handle calls to non-function objects created through the API. This delegate
// function is used when the call is a construct call.
BUILTIN(HandleApiCallAsConstructor) {
  return HandleApiCallAsFunctionOrConstructor(isolate, true, args);
}

}  // namespace internal
}  // namespace v8
