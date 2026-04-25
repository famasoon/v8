# TurboFan（Turbofun）実装ガイド

## 初心者向けの要点
TurboFan は V8 の高最適化 tier です。
コンパイル時間は重いですが、実行性能を強く押し上げます。

## データ構造
- `Graph` / `Node` / `Operator`
- `Schedule`
- `InstructionSequence`
- `FrameState` / `DeoptInfo`

## 主要パス
1. `pipeline.cc` で最適化パス開始。
2. lowering / simplification。
3. instruction selection。
4. register allocation。
5. code generation と metadata 生成。

## 主要フラグ
- `--turbofan`
- `--trace-turbo`
- `--trace-turbo-graph`
- `--trace-opt`
- `--trace-deopt`

## 典型デバッグ手順
1. `d8 --trace-turbo --trace-opt sample.js` で phase を観測。
2. `pipeline.cc` でどの最適化が有効か確認。
3. `instruction-selector*.cc` と `register-allocator*.cc` を追う。
4. deopt が出る場合は reason と guard を突き合わせる。

## まず読むファイル
- `src/compiler/pipeline.cc`
- `src/compiler/instruction-selector*.cc`
- `src/compiler/register-allocator*.cc`
