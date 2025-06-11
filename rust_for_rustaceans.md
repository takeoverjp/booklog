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

