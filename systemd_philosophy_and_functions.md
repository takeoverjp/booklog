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

- 

# 8. control group, slice unit, scope unit

- 

# 9. udev, device unit

- 

# 10. systemd-journald

- 

# 11. core dump管理

- 

# 12. systemd-logind, pam_systemd

- 

# 13. systemd-tmpfiles, systemd-sysusers

- 

# 14. D-Busとpolkit

- 

# 15. systemd-resolved

- 

# 16. systemdのその他の機能

- 

