# links

- [systemdの思想と機能―Linuxを支えるシステム管理のためのソフトウェアスイート](https://gihyo.jp/book/2024/978-4-297-13893-6)

# 1. systemdとは

- systemdが幅広い領域を扱っている理由は、「現代の Linux 環境 でシステムやサービスを起動/終了/管理するPID 1が扱えるべきものは何か?」が多岐に渡るから

# 2. unitとunit file

- unitにはsystemとuserがある
  - system: pid 1のsystemdが管理する
  - user: ユーザのログインからログアウトまで動作するサービスを管理する
 
- systemdで利用できるディレクティブは、拡張不可
  - ディストリビューションごとの方言発生を予防するため

- top-level drop-in
  - 特定のタイプの全てのunitの振る舞いを変更できる

- systemd-analyze verifyでunitファイルを検証できる

# 3. unitの状態、unit間の依存関係、target unit

- unitの状態
  - 基本はinactiveとactive
  - 起動に失敗したらfailed
<img width="400" alt="image" src="https://github.com/takeoverjp/booklog/assets/3068739/73858382-cab9-4c56-a822-0f3c1d0f16bc">

- unitの操作では基本的なトランザクションをサポートしていて、矛盾している場合は全ての操作を実行しない
- unitの操作のときには、まずlunitの状態を確認して不要な操作を省略する
- man bootup(7)に起動・終了に関する重要なtargetの関係が描かれている

- systemctl log-levelで一時的にログレベルを変更することができる
- systemd-analyze cat-configで設定ファイルの内容を確認できる

# 4. プロセス実行環境の用意

- sysytemdは`fork()`して子プロセスに対してcgroup等環境設定をしてから、execする
  - 上記cgroupを使って、孫プロセスなども追跡できる

- `systemd-run(1)`コマンドを使うことで、対話的にserviceタイプのunitと環境を作成することができる
  - `systemd-run -p EnvironmentFile=/etc/environment env`とすれば、環境変数ファイルを読み込んだ上でenvコマンドを起動することができる
  - この場合、あくまでもsytemdの子プロセスとして起動されるので、`systemd-run`を実行したシェルの環境変数は引き継がれない

- sandboxingによるセキュリティ強化もできる
  - 例
    - ネットワークへのアクセス禁止
    - 利用できるシステムコールの制限
    - 実行可能で書き込み可能なメモリの割当を禁止

# 5. service unit

- oneshotの場合、デフォルトではactivatingからactiveにならずにinactiveに遷移する
- idleの場合、他の全てのjobより後にjobを開始する
- ExecStartPost実行中はactivating状態
- simpleの場合、ExecStartPostはExecStartよりも前に実行される可能性がある

- 時間のかかる初期化処理を待つ場合、初期化用と起動用のunitに分ける方法もある
- 全部の起動が完了した後に特定サービスを起動する簡単な方法はない

# 6. timer/path/socket unit

- `OnBootSec`と`OnUnitInactiveSec`を組み合わせることで、繰り返し実行できる
- path unitを使えば、ファイルの生成などをトリガにserviceをactiveにすることができる
- socket unitを使えば、socketへの接続をトリガにserviceをactiveにすることができる
  - その際、systemdではacceptせずservice側でacceptすることも、systemdでacceptしてからserviceにfdを渡すこともできる

# 7. generatorとmount/automount/swap unit

- mount typeのunitは、fstabからgeneratorによって生成されたunitと、静的に置かれた.mountファイルを混在させることができる
  - fstabのmountオプションを指定するフィールドに、`x-systemd.automount`などを指定することで、generatorが生成するunit fileの設定を行うことができる
- automount typeのunitは、マウントポイントへのアクセスを契機として自動的にmountする
- fstabでsystemd固有のoptionを指定している場合、`mount`コマンドが使えない。代わりに`systemd-mount`コマンドを使う。

# 8. control group, slice unit, scope unit

- systemdはserviceごとにcgroupを作るため、serviceが起動したプロセスも追跡してまとめてsignalを投げることができる
- slice unitはリソース管理のグルーピングに使われる
- scope unitはsystemd以外のプロセスがforkしたプロセスをunitとして扱いたいときに使う
- systemd-cgtopでグループごとのリソース消費を確認することができる

# 9. udev, device unit

- udevはsystemdよりも前からあるプロジェクトだが、今はsysemdに統合されている
- 物理デバイスから取得できない情報を得るために、hwdbという仕組みが用意されている

# 10. systemd-journald

- systemd-journaldは、journalだけでなくkmsg, syslog, console, wallに転送することもできる
- journalにはboot Id (起動ごとのUUID)も記録する
- journalは「最低何日分のログを残す」という設定ができないため、長期保存には向かない
- journalはunitごとにログのレート制限をかけることができる
- loggerコマンドに--journaldオプションをつけることでjournalにログを残すことができる

# 11. core dump管理

- gdb付属のgcoreコマンドで実行中プロセスのcore dumpを生成することができる
- `systemd-coredump`により、`/proc/<PID>/status`やopenしているファイルの情報、バックトレースなど、かなり細かい情報をjournalに保存してくれる
- `coredumpctl`コマンドで`systemd-coredump`が保存したcoreの情報を確認することができる

# 12. systemd-logind, pam_systemd

- logindはセッションの管理と、sleep/shutdownと言った電源制御の要求を行う
  - セッションとは、ユーザがログインしてからログアウトするまでの期間を表す
- pam_systemdは、PAMによるアカウント管理、認証、パスワード管理、セッション管理を行う
  - PAM: Pluggable Authentication Modules
- `loginctl`でセッションの状態を確認することができる
- Inhibitor Locksを使うことで、sleep/shutdownを待たせることができる
  - `systemd-inhibit`コマンドで利用できる

# 13. systemd-tmpfiles, systemd-sysusers

- `systemd-tmpfiles`を使うことで、システム起動時にtmpfsのファイルの作成・削除・属性設定を行うことができる
- `systemd-sysusers`を使うことで、システム起動時に必要なユーザやグループの作成をすることができる

# 14. D-Busとpolkit

- D-Bus は、「メッセージ」を「オブジェクト」に送るしくみ
- メッセージの種類は、METHOD_ CALL、METHOD_RETURN、ERROR、SIGNAL の 4 種類
- D-Bus Activationを使うことで、dbusにメッセージを送って起動できるserviceを作ることができる
- polkitを使うことで特定のプロセスの特定のアクションに対して、認証の要否を設定することができる
- Cockpitというwebベースのシステム管理UIは、内部でD-Bus APIを使っている人

# 15. systemd-resolved

- 'resolv'は従来の仕組みで、'resolve'はsystemd-resolvedの表記
- glibcはNSS(Name Service Switch)という仕組みで名前解決関数を提供していた
  - hostsファイルとDNSという２つの仕組みを切り替えることができる
- systemd-resolvedでは、リンクごとに名前解決を設定することができる

# 16. systemdのその他の機能

- systemd-networkdはNetworkManageと比較して依存が少ないので、initramfsに入れてネットワーク起動するといった使い方に向く
- networkctrlコマンドでsystemd-networkdを制御できる
- systemd-credsでパスワード等のクレデンシャルの暗復号ができる
- systemd-oomdはユーザ空間で実装されたOOM
  - [スワップの弁護:よくある誤解を解く](https://chrisdown.name/ja/2018/01/02/in-defence-of-swap.html)

