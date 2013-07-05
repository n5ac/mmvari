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



#ifndef ComLibH
#define ComLibH
#include <ComCtrls.hpp>
#include <inifiles.hpp>
#include <Grids.hpp>
#include <Buttons.hpp>
#include <values.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mbstring.h>
#include <dir.h>
#include <Wingdi.h>	//ja7ude 0522

#define VERBETA ""
#define	VERNO	"0.45"
#define	INI_SCHEMA	3

#define DEBUG		    TRUE
#define	MEASIMD			FALSE
#define	LOGFFT			FALSE
#define	SHOWERRCOUNT	FALSE
#define	FORCELANG		0
#if DEBUG
#include <assert.h>
#define ASSERT(c)   assert(c)
#else
#define ASSERT(c)
#endif

#if SHOWERRCOUNT
#define	EPHASE(c)	sys.m_ErrPhase = c
enum {
	P_NULL,
	P_DEMBPF,
    P_DEMAGC,
	P_DEMLPF,
    P_DEMAFC,
    P_DEMLOCK,
    P_DEMVCO,
    P_DEMOUT,
    P_SUBCREATE,
    P_SUBVIEW,
};
#else
#define	EPHASE(c)
#endif

#define	VERID	"beta ver "VERNO
#define	VERTTL2	"MMVARI "VERID VERBETA
#define	VERTTL  VERTTL2" (C) JE3HHT 2004-2010."

#define	WM_WAVE		WM_USER+400
#define CM_CRADIO   WM_USER+400
#define	CM_CMML		WM_USER+401
#define	CM_CMMR		WM_USER+402

enum {
	waveIN,
    waveOUT,
    waveCloseFileEdit,
    waveCodeView,
    wavePlayDlg,
    waveCloseRxView,
    waveSwapRxView,
    waveClockAdj,
    waveSeekMacro,
    waveDoMacro,
    waveLoadMacro,
#if DEBUG
    waveRepeatMacro = 1024,
#endif
};

enum {
	macOnTimer,
    macOnPTT,
    macOnQSO,
    macOnFind,
	macOnBand,
    macOnStart,
    macOnExit,
    macOnMode,
    macOnSpeed,
    macOnClick,
    macOnFFTScale,
    macOnEnd,
};
//extern  char    g_MMLogDir[256];

extern	const char	MONN[];
extern	const char	MONU[];
extern	const LPCSTR g_tLogModeTable[];
extern	const LPCSTR g_tDispModeTable[];
extern	const LPCSTR g_tMacEvent[];
extern	const LPCSTR g_tSoundCH[];
extern	const LPCSTR g_tOnOff[];

#ifndef LPCUSTR
typedef const unsigned char *	LPCUSTR;
typedef unsigned char *	LPUSTR;
#endif

#define ABS(c)	(((c)<0)?(-(c)):(c))
#define AN(p)	(sizeof(p)/sizeof(*(p)))
#define	CR		0x0d
#define	LF		0x0a
#define	TAB		'\t'

#define FSBOLD      1
#define FSITALIC    2
#define FSUNDERLINE 4
#define FSSTRIKEOUT 8

typedef struct {
	AnsiString	m_Name;
    BYTE		m_Charset;
    int			m_Height;
    DWORD		m_Style;
}FONTDATA;

enum {
	kkTX,
    kkTXOFF,
    kkEOF,
};
typedef struct {
	WORD	Key;
	LPCSTR	pName;
}DEFKEYTBL;
extern const DEFKEYTBL KEYTBL[];
LPCSTR __fastcall ToDXKey(LPCSTR s);
LPCSTR __fastcall ToJAKey(LPCSTR s);
LPCSTR __fastcall GetKeyName(WORD Key);
WORD __fastcall GetKeyCode(LPCSTR pName);

enum {
	fmJA,
    fmHL,
    fmJOHAB,
    fmBV,
    fmBY,
    fmEND,
};

