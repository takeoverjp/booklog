# 脳に収まるコードの書き方 ～複雑さを避け持続可能にするための経験則とテクニック

- [link](https://www.oreilly.co.jp//books/9784814400799/)

## 1. 加速

### 1. アートかサイエンスか

- 建築のメタファは、ソフト開発を終わりのあるものと誤解させる
- 実際には、リリース後であれ思い立ったらすぐにリリースできる能力が求められるもの
- ソフト開発における実装は、実際に建築するフェーズと対応付けられることで、交換可能な作業と誤認させる
- ソフト開発における実際の建築相当はコンパイルであり、実装は設計の要素を多分に含む
- 知識体系を身に付けないままプログラマーになる大衆文化が増加したことで、歴史が軽んじられる

### 2. チェックリスト

- チェックリストを使うことで、スキルの向上なしに結果を改善できる
- `git commit --allow-empty -m "Initial commit"`で、空のコミットを最初に追加することができる

- 新規コードベースのためのチェックリスト
  - Gitを使え
  - ビルドを自動化せよ
  - エラーメッセージをすべて有効にせよ

### 3. 複雑さに対処する

- コードは書く回数より読む回数の方が多いので、可読性のために最適化せよ
  - どんな馬鹿でもコンピュータが理解できるコードをかける。良いプログラマーは人間が理解できるコードを書く

### 4. バーティカルスライス

- 考えつく中で一番シンプルな機能をユーザーインターフェイスからデータストアまで全部実装することで、ソフトウェアが動くかどうかをなるべく早く確かめる。そこから広げていく
- データベーススキーマもGitリポジトリにコミットする

### 5. カプセル化

- カプセル化とは、オブジェクトと呼び出し側の有効なやりとりを記述する契約
- ポステルの法則
  - 送るものは慎重に、受け取るものは寛容に
  - もとはTCPのコンテキストで提唱された法則だが、API設計でも役に立つ
  - API設計においては、受け取るものは意味のある処理をすることができる範囲において寛容に、受け取れない入力についてはすぐにエラーにすることになる
- オブジェクトが満たすべき不変条件は、誤った使い方を防ぐために外から変更できないように保護するべき
  - 例外の値・型はテストするべきだが、例外メッセージのテキストはテストするべきではない（オブジェクトのふるまいの一部ではないので、必要以上にテストと実装詳細を結びつけるべきではない）