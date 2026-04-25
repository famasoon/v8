# オブジェクトモデル / Hidden Class 実装ガイド

## 初心者向けの要点
JavaScript オブジェクトは動的ですが、V8 は `Map`（hidden class）で形状を固定化し、
高速アクセスを実現します。

## データ構造
- `Map`
- `DescriptorArray`
- `TransitionArray`
- elements / properties backing store

## 主要パス
1. オブジェクト生成で初期 `Map` を持つ。
2. プロパティ追加で map transition。
3. IC がアクセスパターンを学習。
4. JIT が IC 情報で最適化。

## 主要フラグ
- `--trace-maps`
- `--trace-ic`
- `--allow-natives-syntax`

## 典型デバッグ手順
1. `--trace-maps --trace-ic` で形状変化を見る。
2. map が不安定な箇所を特定。
3. `ic.cc` と lookup 経路を追って失敗点確認。

## まず読むファイル
- `src/objects/map.h`
- `src/objects/descriptor-array.h`
- `src/objects/transitions.h`
- `src/ic/ic.cc`
