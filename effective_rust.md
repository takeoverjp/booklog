# Effective Rust

## Links

- [Effective Rust](https://www.oreilly.co.jp//books/9784814400942/)

## 1. 型

### 項目1：データ構造を表現するために型システムを用いよう

- Rustの型システムは、他の静的型付け言語に馴染みがある
- isizeとusizeはポインタサイズと一致する
- Rustでは整数型の変換は明示的に行う必要がある
- 配列、タプル、構造体などの集約型が用意されている
- enum型は相互に排他な値の集合を指定するために用いられる
- enumは個々のヴァリアントにデータを付加できる
- Option<T>とResult<T, E>は非常に一般的なenum型である
- match式で型安全性を確保する
- **型で無効な状態を表現できないようにする**ことが重要
- Rustの型システムを用いて設計の意図を明確に伝える

### 項目2：型システムを用いて共通の挙動を表現しよう

- `fn`型は`Copy`と`Eq`を実装し、`std::fmt::Pointer`も実装する
  - 例: `let op1 = op; let op2 = op; assert!(op1 == op2); println!("op = {:p}", op);` 出力例: `"op = 0x101e9aeb0"`

- 関数の名前だけでは`fn`型にならず、明示的な自動型変換が必要である
  - 例: `let op1 = sum; let op2 = sum;` はコンパイルエラーになる
  - エラーメッセージ: `error[E0369]: binary operation == cannot be applied to type fn(i32, i32) -> i32 {main::sum}`

- クロージャは関数定義のボディ部のように見えるコード片（ラムダ式）である
  - 式の一部にできるため、名前を付ける必要がない
  - 入力引数はバー（|）で囲って与える
  - 包含する環境の一部をキャプチャできる

- 例: `let amount_to_add = 3; let add_n = |y| { y + amount_to_add }; let z = add_n(5); assert_eq!(z, 8);`

- コンパイラがラムダ式で参照されている環境の部分を保持した特注の内部型を作成し、クロージャが作成されると、この一時的な型のインスタンスが作られ、関連する値が格納される

- クロージャを関数ポインタの代わりに使うことはできない
  - エラーメッセージ: `error[E0308]: mismatched types`

- Rustには3種類のFn*トレイトがある
  - `FnOnce`: 1度だけ呼び出せるクロージャ
  - `FnMut`: 繰り返し呼び出せるクロージャ（環境を可変借用）
  - `Fn`: 繰り返し呼び出せるクロージャ（環境を共有借用）

- 例: `pub fn modify_all<F>(data: &mut [u32], mut mutator: F) where F: FnMut(u32) -> u32 { for value in data { *value = mutator(*value); } }`

- トレイトは関数のシグネチャだけでなく、関数の意図を表現するための機構である
- トレイトを実装する型は、トレイトのすべての関数を実装しなければならない

- 例: `pub trait Sort { fn sort(&mut self); } pub trait StableSort: Sort {}`

- Rustの型システムにおけるトレイトの使い方
  - トレイト制約として用いる。ジェネリック型や関数の型をコンパイル時に制約する
  - トレイトオブジェクトとして用いる。格納や関数に渡す型を実行時に制約する

## 2. トレイト

## 3. さまざまなコンセプト

## 4. 依存ライブラリ

## 5. ツール

## 6. 標準Rustの向こうへ
