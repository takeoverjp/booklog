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

### 1.7 Static Instrumentation: Tracepoints and USDT

- **動的インストルメンテーションの課題**:
  - kprobeやuprobeなどの動的インストルメンテーションは、カーネルやアプリケーションのバージョンアップで関数名が変更されたり、コンパイラの最適化でインライン化されたりすると、機能しなくなるというインターフェースの安定性の問題がある。

- **静的インストルメンテーションによる解決**:
  - この問題に対し、開発者がコード内に安定したイベント名を埋め込む「静的インストルメンテーション」が解決策となる。
  - BPFは、カーネル向けの**トレースポイント**と、ユーザーレベル向けの**USDT (User-level Statically Defined Tracing)** の両方をサポートしている。

- **推奨される開発戦略**:
  - BPFツールを開発する際は、まずトレースポイントやUSDTといった静的トレーシングを利用し、それが利用できない場合にのみ動的トレーシング（kprobeやuprobe）を利用することが推奨される。

### 1.8 A First Look at bpftrace: Tracing `open()`

- **基本的なトレース**
  - `bpftrace` は、コマンドラインからワンライナーでBPFプログラムを実行できる
  - 例: `open(2)` システムコールのトレースポイント (`syscalls:sys_enter_open`) を指定し、プロセス名と開かれたファイル名を出力する

- **オンデマンド計測**
  - `bpftrace` のコマンドは、実行されている間だけ計測を有効にする
  - Ctrl-Cで停止すると、トレースポイントは無効化され、BPFプログラムも削除されるため、システムへの影響はなくなる

- **ワイルドカードによる探索**
  - `open` には `openat` などの亜種が存在する
  - `bpftrace` の `-l` オプションとワイルドカード (`open*`) を使用することで、関連する全てのトレースポイントを一覧表示できる。

- **カーネル内集計**
  - イベントごとに出力する代わりに、カーネル内で効率的にイベントをカウントし、要約だけを表示することもできる

- **スクリプト化**
  - より複雑なトレースロジックは、スクリプトファイルとして保存し、実行することができる

### 1.9 Back to BCC: Tracing `open()`

- **BCC版opensnoop(8)の紹介**:
  - bpftrace版と同様に`open()`システムコールをトレースするが、BCC版はより多機能である。
  - コマンドラインオプションが豊富に用意されており、特定のPID、失敗したオープンのみ、タイムスタンプ付きでの表示など、多彩なフィルタリングや出力形式の変更が可能である。

- **BCCツールとbpftraceツールの比較**:
  - **BCCツール**: 一般的に多機能で、多くの操作モードをサポートしている。問題解決に必要な機能がすでにツールに組み込まれている可能性が高く、最初の選択肢として優れている。
  - **bpftraceツール**: シンプルで単一の機能に特化していることが多い。BCCツールに必要な機能がない場合、bpftraceのより平易な言語を使ってカスタムツールを開発することが推奨される。

## 2. Technology Background

### 2.1 BPF Illustrated

- 2章で取り上げる、BPF関連技術の相関関係を図で説明している。

### 2.2 BPF

- BPFは、ユーザーが定義したフィルタ式をカーネル内で直接実行することにより、tcpdumpのようなツールでのパケットフィルタリングのパフォーマンスを向上させる。
- この方式により、全パケットをユーザー空間にコピーするコストが不要になる。
- 安全性が重視されており、ユーザー空間から渡されたフィルタは、カーネルで実行される前に安全性が検証される。

### 2.3 Extended BPF

#### 2.3.1 Why Performance Tools Need BPF

- BPFの優位性は、
    - 効率的
    - 本番環境で安全
    - Linuxカーネルに組み込まれている

- BPF導入による効率化の例（ディスクI/Oサイズのヒストグラム生成）
    - BPF導入前:大量のデータをカーネルからユーザー空間へコピーし、再処理するため、特に高頻度のI/Oではオーバーヘッドが非常に大きい
    - BPF導入後:カーネルからユーザー空間へのデータ転送量が劇的に削減され、高コストなイベントのトレースを本番環境で実行可能になる

#### 2.3.2 BPF Versus Kernel Modules

  BPFのオブザーバビリティにおける利点を理解するもう一つの方法は、カーネルモジュールとの比較です。kprobesやトレースポイントはカーネルモジュールから直接利用できますが、BPFには以下の利点があります。

