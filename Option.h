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



//----------------------------------------------------------------------------
#ifndef OptionH
#define OptionH
//----------------------------------------------------------------------------
#include <System.hpp>
#include <Windows.hpp>
#include <SysUtils.hpp>
#include <Classes.hpp>
#include <Graphics.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Controls.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include <Dialogs.hpp>
//----------------------------------------------------------------------------
#include "ComLib.h"
#include "MMLink.h"
//----------------------------------------------------------------------------
class TOptDlgBox : public TForm
{
__published:
	TButton *OKBtn;
	TButton *CancelBtn;
	TPageControl *Page;
	TTabSheet *TabRX;
	TTabSheet *TabTX;
	TTabSheet *TabMisc;
	TGroupBox *GB1;
	TComboBox *CBComPTT;
	TCheckBox *CBComLock;
	TGroupBox *GB2;
	TGroupBox *GB3;
	TLabel *L2;
	TComboBox *CBFifoRX;
	TLabel *Label1;
	TComboBox *CBFifoTX;
	TGroupBox *GB4;
	TLabel *L4;
	TComboBox *CBClock;
	TLabel *L5;
	TLabel *L6;
	TUpDown *UdTxOffset;
	TLabel *L7;
	TRadioGroup *RGLoopBack;
	TGroupBox *GB5;
	TLabel *L9;
	TEdit *ECallSign;
	TGroupBox *GB6;
	TLabel *L10;
	TComboBox *CBAFCW;
	TLabel *L11;
	TRadioGroup *RGSoundIn;
	TGroupBox *GB8;
	TTrackBar *TBModGain;
	TGroupBox *GB10;
	TCheckBox *CBHPF;
	TRadioGroup *RGPriority;
	TGroupBox *GB11;
	TSpeedButton *SBRxFont;
	TSpeedButton *SBRxChar;
	TPanel *PCRx1;
	TPanel *PCRx2;
	TPanel *PCRx3;
	TPanel *PCRx4;
	TGroupBox *GB12;
	TSpeedButton *SBTxFont;
	TPanel *PCTx1;
	TPanel *PCTx2;
	TPanel *PCTx3;
	TGroupBox *GB13;
	TPanel *PS1;
	TPanel *PS2;
	TPanel *PS3;
	TPanel *PS4;
	TPanel *PS5;
	TPanel *PS6;
	TGroupBox *GB14;
	TPanel *PW1;
	TPanel *PW2;
	TPanel *PW3;
	TPanel *PW4;
	TPanel *PW5;
	TPanel *PW6;
	TComboBox *CBTxOffset;
	TUpDown *UdClock;
	TLabel *L13;
	TEdit *EAFCL;
	TUpDown *UdAFCL;
	TLabel *L14;
	TLabel *LH;
	TSpeedButton *SBRadio;
	TGroupBox *GBKey;
	TComboBox *CBKTX;
	TLabel *L15;
	TComboBox *CBKTXOFF;
	TLabel *L16;
	TPanel *PCTx4;
	TLabel *L17;
	TLabel *L18;
	TGroupBox *GB9;
	TSpeedButton *SBJA;
	TSpeedButton *SBEng;
	TSpeedButton *SBFont;
	TPanel *PW7;
	TGroupBox *GB15;
	TLabel *L20;
	TTrackBar *TBATC;
	TLabel *L21;
	TLabel *L22;
	TEdit *EATCL;
	TUpDown *UdATCL;
	TLabel *Label2;
	TPanel *PCRx5;
	TLabel *L23;
	TComboBox *CBATCM;
	TLabel *L24;
	TGroupBox *GB16;
	TCheckBox *CBCA;
	TGroupBox *GB17;
	TCheckBox *CBSWL;
	TPanel *PW8;
	TPanel *PW9;
	TPanel *PL;
	TPaintBox *PBL;
	TLabel *L30;
	TLabel *L31;
	TPanel *PW10;
	TPanel *PW11;
	TPanel *PW12;
	TCheckBox *CBRxUTC;
	TCheckBox *CBMW;
	TSpeedButton *SBAS;
	TCheckBox *CBPB;
	TCheckBox *CBShowCtrl;
	TTrackBar *TBPB;
	TLabel *LPB;
	TGroupBox *GB18;
	TCheckBox *CBMC;
	TCheckBox *CBMM;
	TSpeedButton *SBWLCD;
	TSpeedButton *SBWCRT;
	TGroupBox *GB33;
	TLabel *L33;
	TComboBox *CBEvent;
	TSpeedButton *SBEvent;
	TSpeedButton *SBEventC;
	TRadioGroup *RGRTTY;
	TComboBox *CBSoundID;
	TLabel *LI;
	TLabel *LO;
	TComboBox *CBSoundIDTX;
	TGroupBox *GB30;
	TCheckBox *CBFSK;
	TCheckBox *CBFSKINV;
	TCheckBox *CBST;
	TGroupBox *GBWindow;
	TRadioButton *RBW1;
	TRadioButton *RBW2;
	TCheckBox *CBRestoreSub;
	void __fastcall UdTxOffsetClick(TObject *Sender, TUDBtnType Button);
	void __fastcall SBRxFontClick(TObject *Sender);
	void __fastcall SBRxCharClick(TObject *Sender);
	void __fastcall PCRx1Click(TObject *Sender);
	void __fastcall CBTxOffsetChange(TObject *Sender);
	void __fastcall UdClockClick(TObject *Sender, TUDBtnType Button);
	
	void __fastcall CBComPTTChange(TObject *Sender);
	
	void __fastcall SBRadioClick(TObject *Sender);
	void __fastcall SBEngClick(TObject *Sender);
	void __fastcall SBFontClick(TObject *Sender);
	
	void __fastcall PBLPaint(TObject *Sender);
	void __fastcall PBLMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall PBLMouseMove(TObject *Sender, TShiftState Shift, int X,
          int Y);
	void __fastcall PBLMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	
	void __fastcall SBWLCDClick(TObject *Sender);
	void __fastcall CBSoundIDDropDown(TObject *Sender);
	void __fastcall CBSoundIDChange(TObject *Sender);
	void __fastcall CBComPTTDropDown(TObject *Sender);
	void __fastcall SBASClick(TObject *Sender);
	void __fastcall CBPBClick(TObject *Sender);
	void __fastcall SBEventClick(TObject *Sender);
	void __fastcall SBEventCClick(TObject *Sender);
	
private:
	BOOL			m_fDisEvent;
    TNotifyEvent 	m_fnHintProc;

    AnsiString		m_FontName;
    BYTE			m_FontCharset;

	int				m_MouseDown;
    int				m_MouseWLN;
	int				m_MinWL, m_MaxWL;
    int				m_tWaterLevel[6];

    CMMList			m_MMListF;
    CMMList			m_MMListW;

    AnsiString		m_AS;
    AnsiString		m_MacEvent[macOnEnd];
public:
    BOOL			m_fComChange;
    BOOL			m_fLangChange;
private:
	void __fastcall UpdateButton(void);
	void __fastcall UpdateHint(void);
	void __fastcall UpdateUI(void);
	int __fastcall GetWLNo(int x);

public:
	virtual __fastcall TOptDlgBox(TComponent* AOwner);
	void __fastcall DisplayHint(TObject *Sender);
	int __fastcall Execute(DWORD dwPage);
};
//----------------------------------------------------------------------------
//extern PACKAGE TOptDlgBox *OptDlgBox;
//----------------------------------------------------------------------------
#endif
   
