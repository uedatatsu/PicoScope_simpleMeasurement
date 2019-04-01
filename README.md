# PicoScope_simpleMeasurement
Simple measurement program for "PicoScope" by Pico Technology  
Pico Technology社のUSBオシロスコープ「PicoScope」用計測プログラム

## 概要
4つのプローブの取得電圧をエクセルファイルに出力するプログラム。
出力先はデフォルトで「/output/out.csv」。


## 動作環境
Visual Studio 2017 
PicoScope 6402C

## 使い方
### Visual Studio 2017 の設定
「PicoScope_simpleMeasurement -> プロパティ -> C/C++ -> 全般 -> 追加のインクルードディレクトリ」
`$(ProgramW6432)\Pico Technology\SDK\inc`

「PicoScope_simpleMeasurement -> プロパティ -> リンカー -> 全般 -> 追加のライブラリディレクトリ」
`$(ProgramW6432)\Pico Technology\SDK\lib`

「PicoScope_simpleMeasurement -> プロパティ -> リンカー -> 入力 -> 追加の依存ファイル」
`ps6000.lib`



