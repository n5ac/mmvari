//Copyright+LGPL

//-----------------------------------------------------------------------------------------------------------------------------------------------
// Copyright 2000-2013 Makoto Mori, Nobuyuki Oba
//-----------------------------------------------------------------------------------------------------------------------------------------------
// This file is part of MMVARI.

// MMVARI is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// MMVARI is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public License along with MMTTY.  If not, see 
// <http://www.gnu.org/licenses/>.
//-----------------------------------------------------------------------------------------------------------------------------------------------



//---------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include <Printers.hpp>

#include "MacroKey.h"
#include "MacEdit.h"
#include "ComLib.h"
#include "InputWin.h"
//---------------------------------------------------------------------
#pragma resource "*.dfm"
//TMacroKeyDlg *MacroKeyDlg;
MACKEY	mackeycom[]={
	{1, "<%TX>","送信に切り替え", "Switch to TX"},
	{1, "<%RX>","受信に切り替え", "Switch to RX"},
	{1, "<%TXRX>","送受の切り替え", "Turns over TX/RX"},
	{1, "<%TXOFF>","送信の強制終了", "Abort TX (Switch to RX immediately)"},
	{1, "<%AutoClear>","自動的に送信画面をクリア", "Clear TX window automatically"},
	{1, "<%ClearTXW>","送信画面をクリア", "Clear TX window"},
	{1, "<%ClearRXW>","受信画面をクリア", "Clear RX window"},
	{1, "<%MyCall>","自局のコールサイン", "My callsign"},
	{1, "<%HisCall>","相手局のコールサイン", "His/her callsign"},
	{1, "<%HisName>","相手局の名前", "His/her name"},
	{1, "<%DearName>","Dear xxx または xxxさん", "Dear xxx"},
	{1, "<%HisRST>","相手局のRST", "His/her RST"},
	{1, "<%MyRST>","自局の(伝えられた)RST", "My RST"},
	{1, "<%FREQ>","運用周波数(例 7)", "Logging FREQ (e.g. 7)"},
	{1, "<%BAND>","運用バンド(例 40m)", "Logging BAND (e.g. 40m)"},
	{1, "<%HisQTH>","相手局のQTH", "His/her QTH"},
	{1, "<%Note>","Note(備考)", "Note (Remarks)"},
	{1, "<%UDATE>","日付(UTC)", "UTC date"},
	{1, "<%UTIME>","時刻(UTC) hh:mm", "UTC time (hh:mm)"},
	{1, "<%UTIMES>","時刻(UTC) hh:mm:ss", "UTC time (hh:mm:ss)"},
	{1, "<%LDATE>","日付(Local)", "Local date"},
	{1, "<%LTIME>","時刻(Local) hh:mm", "Local time (hh:mm)"},
	{1, "<%LTIMES>","時刻(Local) hh:mm:ss", "Local time (hh:mm:ss)"},
	{1, "<%PTIME>","1980/JAN/6 からの経過秒(UTC時刻)", "Passing time from 1980/JAN/6 (UTC)"},
	{1, "<%LPTIME>","1980/JAN/6 からの経過秒(ローカル時刻)", "Passing time from 1980/JAN/6 (local time)"},
	{1, "<%QPTIME>","QSO開始UTC時刻(1980/JAN/6 からの経過秒)", "QSO Starting time (passing time from 1980/JAN/6 (UTC)"},
	{1, "<%VALTIME=year,<%PTIME>>","PTIMEを分解 (year/month/day/hour/minute/second..., PTIME)", "Takes the PTIME apart (year/month/day/hour/minute/second..., PTIME)"},
	{1, "<%Capture>","コールサインの捕獲", "Capture callsign"},
	{2, "<%HisGreetings=3>","自動挨拶(例 1-GA, 2-Good morning, 3-おはようございます)", NULL},
	{3, "<%HisGreetings=2>","Auto Greetings (e.g. 1-GA, 2-Good morning)", NULL},
	{1, "<%HisNR>","HisRSTのコンテストNR", "Only the contest number part of his/her RST"},
	{1, "<%MyNR>","MyRSTのコンテストNR", "Only the contest number part of my RST"},
	{1, "<%Entity=<%HisCall>>","エンティティコード", "Code of the entity"},
	{1, "<%EntityName=<%HisCall>>","エンティティ名", "Name of the entity"},
	{1, "<%Continent=<%HisCall>>","大陸コード", "Code of the continent"},
	{1, "<%PLATFORM>","OSのプラットフォーム", "Platform of the OS"},
	{1, "<%CodePage>","OSのコードページ", "Code page of the OS"},
	{1, "<%LanguageID>","OSの言語ID", "Language ID of the OS"},
	{1, "<%VER>","MMVARIのバージョン", "Program version of MMVARI"},
	{1, "<%VERMINOR>","MMVARIのマイナーバージョン", "Program minor version of MMVARI"},
	{1, "<%VARITYPE>","バリコード種別(VariSTD, VariJA, VariHL, ...)", "Type of the VARICODE(VariSTD, VariJA, VariHL, ...)"},
	{1, "<%MODE>","現在のモード(変調方式)種別(GMSK, FSK, ...)", "Current type of the mode(GMSK, FSK, ...)"},
	{1, "<%Level>","信号のS/Nレベル(dB)", "S/N level of the signals"},
	{1, "<%PeakLevel>","信号のピークS/Nレベル(dB)", "Peak S/N level of the signals"},
	{1, "<%AverageLevel>","信号の平均S/Nレベル(dB)", "Average S/N level of the signals"},
	{1, "<%MetricMFSK>","mfsk信号のメトリックレベル", "Metric level in mfsk"},
	{1, "<%MetricMFSK=even>","mfsk信号のビタビ別メトリックレベル(even/odd)", "Metric level of the viterbi in mfsk (even/odd)"},
	{1, "<%SquelchLevel=3.0>","スケルチレベル(S/N=dB)の設定", "Set squelch level (S/N=dB)"},
	{1, "<%CWID>","My callsignをCWで送信", "CWID of the MyCallsign"},
	{1, "<%CWID= 73 :>","文字列をCWで送信 (AS @  SK :  AR ;  KN ]  BT =)", "CWID of the strings (AS @  SK :  AR ;  KN ]  BT =)"},
	{1, "<%CWSpeed=20>","CW速度(10 - 60)を設定", "CWID speed (10-60)"},
	{1, "<%MODE=GMSK>","モード(変調方式)の設定(GMSK, FSK, ...)", "Set the mode(GMSK, FSK, ...)"},
	{1, "<%AutoNET>","自動的にNETをONにする", "Turns NET into ON automatically"},
	{1, "<%NETON>","NETをONにする", "Turns NET into ON"},
	{1, "<%NETOFF>","NETをOFFにする", "Turns NET into OFF"},
	{1, "<%AFCON>","AFCをONにする", "Turns AFC into ON"},
	{1, "<%AFCOFF>","AFCをOFFにする", "Turns AFC into OFF"},
	{1, "<%ATCON>","ATCをONにする", "Turns ATC into ON"},
	{1, "<%ATCOFF>","ATCをOFFにする", "Turns ATC into OFF"},
	{1, "<%RxCarrier>","受信キャリア周波数(Hz)", "RX carrier FREQ(Hz)"},
	{1, "<%TxCarrier>","送信キャリア周波数(Hz)", "TX carrier FREQ(Hz)"},
	{1, "<%RxCarrier=1750>","受信キャリア周波数(Hz)を設定", "Set RX carrier FREQ(Hz)"},
	{1, "<%TxCarrier=1750>","送信キャリア周波数(Hz)を設定", "Set TX carrier FREQ(Hz)"},
	{1, "<%AFCFrequency>","AFC検出周波数(Hz)", "AFC FREQ(Hz)"},
	{1, "<%MouseFrequency>","ｽﾍﾟｸﾄﾗﾑ/ｳｫｰﾀﾌｫｰﾙでのマウス周波数(Hz)", "Mouse frequency on Waterfall(Hz)"},
	{1, "<%TONE>","シングルトーンを送信", "Transmit single tone"},
	{1, "<%BPF=0>","BPF帯域幅(0-3)を設定", "Set BPF width (0-3)"},
	{1, "<%BPFTaps=64,80,128,256>","BPFのタップ数を設定", "Set BPF taps"},
	{1, "<%Notch=ONOFF>","ノッチフィルタのON/OFF (ON/OFF/ONOFF)", "Turns Notch into ON or OFF (ON/OFF/ONOFF)"},
	{1, "<%Notch=1750>","ノッチフィルタの周波数(Hz)を設定", "Set Notch FREQ (Hz)"},
	{1, "<%NotchTaps=128>","ノッチフィルタのタップ数を設定(16～512)", "Set taps of the Notch filter (16-512)"},
	{1, "<%NotchWidth=1>","ノッチフィルタの帯域を設定(1～1000)", "Set width of the Notch filter (1-1000)"},
	{1, "<%BAUD>","伝送ボーレート", "BaudRate"},
	{1, "<%BAUD=31.25>","伝送ボーレート(20～300)を設定", "Set BaudRate(20-300)"},
	{1, "<%AFCWidth=50>","AFC吸い込み幅(Hz)を設定", "Set AFC FREQ width(Hz)"},
	{1, "<%AFCLevel=12>","AFC吸い込みレベル(dB)を設定", "Set AFC sense level(dB)"},
	{1, "<%ATCSpeed=0>","ATC応答速度(0-5)を設定", "Set ATC speed(0-5)"},
	{1, "<%ATCLevel=15>","ATC検出レベル(dB)を設定", "Set ATC sense level(dB)"},
	{1, "<%ATCPPM>","ATC値(ppm)", "ATC value (ppm)"},
	{1, "<%PTT=COM1>","PTTポートを設定", "Set PTT port"},
	{1, "<%COMFSK=ONOFF>","PTTポート FSKのON/OFF (ON/OFF/ONOFF)", "Turns FSK into ON or OFF (ON/OFF/ONOFF)"},
	{1, "<%COMFSKINV=ONOFF>","PTTポート FSKの論理反転のON/OFF (ON/OFF/ONOFF)", "Turns inverting logic (FSK) into ON or OFF (ON/OFF/ONOFF)"},
	{1, "<%Radio=COM2>","Radioポートを設定", "Set Radio port"},
	{1, "<%RadioOut=\\$000000000F>","Radioポートにデータを出力", "Output data to the radio port"},
	{1, "<%RadioCarrierKHz>","リグの補正キャリア周波数(KHz)", "Adjusted carrier FREQ(KHz) of the radio"},
	{1, "<%RadioKHz>","リグのVFO周波数(KHz)", "VFO FREQ(KHz) of the radio"},
	{1, "<%RadioKHz=YAESU-HF,14073.000>","リグのVFO周波数(KHz)の設定", "Set VFO FREQ(KHz) of the radio"},
	{1, "<%RadioMode>","リグのモード", "Mode of the radio"},
	{1, "<%RadioMode=CI-V,LSB>","リグのモードを設定", "Set Mode of the radio"},
	{1, "<%PTTON>","PTTをONにする", "Turns PTT into ON"},
	{1, "<%PTTOFF>","PTTをOFFにする", "Turns PTT into OFF"},
	{1, "<%FFTScale>","FFTスケール(0-100dB, 1-60dB, 2-二乗振幅)", "FFT scale (0-100dB, 1-60dB, 2-Square amplitude)"},
	{1, "<%FFTScale=2>","FFTスケールを設定", "Set FFT scale"},
	{1, "<%FFTWidth=1000>","FFT表示幅(500/1000/2000/3000)", "Choose FFT width(500/1000/2000/3000)"},
	{1, "<%SyncWidth=14>","Sync/Wave表示幅(4-24)", "Choose Sync/Waveform width(4-24)"},
	{1, "<%ShowCH=1,ONOFF>","サブチャンネル(1-8)表示, ON/OFF/ONOFF", "Show sub-channel(1-8), ON/OFF/ONOFF"},
	{1, "<%SetCHSpeed=1,31.25>","サブチャンネル(1-8)のボーレート設定, 15～300", "Set speed to the sub-channel(1-8), 15-300"},
	{1, "<%SetCHMode=1,GMSK>","サブチャンネル(1-8)のモード設定", "Set mode to the sub-channel(1-8)"},
	{1, "<%ClearCHW=1>","サブチャンネル(1-8)の画面クリア", "Clear window of the sub-channel(1-8)"},
	{1, "<%QSOON>","QSOの開始(ログに記録)", "Start QSO (logging)"},
	{1, "<%QSOOFF>","QSOの終了(ログに記録)", "End of the QSO (logging)"},
	{1, "<%FREQ=7>","ログの運用周波数を設定", "Set logging FREQ"},
	{1, "<%BAND=40m>","ログの運用バンドを設定", "Set logging BAND"},
	{1, "<%LogMODE=SSB>","ログの運用モードを設定(ヌル文字列で自動設定)", "Set logging MODE (null is auto)"},
	{1, "<%HisCall=<%Capture>>","相手局のコールを設定", "Set his/her callsign"},
	{1, "<%HisRST=599>","相手局のRSTを設定", "Set his/her RST"},
	{1, "<%MyRST=599>","自局のRSTを設定", "Set my RST"},
	{1, "<%Note=<%VARITYPE>>","Note(備考)に設定", "Set note (Remarks)"},
	{1, "<%RefRXW>","受信画面の参照画面を開く", "Show RX window with the editor"},
   	{1, "<%EditFile=memo.txt, 0>","テキストファイル編集画面を開く(Name, ReadOnly)", "Open text editer(Name, ReadOnly)"},
	{1, "<%SendFile=test.txt>","テキストファイルの送信", "Send file"},
	{1, "<%EditMacro=2>","マクロボタン(1～160)を編集", "Edit macro button (1-160)"},
	{1, "<%EditMacro=OnStart>","イベントマクロを編集(Event名)", "Edit event macro (Name of the event)"},
	{1, "<%EditMacro=AS(CW)>","AS(CW)マクロを編集(Event)", "Edit AS(CW) macro"},
	{1, "<%EditMacro=MacroTxt.txt>","テキストファイルのマクロを編集", "Edit macro written in the text file"},
	{1, "<%MacroText=MacroTxt.txt>","テキストファイルのマクロを実行", "Execute macro written in the text file"},
	{1, "<%SaveMacro=Macros.mac>","全てのマクロをファイルに保存", "Save all Macros"},
	{1, "<%LoadMacro=Macros.mac>","全てのマクロをファイルからロード", "Load all Macros"},
	{1, "<%HEX2DEC=ABCD>","16進数を10進数に変換", "Convert decimal to HEX"},
	{1, "<%CHAR=A>","文字コード(10進数)に変換", "Convert character code (decimal)"},
	{1, "<%CHARX=A>","文字コード(16進数)に変換", "Convert character code (HEX)"},
	{1, "<%CODE=82A0>","文字コード(16進数)で入力", "Input as the character code (HEX)"},
	{1, "<%SP>","スペースを入力", "Input space"},
	{1, "<%CR>","CRを入力", "Input CR"},
	{1, "<%BS>","BSを入力", "Input BS"},
	{1, "<%TAB>","TABを入力", "Input TAB"},
	{1, "<%SkipCR>","マクロ文内の次のCR(改行)をスキップ", "Skip next CR(Enter) in the macro sentence"},
	{1, "<%DisableCR>","マクロ文内のCR(改行)を無効にする", "Disable CR(Enter) in the macro sentence"},
	{1, "<%EnableCR>","マクロ文内のCR(改行)を有効にする", "Enable CR(Enter) in the macro sentence"},
	{1, "<%DisableTAB>","マクロ文内のTABを無効にする", "Disable TAB in the macro sentence"},
	{1, "<%EnableTAB>","マクロ文内のTABを有効にする", "Enable TAB in the macro sentence"},
	{1, "<%DisableSP>","マクロ文内のスペースを無効にする", "Disable SPACE in the macro sentence"},
	{1, "<%EnableSP>","マクロ文内のスペースを有効にする", "Enable SPACE in the macro sentence"},
	{1, "<%Skip$=3,<%HisCall>>","指定の文字数をスキップ(Ascii)", "Skip Ascii characters"},
	{1, "<%MSkip$=3,<%Note>>","指定の文字数をスキップ(MBCS)", "Skip MBCS characters"},
	{1, "<%Find$=xx,ABCxxVVV>","指定の文字列を検索", "Find string"},
	{1, "<%StrLen=<%HisCall>>","文字列の長さ", "String length"},
	{1, "<%MStrLen=<%Note>>","MBCS文字列の長さ", "MBSC string length"},
	{1, "<%ONOFF=<%Cond=IsSQ>>","ON/OFFを得る", "Get ON/OFF"},
	{1, "<%RepeatText=3,<%RepeatText=33,RY><%CR>>","マクロ文字列の繰り返し", "Repeating the macro string"},
	{1, "<%IDLE>","アイドル信号を挿入", "Insert idle signals"},
	{1, "<%DIDDLE=LTR>","RTTYのDIDDLEコードを設定(BLK/LTR)", "Set DIDDLE code in RTTY (BLK/LTR)"},
	{1, "<%UOS=ON>","RTTYのUOSを設定(ON/OFF/ONOFF)", "Set UOS in RTTY (ON/OFF/ONOFF)"},
	{1, "<%RTTYWaitC=0>","RTTYの文字ウエイトを設定(0-100)", "Set character wait in RTTY (0-100)"},
	{1, "<%RTTYWaitD=0>","RTTYのDIDDLEウエイトを設定(0-100)", "Set DIDDLE wait in RTTY (0-100)"},
	{1, "<%RTTYWordOut=ON>","RTTYのワード単位出力を設定(ON/OFF/ONOFF)", "Set word out in RTTY (ON/OFF/ONOFF)"},
	{1, "<%RTTYDEM=FFT>","RTTYの復調器を設定(IIR/FFT)", "Set demodulator in RTTY (IIR/FFT)"},
	{1, "<%TxShift=170.0>","RTTY/FSK-Wの送信シフト幅を設定(10-450)", "Set TX's shift width in RTTY/FSK-W (10-450)"},
	{1, "<%RxShift=170.0>","RTTY/FSK-Wの受信シフト幅を設定(10-450)", "Set RX's shift width in RTTY/FSK-W (10-450)"},
	{1, "<%ToUpper=<%HisName>>","アッパーケース(大文字)に変換", "Convert to upper case"},
	{1, "<%ToLower=<%HisCall> de <%MyCall>>","ロワーケース(小文字)に変換", "Convert to lower case"},
	{1, "<%IME=ON>","IMEを制御する (ON/OFF/ONOFF)", "Control IME (ON/OFF/ONOFF)"},
	{1, "<%RepeatTX=3000>","繰り返し送信の受信時間(ms)を設定", "Repeating TX mode (=RX times(ms))"},
	{1, "<%Repeat=10000>","マクロの繰り返し(ms)を設定", "Repeating the macro (ms)"},
	{1, "<%OnTimer=<%WaterMsg=4,<%AFCFrequency>Hz>>","OnTimerマクロを設定", "Set OnTimer macro"},
	{1, "<%OnTimerInterval=1000>","OnTimerマクロの時間間隔(ms)を設定", "Set time interval(ms) of OnTimer macro"},
	{1, "<%Page=1>","送信画面ページを切り替え (1～4)", "Choose TXW-page (1-4)"},
	{1, "<%AutoReturn>","自動的に送信画面ページを戻す", "Return TXW-page automatically"},
	{1, "<%MoveTop>","カーソルを先頭に移動", "Move cursor to the TOP"},
	{1, "<%MoveEnd>","カーソルを最後に移動", "Move cursor to the END"},
	{1, "<%ResetScroll>","受信画面のスクロール状態を解除", "Reset scroll in the RX window"},
	{1, "<%PopupTXW>","送信画面のポップアップメニューを開く", "Open Popup menu"},
	{1, "<%DupeText>","１つ前の行と同じ内容を挿入", "Create dupe text"},
	{1, "<%SeekNext>","ボタンページを次へ", "Seek to the next Button page"},
	{1, "<%SeekPrev>","ボタンページを前へ", "Seek to the previous Button page"},
	{1, "<%SeekTop>","ボタンページを先頭へ", "Seek to the top Button page"},
	{1, "<%DigitalLevel=16384>","デジタル出力レベルを設定(1024 - 32768)", "Set digital output level (1024 - 32768)"},
	{1, "<%OutputVolume>","出力ボリューム調整画面を開く", "Open output volume"},
	{1, "<%InputVolume>","入力ボリューム調整画面を開く", "Open input volume"},
	{1, "<%Setup>","設定画面を開く", "Open Setup window"},
	{1, "<%SetupLog>","ログ設定画面を開く", "Open Setup logging window"},
	{1, "<%SetupRadio>","リグコントロール設定画面を開く", "Open Setup radio command window"},
	{1, "<%Calibration>","クロック較正画面を開く", "Open calibration window"},
	{1, "<%BaseClock>","ベースクロック値(Hz)", "Base clock(Hz)"},
	{1, "<%Clock>","RXクロック値(Hz)", "RX clock(Hz)"},
	{1, "<%Clock=11025.00>","RXクロック値を設定", "Set RX clock"},
	{1, "<%TxOffset>","TXオフセット値(Hz)", "TX offset clock(Hz)"},
	{1, "<%TxOffset=0.00>","TXオフセット値を設定", "Set TX offset clock"},
	{1, "<%DemodulatorClock>","復調器クロック値(Hz)", "Demodulator clock(Hz)"},
	{1, "<%FFTClock>","FFTクロック値(Hz)", "FFT clock(Hz)"},
	{1, "<%FFTSize>","FFTのサイズ（点数)", "FFT size(points)"},
	{1, "<%WaterNoise>","ウォータフォールの検出ノイズレベル(dB)", "Sense noise level (dB) in WaterFall"},
	{1, "<%WaterNoise=35,65>","ウォータフォールのノイズレベル範囲(dB)を設定", "Set noise level range (dB) in WaterFall"},
	{1, "<%WaterLevels>","ウォータフォールのレベル配分", "Levels in WaterFall"},
	{1, "<%WaterLevels=10,60,134,192,220,240>","ウォータフォールのレベル配分を設定", "Set levels in WaterFall"},
	{1, "<%WaterColors>","ウォータフォールの配色", "Colors in WaterFall"},
	{1, "<%WaterColors=0,FF0000,FFFFFF,FF,FFFF00,FF0000,FFFF00,FFFF,80FF,FF,FF,FF>","ウォータフォールの配色を設定", "Set colors in WaterFall"},
	{1, "<%SpectrumColors>","スペクトラムの配色", "Colors in Spectrum"},
	{1, "<%SpectrumColors=0,FF00,FFFFFF,808080,FFFF00,FF0000>","スペクトラムの配色を設定", "Set colors in Spectrum"},
	{1, "<%RxColors>","受信画面の配色", "Colors in RX window"},
	{1, "<%RxColors=FFFFFF,0,FF0000,FF,E0E0E0>","受信画面の配色を設定", "Set colors in RX window"},
	{1, "<%TxColors>","送信画面の配色", "Colors in TX window"},
	{1, "<%TxColors=FFFFFF,0,FF0000,FF,FFFFFF>","送信画面の配色を設定", "Set colors in TX window"},
	{1, "<%SoundName>","入力サウンドの名前", "Name of the input sound device"},
	{1, "<%SoundOutName>","出力サウンドの名前", "Name of the output sound device"},
	{1, "<%SoundDevice=MONO,-1>","サウンドCH(MONO/LEFT/RIGHT)とデバイスIDの設定", "Set CH.(MONO/LEFT/RIGHT) es device ID of the sound"},
	{1, "<%Suspend>","サスペンドにする", "Suspend MMVARI"},
	{1, "<%Resume>","サスペンドを解除する", "Resume MMVARI"},
	{1, "<%Wait=1000>","指定時間(ms)ウエイトする", "Wait the time (ms)"},
	{1, "<%Execute=notepad.exe <%Folder>memo.txt>","プログラムの実行", "Execute other program"},
	{1, "<%Shell=<%Folder>emmvari.txt>","シェルでドキュメントを開く", "Open document by Shell32"},
	{1, "<%Exit>","MMVARIを終了する", "Exit MMVARI"},
	{1, "<%ShutDown>","Windowsをシャットダウンする", "Shut down Windows"},
	{1, "<%YesNo=String>","Yes/Noのﾒｯｾｰｼﾞﾎﾞｯｸｽ(Yes=6,No=7)", "Show Yes/No message(Yes=6,No=7)"},
	{1, "<%YesNoCancel=String>","Yes/No/Cancelのﾒｯｾｰｼﾞﾎﾞｯｸｽ(Yes=6,No=7,Cancel=2)", "Show Yes/No/Cancel message(Yes=6,No=7,Cancel=2)"},
	{1, "<%OkCancel=String>","Ok/Cancelのﾒｯｾｰｼﾞﾎﾞｯｸｽ(Ok=1,Cancel=2)", "Show Ok/Cancel message(Ok=1,Cancel=2)"},
	{1, "<%Error=Macro ERROR>","エラーメッセージを表示", "Show error message"},
	{1, "<%Warning=Macro WARNING>","警告メッセージを表示", "Show warning message"},
	{1, "<%Message=Macro message>","メッセージを表示", "Show message"},
	{1, "<%WaterMsg=4,Metric=<%MetricMFSK>><%Repeat=1000>","ウォータフォールにメッセージを表示(位置=0-4)", "Show message in the WaterFall (Pos. 0-4)"},
	{1, "<%RxStatus=Status>","受信画面にステータスを表示", "Show status in the RX window"},
	{1, "<%TableCount=AAA,BBB,CCC>","テーブルの項目数を得る", "Get count of the table"},
	{1, "<%TableStr=1,AAA,BBB,CCC>","テーブルの文字列を得る", "Get string of the table"},
	{1, "<%Table=<%BAUD>,20.0,31.25,45.0>","テーブルのインデクッスを得る", "Get index of the table"},
	{1, "<%Menu=A,B,C,D>","メニューの表示(Input$に入る)", "Show menu (Put into Input$)"},
	{1, "<%MenuB=\x22<%Table=<%BAUD>,20.0,31.25,45.45,62.5>\x22,20.0,31.25,45.45,62.5>","マーク付きメニューの表示(Input$に入る)", "Show menu with mark (Put into Input$)"},
	{1, "<%Menu>","メニューインデックス", "menu index"},
	{1, "<%Input=Input strings>","文字列の入力", "Input strings"},
	{1, "<%Input$>","入力された文字列", "The strings which was entered"},
	{1, "<%Click$>","受信画面でクリックした文字列", "The strings which was clicked in the RX window"},
	{1, "<%CLICK$>","受信画面でクリックした文字列(大文字)", "The strings (upper case) which was clicked in the RX window"},
	{1, "<%vvv>","テストメッセージ(英語)", "test message"},
	{2, "<%VVV>","テストメッセージ(日本語)", "test message"},
	{1, "<%KeyStroke=41>","キー(VK code)を押して離す", "Down and up the keyboard (VK code)"},
	{1, "<%KeyDown=12><%KeyStroke=<%CHARX=V>><%KeyUp=12>","キー(VK code)を押す(Shift=10,Ctrl=11,Alt=12)", "Down the keyboard(VK code, Shift=10,Ctrl=11,Alt=12)"},
	{1, "<%KeyUp=12>","キー(VK code)を離す(Shift=10,Ctrl=11,Alt=12)", "Up the keyboard(VK code, Shift=10,Ctrl=11,Alt=12)"},
	{1, "<%DoMacro=2>","マクロボタン(1～160)を実行", "Execute macro button (1-160)"},
	{1, "<%DoEvent=OnMode>","イベントマクロを実行", "Execute event macro"},
	{1, "<%Events>","イベントのリスト", "List of the events"},
	{1, "<%ButtonName=1,Clear>","マクロボタン(1～160)の名前を設定", "Set name of the button (1-160)"},
	{1, "<%ButtonContents=1,<%ClearTXW>>","マクロボタン(1～160)の内容を設定", "Set contents of the button (1-160)"},
	{1, "<%PlayBack=ON>","サウンドプレーバックボタン機能の設定(ON/OFF/ONOFF)", "Set Sound Playback function (ON/OFF/ONOFF)"},
	{1, "<%PlayBack=10>","サウンドプレーバック(0～60秒)を実行", "Execute Sound Playback (0-60s)"},
	{1, "<%PlayBackSpeed=5>","サウンドプレーバックの速度(1～20)を設定", "Set speed of the Sound Playback (1-20)"},
	{1, "<%PlayBackButtons=60,30,15>","サウンドプレーバックボタンの時間を設定", "Set time of the Sound Playback buttons"},
	{1, "<%PlaySound=sound.mmv>","サウンドファイルの再生", "Sound playing from the file"},
	{1, "<%PlaySound>","サウンドファイルの再生ダイアログを表示する", "Show sound playing dialog"},
	{1, "<%RecordSound=sound.mmv>","サウンドファイルの録音", "Sound recording to the file"},
	{1, "<%RecordSound>","サウンドファイルの録音ダイアログを表示する", "Show sound recording dialog"},
	{1, "<%QuickRecordSound>","サウンドファイルの録音", "Sound recording (filename = Time stamp)"},
	{1, "<%StopPlayRecord>","サウンド再生/録音の停止", "Close recording or playing"},
	{1, "<%SoundTime>","再生/録音時間(秒)", "Time (sec) of sound (playing or recording)"},
	{1, "<%Slider=CW speed,<%CWSpeed>,10,60,1,10>","スライダーを表示(ﾀｲﾄﾙ,値,Min,Max,ｽﾃｯﾌﾟ,目盛の数) Input$に入る", "Show slider (Title,Value,Min,Max,Step,Scale) Put into Input$"},
	{1, "<%FileDialog=Text file,Text Files(*.txt)|*.txt|,Default,txt,>","ファイル選択ダイアログを表示(Input$に入る)", "Show choose file dialog box (Put into Input$)"},
	{1, "<%CPUBENCHMARK>","CPUベンチマーク値 (us)", "The values of CPU-benchmark (us)"},
	{1, "<%RANDOM=<%PTIME>>","乱数の種の初期化", "Initialization of a random seed"},
	{1, "<%RANDOM>","乱数(0～32767)", "A random values (0 - 32767)"},
	{1, "<%String=Name>","#defineで定義した文字列を展開", "Defined strings by #define"},
	{1, "<%CallProc=Name>","プロシジャーを実行", "Execute the procedure"},
	{1, "<%DebugProc=Name>","<%CallProc=...>のデバッグ版", "Debug version of <%CallProc=...>"},
	{1, "<%Format=%.3f,<%BAUD>>","書式変換(C言語)指定で展開 (書式文字列,式)", "Convert format (C Language) string"},
	{1, "<%Inv=<%Cond=IsSQ>>","論理を反転", "Invert logic"},
	{1, "<%Floor=<%BAUD>>","小数点以下を切り捨て", "Returns the largest integer that is not greater than the argument"},
	{1, "<%Log=10>","自然対数を計算", "Calculates logarithms"},
	{1, "<%Exp=2.302585092994046>","指数関数 e の x 乗を計算", "Calculates the exponential"},
	{1, "<%Sqrt=10>","平方根を計算", "Calculates the square root"},
	{1, "<%Pow=10,2>","x の y 乗を計算", "Calculates x raised to the power of y"},
	{1, "<%Sin=0.5>","sin(x)を計算", "Calculates the sin(x)"},
	{1, "<%Cos=0.5>","cos(x)を計算", "Calculates the cos(x)"},
	{1, "<%Tan=0.5>","tan(x)を計算", "Calculates the tan(x)"},
	{1, "<%ArcTan=0.5>","arc-tan(x)を計算", "Calculates the arc-tan(x)"},
	{1, "<%Cond=IsSQ>","条件命令のテスト結果(0-FALSE, 1-TRUE)", "Test result of condition command (0-FALSE, 1-TRUE)"},
	{1, "<%ListSpeed=15.625,31.25,62.5,93.75,125.0,250.0>","Speedボックスのリストを設定", "Set list of Speed box"},
	{1, "<%ListCarrier=500,800,1000,1200,1500,1750,2000,2210>","Carrierボックスのリストを設定", "Set list of Carrier box"},
	{1, "<%ListRST=599,579,559,449,339>","RSTボックスのリストを設定", "Set list of RST box"},
	{1, "<%ListLogFreq=1.8,3.5,7,10,14,18,21,24,28,50,144,430,1200>","ログ周波数ボックスのリストを設定", "Set list of Log-FREQ box"},
	{1, "<%SetCaptureLimit>","文字列捕獲の検索範囲を設定", "Set the limit of the capturing range"},
	{1, "<%ClearCaptureLimit>","文字列捕獲の検索範囲をクリア", "Clear the limit of the capturing range"},
	{1, "<%Font>","受信画面のフォントパラメータ(Name, Size, Charset)", "Font parameters of the RX window (Name, Size, Charset)"},
	{1, "<%Font=,14,>","受信/送信画面のフォントを設定(=Name, Size, Charset)", "Sets font into the TX/RX window (=Name, Size, Charset)"},
	{1, "<%SetFocus>","送信画面にキーボードフォーカスを設定する", "Sets keyboard focus to the TX window"},
	{1, "<%Folder>","MMVARIのフォルダ", "MMVARI's folder"},
	{1, "<%AddMenu=E&X,Caption&1,OnCaption1Click>","拡張メニューの追加 (=Name,Caption,Procedure,Arg...)", "Add extension menu (=Name,Caption,Procedure,Arg...)"},
	{1, "<%InsertMenu=&H,&P,Caption&1, OnCaption1Click>","拡張メニューの挿入 (=Name,InsPos,Caption,Procedure,Arg...)", "Insert extension menu (=Name,InsPos,Caption,Procedure,Arg...)"},
	{1, "<%ShowMenu=&F, ONOFF>","メニューの表示 (=Name...,ON/OFF/ONOFF)", "Show the menu (=Name...,ON/OFF/ONOFF)"},
	{1, "<%EnableMenu=&F, ONOFF>","メニューの許可 (=Name...,ON/OFF/ONOFF)", "Enable the menu (=Name...,ON/OFF/ONOFF)"},
	{1, "<%ShortCut=&V, &C, &1, Ctrl+1>","メニューにショートカットを設定 (=Name...,Key)", "Set menu short cut (=Name...,Key)"},
	{1, "<%CheckMenu=&H, &P, ONOFF>","メニューにチェックの付ける (=Name...,ON/OFF/ONOFF)", "Set menu check (=Name...,ON/OFF/ONOFF)"},
	{1, "<%DeleteMenu=&X>","メニューを削除 (=Name...)", "Delete menu (=Name...)"},
	{1, "<%DoMenu=&V,&C,&1>","メニューを実行 (=Name...)", "Execute the menu (=Name...)"},
	{1, "<%DoButton=FFT>","ボタンをクリックする", "Click the button"},
	{1, "<%ShowHTML=home>","HTML/ウェーブサイトを表示", "Show html or Web site"},
	{1, "<%SendMessage=PSKGNRFUNC,0,1>","ウインドウメッセージの送信 (=Name, wParam, lParam)", "Send window message (=Name, wParam, lParam)"},
//	{1, "<%SendMessage=this,16,0,0>","ウインドウメッセージの送信 (this, uMsg, wParam, lParam)", "Send window message (this, uMsg, wParam, lParam)"},
//	{1, "<%CallDLL=QRZ.DLL,FuncName,<%HisCall>>","DLLファンクションの呼び出し", "Call the function in the DLL"},
	{1, "<%Memory>","メモリ状態(Load[%],Total[MB],Free[MB])", "Memory status(Load[%],Total[MB],Free[MB])"},
	{1, "<%BuffSize=4096>","マクロバッファの最小のサイズを設定(512-65536)", "Set minimum buffer size for Macro function (512-65536)"},
	{1, "<%EOF>","定義の終了", "End of the macro"},
	{0, NULL, NULL},
};

