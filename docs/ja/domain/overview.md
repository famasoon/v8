# V8 ドメイン知識マップ（実装編）

## このページの目的
V8 は巨大なコードベースなので、初心者がいきなり `src/` を読むと迷いやすいです。
このページは「**どこから読み始めるか**」を明確にし、各分野の入口を案内します。

## まず押さえる全体像

V8 の実行はざっくり次の流れです。

1. **Parser** が JavaScript を解析して AST を作る
2. **Ignition** が bytecode を生成・実行する
3. ホットになった関数を **Sparkplug / Maglev / TurboFan** が順次最適化する
4. 前提が崩れたら **Deopt** で安全に戻る
5. メモリは **GC/Heap** が管理する

## 分野別リンク

### 実行・コンパイル
- [Ignition](./ignition.md)
- [Sparkplug](./sparkplug.md)
- [Maglev](../maglev.md)
- [TurboFan（Turbofun）](./turbofan.md)
- [Turboshaft/Turbolev](./turboshaft.md)
- [Deoptimization / Tiering](./deopt-tiering.md)

### オブジェクト・メモリ・言語
- [BigInt](../bigint.md)
- [オブジェクトモデル / Hidden Class](./object-model.md)
- [GC / Heap](./gc-heap.md)
- [Parser / AST](./parser.md)
- [Builtins / Torque / CSA](./builtins-torque-csa.md)
- [Wasm / Snapshot / Sandbox](./platform-features.md)

## 推奨の学習順（初心者向け）

1. `Ignition` → `Sparkplug` → `Maglev` → `TurboFan`
2. `Deoptimization / Tiering`
3. `Object Model` と `GC / Heap`
4. `Parser / AST` と `Builtins / Torque / CSA`
5. `Wasm / Snapshot / Sandbox`

## フラグ利用時の注意
- ドキュメント中のフラグは「代表例」です。
- 利用可否はビルドで異なるため、必ず `d8 --v8-options` で確認してください。
