# Tidy First?

## Links

[Tidy First?](https://www.oreilly.co.jp//books/9784814400911/)

## イントロダクション

- 処理を変更する前に、乱雑なコードをいつ整頓すべきか
- 乱雑なコードを安全かつ効果的に整頓する方法
- 乱雑なコードの整頓のやめどき
- 整頓がうまくいく仕組み

## 第一部　整頓

### 1. ガード節

- 早期リターンのこと
- 数が多すぎない限り、積極的に早期リターンすることで、事前条件を明白にすべし

### 2. デッドコード

- 消すべし

### 3. シンメトリーを揃える

- 似ているが同一ではないルーチンを探し、同じ部分と違う部分を分離することで、共通点を隠してしまわないようにすべし

### 4. 新しいインターフェイス、古い実装

- 自分が呼びたいインターフェースを呼ぶ
  - 後ろからコーディング
  - テストファースト
  - ヘルパーライブラリ、クラス

### 5. 読む順番

- 読むときに知りたい情報がなるべく早く登場する順番に書く

### 6. 凝集の順番

- 関連する手続き、関数、ファイルは、近くに置く

### 7. 変数宣言と初期化を一緒の場所に移動する

- 型の情報と初期値の情報、さらに変数名の情報を、必要なときに読めるように、宣言と初期化を一緒の場所で行うべき

### 8. 説明変数

- 適切な名前をつけた一時変数に値を保存することで、理解の助けにする
