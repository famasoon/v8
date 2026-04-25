# Wasm / Snapshot / Sandbox 実装ガイド

## 初心者向けの要点
このページは V8 の拡張機能群（Wasm、起動高速化 Snapshot、安全性の Sandbox）をまとめて扱います。

## WebAssembly
### データ構造
- `WasmModule`
- `WasmInstanceObject`
- コンパイルキャッシュ

### 主要パス
1. バイナリ decode
2. 検証と tiered compile
3. インスタンス化して JS と接続

### 主要フラグ
- `--trace-wasm`
- `--wasm-tier-up`

### 典型デバッグ手順
- `--trace-wasm` で decode/compile の進行を確認。
- import/export の型不一致を重点確認。

## Snapshot
### データ構造
- serializer object table
- startup snapshot blob

### 主要パス
1. ヒープ serialize
2. 起動時 deserialize
3. 参照再配置

### 主要フラグ
- `--trace-serializer`（環境依存）
- `--trace-deserializer`（環境依存）

### 典型デバッグ手順
- snapshot 有無で起動時間と挙動差分を比較。
- serialize 不能オブジェクト混入を確認。

## Sandbox
### データ構造
- sandbox pointer table
- 境界管理メタデータ

### 主要パス
1. sandbox 初期化
2. 境界管理付き参照
3. 逸脱アクセス防御

### 主要フラグ
- `--sandbox-testing`（環境依存）
- `--trace-sandbox`（環境依存）

### 典型デバッグ手順
- pointer 圧縮/復元経路を追跡。
- 境界違反時 stack trace とアクセス元を確認。

## まず読むファイル
- `src/wasm/*`
- `src/snapshot/*`
- `src/sandbox/*`