typedef struct {
	AnsiString	m_CallSign;

	WORD		m_DefKey[kkEOF];

	BOOL		m_PTTLock;

    int			m_bFSKOUT;
    int			m_bINVFSK;
    AnsiString	m_PTTCOM;

	int			m_DefaultMode;
    int			m_LoopBack;
    int			m_MsgEng;

	LCID		m_LCID;
    WORD		m_wLang;
	BOOL		m_fShowLangMsg;
	BOOL		m_fBaseMBCS;
	DWORD		m_dwVersion;
	BOOL		m_WinNT;
	BOOL		m_WinVista;

    AnsiString	m_FontName;
    BYTE		m_FontCharset;

	BYTE		m_Charset;
    BOOL		m_fFontFam;
    BOOL		m_tFontFam[fmEND];

    int			m_OptionPage;
	int			m_EventIndex;

    char		m_BgnDir[256];
    char		m_SoundDir[256];
    char		m_ExtLogDir[256];
    char		m_LogDir[256];
	char		m_TextDir[256];
	char		m_MacroDir[256];

	int			m_AutoTimeOffset;
	int			m_TimeOffset;
	int			m_TimeOffsetMin;
	int			m_LogLink;
	AnsiString	m_LogName;

	BOOL		m_fRestoreSubChannel;
	BOOL		m_fFixWindow;
    RECT		m_rcWindow;		// ウインドウの配置
	int			m_WindowState;	// ウインドウの状態

    BOOL		m_fAutoTS;

	BOOL		m_fSendSingleTone;

	BOOL		m_EnableMouseWheel;
	AnsiString	m_SoundIDRX;
    AnsiString	m_SoundIDTX;
	AnsiString	m_SoundMMW;		// MMW名

    AnsiString	m_AS;
	AnsiString	m_MacEvent[macOnEnd];

    BOOL		m_fPlayBack;
	int			m_PlayBackSpeed;
	BOOL		m_fShowCtrlCode;

    BOOL		m_MFSK_Center;
	BOOL		m_MFSK_SQ_Metric;

    BOOL		m_MacroError;
	int			m_MaxCarrier;
    int			m_DecCutOff;
	int			m_MacBuffSize;
    int			m_OnTimerInterval;
	RECT		m_PosMacEdit;

#if DEBUG
	BOOL		m_test;
    int			m_testSN;
    AnsiString	m_testName;
    double		m_testGain;
    double		m_testNoiseGain;
    int			m_testCarrier1;
    int			m_testCarrier2;
    int			m_testDB2;
    double		m_testGain2;
    int			m_testQSBTime;
    int			m_testQSBDB;
    BOOL		m_test500;
    BOOL		m_testPhase;
    int			m_testClockErr;
#endif
#if SHOWERRCOUNT
    int			m_ErrPhase;
#endif
}SYSSET;
extern SYSSET   sys;

typedef struct {
	int			WMax;
	int			Max;
    int			Sum;
    int			dBSum;
    int			dBMax;
    int			dBWMax;

    int			Timer;
    int         VW;
	int			LimitL, LimitH;
    int			Sig;
    int			DispSig;
}FFTSTG;

///---------------------------------------------------------
typedef union {
	struct {
		BYTE    r;
		BYTE    g;
		BYTE    b;
		BYTE    s;
	}b;
	DWORD       d;
	TColor      c;
}UCOL;
///---------------------------------------------------------
///  テキスト文字列ストリーマー
class CTextString
{
private:
	LPCSTR	rp;
public:
	inline __fastcall CTextString(LPCSTR p){
		rp = p;
	};
	inline __fastcall CTextString(AnsiString &As){
		rp = As.c_str();
	};
	int __fastcall LoadText(LPSTR tp, int len);
};

class CWebRef
{
private:
	AnsiString	HTML;
public:
	__fastcall CWebRef();
	inline bool __fastcall IsHTML(void){
		return !HTML.IsEmpty();
	};
	void __fastcall ShowHTML(LPCSTR url);
};

///---------------------------------------------------------
///  テキストバッファストリーマー
class StrText{
public:
	char	*Bp;
	char	*Wp;
	inline __fastcall StrText(int max){
		Bp = new char[max];
		Clear();
	};
	inline __fastcall ~StrText(){
		delete Bp;
	};
	inline char *Printf(char *ct, ...){
		va_list	pp;

		va_start(pp, ct);
		vsprintf(Wp, ct, pp );
		va_end(pp);
		ct = Wp;
		Wp += strlen(Wp);
		return(ct);
	};
	inline void __fastcall Add(LPCSTR sp){
		strcpy(Wp, sp);
		Wp += strlen(Wp);
	};
	inline void __fastcall Write(void *p, int n){
		memcpy(Wp, p, n);
		Wp += n;
		*Wp = 0;
	};
	inline int __fastcall GetCnt(void){
		return Wp - Bp;
	};
	inline void __fastcall Clear(void){
		Wp = Bp;
		*Wp = 0;
	};
	inline char *__fastcall GetText(void){
		return Bp;
	};
};

