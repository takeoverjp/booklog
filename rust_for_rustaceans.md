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

- 通常、リソースの解放は`Drop`トレイトを実装することで行うが、`Drop`トレイトはエラーを通知することができないため、リソースの解放の失敗をハンドリングしなければならない場合は、明示的なデストラクタを定義する

### Obvious

#### Documentation

1. **予期しない動作や前提条件を明記する**
   - パニックやエラーが発生しうる場合は、その条件や状況を明確に記載する。
   - `unsafe`関数の場合は、呼び出し側が守るべき安全性の要件を説明する。

2. **end-to-endの使用例を載せる**
   - クレートやモジュール単位で、全体の使い方が分かる例を載せる。
   - 高レベルな例は、ユーザーがインターフェース全体を理解しやすくし、独自の利用方法の出発点となる。

3. **ドキュメントを整理する**
   - 関連する型や関数はモジュールでまとめる。
   - ドキュメント内リンクを活用し、型やトレイト間の関係を分かりやすくする。
   - 内部実装やレガシーな公開不要部分は `#[doc(hidden)]` で隠す。

4. **ドキュメントを充実させる**
   - RFCやブログ、ホワイトペーパーなど外部リソースへのリンクを活用する。
   - 特定の条件下でのみ利用可能なアイテムには `#[doc(cfg(..))]` を使う。
   - 別名での検索性を高めるため `#[doc(alias = "...")]` を使う。
   - よく使うモジュールや型、トレイト、メソッドはトップレベルで案内する。

#### Type System Guidance

- 型システムをうまく活用して、インターフェースを分かりやすくし、誤用を防ぐ
- "semantic typing"
  - 値の意味を表す独自型を導入することで、誤った引数の渡し方をコンパイル時に防ぐことができる（例：`bool`の代わりに2値のenum型を用いる）。  
  - newtypeイディオムも、この一種。
- "zero-sized type"
  - `std::marker::PhantomData`を使って状態管理することで、特定の状態でのみ有効なメソッドを型レベルで制限でき、誤った操作を不可能にできる。  
- 複数の引数のenumによる統合
  - さらに、複数の引数に依存関係がある場合（例：第1引数がtrueのときのみ第2引数にポインタを取る）、まとめて一つのenumで表現することで、実装者・利用者双方の誤解を防ぐことができる。
- `#[must_use]`アノテーション
  - 戻り値の無視によるバグをコンパイラ警告で防止できる（例：`Result`型）。ただし、過剰な利用は避け、無視されると明確に問題となる場合に限定して使うべきである。
  
### Constrained

- **公開インターフェースのあらゆる特性は、意図的・偶発的によらず、ユーザに依存される可能性がある。**

### Type Modifications

- `pub(crate)`や`pub(in path)`などの可視性修飾子を活用し、公開型を最小限に抑えることで将来的な変更の自由度を高めるべき
- 型名以外にも、ユーザーは様々な方法で型に依存する
  - 例えば、フィールドの追加や構造体の形状変更（タプル構造体→通常構造体）も、パターンマッチや構造体リテラルの利用コードを壊すことがある
  - これを防ぐために、Rustの`#[non_exhaustive]`属性を型定義に付与すると、ユーザーによる暗黙の構造体リテラルや網羅的パターンマッチを禁止でき、将来の拡張に備えられる。ただし、ユーザーのコードに制約を与えるため、型が安定している場合は付与を避けるのが望ましい。

### Trait implementations

- トレイトに依存する方法は多岐にわたるので、多くの修正が利用者側でビルドエラーを引き起こす可能性がある
- 影響を限定的にするイディオムとして、Sealedトレイトがある

### Hidden Contract

#### Re-exports

- 外部クレートをそのまま公開すると、外部クレートのメジャーバージョンが上がったときに別の型と認識されて、コンパイルが通らなくなる
- 外部クレートの型は、ラップするのが無難
- メジャーバージョンが変わると別の型だと見なされる理由は複雑だが、一つの例としてCopyマーカートレイトがある
- semver trickでメジャーバージョンアップの影響を緩和できる

#### Auto-Traits

- `Send`や`Sync`などの自動トレイトは、privateなメンバが`Send`や`Sync`でなくなった場合に、型が`Send`や`Sync`でなくなり、braking changeとなる
- `fn is_normal<T: Sized + Send + Sync + Unpin>() {}`のようなテストコードを実装しておくことで、リリース前にチェックできる

## 4. Error Handling

- Rustにおけるエラー処理の方法は、まだ統一的な方法が確立されていない

### Representing Errors

- エラーの表現方法には、API利用者がエラー要因を区別できるenumerationと、単一のopaque errorだけを返すerasureがある
