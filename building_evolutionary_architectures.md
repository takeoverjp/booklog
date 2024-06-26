# links

- [進化的アーキテクチャ](https://www.oreilly.co.jp/books/9784873118567/)

# 本書を読む目的

- 変化に応じて実プロジェクトのアーキテクチャを適切に進化させられるようになる
- 適応度関数を理解し、実プロジェクトに適用できるようになる

# １．ソフトウェアアーキテクチャ

- ソフトウェアアーキテクチャとは、「（それが何であれ）重要なもの」であり、アーキテクトの仕事とは「（それが何であれ）重要なものを全て理解し、釣り合いを取ること」
- アーキテクチャ特性としての「進化可能性」は、「システムが進化していく中で他のアーキテクチャ特性を保護する」ことを意味する
- 進化的アーキテクチャとは、複数の次元にわたる漸進的で誘導的な変更を支援するものである。
- 「漸進的な変更」とは、制御可能な大きさのステップに分解して、段階的に変更することを意味する。
  - 例えば、すべてのコンポーネントについて一斉に新しいバージョンに移行するのではなく、各コンポーネントの事情に合わせて徐々に移行していく、など
- 「誘導的な変更」とは、重要なアーキテクチャ特性を保護できる方向にアーキテクチャの変更を誘導することを示す。
  - 「適応度関数」によって、変更がアーキテクチャ上の重要な特性にどのように影響するかを評価し、それらの特性が経年劣化するのを防ぐ
  - 適応度関数によって、組織の必要性やビジネス機能に即した決定を可能にし、さらにそれらの決定の根拠を明確かつ検証可能にする。
  - 急速な変化の必要性とシステムやアーキテクチャ特性への厳密さの必要性の間で、釣り合いを取るためのアプローチ。
- 「次元」とは、直行する方法で組み合わさることの多い、アーキテクチャの一部を指す
  - 例：技術、データ、セキュリティ、運用系、監査可能性、合法性、スケーラビリティ、

- コンウェイの法則
  - システムを設計するあらゆる組織は、必ずその組織のコミュニケーション構造に倣った構造を持つ設計を生み出す
  - 移譲が行われることで誰かが調査できる範囲が狭まるたびに、実際に追い求めることができる設計の選択肢の種類も狭まる
- 逆コンウェイ戦略
  - サイロ化した技術アーキテクチャによる分割ではなく、サービス境界を中心にチームを組織する
  - こういう組織づくりのときの理由って、もっと言っていったほうが良いと思う

- 「進化」という名前に込めた想い
  - 「漸進的」と「誘導的」を込めた
  - 生物学上の「進化」に倣い、「進化」という言葉に目的に沿った、絶え間なく変化する環境で生き残ることができるシステムを持つこと
  - これぐらいしっかり名前付けに意図を持ちたい

# ２．適応度関数

- **アーキテクチャの適応度関数**は、あるアーキテクチャ特性がどの程度満たされているかを評価する客観的な指標を与える
- 特定のアーキテクチャ特性に対応する適応度関数だけでなく、**システム全体の適応度関数**を定義することもできる。この適応度関数によって、トレードオフを主観的ではなく具体的に評価することができる。

- 適応度関数の分類
  - アトミック(単一次元) vs ホリスティック(複数次元)
  - トリガー式 vs 継続的
  - 評価指標が静的 vs 評価指標が動的(状況による)
  - 自動 vs 手動
  - 一時的
    - 例：暗号化ライブラリの重要な更新のリマインダ
  - ドメイン特化（法規など）
  - 主要なもの(これらを中心とした変更を容易にできるアーキテクチャを目指す) vs 関連性のあるもの(機能レベルで考慮する) vs 関連性のないもの(アーキテクチャと関係ない)

- 全ての適応度関数を最初に明らかにすることはできないが、早い段階で特定し優先順位をつけることが望ましい
  - 適応度関数を満たすために簡単に変更できなければならない責務を、コードベース全体に分散させてしまったりする

- 適応度関数を実行した結果を共有スペースなどで可視化することで、主要な適応度関数と関連性のある適応度関数の知識を保ち続ける

# ３．漸進的な変更を支える技術

- 漸進的な変更を支えるために、バージョンの互換性をプロキシによって維持しながらマイグレーションしたりする
- デプロイメントパイプラインに、全てのアトミックな適応度関数とホリスティックな適応度関数による検証を追加することで、アーキテクチャ上の関心事についての評価から曖昧さや主幹を減らし、開発プラクティスに対する信頼を増すことができる
- 架空の例として、下記のような適応度関数をあげている
  - アトミックな適応度関数：自動スケーラビリティテスト・セキュリティ侵入テスト
  - ホリスティックな適応度関数：統合点を保護する契約に対するテスト、横断的なスケーラビリティテスト
  - 手動の適応度関数：セキュリティレビュー・監査

# ４．アーキテクチャ上の結合

- 最小のオーバーヘッドで最大の利益をもたらすために結合すべきアーキテクチャの次元を特定したい
- モジュールは論理的なグループ化を意味し、コンポーネントは物理的な分割方法を意味する
- アーキテクチャ漁師とは、高度な機能的凝集を持つ、独立してデプロイ可能なコンポーネントを意味する

- アーキテクチャスタイルの進化可能性
  - 巨大な泥団子
    - 管理されていない結合が多く変更は困難である
    - 保護対象を識別する前提となる構造がないため、適応度関数を構築することも難しい
  - 非構造化モノリス
    - 高度に結合しているため変更が波及し、漸進的に行うことは難しい
    - 高度な結合のためにスケーラビリティやパフォーマンスに対応することは難しい
  - レイヤ化アーキテクチャモノリス
    - コンウェイの法則に従った組織では、レイヤを縦断するサービス単位の変更が難しい
    - レイヤによる関心の分離により多くの部品を独立してテストできるため、適応度関数は構築しやすい
    - レイヤに閉じた変更であれば、最小限の副作用で簡単に交換できる
  - モジュール式モノリス
    - モジュールのデプロイ可能性により漸進的な変更が容易かどうかが決まる
    - モックなどのテスト技法が容易になり適応度関数を構築しやすい
  - マイクロカーネル式モノリス
    - ほとんどの変更はプラグインのみ修正・デプロイすればよいはずなので、漸進的な変更は容易
    - コアとプラグインのそれぞれに対して適応度関数を定義すれば良い
    - 結合特性がマイクロカーネルパターン自体によって明確に定義されているので、変更を容易にするが、プラグイン管で依存がある場合は適応度関数によって適切な結合であることを保証しなければならない
  - Broker型イベント駆動アーキテクチャ
    - 依存は少ないので変更は容易
    - 非同期通信なので、テストは難しい
    - アトミックな適応度関数の構築は容易だが、調整役がいないので、ホリスティックな適応度関数の構築は難しい
    - 結合はサービスとメッセージの間に生じる
  - Mediator型イベント駆動アーキテクチャ
    - 依存は少ないので変更は容易
    - 非同期通信なので、テストは難しい
    - 調整役がいるので、ホリスティックな適応度関数の構築も容易
    - 調整役との結合が増えるため、変更の時に副作用を考慮する必要が生じる
  - エンタープライズサービスバス駆動サービス指向アーキテクチャ (ESB駆動SOA)
    - ESB駆動SOAは進化の性質を示すようには設計されていない
    - 組織構造やデプロイ単位が横軸になっていることが多く、変更に必要なコストが大きい
    - 完結している部品が１つもないので、テストの実施が難しく、アトミックな適応度関数を構築することは事実上不可能である
    - ESB駆動SOAが目的とする再利用を推し進め、各ワークフローの本質を形にできれば、将来の開発はサービスの接続だけでできるかもしれないが、独立して進化できない以上一般的にそのサービスのサポートは貧弱になってしまう
  - マイクロサービスアーキテクチャ
    - ビジネスドメインごとに全ての従属コンポーネントをカプセル化することで、不適切な結合を防止し、他のサービスを中断することなく、あるサービスを置き換えることが目標
    - 「無共有アーキテクチャ」とも呼ばれる
    - ビジネスドメインごとに高度に粗結合になっているため、開発と運用どちらの側面でも、漸進的な変更が容易である
    - アトミックな適応度関数とホリスティックな適応度関数の両方を容易に構築できる
    - 結合は、メッセージング呼び出しと、サービステンプレート（共通処理）のみで管理可能
  - サービスベースアーキテクチャ
    - 既存のアプリケーションをマイクロサービスに移行するためのアーキテクチャ
    - ドメイン中心で分割することは同じだが、粒度がマイクロサービスアーキテクチャより大きい
    - サービス粒度が大きくなるため、変更の単位も大きくなり、変更は容易ではなくなる
    - データベースは細分化することは難しいので、モノリシックであることが多い
    - 統合ミドルウェアによって、レガシーシステムとの橋渡しをし、サービスの追加や変更を容易にする
  - サーバーレスアーキテクチャ
    - インフラは隠蔽されており、デプロイメントパイプラインと相性がいい
    - サードパーティのAPIを保証するためのホリスティックな結合度関数が必要
    - 状態やトランザクションといった複雑性は呼び出し側に押し付けられる
    - 多くのユースケースには素早く対応できるが、完全に要件を満たすものを作るのは難しい傾向が強い

- 循環依存を避けるために、コーディング標準は役に立たない。検証メカニズムを確立するべきである

# ５．進化的データ

- データベーススキーマとは、物事を抽象化したものであり、対象となる現実世界が変化したのであれば、抽象にも反映されなければならない
- データベースリファクタリング
  - expand/contractパターン
    - 一時的に後方互換性を保って新旧両データを定義(expand)し、移行期間ののち、旧データを削除(contract)する
    - 並列変更と呼ばれる、インターフェースに後方互換性のない変更を安全に実装するために使用される汎用的なパターンのサブパターン

- DBAは企業ではなくデータベースベンダーに対して忠誠心をもつため、ベンダー提供ではないツールを無視し、結果として開発プラクティスのイノベーションレベルが低下していた

- データのあらゆる断面を保持し続けようとすると、アーキテクチャを過去と結びつけ、複雑な回避策を強いてしまう。本当に価値のあるデータを見定め、不要なデータは参照用だけできるようにしておき、進化的な開発の主流からは外すべき。

# ６．進化可能なアーキテクチャの構築

- 適応度関数を用いた仕組みを構築することで、不全が育つことを阻止する
  - ①進化の影響を受ける次元を特定する
  - ②それぞれの次元に対して適応度関数を定義する
  - ③デプロイメントパイプラインを使って適応度関数を自動化する
- 「グリーンフィールドプロジェクト」とは、技術的負債のない、新規プロジェクトのこと
- 既存のアーキテクチャに対して進化可能性を加えることができるかどうかは、コンポーネント結合、開発プラクティスの成熟度、適応度関数の作りやすさに依存する
  - ビジネス上の問題を理解した上で、適切な機能的凝集をもつ単位にコンポーネントを分解するべき。ビジネス上結合が強いものを無理に細分化することは非生産的
  - 開発者が変更を容易にデプロイすることができる運用が必要
  - あらゆる種類のアーキテクチャ検証メカニズムを適応度関数として考えるべき
  - パッケージソフトは自由度が低いため、必要なものに限定して閉じ込め、できるだけ堅牢な適応度関数の集合を構築し、あらゆる可能な機会に自動実行すべき

- アーキテクチャの移行を考える際、クラスやコンポーネントの結合特性は考えるものの、データなどの他の次元を無視してしまうことが多い
- シニア開発者は単調なアプリ開発に飽きて、フレームワーク作成などのメタ作業を面白がるが、それだけでアーキテクチャを構築してはいけない
- アーキテクチャの移行には、「今の理由だから」以上の理由がなければいけない
- アーキテクチャの移行時には、正しいサービスの粒度を見つける必要がある
  - ビジネス機能グループ・トランザクション境界・デプロイメント目標など

- サービス分割する
  - 共有モジュールに依存する複数モジュールを含む大きなサービスを、複数のサービスに分割する場合、共有モジュールを共有ライブラリに抽出する・複製する・サービス呼び出しにする、といった対応が必要である
  - 共有は結合なので、マイクロサービスアーキテクチャでは強く避けられる
- UIからビジネスレイヤを分割する
- サービスディスカバリの仕組みを構築する適応度関数を確認しながら、サービスを切り話していく

- 大企業のアーキテクチャは、低レベルのシステムについて古い荷物を抱えながらも以前重要な処理を実行させつつ、それを現在の要件に順応させている。結果として、アーキテクチャに進化可能性を組み込むことは困難になる。
- 不要な変数を取り除く
  - ソースコードにおいて変数を減らしできるだけ定数にする考え方を、インフラにも取り込み、予期しない変更の可能性を閉じ、システムがもろくなりにくくする
  - スノウフレーク（雪の結晶のように、似て非なるサーバ）をイミュータブルインフラストラクチャ（完全にプログラム的に定義されたシステム）に置き換える
- 決定を可逆にする
  - ブルーグリーンデプロイメント・昨日トグル・サービスルーティングなどの手法により、カナリアリリースを行い、課題が見つかったら即座に切り戻すことができるようにしておく

- 予測可能ではなく進化可能を選ぶ
  - 未知の未知に対応するために容易に変化できること、つまり進化可能性がたいせつになるあります

- ジャストインタイムの腐敗防止層
  - 「今この判断をしなければならないだろうか」「仕事を遅らせることなく、この判断を安全に延期する方法はあるだろうか」「今ここに置くのに十分足りるものは何で、必要に応じてあとで変更が容易だろうか」を考えて、早すぎる複雑さを導入してしまわないように気をつける
  - 腐敗防止層を導入するときにも、なんでもかんでも抽象化するのではなく、必要になった時点で抽象化する

- サービステンプレート
  - サービスディスカバリ、監視、ロギング、メトリクス、認証・認可などの一貫性を保証するための解決策
  - サービスを実装するチームは、テンプレートを足場とする
  - ロギングツールを更新する場合、インフラチームはサービスチームとは直行して管理できる
    - とはいえ開発済みのサービスへのどうやって全体に反映するんだろうか？
  - 関連
    - [社内で提供しているマイクロサービスの参考実装について - DMM inside](https://inside.dmm.com/articles/microservices-template/#content0)
    - [マイクロサービスのためのプロジェクト生成ツール Lazybones を使ってみた](https://engineering.linecorp.com/ja/blog/lazybones-project-generation-tool-for-micro-service)

- 犠牲的アーキテクチャ
  - 考えがうまくいくことがわかったら投げ捨てるよう設計されたアーキテクチャ
  - 正しいアーキテクチャを選択できるくらい十分に学ぶ方法として、PoCをつくることが挙げられる
  - クラウドサービスの場合は、市場があることを証明することもできる

- 外部の依存関係にもっと積極的にアプローチし、更新を自分たちが管理するリポジトリへのプルリクエストとし、デプロイメントパイプラインを通すべきである

- ライブラリとフレームワーク
  - 開発者のコードが呼び出すものがライブラリ。結合の度合いは低く、置き換えは容易。
  - 開発者のコードを呼び出すものがフレームワーク。結合の度合いは強く、置き換えは難しい。
  - フレームワークは基礎部分となるため、積極的に更新しなければならない。フレームワークに対して複数のメジャーバージョンアップを一度に対応することは大きな痛みを伴う
  - ライブラリは必要なときに更新する。フレームワークはメジャーバージョンリリース時に更新する

- 不安定な依存関係と保護された依存関係
  - デフォルトでは積極的に新しいものを使い、デプロイメントパイプラインのどこかで異常が発生した場合、特定のバージョンを使うように自動的にする
  - 一般的なビルドツールではサポートされない機能だが、進化的アーキテクチャにおいてうまく機能する

- バージョン付
  - 複数バージョンを別のAPI名で提供する方法と、呼び出し元のコンテキストからふるまいを自動的に判断する方法がある
  - 望ましいのは自動的に判断する方法
  - いずれにせよ、一度にサポートするのは２つのバージョンまでとし、一時的なものとするべき
 
- プロキシがリクエストタイプを解決することで、呼び出し元がサービスのバージョンを意識することなく新しいバージョンと古いバージョンをサポートしマイグレートすることができること

# ７．進化的アーキテクチャの落とし穴とアンチパターン

- 悪いプラクティス
  - 落とし穴：表面的には良い考えに見えるが、すぐに筋の悪さが明らかになる
  - アンチパターン：当初は良い考えに見えるが、結果的に誤りであり、より良い選択肢が存在することが判明する

- アンチパターン：ベンダーキング
  - ベンダーソフトウェアをアーキテクチャの中心に据えてしまうと、技術的にもビジネスプロセス的にも厳しく制限されてしまう
  - ベンダーソフトウェアを単なる統合点の一つとして扱い、統合点との間に腐敗防止層を設けることで、アーキテクチャに影響を与えることを防ぐことができる

- 落とし穴:抽象化の欠如
  - 本来上位が意識しなくて良い下位レイヤの振る舞いが、不具合として表面化してしまうこと
  - 近代的な多段の抽象化の上で構築するソフトウェアでは、この種の不具合の解析は困難を極める
  - 下位レイヤの振る舞いを理解することに努めること
  - 下位レイヤを結合点として適応度関数で保護することで、抽象化の欠如の発生を抑止するべき
:
- アンチパターン：ラスト10%の罠
  - パッケージ・ソフトウェアやプラットフォーム、フレームワークを使用することで、やりたいことの80%は簡単に実現できるが、90%実現するのは困難で、100%実現することは不可能、ということに多くのコストを投下したあとで気づくアンチパターン

- アンチパターン：コード再利用の乱用
  - コードの使いやすさと再利用可能性は廃藩であることが多い
  - マイクロサービスは、コードの再利用を避け、結合より重複を選ぶ
  - サービステンプレートで、アーキテクチャ上の関心事を統一する

- 落とし穴:レジュメ駆動開発
  - アーキテクチャはレジュメに書くことを目的に選ぶべきではない
  - 問題を解決するために選択する

- アンチパターン:不適切なガバナンス
  - 複数のリソースをマシンに詰めることは、不注意な結合を生じさせるた めに、開発の観点からは望ましくない。共有リソース間の分離がどれほどうまく行え ていたとしても、最終的にはリソースの競合が起きる。
  - マイクロサービスでは、それぞれのチームが彼らのサービスを実 装するために必要な複雑さと洗練さを適切に選択できる
  - ゴルディロックスガバナンスでは、 標準化のために、単純、中間、複雑という 3 つの技術スタックを選定し、個々のサー ビス要件によってスタック要件が導かれるようにする。
    - 例：
      - 小規模：Ruby on Rails + MySQL
      - 中規模：Go + Cassandra / MongDB / MySQL
      - 大規模：Java + Oracle

- 落とし穴：リリース速度の欠如
  - サイクルタイム（作業の単位あたりの開始から終了にかかる時間）を適応度関数として監視することで防ぐ
  - 進化の速さはサイクルタイムに比例する

- 落とし穴：製品のカスタマイズ
  - 営業のために、個別ビルドや機能トグル・UIによるカスタマイズ機能を追加する必要に迫られることは多い
  - しかしカスタマイズを増やすことはテストの負荷を大幅に増加させ、適応度関数の数も増加する可能性があるため、進化可能性の妨げとなる
  - カスタマイズを追加することによって生じるコストを現実的に評価しなければならない

- 落とし穴：計画範囲
  - 多くの労力をかけると、その方針に固執しがち
  - フィードバックを得られない状態で大規模先行投資をしてはならない

# ８．進化的アーキテクチャの実践

- 機能横断型チーム
  - ビジネスアナリスト・アーキテクト・テスト担当・運用担当DBAなど、ドメインを実現するために全ての役割を一つのチームに持つことで、調整の摩擦を排除する
 
- 期間限定のプロジェクト思考でチームを作るのではなく、プロダクト自体に対する永続的な責任をもつ、顔の見える規模(10名程度)のチームを作るべき

- コンシューマ駆動契約
  - コンシューマがデータのプロバイダに対して自身が必要とするテストを提供する形で契約する、アトミックな適応度関数

- チームメンバは、機能横断型で、小さくなければならない
  - チームメンバ間のコミュニケーションコストは、O(n^2)になるため、小さくなければならない
  - チーム間の調整によって生じる人工的な摩擦を排除するために、機能横断型でなければならない

- 三度目になったらリファクタリング開始

- 実験を促進する方法
  - 外部からアイディアを取り入れる
  - カイゼン
  - 使い捨ての解決策を作成してみる（スパイクソリューション）
  - 自由なプロジェクトに時間を使えるようにする（20%ルール）
  - 複数の選択肢を同時に考える
  - エンドユーザと直接コミュニケーションする

- OSSライセンスの確認のように、解決策を自動化できないものであっても、変更を監視することで、注意を促す適応度関数は構築できる
- 実証が議論を打ち負かす
  - 最も簡単なことを最初に実施して、実証データを得ることで、安全に進化的アーキテクチャを実証することができる

- どこから進化可能性の改善を始めるかは、時と場合による
  - 簡単なところ、効果が高いところ、テスト、インフラなどの中から、ステークホルダーでロードマップを考える

- 生成的テストにより、これまで自動化が難しかったテストも自動化できる可能性が出てきている
  - 生成的テストでは、開発者は大量のテス
トを実行して結果を記録すると、統計分析を使用して結果から異常を探し出す。

- 順応 vs 進化
  - proxyを作り「順応させる」ことは、元のふるまいと新しいふるまいを両方サポートする必要が生じ、より複雑さが増し、技術的負債を増加させる
  - 機能トグルのように、仮設を検証した後、トグルを削除する場合は問題ない
  - 「進化する」とは、内部に潜んでいる時代遅れの解決策を増やさないことが肝要である

- アーキテクチャを検証するものは何であれ適応度関数である
- 適応度関数によって、アーキテクチャ特性を評価可能なものにしようするべきである
