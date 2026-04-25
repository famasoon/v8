# Deoptimization / Tiering 実装ガイド

## 初心者向けの要点
V8 は「まず速く実行して、後で最適化する」ため、
最適化前提が崩れた時に戻る deopt と、実行 tier を上げる tiering が必須です。

## データ構造
- `DeoptimizationData`
- `FrameTranslation`
- `FeedbackVector`
- tiering 状態（hotness / budget）

## 主要パス
1. 実行中に feedback 蓄積。
2. tiering manager が次 tier を判断。
3. guard fail 時に deopt。
4. frame translation で interpreter 状態復元。

## 主要フラグ
- `--trace-opt`
- `--trace-deopt`
- `--trace-turbo`

## 典型デバッグ手順
1. `--trace-opt --trace-deopt` で遷移を収集。
2. deopt reason から該当 guard を特定。
3. feedback 遷移と tiering 条件を確認。

## まず読むファイル
- `src/deoptimizer/*`
- `src/execution/tiering-manager.cc`
- `src/runtime/runtime-compiler.cc`
