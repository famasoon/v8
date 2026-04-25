# Ignition（インタプリタ）実装ガイド

## 初心者向けの要点
Ignition は V8 の最初の実行エンジンです。
JavaScript を bytecode にして実行し、後段の最適化に必要な情報を集めます。

## データ構造
- `BytecodeArray`: 実行される bytecode 列。
- `FeedbackVector`: 型フィードバック。
- `InterpreterFrame`: 仮想レジスタと accumulator。

## 主要パス
1. Parser が AST を作成。
2. BytecodeGenerator が `BytecodeArray` 生成。
3. Interpreter dispatch loop で命令実行。
4. 実行結果を `FeedbackVector` に記録。

## 主要フラグ
- `--print-bytecode`
- `--print-bytecode-filter=<function_name>`
- `--trace-ignition`（環境依存）

## 典型デバッグ手順
1. `d8 --print-bytecode sample.js` で bytecode を確認。
2. `bytecode-generator.cc` でどの AST がどの命令になるか追う。
3. `interpreter-assembler.cc` 側の handler 実装を確認。
4. `FeedbackVector` の更新が行われる箇所を追う。

## まず読むファイル
- `src/interpreter/interpreter.cc`
- `src/interpreter/bytecode-generator.cc`
- `src/interpreter/interpreter-assembler.cc`
- `src/objects/feedback-vector.h`
