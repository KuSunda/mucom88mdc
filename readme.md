MUCOM88md for SGDK(68000/C言語)
------------------
## 概要
MUCOM88mdはNEC PC-8801mkⅡSR以降版のMUCOM88をメガドライブ/Genesisに移植して、
MUCOM88でコンパイルした.mub形式のデータを演奏するサウンドドライバーです。  
SGDKのC言語で実装することでPCMに対応しています。

## ライセンスについて
MUCOM88mdはMUCOM88から移植・派生した音源ドライバーとそれの制御プログラムです。  
ライセンスはオリジナルのMUCOM88(License: CC BY-NC-SA 4.0)に準拠します。  
詳細は本家のサイトをご確認ください。

[OPEN MUCOM PROJECT (株式会社エインシャント様)](https://www.ancient.co.jp/~mucom88/)  
 MUCOM88公式サイト。オリジナルのMUCOM88はこちらのサイトで配布されています。

[OPEN MUCOM88 Wiki (ONION software/おにたま様)](https://github.com/onitama/MUCOM88/wiki)  
 OPEN MUCOM88 公式Wikiサイト。MUCOM88の各種情報はこちらのサイトをご参照下さい。

### オリジナルのMUCOM88から流用しているコード
 - オリジナルのドライバのコード  
 [OPEN MUCOM88プロジェクトについて]のmucom88 Ver.1.7をベースにしています。
 <https://github.com/onitama/mucom88/tree/master/pc8801src/ver1.2/music.asm>
 - サンプルのデータについて  
 このリポジトリには、[mucom88win]の[sampl0.muc][sampl1.muc][mucompcm.bin][voice.dat]が含まれています。  
 MegaDrive向けのPCMデータは[mucompcm.bin]から返還したデータデータです。

## MUCOM88mdについて  
 メガドライブ開発環境はSGDK(Ver.1.62)で作成されています。  
 https://github.com/Stephane-D/SGDK

### MUCOM88mdの仕様
  - BGM : FM 5Ch / DCSG 3Ch / PCM 1Ch
  - SE : DCSG 3Ch / PCM 2Ch(1ChはBGMと併用)
  - DCSG NoiseはF Chのみ演奏可能
  - .mubファイルは、1ファイルにつき1曲の仕様
  - DCSG3チャンネルにSEを割り込んで演奏
  - SEは譜面とアドレステーブル含めて最大0x8000
  - PCMファイルは1音あたり最大0x8000(変更するかも)
  - VSyncのタイミングでの駆動(Timer-Bの値は参照しない)
  - ループ […]n コマンドのネストは6回までに制限（拡張可能）
  - PCMは、音程・音量の調整が出来ず、それに絡むパラメータは処理されない。
  - PCMは5KHz予定（2度ほどずれている？）
  - VSync駆動:Timer-Bの値は参照していません。曲のテンポはクロックと音符の長さで調整となる。
  - G(RHYTHM)チャンネルは非対応。データが有ってもスキップされます。  
  - SSGの低音部分がDCSGの仕様で[o2 a]未満の音は発声出来ず異音が発生します。

## リリースノート
  - Ver.0.5  
  基本的な機能を実装したが、コードが未整理・最適化が十分ではありません。

## 使い方  
 - SGDK 1.62の環境を構築  
 Documentを参考に環境変数の設定とbinフォルダへのPATHを設定
 - mucom88  
 mucom88のコマンドラインコンパイラはPATHの通った場所に配置
 - BGM/SEをコンバート  
 SE/BGM/PCMが1つ以上ないとコンバートで失敗します。
 - データのコンバートとビルド
 muc88Cnv.bat　を実行すると、BGM/SEの.mucをコンパイル、MegaDrive形式に変換、SGDKでMegaDriveでの実行ファイルをビルドします。
 変換される曲・PCM波形・SEは、後述するファイルの説明を参考にしてください。

## TestModeについて
 - 操作  
   - UP/DOWN:　演奏機能の選択.mub/SE/PCM1ch/PSC2ch
   - LEFT/RIGHT: 演奏曲/SE/PCM番号の選択
   - A: 演奏開始(.mub/se/pcm1/pcm2)
   - B: 演奏停止(.mub)
   - C: フェードアウト(.mub)
   - X: ＦＭ音源のチャンネルごとのステータスの更新を停止・再開(負荷計測の為に文字更新を抑制)
 - 表示内容
   - YM: 最近20フレームの最高1フレーム内のFM音源アクセス数
   - CP: 最近20フレームの最高CPU負荷（100で処理落ちが発生）
   - CA: 最近8フレームのCPU負荷平均値

## 変換の概要
 - BGM変換  
   - Mucom88で.mubファイルにコンパイル  
   - テキストファイルに、MDで使う曲(.mub)をリストアップ  
   - テキストファイルに、MDで使うPCM(.wav)をリストアップ  
   - [Mucom88mdcCnv bgmlist.txt wavlist.txt]を実行してMD向けのソースコードを出力  
    - .mubファイルからPCMを削除、BGMリストの順番に結合して、SGDK向けソースコードを出力
  - BGMデータについて
    - 全ての.mucは同じPCMデータを使用すること
    - 繰り返しコマンドのネストは6回以下    
    - Rhythmチャンネルは無視される
    - ドライバはVSync駆動でTimer-Bは参照されない  
 - PCM​データについて  
   - 5Khz/8Bit/モノラルの.wav形式に対応 （細かい調整が有るかも）
   - Wavファイルリストは、.mucで使っているPCMデータと同じ並びで記述  
 - SEの変換
   - Mucom88で.mubファイルにコンパイル   
   - テキストファイルに、MDで使うSE(.mub)をリストアップ  
   - [Mucom88mdcCnvSe selist.txt]を実行するとMD向けのソースコードが出力される  
 - SEデータについて
   - .mucに、D/E/Fチャンネルのみ記載。3チャンネル全て記載する必要はない。
   - 音色は必ず設定が必要

 - 出力されるファイル名
   - 自動生成ファイルの名前は [\_mu88md\_] から始まります。データコンバート時は自動生成ファイルを削除をお勧めします。
   - Gitで管理する必要は有りませんが、サンプルとして管理しています。

## Batファイルの説明
 - clean_mu88md.bat  
 自動作成のファイルを削除します
 - .\mucom\mub\muc2mu88md.bat  
 .mucをコンパイル、mublist.txt wavlist.txtの.mubと.wavをMegadrive向けに変換、srcフォルダに移動
 - .\mucom\se\se2mu88md.bat  
 .mucをコンパイル、se_list.txtの.mubを、Megadrive向けに変換、srcフォルダに移動
 - muc88Cnv.bat  
 ファイルの削除・データのコンバート・ビルドを一括して実行します。


## ファイルの説明  
ファイル一覧  
+  
|　　makefile.gen　　　　　　　　makefile  
|　　clean_mu88md.bat　　　　　自動生成ファイルの削除  
|　　muc88Cnv.bat　　　　　　　自動生成ファイルの削除、BGM/SEの再コンバート、ビルドを行うバッチファイル  
|　　readme.md　　　　　　　　　使い方  
+---out
|　  rom.bin                  MegaDrive(実機・エミュレーター)向け、ROMファイル
|     
+---mucom  
|　|　mucom88mdcCnv.exe　　　　BGMの.mubとPCMファイルをMegaDrive形式に変換  
|　|　mucom88mdcCnvSe.exe　　　SEの.mubファイルをMegaDrive形式に変換  
|　|     
|　+---mub  
|　|　|　muc2muc88md.bat　　　　mubフォルダ内の.mucをmucom88でコンバート、mucom88mdcCnvSe.exeでMegaDrive形式に変換、生成したファイルをsrcフォルダに移動するバッチファイル  
|　|　|　mublist.txt　　　　　　MegaDriveに変換する.mubファイルリスト  
|　|　|　wavlist.txt　　　　　　MegaDriveに変換する.wavファイルリスト  
|　|　|　sampl0.muc  
|　|　|　sampl2.muc  
|　|　|　mucompcm.bin  
|　|　|　voice.dat          
|　|　|     
|　|　\---wav　　　　　　　　　　MegaDrive形式に変換されるWavファイル  
|　|　　　01_muccom88.wav  
|　|　　　02_muccom88.wav  
|　|　　　03_muccom88.wav  
|　|　　　04_muccom88.wav  
|　|　　　05_muccom88.wav  
|　|　　　06_muccom88.wav  
|　|　　　07_muccom88.wav  
|　|　　　08_muccom88.wav  
|　|　　　09_muccom88.wav  
|　|　　　10_muccom88.wav  
|　|　　　11_muccom88.wav  
|　|　　　12_muccom88.wav  
|　|　　　13_muccom88.wav  
|　|　　　14_muccom88.wav  
|　|　　　15_muccom88.wav  
|　|　　　16_muccom88.wav  
|　|　　　17_muccom88.wav  
|　|　　　  
|　\---se  
|　　　se2muc88md.bat　　　seフォルダ内の.mucをmucom88でコンバート、mucom88mdcCnvSe.exeでMegaDrive形式に変換、生成したファイルをsrcフォルダに移動  
|　　　se_list.txt　　　　　MegaDrive形式に変換する.mubファイルのリスト  
|　　　N00_OK.muc          SEのMML
|　　　N00_OK.muc
|　　　N01_CANCEL.muc
|　　　N02_CANT_SELECT.muc
|　　　N03_CURSOR_MOVE.muc
|　　　N04_PAUSE.muc
|　　　N05_TALK.muc
|　　　N10_COIN.muc
|　　　N11_SHOT.muc
|　　　N12_BOM.muc
|　　　N13_NOISE0000.muc
|　　　N14_POWA.muc
|             
\---src  
　　|　s_main.c　　　　　　　　　初期化とメインループ  
　　|　s_main.h  
　　|　mu88md_asub.h　　　　　　68kアセンブラサブルーチン  
　　|　mu88md_asub.s　　　　　　68kアセンブラサブルーチン  
　　|　mu88md_drv.c　　　　　　　Mucom88mdドライバ本体  
　　|　mu88md_drv.h　　　　　　　Mucom88mdドライバ本体  
　　|　mu88md_drv01.c　　　　　　Mucom88mdドライバサブルーチン  
　　|　mu88md_pcm_drv.h　　　　　Mucom88mdドライバサブルーチン  
　　|　mu88md_pcm_drv.s80　　　　Z80 PCMドライバ  
　　|　mu88md_test.c　　　　　　テストモード  
　　|　mu88md_test.h  
　　|　_mu88md_mub.c　　　　　　mucom88mdcCnv.exe自動生成ファイル  
　　|　_mu88md_mub.h　　　　　　mucom88mdcCnv.exe自動生成ファイル  
　　|　_mu88md_mub00.s　　　　　mucom88mdcCnv.exe自動生成ファイル  
　　|　_mu88md_pcm.c　　　　　　mucom88mdcCnv.exe自動生成ファイル  
　　|　_mu88md_pcm.h　　　　　　mucom88mdcCnv.exe自動生成ファイル  
　　|　_mu88md_pcm00.s　　　　　mucom88mdcCnv.exe自動生成ファイル  
　　|　_mu88md_se.c　　　　　　　mucom88mdcCnvSe.exe自動生成ファイル  
　　|　_mu88md_se.h　　　　　　　mucom88mdcCnvSe.exe自動生成ファイル  
　　|　_mu88md_se00.s　　　　　　mucom88mdcCnvSe.exe自動生成ファイル  
　　|     
　　\---boot  
　　　　rom_head.c  
　　　　sega.s  

## 謝辞  
- 古代祐三　様　
- 株式会社エインシャント 様
- ONION software/onitama　様
- SGDK(Stephane-D)　様
- MUCOM88/FM音源/メガドライブ関係の情報を発信しているみなさま