#if AN(mackeycom) >= MACLISTMAX
#error	macro key over follow
#endif

MACKEY	condcom[]={
	{1, "IsCall","Callに文字が存在する時", "Letters exists in the Call-box"},
	{1, "IsName","Nameに文字が存在する時", "Letters exists in the Name-box"},
	{1, "IsHisRST","Hisに文字が存在する時", "Letters exists in the His-box"},
	{1, "IsMyRST","Myに文字が存在する時", "Letters exists in the My-box"},
	{1, "IsQTH","QTHに文字が存在する時", "Letters exists in the QTH-box"},
	{1, "IsNote","Noteに文字が存在する時", "Letters exists in the Note-box"},
	{1, "IsLocal","同じエンティティの時", "Same entity (country)"},
	{1, "IsQSO","QSOボタンを押している時", "QSO button down"},
	{1, "IsDupe","重複QSOの場合", "Is dupe QSO?"},
	{1, "IsAFC","AFCがONの時", "Is AFC ON?"},
	{1, "IsNET","NETがONの時", "Is NET ON?"},
	{1, "IsTX","送信中の時", "Is transmitting?"},
	{1, "IsPTT","PTTがONの時", "Is PTT ON?"},
	{1, "IsSQ","スケルチが開いている時", "Is Squelch open?"},
	{1, "IsTone","シングルトーン送信中の時", "Is single tone transmitting?"},
	{1, "IsTXEmpty","未送信文字が存在しない時", "Letters do not exist in the TX window"},
	{1, "IsRXScroll","受信画面がスクロール中の時", "Is RX window scrolling?"},
	{1, "IsRepeat","マクロが繰り返し中の時", "Is the macro repeating?"},
	{1, "IsIME","IMEがONの時", "Is IME ON?"},
	{1, "IsPlaying","サウンド再生中の時", "Is sound playing?"},
	{1, "IsFileSending","ファイル送信中の時", "Is file sending?"},
	{1, "IsRecording","サウンド録音中の時", "Is sound recording?"},
	{1, "IsFile(test.txt)","ファイルが存在する時", "File exists"},
	{1, "IsDefined(Name)","文字列変数が存在する時", "Variable strings exists"},
	{1, "IsMBCS(<%HisName>)","文字列にMBCSが含まれる時", "Is string including MBCS?"},
	{1, "IsAlpha(<%MyNR>)","文字列がアルファベットの時", "Is string alphabet?"},
	{1, "IsNumber(<%MyNR>)","文字列が数字の時", "Is string number?"},
	{1, "IsUOS","RTTYのUOSがONの時", "Is UOS active in RTTY?"},
	{1, "IsRadioLSB","RigがLSBモードの時", "Is LSB of the radio?"},
	{1, "IsCaptureText(CQ)","受信テキストに文字列が存在する時", "Does string exist in the received text?"},
	{1, "IsOnTimer","OnTimerマクロを実行中の時", "Is OnTimer executing?"},
	{1, "IsIdle","アイドル状態の時", "Is idle?"},
	{1, "IsEnglish","英語モードの時", "Is English mode running?"},
	{1, "IsMfskCenter","mfskが中心周波数合わせの時", "Is center FREQ handling in mfsk?"},
	{1, "IsMfskMetricSq","mfskがメトリックスケルチの時", "Is squelch metric in mfsk?"},
	{1, "Is1stCR","送信開始時に自動でCR/LF出力する時", "Is CR/LF sending automatically at the beginning?"},
	{1, "IsPlayBack","サウンドプレーバック機能がONの時", "Is sound play back enabled?"},
	{1, "IsCodeMM","MMVARIコード(VariJA, VariHL,...)の時", "Is MMVARI code (VariJA, VariHL,...)?"},
	{1, "IsCall(<%CLICK$>)","文字列がコールサインの時", "Is the strings call sign?"},
	{1, "IsRST(<%CLICK$>)","文字列がRSTの時", "Is the strings RST?"},
	{1, "IsMenu(&H, &U)","メニューが存在する時", "Menu exists?"},
	{1, "IsMenuEnabled(&H, &U)","メニューが許可されている時", "Is the menu enabled?"},
	{1, "IsMenuChecked(&H, &U)","メニューがチェックされている時", "Is the menu checked?"},
	{1, "IsMenuVisible(&H, &U)","メニューが表示されている時", "Is the menu visible?"},
	{1, "IsButton(QSO)","ボタンが存在する時", "Button exists?"},
	{1, "IsButtonEnabled(QSO)","ボタンが許可されている時", "Is the button enabled?"},
	{1, "IsButtonDown(QSO)","ボタンが押されている時", "Is the button down?"},
	{1, "ValFreq>=144","ログ周波数の比較", "Compare logging FREQ"},
	{1, "ValBaud==31.25","伝送速度(Bps)の比較", "Compare speed (baudrate)"},
	{1, "ValCarrierRX<1500","受信キャリア周波数の比較", "Compare RX carrier FREQ"},
	{1, "ValCarrierTX<1500","送信キャリア周波数の比較", "Compare TX carrier FREQ"},
	{1, "ValPage!=1","送信画面ページの比較", "Compare TX window page"},
	{1, "ValTimeLocal<1200","時刻(Local)の比較", "Compare Local time"},
	{1, "ValTimeUTC>=0900","時刻(UTC)の比較", "Compare UTC time"},
	{1, "ValDateLocal>=0801","日付(Local)の比較", "Compare Local date"},
	{1, "ValDateUTC<0401","日付(UTC)の比較", "Compare UTC date"},
	{1, "ValScope","スコープの状態を比較(FFT=0, WF=1, SYNC=2)", "Compare condition of the scope (FFT=0, WF=1, SYNC=2)"},
	{1, "ValMacro(<%VER>)>="VERNO,"マクロ展開文字列の数値を比較", "Compare value of the Macro"},
	{1, "ValMenu","選択された<%Menu=...>のインデックス番号を比較", "Compare value of the selected <%Menu=...> index"},
	{1, "StrCall==JA3QRZ","コールサインの比較", "Compare his callsign"},
	{1, "StrMode==GMSK","モードの比較", "Compare current Mode"},
	{1, "StrBand==40m","ログ周波数（バンド）の比較", "Compare logging BAND"},
	{1, "StrHisRST==599","HisRSTの比較", "Compare HisRST"},
	{1, "StrMyRST==599","MyRSTの比較", "Compare MyRST"},
	{1, "StrNote==???","Noteの比較", "Compare Note"},
	{1, "StrEntity==JA","エンティティの比較", "Compare the entity of his callsign"},
	{1, "StrContinent==AS","大陸の比較", "Compare the continent of his callsign"},
	{1, "StrVARITYPE==JA","VARICODE種別(JA/HL/BV/BY/STD)の比較", "Compare type of the VARICODE(JA/HL/BV/BY/STD)"},
	{1, "StrPLATFORM==XP","Windowsプラットフォーム(95/98/98SE/ME/NT/2000/XP)の比較", "Compare platform of the Windows(95/98/98SE/ME/NT/2000/XP)"},
	{1, "StrMacro(<%VER>)=="VERNO,"マクロ展開文字列の比較", "Compare strings of the Macro"},
	{1, "#else","残りのすべてで真", "Rest of all"},
	{1, "#endif","条件ブロックを終了", "Exit condition block"},
	{1, "#macro <%TX>","マクロコマンドをパス1で実行", "Execute macro at the pass 1"},
	{1, "#proc Name","プロシジャーの登録 (Name, Dummy...)", "Register procedure (Name, Dummy...)"},
	{1, "#endp","プロシジャーの登録の終了", "End of the procedure"},
	{1, "#repeat 3","#repeat ～ #endpまでを繰り返し", "Repeat the block #repeat to #endp"},
	{1, "#define Name Strings","文字列を名前変数に定義", "Define variable strings to the name"},
	{1, "#DEFINE Name Strings","文字列を名前変数に定義(INIフィアルに保存)", "Define variable strings to the name (Save to INI file)"},
	{1, "#DELETE Name", "名前変数またはプロシジャーを削除", "Delete variable string or procedure"},
	{1, "#DELETEALL", "すべての名前変数を削除", "Delete all variable strings and procedures"},
	{1, "#exit","マクロ文の終了(パス１で終了)", "Exit macro at the pass 1"},
	{1, "#comment","コメント行", "Comment line"},
	{0, NULL, NULL},
};
static int g_Pos[2]={0,0};
static int g_TopPos[2]={0,0};
static POINT g_xySave;//={0,0};
//---------------------------------------------------------------------
__fastcall TMacroKeyDlg::TMacroKeyDlg(TComponent* AOwner)
	: TForm(AOwner)
{
  g_xySave.x = 0;
  g_xySave.y = 0;
	m_pDlg = NULL;
	FormStyle = ((TForm *)AOwner)->FormStyle;
	Font->Name = ((TForm *)AOwner)->Font->Name;
	Font->Charset = ((TForm *)AOwner)->Font->Charset;
	if( sys.m_MsgEng ){
//		Caption = "Choose macro";
		CancelBtn->Caption = "Close";
		SBIns->Caption = "Insert";
		SBSpace->Caption = "Space";
        SBCR->Caption = "Enter";
        SBUndo->Caption = "Undo";
		SBFind->Caption = "Find";
		SBPrint->Caption = "Print";

        SBIns->Hint = "Insert current command";
        SBSpace->Hint = "Insert 'Space'";
        SBCR->Hint = "Insert 'Enter'";
        SBUndo->Hint = "Cancel of the operation";
        SBFind->Hint = "Find strings";
        SBPrint->Hint = "Print the list (Right click for create file)";
        Grid->Hint = "Insert command for double click (Sort - click the title)";
	}
    OnWave();
	m_Type = 0;
    m_fSort = FALSE;
    m_fSortType = 0;
    m_fDisDblClick = FALSE;
}
//---------------------------------------------------------------------------
void __fastcall TMacroKeyDlg::FormDestroy(TObject *Sender)
{
	g_xySave.x = Left;
	g_xySave.y = Top;
	g_Pos[m_Type] = Grid->Row;
	g_TopPos[m_Type] = Grid->TopRow;
}
//---------------------------------------------------------------------------
void __fastcall TMacroKeyDlg::UpdateUI(int row)
{
	int r = row - 1;
	if( r >= 0 ){
		SBIns->Enabled = TRUE;
	}
	else {
		SBIns->Enabled = FALSE;
	}
}
//---------------------------------------------------------------------------
void __fastcall TMacroKeyDlg::UpdateUndo(BOOL undo)
{
	SBUndo->Enabled = undo;
}
//---------------------------------------------------------------------
int __fastcall TMacroKeyDlg::AddMacKey(MACKEY *mp, int n)
{
	for( ; mp->r; mp++){
		if( (mp->r == 2) && sys.m_MsgEng ) continue;
		if( (mp->r == 3) && !sys.m_MsgEng ) continue;
		mackey[n] = *mp;
		n++;
	}
	return n;
}
//---------------------------------------------------------------------
void __fastcall TMacroKeyDlg::Execute(int x, int y, TMacEditDlg *pDlg, int type)
{
	m_fSort = FALSE;
	m_Type = type;
	Grid->RowCount = AddMacKey(type ? condcom : mackeycom, 0) + 1;
	if( (g_Pos[type] > 0) && (g_Pos[type] < Grid->RowCount) ){
		Grid->Row = g_Pos[type];
		Grid->TopRow = g_TopPos[type];
	}
	UpdateUI(Grid->Row);
	if( type ){
		Caption = sys.m_MsgEng ? "Choose Condition command (pass 1 command)" : "条件命令(パス1命令)の選択";
    }
    else {
		Caption = sys.m_MsgEng ? "Choose Macro command (pass 2 command)" : "マクロコマンド(パス2命令)の選択";
    }
    SBSpace->Enabled = !type;
	SBCR->Enabled = !type;
#if DEBUG
	char bf[256];
	sprintf(bf, "%s (Commands=%u/%u)", Caption.c_str(), Grid->RowCount-1, MACLISTMAX);
    Caption = bf;
#endif
	m_pDlg = pDlg;
	if( (x >= 0) && (y >= 0) ){
		if( g_xySave.x && g_xySave.y ){
			Left = g_xySave.x;
       		Top = g_xySave.y;
	    }
    	else {
			Left = x;
    	    Top = y;
	    }
    }
	Visible = TRUE;
	OnWave();
    Grid->SetFocus();
}
//---------------------------------------------------------------------
LPCSTR __fastcall TMacroKeyDlg::GetComment(const MACKEY *mp)
{
	LPCSTR pCom;

	if( sys.m_MsgEng ){
		pCom = mp->pEng;
		if( pCom == NULL ){
			pCom = mp->pJpn;
		}
	}
	else {
		pCom = mp->pJpn;
	}
    return pCom;
}
//---------------------------------------------------------------------
void __fastcall TMacroKeyDlg::GridDrawCell(TObject *Sender, int Col,
	  int Row, TRect &Rect, TGridDrawState State)
{
	char	bf[256];
	Grid->Canvas->Font->Height = -12;
	Grid->Canvas->FillRect(Rect);
	int X = Rect.Left + 4;
	int Y = Rect.Top + 2;

	if( Row ){
		Row--;
		bf[0] = 0;
		switch(Col){
			case 0:
                OnWave();
				strcpy(bf, mackey[Row].pKey);
				break;
			case 1:
				strcpy(bf, GetComment(&mackey[Row]));
				break;
		}
		Grid->Canvas->TextRect(Rect, X, Y, bf);
	}
	else {		// タイトル
		LPCSTR	_tt[]={
			"Commands","Comments",
		};
		Grid->Canvas->TextRect(Rect, X, Y, _tt[Col]);
	}
}
//---------------------------------------------------------------------------
void __fastcall TMacroKeyDlg::GridSelectCell(TObject *Sender, int Col,
	  int Row, bool &CanSelect)
{
	UpdateUI(Row);
}
//---------------------------------------------------------------------------
void __fastcall TMacroKeyDlg::CancelBtnClick(TObject *Sender)
{
	g_Pos[m_Type] = Grid->Row;
	g_TopPos[m_Type] = Grid->TopRow;
	Visible = FALSE;
}
//---------------------------------------------------------------------------
void __fastcall TMacroKeyDlg::SBInsClick(TObject *Sender)
{
	if( m_pDlg ){
		if( Grid->Row ){
			LPCSTR p = mackey[Grid->Row - 1].pKey;
			if( m_Type ){
				char bf[256];
                sprintf(bf, (*p == '#') ? "%s\r" : "#if %s\r", p);
                p = bf;
            }
        	m_pDlg->OnInsertText(p);
		}
    }
}
//---------------------------------------------------------------------------
void __fastcall TMacroKeyDlg::SBSpaceClick(TObject *Sender)
{
	if( m_pDlg ){
       	m_pDlg->OnInsertText(" ");
    }
}
//---------------------------------------------------------------------------
void __fastcall TMacroKeyDlg::SBCRClick(TObject *Sender)
{
	if( m_pDlg ){
       	m_pDlg->OnInsertText("\r");
    }
}
//---------------------------------------------------------------------------
void __fastcall TMacroKeyDlg::SBUndoClick(TObject *Sender)
{
	if( m_pDlg ){
		m_pDlg->Undo();
	}
}
//---------------------------------------------------------------------------
void __fastcall TMacroKeyDlg::GridMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if( Y < Grid->DefaultRowHeight ){
		if( Button == mbLeft ){
			int type = (X <= Grid->ColWidths[0]) ? 0 : 1;
            if( type == m_fSortType ){
				m_fSort = m_fSort ? FALSE : TRUE;
            }
            else {
				m_fSort = TRUE;
            }
            m_fSortType = type;
            Sort(m_fSort, type);
            Grid->Invalidate();
			m_fDisDblClick = TRUE;
        }
    }
    else {
		m_fDisDblClick = FALSE;
    }
}
//---------------------------------------------------------------------------
static int _USERENTRY _cmpK(const void *s, const void *t)
{
	const MACKEY *sp = (const MACKEY *)s;
    const MACKEY *tp = (const MACKEY *)t;
    return strcmpi(sp->pKey, tp->pKey);
}
//---------------------------------------------------------------------------
static int _USERENTRY _cmpC(const void *s, const void *t)
{
	const MACKEY *sp = (const MACKEY *)s;
    const MACKEY *tp = (const MACKEY *)t;
	LPCSTR pComS, pComT;
	if( sys.m_MsgEng ){
		pComS = sp->pEng;
		if( pComS == NULL ) pComS = sp->pJpn;
		pComT = tp->pEng;
		if( pComT == NULL ) pComT = tp->pJpn;
	}
	else {
		pComS = sp->pJpn;
		pComT = tp->pJpn;
	}
    return strcmpi(pComS, pComT);
}
//---------------------------------------------------------------------------
void __fastcall TMacroKeyDlg::Sort(BOOL fSort, int type)
{
	if( fSort ){
		qsort(mackey, Grid->RowCount - 1, sizeof(MACKEY), type ? _cmpC : _cmpK);
    }
    else {
		AddMacKey(m_Type ? condcom : mackeycom, 0);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMacroKeyDlg::GridDblClick(TObject *Sender)
{
	if( m_fDisDblClick ){
		m_fDisDblClick = FALSE;
        return;
    }
    SBInsClick(NULL);
}
//---------------------------------------------------------------------------
void __fastcall TMacroKeyDlg::FormKeyPress(TObject *Sender, char &Key)
{
	if( isalpha(Key) ){
		int c = toupper(Key);
		if( !m_fSort || m_fSortType ){
			m_fSort = TRUE;
            m_fSortType = 0;
            Sort(m_fSort, m_fSortType);
        }
		MACKEY *mp = mackey;
        LPCSTR p;
        for( int i = 0; i < Grid->RowCount - 1; i++, mp++ ){
        	p = mp->pKey;
			if( p ){
	            if( *p == '<' ) p++;
	            if( *p == '%' ) p++;
    	        if( toupper(*p) == c ){
                    int top = i;
                    if( top < 1 ) top = 1;
					Grid->TopRow = top;
                	Grid->Row = i + 1;
	                break;
    	        }
            }
        }
		Grid->Invalidate();
    }
}
//---------------------------------------------------------------------------
static BOOL __fastcall jstrstr(LPCSTR s, LPCSTR p)
{
	CMBCS *pM = &MainVARI->m_RxSet[0].m_MBCS;
    int l = strlen(p);
	for( ; *s; s++ ){
		if( !strnicmp(s, p, l) ) return TRUE;
		if( pM->IsLead(BYTE(*s)) ) s++;
        if( !*s ) break;
    }
    return FALSE;
}
//---------------------------------------------------------------------------
void __fastcall TMacroKeyDlg::SBFindClick(TObject *Sender)
{
	AnsiString as;
    if( InputMB("MMVARI", sys.m_MsgEng ? "Find string" : "検索文字列", as) ){
		if( as.IsEmpty() ) return;
		MACKEY mac[MACLISTMAX];
        memset(mac, 0, sizeof(mac));
		int N = 0;
		MACKEY *wp = mac;
		MACKEY *mp = mackey;
        LPCSTR p;
        int i;
        for( i = 0; i < Grid->RowCount - 1; i++, mp++ ){
        	p = mp->pKey;
			if( p ){
				if( jstrstr(p, as.c_str()) ){
					memcpy(wp, mp, sizeof(MACKEY)), wp++;
                    N++;
                }
				else {
					p = GetComment(mp);
                    if( p && jstrstr(p, as.c_str()) ){
						memcpy(wp, mp, sizeof(MACKEY)), wp++;
                        N++;
                    }
                }
            }
		}
        mp = mackey;
        for( i = 0; i < Grid->RowCount - 1; i++, mp++ ){
			BOOL f = FALSE;
			MACKEY *rp = mac;
            for( int j = 0; j < N; j++, rp++ ){
				if( !memcmp(rp, mp, sizeof(MACKEY)) ){
					f = TRUE;
                    break;
                }
            }
            if( !f ){
				memcpy(wp, mp, sizeof(MACKEY)), wp++;
                N++;
            }
        }
        memcpy(mackey, mac, sizeof(mackey));
        m_fSort = FALSE;
        Grid->TopRow = 1;
        Grid->Row = 1;
        Grid->Invalidate();
    }
}
//---------------------------------------------------------------------------
// リストの印刷
void __fastcall TMacroKeyDlg::SBPrintClick(TObject *Sender)
{
	CWaitCursor w;
#if 1		// 設定ダイアログ
	TPrinterSetupDialog *pBox = new TPrinterSetupDialog(this);
#else		// 印刷ダイアログ
	TPrintDialog *pBox = new TPrintDialog(this);
	pBox->Options.Clear();
#endif
	OnWave();
    if( pBox->Execute() ){
		OnWave();
		char bf[512];

		Printer()->Title = VERTTL2;
		Printer()->BeginDoc();

		int kcount = Grid->RowCount - 1;			// 項目の数
		int pyw = Printer()->PageHeight;			// 用紙の縦幅
        int pxw = Printer()->PageWidth;				// 用紙の横幅
		int xoff = pxw * 5 / 100;					// 左右のマージン
        int yoff = pyw * 5 / 100;					// 上下のマージン
        int yw = pyw - (yoff*2);					// 印刷領域の縦幅
        int xw = pxw - (xoff*2);					// 印刷領域の横幅
		int pline;
        if( yw > xw ){		// 縦長
			pline = 50;
        }
        else {				// 横長
			pline = 25;
        }
        int lyw = yw / (pline + 2);					// １行あたりのピクセル数
		yw = lyw * (pline + 2);

		TCanvas *pCanvas = Printer()->Canvas;
        pCanvas->Font->Name = Font->Name;
        pCanvas->Font->Charset = Font->Charset;
        pCanvas->Font->Height = -(lyw * 8 / 10);
        pCanvas->Font->Color = clBlack;
        pCanvas->Pen->Color = clBlack;
		int pagemax = (kcount + pline - 1) / pline;	// ページの数
        TRect rc;									// 表の枠の範囲
        rc.Left = xoff; rc.Top = yoff;
        rc.Right = rc.Left + xw; rc.Bottom = rc.Top + yw;
        int xt = (lyw * 1) / 10;					// 文字のオフセット
        int yt = (lyw * 1) / 10;					// 文字のオフセット
        if( !xt ) xt++;
        if( !yt ) yt++;
        int xp = ((rc.Right - rc.Left) * 4 / 10);	// 垂直分割線の位置
		const MACKEY *mp = mackey;
        int n = 0;
		for( int i = 1; i <= pagemax; i++ ){
			OnWave();
			int y = rc.Top;
			for( int j = 0; j < (pline+2); j++, y += lyw ){
				switch(j){
					case 0:			// タイトルの描画
						sprintf(bf, "%s - " VERTTL2, m_Type ? "Conditions" : "Macros");
                        pCanvas->TextOut(rc.Left, y, bf);
						sprintf(bf, "%u/%u", i, pagemax);
                        pCanvas->TextOut(rc.Right - pCanvas->TextWidth(bf), y, bf);
                    	break;
                    case 1:			// 項目名の描画
                        pCanvas->TextOut(rc.Left+xt, y+yt, sys.m_MsgEng ? "Commands" : "コマンド");
                        pCanvas->TextOut(rc.Left+xp+xt, y+yt, sys.m_MsgEng ? "Comments" : "機能/動作");
                    	break;
                    default:		// 各項目の描画
						if( n < kcount ){
	                        pCanvas->TextOut(rc.Left + xt, y+yt, mp->pKey);
    	                    pCanvas->TextOut(rc.Left + xp + xt, y+yt, GetComment(mp));
                            n++; mp++;
                        }
                    	break;
                }
				pCanvas->MoveTo(rc.Left, y+lyw); pCanvas->LineTo(rc.Right, y+lyw);
            }
			y = rc.Top + lyw;
            pCanvas->MoveTo(rc.Left, y);
            pCanvas->LineTo(rc.Right, y);
            pCanvas->LineTo(rc.Right, rc.Bottom);
            pCanvas->LineTo(rc.Left, rc.Bottom);
            pCanvas->LineTo(rc.Left, y);
			pCanvas->MoveTo(rc.Left + xp, y); pCanvas->LineTo(rc.Left + xp, rc.Bottom);
			if( i < pagemax ) Printer()->NewPage();
        }
		Printer()->EndDoc();
		OnWave();
    }
    delete pBox;
}
//---------------------------------------------------------------------------
void __fastcall TMacroKeyDlg::SBPrintMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if( Button == mbRight ){
		TSaveDialog *pBox = new TSaveDialog(this);
		pBox->Options << ofOverwritePrompt;
	    pBox->Options << ofNoReadOnlyReturn;
		if( sys.m_MsgEng ){
			pBox->Title = "Create list file";
			pBox->Filter = "Text Files(*.txt)|*.txt|";
		}
		else {
			pBox->Title = "一覧表ファイルを作成";
			pBox->Filter = "テキストファイル(*.txt)|*.txt|";
		}
		pBox->FileName = "Temp";
		pBox->DefaultExt = "txt";
		pBox->InitialDir = sys.m_TextDir;
		OnWave();
		if( pBox->Execute() == TRUE ){
			OnWave();
			FILE *fp = fopen(AnsiString(pBox->FileName).c_str(), "wt");	//JA7UDE 0428
			if( fp ){
				const MACKEY *mp = mackey;
        	    for( int i = 1; i < Grid->RowCount; i++, mp++ ){
					fprintf(fp, "%d\t%s\t%s\n", i, mp->pKey, GetComment(mp));
            	}
                fclose(fp);
            }
		}
		delete pBox;
    }
}
//---------------------------------------------------------------------------
