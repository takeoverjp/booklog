# Binary Hacks Rebooted

## Links

- [Binary Hacks Rebooted](https://www.oreilly.co.jp/books/9784814400850/)

## 1. イントロダクション

### #1 未知のバイナリの読み方

- `file`, `hexdump -C`
  - `hexdump`で見ることで、制御文字も確認できる。マッチするはずの正規表現がマッチしない時は見てみると良いかも？
- `__attribute__((packed))`を付与することで、構造体をパディングせずに配置することを指示できる。デバイスドライバ等でC言語のアラインメントのルールに従えない場合に利用する。
- ELFでは4KiBでアラインメントをしている。これはページサイズに合わせることで、複数プロセス間のコードページ等の共有を用意にするため。

### #2 アセンブリ入門

- `objdump --disassembile=XXX`とすることで、`XXX`関数のみをディスアセンブルすることができる

## 2. ELF Hack

- 

## 3. OS Hack

- 

## 4. コンテナHack

- 

## 5. デバッガ・トレーサHack

- 

## 6. セキュリティHack

- 

## 7. 数値表現とデータ処理Hack

- 

## 8. 言語処理系Hack

- 

## 9. そのほかのHack

- 