class CWaitCursor
{
private:
	TCursor sv;
public:
	__fastcall CWaitCursor();
	__fastcall ~CWaitCursor();
	void __fastcall Delete(void);
	void __fastcall Wait(void);
};

///---------------------------------------------------------
///  コントロールのアラインの管理クラス
class CAlign
{
private:
	int		BTop, BLeft;
	int		BWidth, BHeight;
	int		OTop, OLeft;
	int		OWidth, OHeight;
	int		OFontHeight;
	double	m_FontAdj;

	TControl	*tp;
	TFont		*fp;
public:
	inline __fastcall CAlign(void){
		tp = NULL;
		fp = NULL;
		m_FontAdj = 1.0;
	};
	inline __fastcall ~CAlign(){
	};
	void __fastcall InitControl(TControl *p, TControl *pB, TFont *pF = NULL);
	void __fastcall InitControl(TControl *p, RECT *rp, TFont *pF = NULL);
	void __fastcall NewAlign(TControl *pB);
	inline double __fastcall GetFontAdj(void){return fabs(m_FontAdj);};
	inline TControl *__fastcall GetControl(void){return tp;};
	void __fastcall NewAlign(TControl *pB, double hx);
	void __fastcall NewFont(AnsiString &FontName, BYTE Charset, TFontStyles fs);
	void __fastcall NewFixAlign(TControl *pB, int XR);
	void __fastcall Resume(void);
};

///---------------------------------------------------------
///  コントロールのアラインの管理クラス
class CAlignList
{
private:
	int		Max;
	int		Cnt;
	CAlign	**AlignList;
	void __fastcall Alloc(void);
public:
	__fastcall CAlignList(void);
	__fastcall ~CAlignList();
	void __fastcall EntryControl(TControl *tp, TControl *pB, TFont *pF = NULL);
	void __fastcall EntryControl(TControl *tp, RECT *rp, TFont *pF = NULL);
	void __fastcall EntryControl(TControl *tp, int XW, int YW, TFont *pF = NULL);
	void __fastcall NewAlign(TControl *pB);
	double __fastcall GetFontAdj(TControl *pB);
	void __fastcall NewAlign(TControl *pB, TControl *pS, double hx);
	void __fastcall NewFont(AnsiString &FontName, BYTE Charset, TFontStyles fs);
	void __fastcall NewFixAlign(TControl *pB, int XR);
	void __fastcall Resume(TControl *pB);
};

int __fastcall FindStringTableStrictly(const LPCSTR _tt[], LPCSTR pName, int max);
int __fastcall FindStringTable(const LPCSTR _tt[], LPCSTR pName, int max);
int __fastcall GetModeIndex(LPCSTR pName);
int __fastcall IsFile(LPCSTR pName);
BOOL __fastcall StrWindowsVer(LPSTR t);

//---------------------------------------------------------------------------
int __fastcall SetTimeOffsetInfo(int &Hour, int &Min);
void __fastcall AddjustOffset(SYSTEMTIME *tp);
void __fastcall GetUTC(SYSTEMTIME *tp);
void __fastcall GetLocal(SYSTEMTIME *tp);
LPSTR __fastcall StrDupe(LPCSTR s);

LPUSTR __fastcall jstrupr(LPUSTR s);
LPUSTR __fastcall jstrlwr(LPUSTR s);
inline LPSTR __fastcall jstrupr(LPSTR s){return (LPSTR)jstrupr(LPUSTR(s));};
inline LPSTR __fastcall jstrlwr(LPSTR s){return (LPSTR)jstrlwr(LPUSTR(s));};
LPCSTR __fastcall ConvAndChar(LPSTR t, LPCSTR p);

