# daqmw-tc-server

DAQ-Middlewareトレーニングコースでボードからデータを読むことが
できない場合に使うソフトウェアエミュレータ。
データフォーマットは
https://github.com/h-sendai/daqmw-tc-network-2/blob/master/doc/data-format.pdf
にある。ASCIIで書いたものはdata.txtとしてこのディレクトリにおいてある。

## コンパイル方法

```
% git clone https://github.com/h-sendai/daqmw-tc-server.git
% cd daqmw-tc-server
% make
```

以上でdaqmw-tc-serverという実行ファイルが作られる。

## 実行方法

```
% ./daqmw-tc-server
```

でポート1234番で接続されるのを待つ。ポート番号を変えるには-pオプションを
使う:

```
% ./daqmw-tc-server -p 1235
```

## サーバーに接続してデータをとる

ncコマンドを使うと、新規プログラムを作ることなしにこのサーバーに接続し
データを取得することができる:

```
% nc localhost 1234 > data
```

数秒待ってControl-Cを押すとncコマンドが終了する。

自動で終了するようにするにはtimeoutコマンドを使えばよい。5秒後に終了
するようにする:

```
% timeout 5 nc localhost 1234 > data
```

