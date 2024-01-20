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