void __fastcall OnWave(void);
int __fastcall SetTimeOffsetInfo(int &Hour, int &Min);
WORD __fastcall AdjustRolTimeUTC(WORD tim, char c);
void __fastcall DrawMessage(TCanvas *pCanvas, int XW, int YW, LPCSTR p, int Pos);
void __fastcall FormCenter(TForm *tp, int XW, int YW);
void __fastcall FormCenter(TForm *tp, TForm *pOwner);
void __fastcall FormCenter(TForm *tp);
char *__fastcall lastp(char *p);
char *__fastcall clipsp(char *s);
LPCSTR __fastcall _strdmcpy(LPSTR t, LPCSTR p, char c);
const char *__fastcall StrDlmCpy(char *t, const char *p, char Dlm, int len);
const char *__fastcall StrDlmCpyK(char *t, const char *p, char Dlm, int len);
void __fastcall StrCopy(LPSTR t, LPCSTR s, int n);
char __fastcall LastC(LPCSTR p);
LPCSTR __fastcall GetEXT(LPCSTR Fname);
void __fastcall SetEXT(LPSTR pName, LPSTR pExt);
BOOL __fastcall CheckEXT(LPCSTR pName, LPCSTR pExt);
void __fastcall SetCurDir(LPSTR t, int size);
void __fastcall SetDirName(LPSTR t, LPCSTR pName);
void __fastcall GetFileName(AnsiString &Name, LPCSTR pName);
void __fastcall GetFullPathName(AnsiString &as, LPCSTR pName);
void __fastcall GetFullPathName(AnsiString &as, LPCSTR pName, LPCSTR pFolder);
void __fastcall LimitInt(int *pInt, int min, int max);
void __fastcall LimitDbl(double *pInt, double min, double max);
LPCSTR __fastcall StrDbl(LPSTR bf, double d);
void __fastcall AdjustAS(AnsiString *pAS);
void __fastcall ClipLF(LPSTR sp);
void __fastcall DelCR(AnsiString &ws, LPCSTR s);
void __fastcall DeleteComment(LPSTR bf);
LPSTR __fastcall FillSpace(LPSTR s, int n);
LPCSTR __fastcall SkipToValue(LPCSTR sp);
LPSTR __fastcall SkipSpace(LPSTR sp);
LPCSTR __fastcall SkipSpace(LPCSTR sp);
LPSTR __fastcall DelLastSpace(LPSTR t);
LPSTR __fastcall StrDlm(LPSTR &t, LPSTR p);
LPSTR __fastcall StrDlm(LPSTR &t, LPSTR p, char c);
void __fastcall ChgString(LPSTR t, char a, char b);
void __fastcall DelChar(LPSTR t, char a);
int __fastcall atoin(const char *p, int n);
int __fastcall htoin(const char *p, int n);

int __fastcall GetActiveIndex(TPageControl *pp);
void __fastcall SetActiveIndex(TPageControl *pp, int page);
int __fastcall InvMenu(TMenuItem *pItem);

void InfoMB(LPCSTR fmt, ...);
void ErrorMB(LPCSTR fmt, ...);
void WarningMB(LPCSTR fmt, ...);
int YesNoMB(LPCSTR fmt, ...);
int YesNoCancelMB(LPCSTR fmt, ...);
int OkCancelMB(LPCSTR fmt, ...);
void __fastcall ErrorFWrite(LPCSTR pName);

int __fastcall RemoveL2(LPSTR t, LPSTR ss, LPCSTR pKey, int size);
void __fastcall AddL2(LPSTR t, LPCSTR pKey, LPCSTR s, UCHAR c1, UCHAR c2, int size);
int __fastcall IsJA(const char *s);
LPCSTR __fastcall ClipCall(LPCSTR s);
void __fastcall chgptb(LPSTR s);
LPCSTR __fastcall ClipCC(LPCSTR s);
void __fastcall Yen2CrLf(AnsiString &ws, AnsiString &cs);
void __fastcall CrLf2Yen(AnsiString &ws, AnsiString &cs);

