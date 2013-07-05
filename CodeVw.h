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
#ifndef CodeVwH
#define CodeVwH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Buttons.hpp>

#include "ComLib.h"
#include <ComCtrls.hpp>
#define	WM_FORMCLOSE	(WM_USER+410)
#define	WM_CODEINSERT	(WM_USER+411)
//---------------------------------------------------------------------------
class TCodeView : public TForm
{
__published:	// IDE 管理のコンポーネント
	TPanel *Panel;
	TPaintBox *PBox;
	TSpeedButton *SBClose;
	TSpeedButton *SBUS;
	TSpeedButton *SBJA;
	TSpeedButton *SBHL;
	TSpeedButton *SBBY;
	TSpeedButton *SBBV;
	TUpDown *UDMB;
	TTimer *Timer;
    void __fastcall PBoxPaint(TObject *Sender);
	void __fastcall SBCloseClick(TObject *Sender);
	void __fastcall PBoxMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall SBUSClick(TObject *Sender);
	void __fastcall UDMBClick(TObject *Sender, TUDBtnType Button);
	void __fastcall PBoxMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	
	void __fastcall TimerTimer(TObject *Sender);
private:	// ユーザー宣言
	HWND	m_hWnd;
    UINT	m_uMsg;
    int		m_wParam;

    TFont	*m_pFont;
	int		m_Code;
	int		m_XW1;
    int		m_YW1;
	int		m_XW;
    int		m_YW;
	int		m_MouseDown;
    int		m_Base;
    BOOL	m_tPfx[256];
    int		m_pfxCount;
    int		m_tPfxIdx[256];
	void __fastcall UpdateTitle(void);
	void __fastcall UpdatePfx(void);
	void __fastcall UpdateXW(void);
	void __fastcall SetPBoxFont(TCanvas *pCanvas);
	void __fastcall DrawChar(TCanvas *pCanvas, int c, BOOL f);
	void __fastcall DrawCursor(TCanvas *pCanvas, int c, BOOL f);
	WORD __fastcall GetEUDC(int y, int x);

public:		// ユーザー宣言
	__fastcall TCodeView(TComponent* Owner);

	void __fastcall Execute(HWND hWnd, UINT uMsg, int wParam, TFont *pFont);
};
//---------------------------------------------------------------------------
//extern PACKAGE TCodeView *CodeView;
//---------------------------------------------------------------------------
#endif
