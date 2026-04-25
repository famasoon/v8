# Sparkplug（ベースライン JIT）実装ガイド

## 初心者向けの要点
Sparkplug は Ignition の bytecode を高速に機械語へ変換する軽量 JIT です。
「最適化の強さ」より「立ち上がりの速さ」を重視します。

## データ構造
- `BaselineCompiler`
- `BaselineAssembler`
- `BytecodeOffset` 対応テーブル

## 主要パス
1. Tiering 判断で Sparkplug が選択される。
2. Bytecode を順に読み、機械語を生成。
3. コードオブジェクトを実行可能状態に登録。

## 主要フラグ
- `--sparkplug`
- `--no-sparkplug`
- `--trace-baseline`（環境依存）

## 典型デバッグ手順
1. Sparkplug 有効で実行し tier-up ログを見る。
2. `baseline-compiler.cc` の bytecode ごとの生成分岐を確認。
3. bytecode offset と生成コード位置の対応を検証。

## まず読むファイル
- `src/baseline/baseline-compiler.cc`
- `src/baseline/baseline-assembler-*.inl`
- `src/baseline/baseline.cc`