void __fastcall GetLogFont(LOGFONT *pLogfont, TFont *pFont);
void __fastcall AddStyle(AnsiString &as, BYTE charset, DWORD style);
TFontStyles __fastcall Code2FontStyle(int code);
int __fastcall FontStyle2Code(TFontStyles style);
void __fastcall LoadFontFromInifile(TFont *pFont, LPCSTR pSect, TMemIniFile *pIniFile);
void __fastcall SaveFontToInifile(TFont *pFont, LPCSTR pSect, TMemIniFile *pIniFile);
UCOL __fastcall GetGrade2(UCOL s[2], int x, int xw);
void __fastcall CheckFontCharset(void);
BOOL __fastcall SetLangFont(TFont *pFont, WORD wLang);
void __fastcall SetMBCP(BYTE charset);

void __fastcall Font2FontData(FONTDATA *pData, TFont *pFont);
void __fastcall FontData2Font(TFont *pFont, FONTDATA *pData);
void __fastcall LoadFontFromInifile(FONTDATA *pData, LPCSTR pSect, TMemIniFile *pIniFile);
void __fastcall SaveFontToInifile(FONTDATA *pData, LPCSTR pSect, TMemIniFile *pIniFile);

void __fastcall NumCopy(LPSTR t, LPCSTR p);
int __fastcall IsNumbs(LPCSTR p);
int __fastcall IsNumbAll(LPCSTR p);
int __fastcall IsAlphas(LPCSTR p);
int __fastcall IsAlphaAll(LPCSTR p);
int __fastcall IsRST(LPCSTR p);
int __fastcall IsCallChar(char c);
int __fastcall IsCall(LPCSTR p);
int __fastcall IsJA(const char *s);
LPCSTR __fastcall ClipCall(LPCSTR s);
LPCSTR __fastcall ClipCC(LPCSTR s);
/*
inline LPUSTR __fastcall StrDlm(LPUSTR &t, LPUSTR p, char c){return (LPUSTR)StrDlm(LPSTR(t), LPSTR(p), c);};
inline LPUSTR __fastcall StrDlm(LPUSTR &t, LPUSTR p){return (LPUSTR)StrDlm(LPSTR(t), LPSTR(p));};
inline LPUSTR __fastcall StrDlm(LPUSTR &t, LPSTR p, char c){return (LPUSTR)StrDlm(LPSTR(t), p, c);};
inline LPUSTR __fastcall StrDlm(LPUSTR &t, LPSTR p){return (LPUSTR)StrDlm(LPSTR(t), p);};
*/
void __fastcall SetGroupEnabled(TGroupBox *gp);
void __fastcall KeyEvent(const short *p);

int __fastcall Calc(double &d, LPCSTR p);
int __fastcall CalcI(int &d, LPCSTR p);
int __fastcall CalcU(int &d, LPCSTR p);

void __fastcall GPS2SystemTime(ULONG gps, SYSTEMTIME *sp);
ULONG __fastcall SystemTime2GPS(SYSTEMTIME *sp);

void __fastcall SetComboBox(TComboBox *pCombo, LPCSTR pList);
void __fastcall GetComboBox(AnsiString &as, TComboBox *pCombo);

void __fastcall ConvDummy(AnsiString &ws, LPCSTR p, LPCSTR pNames, LPCSTR pValues);

typedef struct {
	LPSTR   pStr;
	int     Count;
}MULTSET;

class CMULT
{
private:
	int     m_CNT;
	int     m_MAX;
	MULTSET *m_pBase;
//    LPSTR   *m_pStr;
//    LPSTR   m_pStr[MULTMAX];
private:
	void __fastcall Alloc(void);

public:
	__fastcall CMULT();
	__fastcall ~CMULT(){
		Clear();
	};
	void __fastcall Clear(void);
	int __fastcall Add(LPCSTR pKey);
	int __fastcall Set(LPCSTR pKey, int n);
	int __fastcall GetCount(void){
		return m_CNT;
	};
	void __fastcall Sort(void);
	void __fastcall SortCount(void);
	LPCSTR  __fastcall GetText(int n){
		if( (n >= 0) && (n < m_CNT) ){
			return m_pBase[n].pStr;
		}
		else {
			return NULL;
		}
	};
	int __fastcall GetCount(int n){
		if( (n >= 0) && (n < m_CNT) ){
			return m_pBase[n].Count;
		}
		else {
			return 0;
		}
	};
	int __fastcall GetCount(LPCSTR pKey);
	int __fastcall GetTotal(void);
};

