# BPF Performance Tools

## 1. Introduction

### 1.1 What Are BPF and eBPF

- kernelに組み込まれたsandbox実行環境で、mini programを実行できる
- 主にネットワーク、observability、セキュリティの分野で使われるが、この本では主にobservabilityに焦点を当てる

### 1.2 What Are Tracing Snooping, Sampling, Profiling and Observability?

- **Tracing**, **Snooping**, **Event Dump**: イベントベースの記録。の発生を追跡し、詳細な情報を収集する。
- **Sampling**, **Profiling**: イベントの一部をランダムに選び、全体の傾向を把握する。
- **Observability**: システムの内部状態を観測することでシステムを理解すること

### 1.3 What Are BCC, bpftrace, and IO Visor?

- BCC
  - BPF用の高水準トレーシングフレームワーク
  - PythonやLua、C++など複数言語からカーネルBPFコードを記述できる環境を提供する
  - libbccやlibbpfなどのライブラリも提供し、多数のツールが利用可能

- bpftrace
  - BPFツール開発のための専用高水準言語を提供するフロントエンド
  - シンプルな構文で強力なワンライナーや短いスクリプトに適している
  - libbccやlibbpf上に構築されている

- 本書では両者をまとめて「BPFトレーシング」と呼ぶ

