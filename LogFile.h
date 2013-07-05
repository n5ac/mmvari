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



//---------------------------------------------------------------------------
#ifndef LogFileH
#define LogFileH

#include "ComLib.h"
//---------------------------------------------------------------------------
#define	MMLOGID	"MMLOG DATA Ver1.00\032"

extern const char	*_band[];

typedef struct {	// Logの動作オプション
	char	m_TimeZone;				// 表示用タイムゾーン

	int		m_UpperName;			// 大文字への変換
	int		m_UpperQTH;				// 大文字への変換
	int		m_UpperREM;				// 大文字への変換
	int		m_UpperQSL;				// 大文字への変換

	int		m_DefMyRST;				// 0-OFF, 1-ON

	int		m_CopyFreq;				// 0-Band, 1-Freq
	int		m_CopyHis;				// 0-599, 1-599001, 2-599UTC
	int		m_CopyName;				// 0-OFF, 1-ON
	int		m_CopyQTH;				// 0-OFF, 1-ON
	int		m_CopyREM;				// 0-OFF, 1-ON
	int		m_CopyQSL;				// 0-OFF, 1-ON
	int		m_CopyREMB4;			// 0-OFF, 1-ON

	int		m_AutoSave;				// 自動セーブ

	int		m_QSOMacroFlag;			// 0-OFF, 1-ON
	int		m_CheckBand;			// 同一バンドでの重複をチェック
	int		m_QSOMacro[5];			// 0-Run 1st, 1-Run 2nd, 2-Run Dupe, 3-S&P 1st, 4-S&P 2nd
	WORD	m_QSOMacroKey[5];
	AnsiString	m_QSOMacroStr[5];

	AnsiString	m_THRTTY;
	AnsiString	m_THSSTV;
	AnsiString	m_THGMSK;
	int     m_THTZ;
	int		m_ClipRSTADIF;
	int		m_DateType;

    int		m_Backup;				// バックアップ作成

	BYTE	m_Hamlog5Len[17];
}LOGSET;

#define	MLCALL	16		/* ｺｰﾙｻｲﾝの長さ		*/
#define	MLRST	20		/* RSTナンバの長さ	*/
#define	MLREM	56		/* 記事の長さ		*/
#define	MLNAME	16		/* 名前の長さ		*/
#define	MLQTH	28		/* ＱＴＨの長さ		*/
#define	MLQSL	54		/* ＱＳＬの長さ		*/
#define	MLPOW	4		/* 電力				*/
#define	MLOPT	8		/* オプション		*/

#define MODEMAX	48

#define	YEAR(c)	(((c)<50)?(2000+(c)):(1900+(c)))

#pragma option -a-	// パックの指示
typedef struct {			/* ＭＭＬＯＧデータ形式	*/
	BYTE	year;		/* 年		*/
	WORD	date;			/* 日付		*/
	WORD	btime;			/* 開始時刻	*/
	WORD	etime;			/* 終了時刻	*/
	char	call[MLCALL+1];	/* ｺｰﾙｻｲﾝ	*/
	char	ur[MLRST+1];	/* T ﾘﾎﾟｰﾄ	*/
	char	my[MLRST+1];	/* R ﾘﾎﾟｰﾄ	*/
	BYTE	band;		/* ﾊﾞﾝﾄﾞ	*/
	SHORT	fq;				/* 周波数	*/
	BYTE	mode;		/* ﾓｰﾄﾞ		*/
	char	pow[MLPOW+1];	/* 電力		*/
	char	name[MLNAME+1];	/* 名前		*/
	char	qth[MLQTH+1];	/* QTH		*/
	char	qsl[MLQSL+1];	/* QSL		*/
	char	send;			/* QSL 送信	*/
	char	recv;			/* QSL 受信	*/
	char	cq;				/* cq/call	*/
	WORD	env;			/* 自局環境	*/
	char	rem[MLREM+1];	/* 記事		*/
	char	opt1[MLOPT+1];	/* ｵﾌﾟｼｮﾝ	*/
	char	opt2[MLOPT+1];	/* ｵﾌﾟｼｮﾝ2	*/
}SDMMLOG;

#define	FHDOFF	256			/* 先頭のヘッダのオフセット		*/
#define	UMODEMAX	32
typedef struct {
	char	id[20];			/* ファイルバージョン			*/
	char	dmy[4];			/* ダミー領域					*/
	USHORT	mlt;			/* マルチ情報のサイズ			*/
	char	td;				/* 時差コード					*/
	char	hash;			/* ハッシュデータ格納フラグ		*/
	long	size;			/* データサイズ					*/
	char	master;			/* マスターファイルフラグ		*/
	char	dm2[15];		/* ダミー２						*/
	char	mode[UMODEMAX][6];	/* ユーザ定義モード				*/
}FHD;
#pragma option -a.	// パック解除の指示

class CIndex
{
private:
	int		m_IndexMax;		// 確保中のインデックスの数
	int		m_IndexCnt;		// 現在のインデックスの数
	LPSTR	pIndex;			// インデックス領域のポインタ(16バイトインデックス)
	BYTE	*pMult;			// マルチ情報のポインタ
public:
	__fastcall CIndex();
	__fastcall ~CIndex();
	void __fastcall AllocIndex(int n);
	void __fastcall ClearIndex(void);
	void __fastcall WriteIndex(LPCSTR pCall, int n);
	void __fastcall ReadIndex(LPSTR pCall, int n);
	LPSTR __fastcall PointIndex(int n);
	void __fastcall AddBlock(LPCSTR p, int len);
	int __fastcall ReadIndex(int handle, FHD *hp);
	void __fastcall MakeIndex(int handle, FHD *hp);
	int __fastcall WriteIndex(int handle, FHD *hp);
};

