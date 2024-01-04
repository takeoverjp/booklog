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

# 7. eBPFのプログラムとアタッチメントタイプ

# 8. ネットワーク用eBPF

# 9. セキュリティ用eBPF

# 10. プログラミングeBPF

# 11. eBPFの将来の進化
