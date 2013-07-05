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
#ifndef ClockAdjH
#define ClockAdjH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>

//---------------------------------------------------------------------------
#include "ComLib.h"
#include "dsp.h"
//---------------------------------------------------------------------------
class TClockAdjDlg : public TForm
{
__published:	// IDE 管理のコンポーネント
	TPanel *PCFFT;
	TPanel *PCT;
	TLabel *L1;
	TEdit *EditClock;
	TLabel *L2;
	TUpDown *UdClock;
	TSpeedButton *SBClose;
	TPaintBox *PBoxFFT;
	TPaintBox *PBoxT;
	TPanel *PCC;
	TSplitter *Splitter;
	TSpeedButton *SBSet;
	TComboBox *CBTone;
	TLabel *L3;
	TLabel *L4;
	TLabel *LPPM;
	void __fastcall SBCloseClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall FormResize(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall PBoxTPaint(TObject *Sender);
	void __fastcall PBoxFFTPaint(TObject *Sender);
	void __fastcall PCFFTResize(TObject *Sender);
	void __fastcall PCTResize(TObject *Sender);
	void __fastcall SBSetClick(TObject *Sender);
	void __fastcall UdClockClick(TObject *Sender, TUDBtnType Button);
	void __fastcall PBoxTMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall PBoxTMouseMove(TObject *Sender, TShiftState Shift, int X,
          int Y);
	void __fastcall CBToneChange(TObject *Sender);
	void __fastcall PBoxFFTMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
	void __fastcall PBoxFFTMouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y);
	void __fastcall PBoxFFTMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
private:	// ユーザー宣言
	int				m_fDisEvent;

	double	m_SampFreq;
    double	m_ToneFreq;
    CClock	m_Clock;

	TColor			m_tWaterColors[256];
    Graphics::TBitmap *m_pBitmapT;
    FFTSTG			m_StgT;

    int				m_CursorX;
	int				m_Point;
    int				m_PointX, m_PointY;
    int				m_PointX2, m_PointY2;
	int				m_PointRX;

	int				m_fftXW, m_fftYW;
    Graphics::TBitmap *m_pBitmapFFT;
    int		m_FFTWindow;

    int				m_MouseDown;
private:
	void __fastcall UpdatePPM(void);
	void __fastcall CreateWaterColors(void);
	void __fastcall DrawMsg(void);
	void __fastcall InitWater(void);
	void __fastcall DrawFFT(BOOL fClear);

	double __fastcall GetPointSamp(void);
	void __fastcall DrawMessage(LPCSTR p);
	void __fastcall DrawCursor(void);
	void __fastcall SetToneFreq(int f);


public:		// ユーザー宣言
	__fastcall TClockAdjDlg(TComponent* Owner);

	void __fastcall Execute(void);
    void __fastcall Do(short ds);
	void __fastcall UpdateFFT(void);
};
//---------------------------------------------------------------------------
//extern PACKAGE TClockAdjDlg *ClockAdjDlg;
//---------------------------------------------------------------------------
#endif
  
