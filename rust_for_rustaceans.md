# Rust for Rustaceans

## 1. Foundations

- Rustプログラミングで主に意識するメモリは、stack, heap, static
- Rustのメモリモデルの中心的なコンセプトは、全ての変数のオ-ナ-はただ1人で、唯一の場所(普通はスコープ)が解放責務をもつ
- Dropの順番は、変数は定義と逆順で、構造体の中身は定義順
- コンパイラは借用のルールもふまえて最適化する
- 所有権をもつのと、排他参照の違いは、使わなくなったときにdropするかどうかのみ
- 排他参照の裏の値をmoveする場合、代わりのデータを置く必要がある ( std::mem::take など)
- 内部可変性はborrow checkerに対する抜け道
- 共有参照から排他参照を取得できるパターンと、共有参照経由で値を変更できるパターンがある
- borrow checkerは単純にブロックスコープで見ているのではなく、実際に参照を使う範囲を見ている
- generic lifetimeを使う場合、dropを実装したらそこまで参照が有効でならなければならなくなる
- generic lifetimeは基本一つまでにしておく

## 3. Designing interfaces

- API設計の原則
  - unsurprised
  - flexible
  - obvious
  - constrained
- リソース
  - [Rust API Guidelines](https://
rust-lang.github.io/api-guidelines/)
  - [Rust RFC 
1105](https://rust-lang.github.io/rfcs/1105-api-evolution.html)
  - [the chapter 
of The Cargo Book on SemVer compatibility](https://doc.rust-lang.org/cargo/­
reference/semver.html)

### Unsurprised

- 自分で定義留守方には、下記の順で、できる限りトレイトを実装する
  - `std::fmt::Debug`
  - `std::marker::Sync`, `std::marker::Send`
    - もし何らかの理由でマークすべきでないのであれば、その理由をコメントで明記する
  - `std::clone::Clone`, `std::default::Default`
  - `std::cmp::PartialEq`, `std::cmp::PartialOrd`, `std::hash::Hash`, `std::cmp::Eq`, `std::cmp::Ord`
  - `serde::Serialize`, `serde::Deserialize` (標準ライブラリではないので、必要に応じて)

- Rustのトレイトは、`Bar`がトレイトを実装していても、デフォルトでは`&Bar`にトレイトを自動実装しない
  - そのため、独自でトレイトを定義する場合は、`&T where T: Trait`, `&mut T where T: Trait`, `Box<T> where T: Trait`として空実装しておくのが通常望ましい

- wrapper型を定義する場合は、以下のトレイトの実装を検討する
  - `Deref`: `.`で中身のメソッドを呼び出せるようにする
  - `AsRef`: `&`で中身の参照を取得できるようにする
  - `From<InnerType>`, `Into<InnerType>`: `InnerType`との相互変換を容易にする
  - `Borrow`: `HashMap`などのコレクションで、`InnerType`と`OuterType`をキーとして等価に使えるようにする。本質的に同じであるときだけ提供する（例：`String`と`str`）

- 参照か所有か
  - 内部の処理でスレッドに渡すなど所有権が必要になるなら所有
  - 所有権が不要かつコピーが軽量でなければ、参照
  - 参照か所有かが実行時に決まる場合は、Cowがつかえる
