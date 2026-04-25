# BigInt とは何か（実装編）

## 1. 初心者向けの要点
- `Number` では安全に扱えない巨大整数を扱うための型が `BigInt` です。
- V8 では「符号 + 桁配列」で表現し、演算時は可変オブジェクトで作って最後に不変化します。

## 2. データ構造
- `BigIntBase` (`src/objects/bigint.h`): 共通レイアウト（sign/length/digits）。
- `BigInt`: JavaScript から見える不変オブジェクト。
- `MutableBigInt`: `bigint.cc` 内で使う構築用の可変オブジェクト。
- `digits`: 語長単位 (`uintptr_t`) の配列。

### ここが大事
初心者が混乱しやすい点は「なぜ Mutable があるか」です。
→ 多倍長演算は途中で桁数が増減するため、完成前に可変で持つ必要があります。

## 3. 主要パス
1. `MutableBigInt::New` で領域確保。
2. 演算（加減乗除、シフト、変換）を実行。
3. `Canonicalize` で無駄な先頭ゼロを削除。
4. `MakeImmutable` で `BigInt` に確定。

## 4. 主要フラグ
BigInt 専用フラグより、最適化観測フラグと併用するのが実践的です。
- `--allow-natives-syntax`
- `--trace-opt`
- `--trace-deopt`

## 5. 典型デバッグ手順（最短）
1. `d8 --trace-opt --trace-deopt sample.js` で最適化遷移を確認。
2. `src/objects/bigint.cc` の `Canonicalize` 付近にブレークポイント。
3. 入力値（0/負数/巨大値）ごとの digit 変化を確認。
4. 例外ケース（過大桁、ゼロ除算）を再現してエラーパスを追う。

## 6. まず読むファイル
- `src/objects/bigint.h`
- `src/objects/bigint.cc`
- `src/bigint/*`
