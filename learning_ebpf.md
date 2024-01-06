# links

- [入門eBPF](https://www.oreilly.co.jp/books/9784814400560/)
- [入門eBPFのソースコード](https://github.com/lizrice/learning-ebpf.git)
- libbpf
  - [BCCベースのeBPFコードをlibbpfに移植する方法](https://nakryiko.com/posts/bcc-to-libbpf-howto-guide/#bpf-code-conversion)
  - [libbpf-bootstrap](https://github.com/libbpf/libbpf-bootstrap)
  - [libbpfのBCC版をlibbpf版に書き換えたもの](https://github.com/iovisor/bcc/tree/master/libbpf-tools)


# 1. eBPFとは何か？なぜ、重要なのか？

- eBPFの主な用途
  - システムの性能トレース
  - 可観測性を持ったネットワーク機能
  - セキュリティ検知と不正操作防止の仕組み
  - カーネル内のほとんどの命令に対して追加の命令を実行させる仕組みであるkprobeに対してeBPFプログラムをアタッチできるようになったことが、システム全体をまたがって計測を行う方法の革命のきっかけとなった
    - kprobeを使ったことがある人にとっては、eBPFは新しいカーネルトレースの仕組みというより、kprobeを安全に簡単に使えるようにした仕組みだと捉えた方が頭に入りやすい

- eBPFの名前
  - eBPFはもともとはextended BSD Packet Filterの略だが、守備範囲はパケットフィルタの範囲をとうに超えている
  - また、`bpf()`システムコールが使われているなど、eBPFとBPFを区別していないことも多い

- なぜ生まれたのか
  - 例えばシステムコールをフックして何らかの処理をしたい場合
  - カーネル自体にその機能を追加するためには、パッチの投稿・レビュー・承認、そのkernelのディストリビューションへの採用など、数年単位で時間がかかる
  - さらに、ユーザはその機能を有効にするためにシステムの再起動が必須となる
  - eBPFの場合、カーネルのリリースプロセスとは独立に、開発できる。さらに、動的ロードすればシステムの再起動も不要である。
  - カーネルモジュール（`insmod`などでロードする`.ko`ファイル）を使う場合、カーネルモジュールはカーネルとして動作し何でもできてしまうがゆえに、そのカーネルモジュールが安全であるか（クラッシュしない・脆弱性がない）を証明しなければならない
  - eBPFは、ロードするeBPFプログラムを検証器でチェックし、クラッシュしない・無限ループしない・不正なデータにアクセスしないことを保証する
  - eBPFはkernel空間で機械語として実行されるため、余分なデータをユーザ空間にコピーする必要がなくなり、性能上有利になるケースがある
  - サイドカーモデルでサービスメッシュのようなネットワークの機能を追加する場合、ネットワークスタックを１回多く通らなければならないが、eBPFの場合、従来どおりネットワークスタックを一度とおる構成のまま実現できる
    - サービスメッシュとは、マイクロサービスアーキテクチャの中で複数のマイクロサービス間の通信をインフラ側で制御するための技術のこと

# 2. eBPFの「Hello World」

- BCC Pythonフレームワークを使った簡単なeBPFプログラムの実例紹介
  - https://github.com/lizrice/learning-ebpf.git
  - なお、Ubuntu focal(20.04)は、BCCのapt packageが公開されていないので、ソースからビルドすること
    - https://github.com/iovisor/bcc/issues/3108
    - https://github.com/iovisor/bcc/blob/master/INSTALL.md

```python
#!/usr/bin/python
from bcc import BPF

program = r"""
int hello(void *ctx) {
    bpf_trace_printk("Hello World!");
    return 0;
}
"""
b = BPF(text=program)
syscall = b.get_syscall_fnname("execve")
b.attach_kprobe(event=syscall, fn_name="hello")

b.trace_print()
```

- `BPF(text=program)`と指定することで、eBPFプログラムのコンパイルをBCCフレームワークに任せることができる
  - 自分でコンパイルする方法もある

- `bpf_trace_printk`に渡した文字列は、`/sys/kernel/debug/tracing/trace_pipe`から読み取れる
  - 簡単にカーネル空間BPFプログラムからユーザ空間プログラムにデータを渡すことができる
  - 文字列しか渡せない
  - システムに一つしかない

- BPF Map
  - ユーザ空間とカーネル空間のデータのやり取りや、複数のeBPFプログラム間でデータを共有するときに使うことができる
  - いろいろな種類のBPF Mapが[uapi/linux/ebpf.h](https://elixir.bootlin.com/linux/v5.15.86/source/include/uapi/linux/bpf.h#L878)に定義されている
  - ハッシュマップ、配列、リングバッファなど
  - データが格納されたことをユーザ空間側でわかるようにするためには、PerfリングバッファやBPFリングバッファを使えば良い

- `bpf`を呼び出すために必要なcapability
  - まず`CAP_BPF` (v5.8以降)
  - トレースの場合、`CAP_PERFMON`
  - ネットワーク制御の場合、`CAP_NET_ADMIN`
  - 参考：[Introduction to CAP_BPF](https://mdaverde.com/posts/cap-bpf/)

- 関数呼び出し
  - 当初のeBPFは関数呼び出しをサポートしておらず、インライン関数呼び出ししかできなかった
  - 今のeBPFにはその制限は撤廃されているが、BCCにはその制約が残っている
  - 呼び出し元に戻らないため従来の関数とは異なるが、Tail Callを使うことでも、処理の細分化はできる

# 3. eBPFプログラムの仕組み

- C/Rustで描いたeBPFプログラムを、事前にeBPFバイトコードにコンパイルし、実行時にeBPF仮想マシンにて機械語にJITコンパイルもしくはinterpretされる
- eBPFバイトコード自体はCPUアーキテクチャに依存しない
  - しかし、kprobeで使うpt_regs構造体がCPUアーキテクチャに依存するため、結局CPUアーキテクチャごとにコンパイルは必要
- clangのtargetをbpfとすることでeBPFバイトコードを出力することができる
- `bpftool`コマンドを使うことで、eBPFプログラムをロードし、フック対象のイベントにアタッチすることができる
- `ip`コマンドでもbpfプログラムのアタッチ・デタッチができる

# 4. bpf()システムコール

- eBPプログラムは、`bpf`システムコールでプログラムのロード、Mapの作成・アクセスなどを行う
- `bpf`システムコールは`ioctl`のようにコマンドとattributeを渡す汎用的なインターフェースになっている
  - `BPF_BTF_LOAD`
    - Kernelのバージョン差分を吸収するため、BTF(BPF Type Format)をロードする
  - `BPF_MAP_CREATE`
    - ユーザ空間とカーネル空間の間でデータのやり取りをするためのMapを作成する
  - `BPF_PROG_LOAD`
    - eBPFプログラムをカーネルにロードする
  - `BPF_PROG_BIND_MAP`
    - mapはプログラムのfdに紐づく形で参照カウントを管理しており、参照するfdがなくなったら解放される
    - 明示的に参照カウントを増やし、ピン留めする場合に、`PERF_EVENT_IOC_SET_BPF`を使う
- `perf_event_open`でkprobeタイプのperfイベントを取得する
- `ioctl`の`PERF_EVENT_IOC_SET_BPF`を実行することで、BPFプログラムをkprobeイベントにアタッチする

# 5. CO-RE, BTF, libbpf

- CO-RE (Compile Once, Run Everywhere)とは、一度コンパイルすればどこでも実行可能というアプローチのこと
  - カーネルのデータ構造の情報をBTFとしてプログラムに含め、実行環境と異なる場合調整する
  - **１つのアーキテクチャについて** 一度コンパイルすればどこでも実行できる
    - kprobeイベントにアタッチする場合、cpuアーキ依存のpt_regs構造体に依存するため、それぞれのCPUアーキごとにコンパイルが必要

- BCCの問題点
  - 実行環境にコンパイルツールチェーンとカーネルヘッダが必要
  - 実行時にコンパイルするので実行開始に不要に時間がかかる
  - BCCベースのプロジェクトがツールチェーンをバンドルした場合、ツールのかづだけツールチェーンをインストールすることになる
  - 組み込みの場合、コンパイルに十分なメモリリソースを持っていない可能性がある

- CO-REの構成要素
  - [BTF (BPF Type Format)](https://docs.kernel.org/bpf/btf.html)
    - データ構造や関数シグネチャのレイアウト情報。Linux Kernel v5.4以降
  - カーネルヘッダ
    - bpftoolを使って、実行中のシステムからBPFプログラムが必要とするカーネルの全てのデータ構造を含むvmlinux.hを生成することも可能
  - コンパイラ
    - clang/gccともに、CO-REによる再配置情報をバイナリに含めることができる
  - ユーザ空間ライブラリ
    - libbpfを使うことで、eBPFプログラムをロードするときに、バイトコードに含まれるデータ構造を再配置することができる
    - Goの場合Cilium eBPFを、Rustの場合Ayaを使う
  - BPFスケルトン
    - ユーザ空間のコードのテンプレートをコンパイル済みのBPFオブジェクトから(!)生成できる

- BTF
  - `bpftool btf list`で、カーネルにロードされているBTF情報を見ることができる
  - `bpftool btf dump id <id>`で、特定のBTFの詳細情報を見ることができる
  - `bpftool btf dump file /sys/kernel/btf/vmlinux format c > vmlinux.h`で、すべての定義を含むヘッダを生成できる
    - v5.4から利用可能
    - ただし、使えない環境でも自分で必要なBTFファイルを生成しておけば実行できる可能性がある

- CO-RE eBPFプログラム (kernel側)
  - カーネルヘッダは、生成した`vmlinux.h`か、ここのヘッダファイルを指定するか、自分で定義するか
  - libbpfリポジトリをサブモジュールとして含めることが、書籍執筆時点では一般的
  - CO-REを用いたメモリアクセスには、`bpf_probe_read_*()`を使わなければならない
    - これにより、BTF情報を使ってレイアウトの差分を吸収できる
    - `BPF_CORE_READ`を使うことで簡単にかけるようになる
  - コンパイルしたELFファイルには、BTF専用のセクションが追加される

- ユーザ空間libbpfライブラリ
  - コンパイル時にclangが生成するBPF CO-RE再配置情報を使って、eBPFプログラムの内容を実行時に変更し、ターケットマシン上のカーネルで動作できるようにする

# 6. eBPF検証器

- 検証器の対象は、ソースコードではなくeBPFバイトコード
  - 例えばソースコードに到達不可能な命令があっても、コンパイラの最適化によって消されている場合、検証の失敗は発生しない

- 検証機は、レジスタの状態の履歴を保存しつつ、eBPFバイトコードの命令を１つずつ評価していく
  - 分岐を深さ優先で探索していき、プログラムの終了に到達するか、100万命令に到達したら、検証を終了する
  - 不正な操作につながる命令を発見したら検証を停止する
  - 全ての組み合わせを検証するとコストが大きすぎるので、state pruning (状態の刈り取り)を行う

- 検証が失敗したときには、その理由の詳細を`libbpf_set_print`で取得できる

- 検証器が検証した全ての経路をDOT形式で出力することもできる
  - `bpftool prog dump xlated name kprobe_exec visual > out.dot`

- 検証器が検出する例
  - 不適切なBPFプログラムタイプからヘルパ関数を呼び出した
    - 例えば、パケット処理のタイプで、プロセスIDを取得するヘルパ関数`bpf_get_current_pid_tgid`を呼び出した場合、プロセスに紐付かないため不正となる
  - ヘルパ関数に不正な型の引数を渡した
  - ヘルパ関数を呼び出すために必要なライセンスになっていない
  - 許可されていないメモリにアクセスした
  - 許可されていないコンテキスト情報にアクセスした
  - nullになりうるポインタのnullチェックをしていない
  - 100万命令で完了しないフローがある
  - 関数の中で戻り値（レジスタ０）が初期化されていない
  - 不正な命令コードがある
    - コンパイラがバグっていて、不正な命令コードを出力した
    - 自分で直接書いたBPFバイトコードが誤っていた
    - 新しい命令を古いカーネルで実行しようとした場合
  - 到達不可能な命令がある（通常はコンパイラが最適化で削除する）

# 7. eBPFのプログラムとアタッチメントタイプ

- eBPFプログラムのタイプにより決まるもの
  - アタッチできるイベント種別
  - eBPFプログラムが受け取るコンテキスト情報の型
  - eBPFプログラムから使えるカーネル関数群

- eBPFプログラムから使えるカーネル関数の種別
  - ヘルパ関数
    - カーネル外部向けのインタフェースであり、互換性が保証される
  - BPFカーネル関数（Kfuncs）
    - 互換性が保証されない（カーネルバージョンが変わると動かない可能性がある）
    - [「core」BPF kfuncs](https://docs.kernel.org/bpf/kfuncs.html#core-kfuncs)

- eBPFのプログラムタイプ
  - トレーシングプログラムタイプ（Perfプログラムタイプ）
    - イベントの情報をトレースして、効率よくユーザ空間に報告する
    - kprobe / kretprobe
      - カーネル関数の開始と終了をトレースする
      - オフセットを指定することもできる
      - カーネル関数がインライン化された場合はアタッチできない
    - fentry / fexit
      - kprobe / kretprobeより効果的にカーネル関数の開始と終了をトレースする
      - fexitフックで呼び出された時点の引数の情報も取得できる(kretprobeでは取得できない)
      - x86-64ではv5.5でサポート
      - arm64はv6.0時点で未サポート
    - [Tracepoint](https://www.kernel.org/doc/html/v6.0/trace/events.html)
      - カーネルコード内でトレース用にマークされたポイント
      - eBPF固有ではなく、SystemTapなどでも使われてきた
      - `/sys/kernel/tracing/available_events`で利用可能なtracepointが確認できる
      - BTFが使えない環境
        - `/sys/kernel/tracing/events/トレースするサブシステム名/tracepointの名前/format`からコンテキスト引数の型を自分で定義する
        - カーネルのバージョンが変わり、型定義が変わると動作しなくなる
      - BTFが使える環境での Tracepoint
        - `vmlinux.h`に定義されている`struct trace_event_raw_(tracepointの名前)`を使う
    - uprobe / uretprobe
      - ユーザ空間の関数と解と終了をトレースする
      - トレース対象の関数シンボルを定義している共有ライブラルのフルパスが必要など、制約がある
      - 使用例
        - [`bashreadline`](https://github.com/iovisor/bcc/blob/master/libbpf-tools/bashreadline.bpf.c)
        - [funclatency tools](https://github.com/iovisor/bcc/blob/master/libbpf-tools/funclatency.c)
        - [BCCのUSDTのサンプル](https://github.com/iovisor/bcc/blob/master/examples/usdt_sample/usdt_sample.md)
  - ネットワーク関連プログラムタイプ

# 8. ネットワーク用eBPF

# 9. セキュリティ用eBPF

# 10. プログラミングeBPF

# 11. eBPFの将来の進化
