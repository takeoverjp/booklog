# Links

<https://www.amazon.co.jp/Philosophy-Software-Design-John-Ousterhout/dp/1732102201>

# 1. Introduction

- 開発のスピードを落とさず、バグを出さないためには、『複雑性』を減らすことが重要

- 複雑性を減らす方法
  1. シンプルでわかりやすいコードを書く
     - 例
       - special caseを減らす
       - 命名規則を統一する
  2. 複雑性を隠蔽する
     - modular design。
     - このときの分割単位のことをmoduleと呼ぶ

- 設計は開発が続く限りずっとやりつづけなければならない

- この本のゴール
  - "complexity"はどこからくるのか
  - "complexity"はどうすれば最小化できるのか

# 2. The Nature of Complexity

- 「複雑性」とは、「ソフトの理解・修正を難しくするありとあらゆる物」と定義している
- システム全体の複雑性は、局所的な複雑性とその部分をに関わらなければならない時間との積の総和としている。
  - すなわち、ほとんど中を見ることのないコンポーネントは、中身が複雑でも大きな問題にはならない

- 複雑性の兆候
  - 修正箇所の爆発
  - 認知的負荷
    - グローバル変数とか、一貫性の欠如とか、依存性とかで増える
    - 行数が多くても素朴な実装のほうが、凝った短い実装より認知的負荷が低いこともある
  - 何がわからないのかもわからない
    - どうすれば正しく修正すれば良いのかがわからない
    - ３つの兆候のうち、これがもっともたちが悪い

- 複雑性の原因
  - 依存関係
  - 不明瞭性

# 3. Working Code Isn't Enough

- Tactical Tornado
  - ガンガン場当たり的に対応して表面的にタスクをこなすひと
  - マネージャーからは英雄視されるが、後処理しなければならないエンジニアには憎まれる
- いつ改善する？
  - 一気に改善するのは難しい。
  - 継続的に改善する
  - 筆者の経験上、開発の時間の10-20%ぐらいを将来への投資に回せば、スピードを落とさずに開発できそう
