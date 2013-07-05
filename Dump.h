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
#ifndef DumpH
#define DumpH

#include "stdio.h"
#include "ComLib.h"
//---------------------------------------------------------------------------
#define	DUMPCOLMAX	160
#define	DUMPCRLIMIT	4
enum {
	csNONE,
    csCARET,
    csSTATIC,
};
enum {
	dmpMoveTOP=0x1000,
	dmpMoveLAST,
};
//---------------------------------------------------------------------------
class CDump
{
private:
	int		**m_pBase;

	TRect	m_rcScreen;
	BOOL	m_fDisEvent;

    int		m_ScreenLineMax;
    int		m_ScreenColMax;
    int		m_StgLineMax;
//    int		m_StgColMax;
	int		m_FontYW;
	int		m_FontXW;

	BOOL	m_fLastCP;
	int		m_MLine;		// åªç›ÇÃç≈èIçsêî
	int		m_MCol;			// åªà§ÇÃç≈èIåÖ

	int		m_WTop;
    int		m_WLine;
    int		m_WCol;

    int		m_DTop;
	int		m_X;
    int		m_Y;

    int		m_RLine;
    int		m_RCol;
	int		m_RX;
    int		m_RY;

	int		m_fSelText;
    int		m_fSelect;
    int		m_SLine;
    int		m_SCol;

    int		m_FLine;

    BOOL	m_fWindow;

	char	m_Buffer[16];

    int		m_CursorType;
    int		m_Cursor;

    BOOL	m_fKanji;
    BOOL	m_fCR;
    BOOL	m_fActive;
	int		m_CRCount;

    FILE	*m_wfp;

	UINT	m_nTimerID;
    int		m_MouseWheelCount;

	BOOL	m_fShowCtrlCode;

	BOOL	m_fRTTY;
    int		m_SMX, m_SML;
public:
    UCOL	m_Color[5];
    TColor	m_CursorColor;
	BOOL	m_fConvAlpha;
private:
	HWND		m_hWnd;
	TPanel		*m_pPanel;
	TFont		*m_pFont;
	TCanvas		*m_pCanvas;
    TPaintBox	*m_pPaintBox;
    TScrollBar	*m_pScrollBar;
    CMBCS		*m_pMBCS;
    int		m_ScreenXW;
    int		m_ScreenYW;
    int		m_ScreenLXW;
    LOGFONT	m_LogFont;
private:
	void __fastcall DeleteStg(void);
	void __fastcall SetBuffer(int c);
	int __fastcall CharWidth(int c);
	void __fastcall MemScroll(void);
	void __fastcall WriteLine(AnsiString &as, int line);
	void __fastcall WriteLine(int line);
	void __fastcall PutScroll(void);
	int __fastcall DrawChar(int x, int y, int c, BOOL f);
	void __fastcall DrawCursor(int x, int y, int sw);
	void __fastcall SetCursorPos(void);
	void __fastcall FollowRP(void);
	BOOL __fastcall BackSpace(void);
	void __fastcall InsChar(int c, int col);
	void __fastcall DeleteCR2(void);
	void __fastcall SetCompositionWindowPos(BOOL fFont);
	void __fastcall DeleteChar(BOOL fForce);
	void __fastcall DoWheel(int z);
	BOOL __fastcall IsSpace(int c, int ci);

	int *__fastcall CreateContP(int line, int col, int add);
	void __fastcall CloseContP(int line, int col, int *bp, int offset);

public:
	__fastcall CDump(void);
    __fastcall ~CDump();
    void __fastcall Create(HWND hWnd, TPanel *pPanel, TPaintBox *pPaintBox, TFont *pFont, TScrollBar *pBar, int line);
	BOOL __fastcall PutChar(int c, int col);
	BOOL __fastcall PutKey(char c, int col);
	void PutStatus(int col, LPCSTR pFmt, ...);
	int __fastcall GetCharCount(BOOL fTX);
	int __fastcall GetChar(BOOL fDraw);
	int __fastcall GetCharNB(void);
	void __fastcall Clear(void);
	void __fastcall Paint(void);
	void __fastcall SetFont(TFont *pFont);
	void __fastcall Resize(void);
	void __fastcall SetScrollBar(void);
	void __fastcall OnScrollBarChange(void);
	BOOL __fastcall OnTimer(void);
	BOOL __fastcall OnMouseWheel(int z);
	void __fastcall SetCursorType(int type);
    void __fastcall Cursor(int sw);
    void __fastcall CreateCaret(void);
    void __fastcall DestroyCaret(void);
	void __fastcall MoveCursor(int Key);
	void __fastcall MoveCursor(int SX, int SY, BOOL fDown);
	inline __fastcall DeleteChar(void){DeleteChar(FALSE);};
	BOOL __fastcall CanEdit(void);

	BOOL __fastcall ClearAllSelect(BOOL fPaint);
	BOOL __fastcall IsWindowText(LPCSTR pText);
	BOOL __fastcall GetWindowCallsign(AnsiString &as);
	void __fastcall GetWindowText(AnsiString &as);
	int __fastcall GetWindowText(AnsiString &as, int SX, int SY);

	void __fastcall OpenLogFile(LPCSTR pName);
	void __fastcall CloseLogFile(void);
	inline __fastcall IsLogging(void){return m_wfp != NULL;};
	void __fastcall FlushLogFile(void);

	void __fastcall DupeText(int col);

    void __fastcall OpenSelect(void);
    void __fastcall CloseSelect(void);
	void __fastcall UpdateSelect(void);
	BOOL __fastcall ClearSelect(BOOL fPaint);
	BOOL __fastcall GetSelText(AnsiString &as);
	BOOL __fastcall DeleteSelText(void);
	inline __fastcall IsSelText(void){return m_fSelText;};
	void __fastcall Invalidate(void);
	void __fastcall SwapLTR(void);

    inline int __fastcall GetScreenLine(void){return m_ScreenLineMax;};
    inline void __fastcall GetCursorPos(int &x, int &y){
		x = m_X; y = m_Y;
    };
    inline BOOL __fastcall IsCursorLast(void){return m_fLastCP;};

    inline int __fastcall GetWindowSize(void){
		return (m_ScreenYW << 16) + m_ScreenXW;
    }
    inline LOGFONT *__fastcall GetLogFontP(void){return &m_LogFont;};
    inline BOOL __fastcall IsCreate(void){return m_hWnd != NULL;};
    inline BOOL __fastcall IsPrinted(void){return m_WLine + m_WCol;};
    inline void __fastcall SetMBCS(CMBCS *pMBCS){ m_pMBCS = pMBCS;};
    inline void __fastcall SetRTTY(BOOL f){m_fRTTY = f;};
	int __fastcall IsWord(void);
	void __fastcall ShowCtrl(BOOL f);

    inline void __fastcall UpdateCaptureLimit(void){
		m_FLine = m_WLine;
    };
    inline void __fastcall ClearCaptureLimit(void){
		m_FLine = 0;
    }
    inline BOOL __fastcall IsActive(void){return m_fActive;};
#if DEBUG
    void __fastcall Debug(LPSTR t);
#endif
};
#endif
