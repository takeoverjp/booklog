# links

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
