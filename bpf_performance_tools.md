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

### 1.4 A First Look at BCC: Quick Wins

BCCの利用例の紹介。

- execsnoop(8)
  - 新規プロセスの生成をトレースし、execve(2)システムコールを監視することで、各プロセスの起動時に一行要約を出力する
  - -tオプションを付与すると、タイムスタンプ付きで出力される。これにより、プロセスの生成間隔やバッチ的な起動パターンを把握できる。
  - execsnoop(8)はワークロード特性把握（workload characterization）に有効であり、システムにどのような負荷がかかっているかを素早く把握できる。

```
PCOMM            PID    PPID   RET ARGS
run              12983  4469     0 ./run
bash             12983  4469     0 /bin/bash 
...
```

- biolatency(8)
  - ブロックデバイスI/Oのレイテンシをヒストグラムで要約する。
  - -mオプションでミリ秒単位の集計が可能であり、以下のような出力となる。
  - この出力から、I/Oレイテンシの分布や異常値（アウトライア）が一目で分かる。

```
    msecs               : count     distribution
        0 -> 1          : 16335    |****************************************|
        2 -> 3          : 2272     |*****                                   |
        ... 
      512 -> 1023       : 11       |                                        |
```

### 1.5 BPF Tracing Visibility

BPFトレーシングは、

- システムの再起動やアプリケーションの再起動なしに、
- カーネルの深い部分、デバイス、アプリケーションライブラリなど、これまで見ることのできなかった領域を、
- 本番環境でライブに

調査することができる。

### 1.6 Dynamic Instrumentation: kprobes and uprobes

- 動的インストルメンテーションとは:
  - 本番環境で稼働中のソフトウェア（カーネルやアプリケーション）を再起動することなく、その内部の任意の関数に計測コード（インストルメンテーション）を挿入する技術。
  - これにより、何万もの関数の中から特定のものをトレースし、その動作を詳細に可視化できる。

- kprobes と uprobes:
  - kprobes: カーネル関数に対する動的インストルメンテーション
  - uprobes: ユーザーレベルのアプリケーション関数に対する動的インストルメンテーション
