# Maglev とは何か（実装編）

## 1. 初心者向けの要点
- Maglev は「速くコンパイルして、そこそこ速く実行する」中間 tier の JIT です。
- Sparkplug より最適化し、TurboFan ほど重くない、という立ち位置です。

## 2. データ構造
- `MaglevCompilationInfo`: コンパイル全体の状態。
- `Graph` / `BasicBlock` / `ValueNode`: IR の骨格。
- `InterpreterFrameState`: bytecode 側の値状態を保持。
- `EagerDeoptInfo` / `LazyDeoptInfo`: 失敗時に復元する情報。

## 3. 主要パス
1. `MaglevGraphBuilder` が bytecode を読みながら IR 構築。
2. 軽量最適化パスを実施。
3. pre-regalloc で live range を計算。
4. 線形走査レジスタ割当。
5. `GenerateCode` で直接命令生成。

## 4. 主要フラグ
- `--maglev` / `--no-maglev`
- `--trace-opt`
- `--trace-deopt`
- `--print-maglev-graph`（環境依存）

## 5. 典型デバッグ手順
1. `d8 --maglev --trace-opt --trace-deopt sample.js` を実行。
2. `maglev-graph-builder.cc` で対象 bytecode のノード化を確認。
3. `maglev-regalloc.cc` でレジスタ割当を確認。
4. `maglev-ir-*.cc` の `GenerateCode` で最終命令を追う。

## 6. まず読むファイル
- `src/maglev/maglev-compiler.cc`
- `src/maglev/maglev-graph-builder.h`
- `src/maglev/maglev-regalloc.cc`
- `src/maglev/*/maglev-ir-*.cc`
