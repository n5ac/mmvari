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
#ifndef RxViewH
#define RxViewH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
#include "Dump.h"
#include "ComLib.h"
#include <Menus.hpp>
class CRxSet;
enum {
	sbMODE,
    sbSN,
    sbFREQ,
    sbWATER,
    sbLEVEL,
    sbEND,
};
//---------------------------------------------------------------------------
class TRxViewDlg : public TForm
{
__published:	// IDE 管理のコンポーネント
	TStatusBar *StatusBar;
	TPanel *PC;
	TPaintBox *PBox;
	TScrollBar *SBar;
	TPopupMenu *Pup;
	TMenuItem *KFQSW;
	TPopupMenu *PupMode;
	TPopupMenu *PupW;
	TMenuItem *KW1;
	TMenuItem *KW2;
	TMenuItem *KW3;
	TMenuItem *KS1;
	TMenuItem *KS2;
	TMenuItem *KS3;
	TMenuItem *KS4;
	TMenuItem *KS5;
	TMenuItem *KS6;
	TMenuItem *NS;
	TMenuItem *KS7;
	TPopupMenu *PupS;
	TMenuItem *KSSN;
	TMenuItem *KSTM;
	TMenuItem *N2;
	TMenuItem *KFAFC;
	TMenuItem *NF;
	TMenuItem *KS0;
	TMenuItem *NR;
	TMenuItem *KRIIR;
	TMenuItem *KRFFT;
	void __fastcall PBoxPaint(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall FormResize(TObject *Sender);
	void __fastcall StatusBarDrawPanel(TStatusBar *StatusBar,
          TStatusPanel *Panel, const TRect &Rect);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall StatusBarMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall SBarChange(TObject *Sender);
	void __fastcall KFQSWClick(TObject *Sender);
	void __fastcall PBoxMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall StatusBarMouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y);
	void __fastcall StatusBarMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall PupModePopup(TObject *Sender);
	void __fastcall PupWPopup(TObject *Sender);
	void __fastcall KW1Click(TObject *Sender);
	void __fastcall KS1Click(TObject *Sender);
	void __fastcall KS2Click(TObject *Sender);
	void __fastcall KS3Click(TObject *Sender);
	void __fastcall KS4Click(TObject *Sender);
	void __fastcall KS5Click(TObject *Sender);
	void __fastcall KS6Click(TObject *Sender);
	void __fastcall PupSPopup(TObject *Sender);
	void __fastcall KSSNClick(TObject *Sender);
	void __fastcall KFAFCClick(TObject *Sender);
	void __fastcall PupPopup(TObject *Sender);
	
	void __fastcall KRIIRClick(TObject *Sender);
	void __fastcall KRFFTClick(TObject *Sender);
	
	
	void __fastcall KS0Click(TObject *Sender);
	void __fastcall KS7Click(TObject *Sender);
private:	// ユーザー宣言
	BOOL	m_fDisEvent;
	int		m_MouseDown;

	TRect	m_rcBar[sbEND];
	int		m_levelXW, m_levelYW;
    Graphics::TBitmap *m_pBitmapLevel;

	int				m_fftXW, m_fftYW;
    Graphics::TBitmap *m_pBitmapFFT;
    FFTSTG	m_StgWater;
    int		m_WaterLowFQ;
    int		m_WaterHighFQ;
    int		m_FFTWindow;
    BOOL	m_ShowTiming;
private:
	void __fastcall DrawMode(void);
	void __fastcall DrawSN(void);
	void __fastcall DrawFREQ(void);

	void __fastcall DrawLevel(void);
	void __fastcall DrawWater(BOOL fClear);
	void __fastcall InitWater(void);
	void __fastcall DrawWaterCursor(void);
	void __fastcall CalcWaterCenter(void);

	void __fastcall AddModeMenu(void);
	void __fastcall KMClick(TObject *Sender);
	void __fastcall AddToneMenu(void);
	void __fastcall KFQClick(TObject *Sender);

	void __fastcall UpdateSpeedMenu(void);

public:
    CRxSet	*m_pRxSet;
	CDump	m_Dump;
    int		m_WaterWidth;
public:		// ユーザー宣言
	__fastcall TRxViewDlg(TComponent* Owner);

	void __fastcall UpdateStatus(void);
	void __fastcall OnUpdateFont(void);
	void __fastcall SetMode(int mode);
	void __fastcall SetSpeed(double spd);
	void __fastcall UpdateWaterWidth(void);
	void __fastcall ChangeFont(void);
	inline __fastcall PopupMode(int x, int y){
		PupMode->Popup(x, y);
    };

protected:
    void __fastcall OnMouseWheel(TMessage Message);
BEGIN_MESSAGE_MAP
	MESSAGE_HANDLER(WM_MOUSEWHEEL, TMessage, OnMouseWheel)
END_MESSAGE_MAP(TForm)

};
//---------------------------------------------------------------------------
//extern PACKAGE TRxViewDlg *RxViewDlg;
//---------------------------------------------------------------------------
#endif
 
