# 3DMazeGame
一点透視図法を用いた疑似立体迷路ゲームです。  
このプログラムはX Window Systemを用いて作成しています。
  
## 使い方
1. 3dmaze.cをコンパイルし，a.outを実行します。  
2. ウィンドウが開きレベル選択画面が表示されます。  
3. ウィンドウ上の任意のレベルをクリックするとレベルが設定され、ゲームが開始します。
4. 画面左側にプレイヤーの視点、右側にマップ等の情報が表示されます。
5. プレイヤーを操作し、青色で表されるゴールを見つけましょう。
6. ゴールにたどり着くとクリア画面に移動し、ゲームクリアとなります。  

## 操作方法
<dl>
  <dt>十字キー</dt>
  <dd>左右でプレイヤーの方向転換・上下で描画距離の変更</dd>
  <dt>スペースキー</dt>
  <dd>プレイヤーを前進させる</dd>
  <dt>Rキー</dt>
  <dd>スタート地点からやり直し</dd>
  <dt>Qキー</dt>
  <dd>レベル選択画面に戻る</dd>
  <dt>Mキー</dt>
  <dd>マップの表示/非表示の切り替え</dd>
  <dt>Escキー</dt>
  <dd>ゲームの終了(プログラムの終了)</dd>
</dl> 

## 実行環境 (PC)
- AMD Ryzen7 3700X 8-Core Processor 3.60GHz
- Memory 16.0GB
- OS Windows 10 20H2  

## 実行環境 (仮想マシン)
* VirtualBox 6.1.32 r149290 (Qt5.6.2)
- AMD Ryzen7 3700X 8-Core Processor 3.60GHz
- Memory 3.8GiB
- gcc version 9.3.0
- OS Ubuntu 20.04.3 LTS
  
## 注意事項
ソースファイルのコンパイルにはlibx11-devパッケージが必要です(ubuntu環境では`sudo apt-get install libx11-dev`でインストールできます)。  
また、X11を用いているのでコンパイルにはオプションが必要です。

コンパイルはこちらのコマンドで行ってください。  
`cc 3dmaze.c Flib/FFont.c Flib/FillMask.c -I /usr/include/ -L /usr/lib/ -l X11`