- カーネルモジュールと比較したBPFの優位性
    - 安全性: BPFプログラムはVerifierによって安全性が検証されるが、カーネルモジュールはバグ（カーネルパニック）やセキュリティ脆弱性を引き起こす可能性がある。
    - 豊富なデータ構造: BPFはマップを介して豊富なデータ構造を提供する。
    - ポータビリティと安定したABI: BPFは命令セット、マップ、ヘルパーなどが安定したABIとして提供されるため、一度コンパイルすればどこでも実行可能である（ただし、一部のトレーシングプログラムには例外がある）。
    - ビルド要件の簡潔さ: BPFプログラムのコンパイルにカーネルのビルド成果物は不要である。
    - 学習の容易さ: カーネルモジュールの開発に必要なカーネルエンジニアリングに比べ、BPFプログラミングは学習が容易である。
    - アトミックな置換: （ネットワーキングの文脈では）BPFプログラムはアトミックに置換できるためサービスの停止を避けられるが、カーネルモジュールはアンロードとリロードが必要でサービス中断を引き起こす可能性がある。
- BPFと比較したカーネルモジュールの優位性
    - BPFヘルパー関数に限定されず、他のカーネル関数や機能を利用できる

#### 2.3.3 Writing BPF Programs

- BPFプログラムは、複数のフロントエンドを介して、様々なレベルの言語で記述できる。トレーシングにおける主要なフロントエンドは、低レベルから高レベルの順に以下の通りである。
    - LLVM: BPFをコンパイルターゲットとしてサポートする。C言語（Clang経由）やLLVM IR（中間表現）のような高水準言語で書かれたプログラムを、最適化しつつ効率的なBPF命令にコンパイルする。
    - BCC: BPFプログラムをC言語で記述することを可能にする。
    - bpftrace: 独自のさらに高レベルな言語を提供する。
- BCCとbpftraceは、内部的にLLVM IRとLLVMライブラリを使用してBPFプログラムへのコンパイルを行っている。
- 本書で扱うパフォーマンスツールはBCCとbpftraceでプログラムされており、BPF命令やLLVM IRを直接記述することは、BCCやbpftraceの内部開発者の領域であり、本書の範囲外である。

#### 2.3.4 Viewing BPF Instructions: bpftool

- bpftool(8)は、Linux 4.15で追加された、プログラムやマップなどのBPFオブジェクトを表示・操作するためのユーティリティである。

- 主なサブコマンドと機能
    - `bpftool perf`:
        - perf_event_open()を介してアタッチされたBPFプログラム（BCCやbpftraceで通常使われる方法）を表示する。
        - プロセスID(PID)、プログラムID(prog_id)、アタッチタイプ（例: kprobe）などの情報を確認できる。
    - `bpftool prog show`:
        - ロードされている全てのBPFプログラムを一覧表示する。
        - perfサブコマンドよりも詳細な情報（ロード時刻、UID、メモリロックサイズ、関連マップIDなど）を提供する。
        - プログラムがBPF Type Format (BTF)付きでコンパイルされている場合、btf_idも表示される。
    - `bpftool prog dump xlated`:
        - 指定したプログラムIDのBPF命令（アセンブリ形式）をダンプする。
        - BTFの重要性: プログラムがBTF付きでコンパイルされている場合、ダンプされた命令に元のCソースコードが注釈として挿入されるため、可読性が大幅に向上する。
        - 修飾子:
            - linum: ソースファイル名と行番号を追加する。
            - opcodes: BPF命令のオペコード（16進数）を表示する。
            - visual: DOT形式で制御フローグラフを出力し、GraphVizなどで視覚化できる。
    - `bpftool prog dump jited`:
        - BPF命令がJITコンパイルされた後の、プロセッサのネイティブな機械語コードを表示する。
    - `bpftool btf`:
        - BTF自体の情報（型や構造体の定義など）をダンプすることができる。

#### 2.3.5 Viewing BPF Instructions: bpftrace

   - bpftraceでは、-vオプションを使用することでBPF命令を表示できる。これはtcpdumpの-dオプションに相当する。
   - この機能は、主にbpftraceの内部的なエラーが発生した場合にデバッグするために用いられる。
   - 例えば、開発したbpftraceプログラムがカーネルのVerifierによってリジェクトされた場合、これらのBPF命令が出力される。
   - 開発者は、出力された命令を調査することで、リジェクトの原因を特定し、修正を行うことができる。
   - ほとんどのユーザーはbpftraceやBCCの内部エラーに遭遇することはなく、BPF命令を直接見る必要はない。
   - もしこのような問題に遭遇した場合は、bpftraceやBCCのプロジェクトにチケットを提出するか、修正に貢献することが推奨される。

