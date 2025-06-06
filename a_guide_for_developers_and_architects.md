# 開発者とアーキテクトのためのコミュニケーションガイド

## 1. コミュニケーションの基礎

- 受け手を理解する
  - 受け手はあなたに何を求めているか？
  - あなたは受け手に何を求めているか？
  - 受け手は技術に明るいか？
- 抽象度を分離して、目的に合わせた抽象度を選択する
  - システムコンテキスト, コンテナ, コンポーネント, コード
- 一般的で一貫した表現を選択することで、認知不可を下げる
  - C4, データフロー図, ...

## 2. ごちゃごちゃをすっきりと

- 色はメッセージを伝えるために必要最低限の種類を使う
- 四角によるグルーピングは、注釈等を使って、入れ子にならないようにする。どうしても必要な場合、四角の種類を変える
- 矢印は直線ではなく直角線を使い、交差はラインジャンプを使う
- 自明な情報は省略し、補足的な情報は注釈にすることで、図で伝えたいことの濃度を上げる

## 3. アクセシビリティ

- 色覚異常などを考慮し、色だけに頼ったデザインにしない
- 前提知識を不当に求めないように凡例を含める
- 伝えたい内容に応じて必要十分なラベル・テキストを記述する

## 4. ナラティブ

- 概要→詳細の順で説明する
- 説明の流れは左から右に、上から下に
- ユーザ向けのレイヤを上に、インフラを下に
- 矢印を書く場合、関係性が明確になるように片方向の矢印を書き、さらにラベルで関係を記述する
- 特殊な矢印を使う場合は凡例と共に描く

## 5. 表記法

- アイコンは伝えたい情報を補足する形でのみ使用し、それだけで情報を伝えようとしないこと
- UMLにこだわりすぎず、図の目的や想定読者に応じて適切な表記法を選択する
- ふるまいと構造は分離する
  - ふるまいを表す図は「どのように」「誰に」を示し、構造を表す図は「何」が「どこ」」にあるかを示す
- まずは慣例に従う
  - 正当な理由があれば慣例を破ることを恐れてはならないが、慣例を破るときにはその理由を明確にすること
 
## 6. 構成

- 図はディスプレイにあわせて横長にする
- スタイル・対称性を気にかける
- 誤解を与えない表現をする(グラフの基準線やスケールなど)

## 7. 文章コミュニケーション

- シンプルな言い回しを使う
- 略語を使う場合は、確実に定義する
- 大事で抽象的な主張を最初に説明する
- 技術文書では、
  - 強い動詞を使う
  - 文を短くする
  - 段落の最初に重要な文を書く
  - 一つの段落では一つの話題だけを書く
  - 語彙は一貫する
  - 受け手を定義する

## 8. 言語コミュニケーションと非言語コミュニケーション

- コミュニケーションとは、メッセージをエンコードして送り、受け取ったメッセージをデコードすること
- 誰かが何を言おうとしているかを予測しない
