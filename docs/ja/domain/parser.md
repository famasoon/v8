# Parser / AST 実装ガイド

## 初心者向けの要点
Parser は JavaScript テキストを AST に変換する入口です。
ここで作られた構造が後段の bytecode/JIT 品質に影響します。

## データ構造
- `Scanner`（トークン化）
- `Parser` / `PreParser`
- `AstNode`
- `Scope` / `ScopeInfo`

## 主要パス
1. scanner がトークン化。
2. parser が AST 構築。
3. lazy parse でコスト最適化。
4. bytecode generator へ引き渡し。

## 主要フラグ
- `--print-ast`（環境依存）
- `--trace-parse`（環境依存）
- `--print-bytecode`

## 典型デバッグ手順
1. 最小入力を用意し parse 分岐を追う。
2. token 列と AST ノード生成を照合。
3. 出力 bytecode が想定と一致するか確認。

## まず読むファイル
- `src/parsing/parser.cc`
- `src/parsing/scanner.cc`
- `src/parsing/preparser*`
- `src/ast/*`
