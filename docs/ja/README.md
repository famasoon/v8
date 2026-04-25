# V8 実装解説（日本語）

このディレクトリは、V8 の内部実装を **初心者でも追える順番** で学ぶためのガイドです。
「概念」だけでなく、「どの C++ ファイルから読むか」「どんなフラグで観測するか」まで含めています。

## まず最初に読むページ

1. [V8 ドメイン知識マップ（実装編）](./domain/overview.md)
2. [Ignition](./domain/ignition.md)
3. [Sparkplug](./domain/sparkplug.md)
4. [Maglev](./maglev.md)
5. [TurboFan](./domain/turbofan.md)

この順で読むと、V8 の実行 tier が段階的に理解できます。

## トピック一覧

### コア解説
- [BigInt とは何か（実装編）](./bigint.md)
- [Maglev とは何か（実装編）](./maglev.md)

### ドメイン別
- [Ignition](./domain/ignition.md)
- [Sparkplug](./domain/sparkplug.md)
- [TurboFan（Turbofun）](./domain/turbofan.md)
- [Turboshaft / Turbolev](./domain/turboshaft.md)
- [Deoptimization / Tiering](./domain/deopt-tiering.md)
- [オブジェクトモデル / Hidden Class](./domain/object-model.md)
- [GC / Heap](./domain/gc-heap.md)
- [Parser / AST](./domain/parser.md)
- [Builtins / Torque / CSA](./domain/builtins-torque-csa.md)
- [Wasm / Snapshot / Sandbox](./domain/platform-features.md)

## 使い方（初心者向け）

- 各ページの「データ構造」を先に読み、登場人物を把握する。
- 次に「主要パス」で処理の流れを追う。
- 実際に `d8` フラグを使ってログを取り、「典型デバッグ手順」をなぞる。
- 最後に「まず読むファイル」を開いて、実装を確認する。

> メモ: フラグはバージョンやビルド設定で変わるため、実行前に `d8 --v8-options` を確認してください。
