# Turboshaft / Turbolev 実装ガイド

## 初心者向けの要点
Turboshaft は新しい最適化 IR 基盤です。
Turbolev は Maglev から Turboshaft へ渡す橋渡しレイヤです。

## データ構造
- `turboshaft::Graph`
- `Operation`
- `Block` / `Phi`
- Maglev ノード対応情報

## 主要パス
1. Turbolev が Maglev graph を受け取る。
2. Turboshaft graph に変換。
3. reducer パスで最適化。
4. backend でコード生成。

## 主要フラグ
- `--turbolev`
- `--trace-turbo`
- `--trace-turbo-graph`

## 典型デバッグ手順
1. `--turbolev` で変換経路を再現。
2. `turbolev-graph-builder.cc` で対応関係を確認。
3. CFG/phi の整合性をチェック。

## まず読むファイル
- `src/compiler/turboshaft/graph.h`
- `src/compiler/turboshaft/operations.h`
- `src/compiler/turboshaft/turbolev-graph-builder.cc`
