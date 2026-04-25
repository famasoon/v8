# GC / Heap 実装ガイド

## 初心者向けの要点
V8 の GC は「短寿命オブジェクトを高速回収」しつつ、
長寿命オブジェクトも効率的に管理するために世代別管理を行います。

## データ構造
- New/Old 空間
- `MarkingWorklist`
- `RememberedSet`
- write barrier 関連データ

## 主要パス
1. 割当失敗や閾値で GC 発火。
2. minor/major GC を選択。
3. marking → sweeping/compaction。
4. barrier と remembered set を更新。

## 主要フラグ
- `--trace-gc`
- `--trace-gc-verbose`
- `--stress-compaction`
- `--max-old-space-size=<MB>`

## 典型デバッグ手順
1. `--trace-gc --trace-gc-verbose` で停止時間と理由を収集。
2. どの空間で回収が詰まっているか確認。
3. barrier 経路を追い、参照更新漏れを確認。

## まず読むファイル
- `src/heap/heap.cc`
- `src/heap/mark-compact*`
- `src/heap/write-barrier*`