#### 2.3.6 BPF API

- BPF Helper Functions (for kernel BPF programs)
    - マップ操作: bpf_map_lookup_elem(), bpf_map_update_elem() など。
    - データ読み込み: bpf_probe_read() は、カーネルメモリやユーザー空間メモリから安全にデータを読み込みます。ページフォルトを無効化し、安全性を確保する重要な関数です。
    - 情報取得: bpf_ktime_get_ns() (タイムスタンプ)、bpf_get_current_pid_tgid() (プロセスID)、bpf_get_current_comm() (プロセス名)、bpf_get_stackid() (スタックトレース) など。
    - データ出力: bpf_perf_event_output() は、perfイベントリングバッファを介してユーザー空間にデータを送信するために使用されます。bpf_trace_printk() はデバッグ用の簡易的な出力関数です。

- BPF Syscall Commands (for user-space programs)
    - BPF_MAP_CREATE: 新しいBPFマップを作成する
    - BPF_PROG_LOAD: BPFプログラムを検証し、カーネルにロードする
    - BPF_PROG_ATTACH: ロードされたBPFプログラムを特定のイベント（kprobeやトレースポイントなど）にアタッチする
    - BPF_OBJ_PIN: BPFオブジェクトを/sys/fs/bpfファイルシステムに作成する

- BPF Program Types : BPFプログラムがどの種類のイベントにアタッチされるか。プログラムタイプによって、プログラムが受け取る引数の種類が決まる。
    - BPF_PROG_TYPE_KPROBE: kprobesやuprobes用。
    - BPF_PROG_TYPE_TRACEPOINT: カーネルの静的トレースポイント用。
    - BPF_PROG_TYPE_PERF_EVENT: パフォーマンスモニタリングカウンタ（PMC）を含むperf_events用。
    - BPF_PROG_TYPE_RAW_TRACEPOINT: 高速だが引数処理が簡素化されたトレースポイント用。

- BPF Map Types
    - BPF_MAP_TYPE_HASH:汎用ハッシュテーブル。
    - BPF_MAP_TYPE_ARRAY: 配列。
    - BPF_MAP_TYPE_PERF_EVENT_ARRAY: perfイベントリングバッファへのインターフェースとして機能し、ユーザー空間へのデータ送信に使われる。
    - BPF_MAP_TYPE_PERCPU_*: CPUごとにデータを持つマップで、ロックなしで並行処理の競合を避けるために使用される。
    - BPF_MAP_TYPE_STACK_TRACE: スタックトレースを効率的に格納する。

#### 2.3.7 BPF Concurrency Controls

- BPFは、並行して実行されるスレッドからの同時アクセスによるデータ破損（ロストアップデート問題）を避けるための仕組みを備えています。

- Per-CPUマップ:
    - トレーシングフロントエンド（BCCやbpftrace）がこの問題を回避するために主に使用する方法である。
    - 各論理CPUごとにマップのインスタンスを作成することで、複数のスレッドが共有の場所に同時に書き込むことを防ぐ。
    - 例では、Per-CPUハッシュマップと比較して、通常のハッシュでは0.01%のアンダーカウントが発生した

- その他の並行性制御メカニズム:
    - アトミック命令: BPF_XADDのようなアトミックな加算命令があります。
    - マップ・イン・マップ: マップ全体をアトミックに更新できます。
    - スピンロック: Linux 5.1で追加されたbpf_spin_lock()とbpf_spin_unlock()ヘルパー関数によって制御されます（ただし、トレーシングプログラムではまだ利用できません）。
    - アトミックなマップ更新: 通常のハッシュマップやLRUマップのbpf_map_update_elem()による更新も、データ競合を避けるためにアトミックに実行されます。

#### 2.3.8 BPF sysfs Interface

- Linux 4.4で、BPFプログラムとマップを仮想ファイルシステム（通常は/sys/fs/bpfにマウントされる）を介して公開する機能が導入された。
- この機能を「pinning」と呼ぶ。
- pinningの主な利点:
    - 永続性: BPFプログラムをロードしたプロセスが終了した後も、そのプログラムを実行し続けることができる（デーモンのように動作する）。
    - 相互作用: ユーザーレベルのプログラムが、実行中のBPFプログラムと対話するための別の方法を提供する。具体的には、ピン留めされたBPFマップに対して読み書きが可能になる。
- 本書での利用:
    - 本書で紹介されているBPFオブザーバビリティツールは、起動・停止する標準的なUnixユーティリティをモデルとしているため、ピニングは使用していない。
    - しかし、必要であればこれらのツールをピニングを使用するように変換することは可能である。
