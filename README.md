# PicoScope_simpleMeasurement
Simple measurement program for "PicoScope" by Pico Technology  
Pico Technology社のUSBオシロスコープ「PicoScope」用計測プログラム

## 概要
4つのプローブの取得電圧をエクセルファイルに出力するプログラム。
出力先はデフォルトで「/output/out.csv」。


## 動作環境
Visual Studio 2017 
PicoScope 6402C
Windows 10

## 使い方
### 1. PicoScopeのSDKを取得
https://www.picotech.com/downloads
筆者の環境では次の場所にある。
`C:\Program Files\Pico Technology\SDK`

### 2. Visual Studio 2017 の設定を行う

- 「PicoScope_simpleMeasurement -> プロパティ -> C/C++ -> 全般 -> 追加のインクルードディレクトリ」
`$(ProgramW6432)\Pico Technology\SDK\inc`

- 「PicoScope_simpleMeasurement -> プロパティ -> リンカー -> 全般 -> 追加のライブラリディレクトリ」
`$(ProgramW6432)\Pico Technology\SDK\lib`

- 「PicoScope_simpleMeasurement -> プロパティ -> リンカー -> 入力 -> 追加の依存ファイル」
`ps6000.lib`

### 3. USBオシロスコープを接続し、プログラムを実行する
コマンドウィンドウとともに「PicoScope」のウィンドウが出現したら成功。

