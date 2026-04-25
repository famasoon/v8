# Builtins / Torque / CSA 実装ガイド

## 初心者向けの要点
V8 の組み込み処理は、Torque（DSL）・CSA（低レベル生成 API）・C++ を組み合わせて実装されます。

## データ構造
- Torque AST / 型情報
- CSA ノード
- builtin descriptor

## 主要パス
1. Torque 記述をコンパイル。
2. CSA/C++ 実装へ展開。
3. builtin テーブルへ登録。
4. runtime や interpreter から呼び出し。

## 主要フラグ
- `--print-code`
- `--trace-turbo`
- `--allow-natives-syntax`

## 典型デバッグ手順
1. 対象 builtin 名を決める。
2. Torque 定義→生成コード→実体を順に追う。
3. 引数 descriptor と戻り型整合を確認。

## まず読むファイル
- `src/torque/*`
- `src/codegen/code-stub-assembler*`
- `src/builtins/*`