- 主な用途:
    - ピニングは、ネットワーキングプログラム（例: Cilium）でより一般的に使用される。
    - Android OSでは、/system/etc/bpfにあるBPFプログラムを自動的にロードしてピン留めするためにこの機能を利用している。

#### 2.3.9 BPF Type Format (BTF)

- BTFとは:
    - BPFプログラムやマップなどを記述するためのデバッグ情報を含むメタデータフォーマットである。
    - 当初はデータ型を記述するために作られたが、現在ではサブルーチンの関数情報、ソースコードの行情報、グローバル変数情報なども含むように拡張されている。
    - BTFは本書執筆時点ではまだ開発中である。

- BTFの利点と目的:
    - 自己記述的なBPFプログラム: BTFデバッグ情報をvmlinuxバイナリに埋め込むか、BPFプログラムと一緒に生成することで、bpftoolやperfのようなツールがプログラムをより容易に検査できるようになる。これにより、ソースコード付きのBPFプログラムの表示や、C構造体に基づいたマップのキー/バリューの整形表示が可能になる。
    - カーネルヘッダの代替:
      BPFトレーシングツールは、C構造体を解釈するためにカーネルヘッダ（linux-headersパッケージなど）を必要とすることが多い。しかし、ヘッダには全てのカーネル構造体の定義が含まれていなかったり、複雑なヘッダが正しく処理されない問題があった。BTFは、カーネル内の全ての構造体に対して信頼性の高い定義を提供することで、これらの問題を解決する。将来的には、BTFを含むカーネルバイナリは自己記述的になる。
    - CO-RE (Compile Once - Run Everywhere) のサポート: BTFは、異なるカーネルバージョン間での構造体のオフセットの違いなどを解決するための情報を提供し、一度コンパイルしたBPFプログラムを様々な環境で実行可能にするCO-REプロジェクトの基盤となる。

#### 2.3.10 BPF CO-RE

- CO-REの目的:
    - BPF CO-RE (Compile Once - Run Everywhere) プロジェクトは、BPFプログラムを一度バイトコードにコンパイルし、それを保存・配布して、コンパイラがインストールされていない他のシステムでも実行可能にすることを目指す。

- CO-REが解決する課題:
    - コンパイラの不要化: 各システムにLLVMやClangといった重量級のBPFコンパイラをインストールする必要がなくなる。これは特に、リソースが限られた組込みLinux環境で大きな利点となる。
    - ランタイムコストの削減: BPF観測ツールを実行するたびに発生する、コンパイルに伴うCPUやメモリのコストを回避できる。

- 技術的な挑戦と解決策:
    - カーネルバージョンの差異: CO-REが直面する最大の課題は、カーネルのバージョンによってデータ構造体内のフィールドオフセットが異なるという点である。
    - 再配置 (Relocation): CO-REは、BPFプログラムのロード時に、BPFバイトコード内のフィールドオフセットをターゲットカーネルの実際のレイアウトに合わせて書き換えることでこの問題を解決する。
    - BTFの活用: この再配置を行うために、CO-REはBTF (BPF Type Format) から提供されるメタデータ情報を利用して、ターゲットカーネルの正確な構造体レイアウトを把握する。
    - メンバーの欠損: 構造体メンバーがカーネルによっては存在しないという課題にも、条件付きのフィールドアクセスで対応する。

#### 2.3.11 BPF Limitations

- 関数呼び出しの制限:
    - BPFプログラムは、カーネル内の任意の関数を呼び出すことはできず、APIとして提供されているBPFヘルパー関数のみに限定される。

- ループの制限:
    - 無限ループに陥る可能性があるため、ループの使用は厳しく制限されている。これは、クリティカルなロックを保持したスレッドがシステムをブロックするのを防ぐためである。
    - 回避策としてループのアンローリング（展開）がある。Linux 5.3からは、実行回数が検証可能な有界ループがサポートされた。

- スタックサイズの制限:
    - BPFプログラムが使用できるスタックサイズは512バイトに制限されている。
    - この制限を超えるデータを扱う場合は、代わりに容量が実質的に無限であるBPFマップストレージを使用する必要がある。

- 命令数の制限:
    - 当初、プログラムの命令数は4096に制限されていた。
    - Linux 5.2でこの制限は大幅に緩和され（現在は100万命令）、長いプログラムも記述しやすくなった。ただし、特権のないBPFプログラムには依然として4096命令の制限が残る。

#### 2.3.12 BPF Additional Reading