//---------------------------------------------------------------------------
class CCond
{
private:
	int		m_CIFMAX;
	int		m_f;			/* 条件のカウンタ				*/
	int		m_fc;			/* 偽の場合のレベルカウンタ		*/
	int		*m_pE;			/* 状態のネスト配列				*/
	int		*m_pAF;		/* 展開フラグのネスト配列		*/
	int		m_err;		/* エラーチェック				*/
	BOOL	m_fCond;

    int	__fastcall BgnCond(void);
public:
    int	__fastcall CondJob(const char *p, TSpeedButton *pButton);
	int __fastcall CondStr(AnsiString &Cond, const char *p);
	inline void __fastcall SetCond(void){m_fCond=TRUE;};
	inline BOOL __fastcall IsCond(void){return m_fCond;};
	inline int __fastcall GetLevel(void){return m_f;};
    inline void __fastcall ClearLevel(void){m_f = 0;};

	__fastcall CCond(int max);
    __fastcall ~CCond(void);
};
//---------------------------------------------------------------------------
#pragma option -a-	// パックの指示
typedef struct {
    WORD		hash;
    BYTE        mtype;
    BYTE		dummy;
	LPCSTR		pName;
    LPCSTR		pString;
    LPCSTR		pPara;
}VALDATA;
#pragma option -a.	// パック解除の指示

#define	_VAL_NORMAL		1
#define	_VAL_INIFILE	2
#define	_VAL_PROC		4

class CVal
{
private:
	VALDATA	*m_pBase;
    int		m_Max;
    int		m_Count;
    BOOL	m_fHandleProc;
private:
	void __fastcall Alloc(void);

public:
	__fastcall CVal();
    __fastcall ~CVal(){Delete();};

    inline int __fastcall GetCount(void){return m_Count;};
    inline BOOL __fastcall IsHandleProc(void){return m_fHandleProc;};

	void __fastcall Delete(void);
	void __fastcall Delete(BYTE mtype);
	BOOL __fastcall Delete(LPCSTR pName, BYTE mtype);

	int __fastcall FindName(LPCSTR pName, BYTE mtype);
    void __fastcall RegisterString(LPCSTR pName, LPCSTR pString, LPCSTR pPara, BYTE mtype);

	void __fastcall WriteInifile(TMemIniFile *pIniFile, LPCSTR pSect);
	void __fastcall ReadInifile(TMemIniFile *pIniFile, LPCSTR pSect);

    inline LPCSTR __fastcall GetString(int n){ return m_pBase[n].pString; };
    inline LPCSTR __fastcall GetPara(int n){ return m_pBase[n].pPara; };
};
//---------------------------------------------------------------------------
#pragma option -a-	// パックの指示
typedef struct {
	WORD	cMBCS;
    WORD	cASCII;
}CONVALPHA;
#pragma option -a.	// パック解除の指示

class CMBCS
{
private:
	BYTE	*m_pLead;
    int		m_Charset;
public:
	__fastcall CMBCS();
    __fastcall ~CMBCS();
	void __fastcall Create(BYTE charset);
	void __fastcall Create(void);
    BOOL __fastcall IsLead(BYTE c);
    BOOL __fastcall IsLead(const unsigned char *p);
	int __fastcall ConvAlpha(int code);

    inline BOOL __fastcall IsCreate(void){return m_pLead != NULL;};
    inline void __fastcall SetCharset(BYTE charset){m_Charset = charset;};
};

//---------------------------------------------------------------------------
// CLIBLクラス
typedef struct {
	LPCSTR  pName;
	//HANDLE  hLib;	//ja7ude 0522
	HINSTANCE  hLib;
}LIBD;

typedef void (__stdcall *tmmMacro)(HWND hWnd, LPSTR t, LPCSTR p);
class CLIBL {
public:
	int     m_AMax;
	int     m_Count;
	LIBD    *m_pBase;
private:
	void __fastcall Alloc(void);
	void __fastcall Add(LPCSTR pName, HANDLE hLib);
public:
	CLIBL(){
		m_pBase = NULL;
		Delete();
	};
	~CLIBL(){
		Delete();
	};
	void __fastcall Delete(void);
	HANDLE __fastcall LoadLibrary(LPCSTR pName);
	void __fastcall DeleteLibrary(HANDLE hLib);
};
#endif