#define	FINDMAX		32768
class CLogFind
{
public:
	int			m_FindCnt;
	int			m_FindCmp1Max;
	int			m_FindCmp2Max;
	int			m_FindStr1Max;
	int			m_FindStr2Max;

	int			*pFindTbl;
	AnsiString	m_FindStr;
public:
	__fastcall CLogFind();
	__fastcall ~CLogFind();
	inline int __fastcall GetCount(void){return m_FindCnt;};
	inline void __fastcall Clear(void){
		m_FindCnt = m_FindCmp1Max = m_FindCmp2Max = m_FindStr1Max = m_FindStr2Max = 0;
	};
	int __fastcall Add(int n);
	void __fastcall Ins(int n);

	inline void __fastcall Write(int n){
		pFindTbl[m_FindCnt] = n;
		m_FindCnt++;
	};
	inline void __fastcall SetText(LPCSTR p){
		if( p != m_FindStr.c_str() ){
			m_FindStr = p;
		}
	};
	inline LPCSTR __fastcall GetText(void){
		return m_FindStr.c_str();
	};
	inline void __fastcall ClearText(void){
		m_FindStr = "";
	};
};

class CLogFile
{
private:
	int		m_Open;			// オープンフラグ
	int		m_EditFlag;		// 編集フラグ
	int		m_Handle;		// ファイルハンドル
	CIndex	m_Index;		// 現在オープン中のインデックス
	FHD		m_fhd;			// 現在オープン中のファイルヘッダ
	AnsiString	m_Name;		// ログファイルの名前
	char	m_modebuf[8];
public:
	AnsiString	m_FileName;	// ログファイルの名前（フルパス）

	int		m_CurNo;
	int		m_CurChg;
	SDMMLOG	m_sd;
	SDMMLOG	m_bak;
	SDMMLOG	m_asd;

	CLogFind	m_Find;		// カレント検索データ
	LOGSET		m_LogSet;
private:
	void __fastcall SetHisRST(SDMMLOG *sp);
	void __fastcall InitHeader(void);

public:
	__fastcall CLogFile();
	__fastcall ~CLogFile();

	void __fastcall DoBackup(void);

	int __fastcall Open(LPCSTR pName, BOOL fNew);
	int __fastcall Close(void);

	void __fastcall ReadIniFile(LPCSTR pKey, TMemIniFile *pIniFile);
	void __fastcall WriteIniFile(LPCSTR pKey, TMemIniFile *pIniFile);

	void __fastcall MakeIndex(void);

	inline int __fastcall IsEdit(void){return m_EditFlag;};
	void __fastcall MakePathName(LPCSTR pName);
	void __fastcall MakeName(LPCSTR pName);
	inline LPCSTR __fastcall GetName(void){return m_Name.c_str();};

	inline int __fastcall GetCount(void){return m_fhd.size;};
	inline int __fastcall IsOpen(void){return m_Open;};
	int __fastcall GetData(SDMMLOG *sp, int n);
	int __fastcall PutData(SDMMLOG *sp, int n);

	LPCSTR __fastcall GetDateString(SDMMLOG *sp, int sw);
	inline LPCSTR __fastcall GetDateString(SDMMLOG *sp){ return GetDateString(sp, m_LogSet.m_DateType); };
	LPCSTR __fastcall GetTimeString(WORD d);


	LPCSTR __fastcall GetModeString(BYTE m);
	void __fastcall SetMode(SDMMLOG *sp, LPCSTR s);

	LPCSTR __fastcall GetFreqString(BYTE b, short fq);
	void __fastcall SetFreq(SDMMLOG *sp, LPCSTR p);
	LPCSTR __fastcall GetOptStr(int n, SDMMLOG *sp);
	void __fastcall SetOptStr(int n, SDMMLOG *sp, LPCSTR pOpt);

	void __fastcall CopyAF(void);

	void __fastcall SortDate(int bb, int eb);
	int __fastcall FindSameBand(BOOL fMode);
	int __fastcall FindSameDate(void);

	void __fastcall FindStrSet(CLogFind *fp, LPCSTR pCall);
	int __fastcall FindCmpSet(CLogFind *fp, LPCSTR pCall);
	int __fastcall FindClipSet(CLogFind *fp, LPCSTR pCall);
	int __fastcall FindSet(CLogFind *fp, LPCSTR pCall);
	int __fastcall Find(LPCSTR pCall, int b, int dir);
	int __fastcall IsAlready(LPCSTR pCall);
	void __fastcall DeleteAll(void);
	void __fastcall DeleteLast(void);
	void __fastcall Delete(int top, int end);
	void __fastcall Insert(int n, SDMMLOG *sp);

	void __fastcall InitCur(void);
	void __fastcall SetLastPos(void);

	int __fastcall ReadAscii(SDMMLOG *sp, LPSTR p);

};

void __fastcall JSTtoUTC(int &Year, int &Mon, int &Day, int &Hour);
void __fastcall JSTtoUTC(SDMMLOG *sp);
void __fastcall UTCtoJST(int &Year, int &Mon, int &Day, int &Hour);
void __fastcall UTCtoJST(SDMMLOG *sp);
void __fastcall UTCtoJST(SYSTEMTIME *tp);
void __fastcall mBandToBand(SDMMLOG *sp, LPCSTR p);
LPCSTR __fastcall FreqTomBand(SDMMLOG *sp);
int __fastcall GetLMode(BYTE m);

extern	CLogFile	Log;
extern	const char	MONN[];
extern	const char	MONU[];
#endif