- カーネルソース内のドキュメント:
    - [Documentation/networking/filter.txt](https://www.kernel.org/doc/Documentation/networking/filter.txt)
    - [Documentation/bpf/bpf_design_QA.txt](https://www.kernel.org/doc/Documentation/bpf/bpf_design_QA.txt)
- manページ:
    - [bpf(2)](https://man7.org/linux/man-pages/man2/bpf.2.html)
    - [bpf-helpers(7)](https://man7.org/linux/man-pages/man7/bpf-helpers.7.html)
- オンライン記事:
    - [Jonathan Corbetによる「BPF: the universal in-kernel virtual machine」](https://lwn.net/Articles/599755/)
    - [Suchakra Sharmaによる「BPF Internals-II」](https://www.iovisor.org/blog/2015/10/15/bpf-internals-ii)
- リファレンスガイド:
    - [Ciliumによる「BPF and XDP Reference Guide」](https://docs.cilium.io/en/stable/reference-guides/bpf/index.html)

### 2.4 Stack Trace Walking

この章では、イベントに至るまでのコードパスを理解するための重要な手法であるスタックトレースの取得方法について解説している。BPFは、カーネルおよびユーザーコードの実行時間を観察するために、スタックトレースを利用する。

1. フレームポインタベース
    - x86-64アーキテクチャにおいて、RBPレジスタをスタックフレームのリンクリストの先頭として使用する伝統的な手法。
    - これにより、デバッガやトレーサーはスタックを遡り、リターンアドレスを容易に取得できる。
    - しかし、最近のコンパイラはパフォーマンス最適化のため、デフォルトでフレームポインタを省略する設定（-fomit-frame-pointer）になっていることが多く、この手法が使えない場合がある。
    - -fno-omit-frame-pointerオプションを付けて再コンパイルすることで、多くの場合、性能劣化をほとんど伴わずにこの問題を解決できる。

2. debuginfo
    - DWARF形式のデバッグ情報を含むdebuginfoパッケージを利用する手法。
    - gdbのようなデバッガは、フレームポインタがなくてもこの情報を使ってスタックをたどることができる。
    - BPF自体は、処理が重く複雑なため、この手法をスタックウォークには直接使用しない。しかし、BCCやbpftraceなどのフロントエンドはシンボル解決のためにこの情報を利用する。

3. Last Branch Record (LBR) 
    - Intel CPUの機能で、最近の分岐命令をハードウェアバッファに記録するもの。
    - オーバーヘッドなしでスタックトレースを再構築できるが、記録できる深さが4〜32フレームと浅いのが欠点である。
    - BPFは現在サポートしていないが、将来的に対応する可能性がある。

4. ORC (Oops Rewind Capability) 
    - Linuxカーネルのスタックトレース用に考案された、DWARFよりも軽量な新しいデバッグ情報フォーマット。
    - BPFはカーネルのスタックトレースにはこの手法を利用できるが、ユーザースペースにはまだ対応していない。

5. シンボル
    - スタックトレースはまずアドレスの配列として記録され、その後ユーザースペースのプログラムによってシンボル（関数名）に変換される。
    - 収集と変換の間にシンボルマッピングが変わると、不正な結果が生じる可能性がある。

### 2.5 Flame Graphs

Flame Graphとは、プロファイリングされたスタックトレースを視覚化する強力な手法であり、システムのパフォーマンス問題、特にCPU使用率の原因を特定するために頻繁に利用される。

- スタックトレースとプロファイリング
   - スタックトレース: コードの実行フロー（どの関数がどの関数を呼び出したか）を示す一連の関数リストである。
   - プロファイリング: システムのパフォーマンスを分析するために、一定時間、多数のスタックトレースをサンプリングして収集する。しかし、収集されたデータは膨大（数千のスタックトレース）になるため、テキスト形式のままでは根本原因の特定が困難な場合が多い。

- Flame Graphの構造
  フレイムグラフは、この膨大なスタックトレースのデータを集約し、一つの視覚的な図として表現する。
   - Y軸: スタックの深さを表す。下にある関数が上にある関数を呼び出す親子関係を示す。根本（root）は一番下に、最新の関数は一番上に表示される。
   - X軸: プロファイリングに登場したサンプルの数を表す。重要な点として、これは左から右に時間の経過を示すものではない。 できるだけ同一関数をまとめて表示できるように、アルファベット順でソートされる。
   - フレームの幅: 各関数（ボックス）の幅は、プロファイル全体の中でその関数がスタックに出現した頻度に比例する。幅が広いほど、その関数がパフォーマンスに与える影響が大きいことを示す。
   - 読み方: 最も幅の広い「タワー」を探し、その頂上にある関数（フレーム）に注目する。CPUプロファイルのフレイムグラフでは、この頂上のフレームがCPU時間を直接消費している関数である。

- Flame Graphの機能
  オリジナルのフレイムグラフ実装（SVG形式）には、対話的な機能が含まれている。
   - 色の設定: フレームは様々なスキームで色付けできる。例えば、コードの種類（カーネル、ユーザー、JITコンパイルされたコードなど）によって色相を変えることで、視覚的に区別しやすくなる。
   - マウスオーバー: フレームにカーソルを合わせると、その関数の情報（全体に対する出現率など）が表示される。
   - ズーム: フレームをクリックすると、その部分が水平方向に拡大され、幅の狭いフレームでも関数名を確認できる。
   - 検索: 特定のキーワードで関数を検索し、該当するフレームをハイライト表示できる。

- バリエーション
   - フレームチャート: X軸がアルファベット順ではなく時間経過を表す。シングルスレッドアプリケーションの時間ベースのパターンを理解するのに適している。
   - 差分フレイムグラフ (Differential Flame Graphs): 2つのプロファイルを比較し、その差分を視覚化する。

### 2.6 Event Sources

- 動的インストルメンテーション (Dynamic Instrumentation)
    - kprobes: カーネル空間のほぼ全ての関数に動的にアタッチできる。
    - uprobes: ユーザー空間のアプリケーションやライブラリの関数に動的にアタッチできる。
- 静的インストルメンテーション (Static Instrumentation / Tracepoints)
    - カーネル開発者によってコード内に意図的に配置された、安定したトレースポイント。
    - syscalls（システムコール）、sched（スケジューラ）、tcp、block（ブロックI/O）など、様々なサブシステムに存在する。
- ハードウェアイベント (Hardware Events / PMCs)
    - CPUのパフォーマンスモニタリングカウンタ（PMC）を利用する。
    - cycles（サイクル数）、instructions（命令数）、キャッシュミスなど、プロセッサレベルのイベントを計測できる。
- ソフトウェアイベント (Software Events)
    - カーネルによって定義されたソフトウェアベースのイベント。
    - CPUクロック、コンテキストスイッチ、ページフォルト、CPUマイグレーションなどが含まれる。

### 2.7 kprobes

- kprobesの仕組み
    - kprobe: ほぼ全てのカーネル関数の命令を対象に、動的にインストルメンテーション（トレースポイント）を挿入する技術である。これにより、再起動や特別なカーネルモードを必要とせず、本番環境でライブにカーネルの動作を分析できる。
        - 具体的には、ターゲット命令をブレークポイント命令（x86-64ではint3）に置き換える。
        - 命令が実行されブレークポイントに達すると、kprobeハンドラが起動し、アタッチされたBPFプログラムなどを実行する。
        - その後、元の命令を実行して処理を再開する。
    - kretprobe: 関数のリターン（終了）を捕捉するためのインターフェース。kprobeと組み合わせることで、関数の実行時間（レイテンシ）を計測できる。
        - これは、関数のエントリーポイントでリターンアドレスを「トランポリン」と呼ばれる代替関数に書き換えることで実現される。


- 安全性
    - 命令を直接書き換えるためリスクが高いように見えるが、再帰的なトラップを避けるためのブラックリストや、安全な命令置換メカニズム（stop_machine()など）によって安全性が確保されている。実用上の最大のリスクは、非常に高頻度で呼び出される関数をトレースすることによるパフォーマンスのオーバーヘッドである。

- kprobesのインターフェース
    - kprobesにはいくつかの利用方法があるが、BCCやbpftraceのような現代的なBPFツールは、主にperf_event_open()システムコールを介して利用する（Linux 4.17以降）。

- BPFとkprobesの関係
    - BPFトレーシングツールは、kprobesをカーネルの動的インストルメンテーションの基盤として広く利用している。
        - BCC: attach_kprobe()やattach_kretprobe()といったPythonメソッドを通じて利用する。
        - bpftrace: kprobe:関数名やkretprobe:関数名といったプローブタイプで指定する。

### 2.8 uprobes

- uprobesの仕組み
    - uprobes はカーネルレベルの kprobes に似ているが、ユーザー空間のプロセスを対象とする。
    - ターゲット命令にブレークポイントを挿入し、実行がそのアドレスに達するとuprobeハンドラに制御を渡す。
    - 関数のリターンをトレースする uretprobes も同様に、リターンアドレスを書き換えることで実現される。

- インターフェース
    - 主にFtraceベース (/sys/kernel/debug/tracing/uprobe_events) と perf_event_open() の2つのインターフェースが存在する。
    - BPFトレーシングツールは、Linux 4.17以降で perf_event_open() を使用する。

- BPFとuprobes:
    - BCCやbpftraceなどのBPFツールは、uprobes を利用してユーザーレベルの関数を動的にトレースする。
    - これにより、ライブラリ呼び出しなどをシステム全体でトレースすることが可能になる。


- オーバーヘッドと将来の展望:
    - malloc() のような高頻度で呼び出される関数に uprobes を使用すると、ターゲットアプリケーションのパフォーマンスが10倍以上低下する可能性がある。
    - このため、高頻度のイベントに対する uprobes の使用は、本番環境での常時監視よりも、テスト環境でのトラブルシューティングなどに適している。
    - 将来的には、カーネルへのトラップを回避する共有ライブラリ方式の導入により、オーバーヘッドが大幅に削減される可能性が議論されている。

### 2.9 Tracepoints

- Tracepointsとは
    - カーネル開発者によって、コード内の論理的な重要箇所に予め埋め込まれた静的なトレースポイントである。
    - 動的にどこにでも設定できる kprobes とは対照的に、Tracepoints は安定したAPIを提供することが大きな利点である。これにより、カーネルのバージョンが上がっても、それを利用するツールが壊れにくい。
    - そのため、利用可能であれば kprobes よりも先に Tracepoints を使うことが推奨される。

- 仕組みとオーバーヘッド
    - 「static jump patching」という技術により、Tracepoints が有効化されていない時のオーバーヘッドは、NOP命令数個分と非常に小さい。
    - トレースが有効になると、このNOP命令がトレース処理を呼び出すジャンプ命令に書き換えられる。

- BPFでの利用
    - BPFはLinux 4.7から Tracepoints の利用をサポートしており、BCCやbpftraceから容易にアタッチできる。
    - Raw Tracepoints という、より生の引数を直接扱うことでパフォーマンスを向上させたインターフェースも存在するが、APIの安定性は犠牲になる。

### 2.10 USDT (User-level Statically Defined Tracing)

- USDTとは
    - USDTは、アプリケーション開発者によってソースコード内の特定の場所に予め埋め込まれた、ユーザー空間向けの静的なトレースポイントである。
    - これにより、カーネルのTracepointsと同様に、ツールの互換性を損なうことなく安定したトレーシングが可能となる。

- 特徴と仕組み
    - 安定したAPI: USDTは、kprobesやuprobesのような動的トレーシングと異なり、アプリケーションのバージョンアップによる関数名の変更などの影響を受けにくい安定したAPIを提供する。
    - 低オーバーヘッド: USDTプローブは、有効化されていない状態では`nop`としてコード内に存在するため、パフォーマンスへの影響はほとんどない。トレーサーによって有効化されると、カーネルはこの`nop`命
     令をブレークポイント命令に書き換え、アタッチされたBPFプログラムを実行する。
    - セマフォ: 高価な引数処理を伴うプローブのために、プローブが有効なときだけ引数処理を実行させる「セマフォ」の仕組みがある。これにより、プローブが無効なときのオーバーヘッドを回避できる。

- USDTプローブの追加方法
    - https://github.com/iovisor/bcc/blob/master/examples/usdt_sample/usdt_sample.md
    - systemtap-sdt-develか、FacebookのFollyが提供するマクロでプローブを追加することができる

- BPFでの利用
    - BCCやbpftraceはUSDTをサポートしており、それぞれ`USDT().enable_probe()`や`usdt:`プローブタイプで簡単に利用できる。
    - これにより、MySQLのクエリ開始・終了や、各種プログラミング言語ランタイムの内部動作など、アプリケーションレベルのイベントを効率的にトレースできる。
    - 多くのアプリケーションでは、USDTプローブはデフォルトで無効になっており、コンパイル時に`--enable-dtrace-probes`のような特別なオプションを指定してビルドする必要がある。

### 2.11 Dynamic USDT

- StaticなUSDTの課題
    - 通常のUSDTは、ソースコードにあらかじめプローブを埋め込み、コンパイル時にバイナリに含める必要がある。
    - このため、JavaのJVMのように、実行時にコードをJITコンパイルする言語には適用できないという問題があった。

- 動的USDTによる解決策
   1. USDTプローブを含む共有ライブラリの事前コンパイル:
      トレースしたいイベントに対応するUSDTプローブをCやC++で記述し、共有ライブラリとしてコンパイルしておく。このライブラリは、通常のUSDTプローブと同様にELFノートセクションを持つ。
   2. 実行時のライブラリロードと呼び出し:
       - ターゲットとなる言語（Node.jsやPythonなど）の実行時に、この共有ライブラリをdlopen(3)で動的にロードする。
       - ターゲット言語は、APIを介してこの共有ライブラリ内の関数を呼び出す。これにより、間接的にUSDTプローブを発火させることができる。

- 実装例：libstapsdt
    - この動的USDTの仕組みを実装したlibstapsdtというライブラリが存在する。
    - これはNode.jsやPython、Rubyといった言語で動的にUSDTプローブを定義・利用することを可能にする。
    - libstapsdtは、実行時にUSDTプローブを含む共有ライブラリを自動生成し、実行中プロセスのメモリ空間にマッピングする。

### 2.12 PMCs

- PMCとは
    - PMCは、CPUに組み込まれたプログラム可能なハードウェアカウンタであり、CPUのサイクル数、実行命令数、キャッシュミス、ストールサイクルといった、プロセッサレベルの非常に低レベルなイベントを計測するための重要なリソースである。

- 特徴
    - 計測対象: 何百種類ものイベントを計測できるが、同時に使用できる物理レジスタの数は限られている（例: 6個）。
    - 利用モード:
        1. カウンティングモード: イベントの発生回数を単純に集計する。カーネルがカウンタ値を読み取るだけであり、オーバーヘッドはほぼゼロである。
        2. オーバーフローサンプリングモード: イベントが設定した回数（例: 100万回）発生するごとにCPUに割り込みをかけ、その時点の追加情報（命令ポインタやスタックトレースなど）を収集する。BPFトレーシングでは、このサンプリングによって生成されるイベントを計測対象として利用する。

- PEBS (高精度イベントベースサンプリング)
    - 通常のオーバーフローサンプリングでは、割り込みの遅延（スキッド）により、イベントを発生させた正確な命令を特定できないことがある。
    - IntelのPEBSは、ハードウェアバッファを用いてこの問題を解決し、正確な命令ポインタを記録する。Linuxの`perf_events`フレームワークはPEBSをサポートしている。

- クラウド環境での利用
    - 多くのクラウド環境では、ゲストVMからホストのPMCへのアクセスが提供されていない。
    - しかし、技術的には可能であり、AWSのNitroハイパーバイザのように、多くのPMCをゲストに公開している例もある。

### 2.13 perf_events

- `perf_events`とは
    - `perf_events`は、Linuxカーネルに組み込まれたトレーシングおよびサンプリングのための基盤的な機能である。
    - `perf(1)`コマンドはこの機能を利用しており、長年にわたって開発が続けられてきた。

- BPFとの関係
    - BPFトレーシングツール（BCCやbpftrace）は、perf_eventsの機能を様々な形で活用している。
    - リングバッファ: 当初は、カーネルからユーザー空間へデータを効率的に転送するためのリングバッファとして利用された。
    - PMC計測: その後、PMC（パフォーマンスモニタリングカウンタ）の計測にも利用されるようになった。
    - 主要インターフェース:
      現在では、`perf_event_open()`システムコールを介して、kprobesやtracepointsなど、ほぼ全てのイベントソースの計測に利用される、BPFトレーシングにとって不可欠なインターフェースとなっている。

- `perf(1)`とBPF
    - `perf(1)`コマンド自体もBPFプログラムを実行する機能を持っており、BCCやbpftraceとは異なり、Linuxカーネルのソースツリー内で開発されている唯一のBPFフロントエンドトレーサーである。

### 2.14 Summary

以下の技術を組み合わせることで、BPFは強力なオブザーバビリティ（可観測性）ツールとして機能する。

- 多様なイベントソース: BPFは、動的インストルメンテーション（kprobes, uprobes）、静的トレーシング（tracepoints, USDT）、そしてperf_eventsを介したソフトウェア・ハードウェアイベントなど、多岐にわたるイベントソースを利用できる。
- スタックトレース: イベント発生時のコード実行パスを理解するために、スタックトレースは不可欠である。BPFは、カーネルスタックについてはフレームポインタ方式やORC（Oops Rewind Capability）を用いてスタックトレースを取得できる。
- フレームグラフ: 大量のスタックトレースデータを集約し、パフォーマンスのボトルネックを視覚的に特定するための強力な可視化手法である。
