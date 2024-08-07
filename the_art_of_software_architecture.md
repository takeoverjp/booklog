# アーキテクトの教科書

## Links

- [アーキテクトの教科書](https://www.seshop.com/product/detail/26378)

## 1. アーキテクトの仕事

- アーキテクトが備えるべき能力や考え方
  - 設計力・コーディング力
  - 抽象化能力
  - ビジネスの理解
  - 好奇心
  - 完璧主義よりも合理主義
    - どのコード断片をとっても良い設計、良いコードになっているというのは理想だが現実的ではない
    - ソフトウェアの重要な部分は徹底的にレビューを実施するが、そうでもない部分については多少は目を瞑るという割り切りも、時には必要
    - アーキテクチャは目的ではなく手段

## 2. ソフトウェア設計

- SOLID原則
- CLEANコード
  - Cohesive (凝集性)
  - Loosely Coupled (粗結合)
  - Encapsulated (カプセル化)
  - Assertive (断定的)
  - Nonredundant (非冗長)

## 3. アーキテクチャの設計

- Sagaパターン (サーガパターン)
  - マイクロサービスアーキテクチャのいて、複数のサービスをまたがる分散トランザクションの一部が失敗したときに結果整合性を実現するパターン
  - 保留と確定の２段階にする、エラー発生時に打ち消す処理を実行する、と行ったアプローチがある
  - 調整役のあるオーケストレーション方式と、自律的に相互調整を行うコレオグラフィ方式がある
- BFF (Backends for frontends) パターン
  - Webやモバイルなど、複数のクライアント種別をサポートするときに、それぞれごとにAPIゲートウェイを用意するパターン
  - 全てのクライアント種別をサポートするAPIゲートウェイを作ろうとすると、肥大化してしまうときに選択する
- アーキテクチャを検討するときには、複数案に対して評価項目ごとの比較評価マトリクスを作成するとよい
- ADRに個々の設計判断を記録し共有する

## 4. アーキテクチャの実装

- 開発プロセス標準化
  - 本来アーキテクトが行うべき活動に、開発プロセスやドキュメントの標準化は一般的には含まれないが、実際には標準化の支援や中心的な役割を担うことが多い

## 5. 品質保証とテスト

- Googleでは、テストのコードカバレッジに対するガイドラインとして、60%だと許容範囲、75%だと称賛に値する、90%を超えると模範的と定めている
  - https://testing.googleblog.com/2020/08/code-coverage-best-practices.html
  - コードカバレッジを拡大する努力は、設計・レビューの質を高め、高品質なソフトにつながる
  - コードカバレッジが高くても、テストカバレッジの質が高いとは限らない
  - しかし、コードカバレッジが低い場合、自動テストで完全にテストできていないことが保証される
  - 100%を目指すことは不毛
  - コードカバレッジの数値そのものよりも、カバーされていないコードに対する人間の判断こそが重要
  - 頻繁に変更されるコードの方が、テストする優先順位は高い
  - 統合テスト・システムテストのコードカバレッジも可視化することで、「統合テスト・システムテストでテストできているはず」という誤った安心感を避けることができる
  - チームごとにしきい値を決めて、CIでチェックすべき

## 6. アーキテクトとしての学習と成長

- [本を読む本](https://bookclub.kodansha.co.jp/product?item=0000150900)
- [Design It!](https://www.oreilly.co.jp/books/9784873118956/)
- [レガシーコードからの脱却 ソフトウェアの寿命を延ばし価値を高める9つのプラクティス](https://www.oreilly.co.jp//books/9784873118864/)
- [アジャイルソフトウェア開発の奥義](https://www.sbcr.jp/product/4797347784/)
