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
#ifndef RadioSetH
#define RadioSetH
//----------------------------------------------------------------------------
/* JA7UDE 0428
#include <vcl\System.hpp>
#include <vcl\Windows.hpp>
#include <vcl\SysUtils.hpp>
#include <vcl\Classes.hpp>
#include <vcl\Graphics.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
#include <vcl\Controls.hpp>
#include <vcl\Buttons.hpp>
#include <vcl\ExtCtrls.hpp>
*/
//----------------------------------------------------------------------------
#include "Cradio.h"
#include <Classes.hpp>
#include <Controls.hpp>
#include <ExtCtrls.hpp>
#include <StdCtrls.hpp>
/* JA7UDE 0428
#include <vcl\Dialogs.hpp>
#include <vcl\ComCtrls.hpp>
*/
//----------------------------------------------------------------------------
class TRADIOSetDlg : public TForm
{
__published:        
	TButton *OKBtn;
	TButton *CancelBtn;
	TGroupBox *GB1;
	TComboBox *PortName;
	TLabel *Label1;
	TLabel *Label2;
	TComboBox *Baud;
	TRadioGroup *BitLen;
	TRadioGroup *Stop;
	TRadioGroup *Parity;
	TGroupBox *GB3;
	TCheckBox *flwXON;
	TCheckBox *flwCTS;
	TLabel *Label3;
	TComboBox *ByteWait;
	TLabel *Label8;
	TButton *LoadBtn;
	TButton *SaveBtn;
	TGroupBox *GGNR;
	TLabel *LGNR;
	TEdit *CmdGNR;
	TButton *RefBtn;
	TCheckBox *OpenGNR;
	TLabel *LPTT;
	TGroupBox *GCmd;
	TLabel *Label4;
	TLabel *Label5;
	TLabel *Label6;
	TLabel *Lxx;
	TLabel *LVFO;
	TEdit *CmdInit;
	TEdit *CmdRx;
	TEdit *CmdTx;
	TEdit *Cmdxx;
	TComboBox *VFOType;
	TComboBox *PollInt;
	TLabel *LInt;
	TLabel *LInts;
	TCheckBox *AddrScan;
	TGroupBox *GB2;
	TCheckBox *CBPTT;
	TRadioGroup *RGFS;
	TComboBox *Maker;
	TLabel *L1;
	TLabel *LH;
	void __fastcall LoadBtnClick(TObject *Sender);
	void __fastcall SaveBtnClick(TObject *Sender);
	void __fastcall MakerChange(TObject *Sender);
	void __fastcall PortNameChange(TObject *Sender);
	void __fastcall RefBtnClick(TObject *Sender);
	
	void __fastcall VFOTypeChange(TObject *Sender);
	
	
private:
	int				m_DisEvent;
    TNotifyEvent 	m_fnHintProc;

	int __fastcall IsXX(void);
	int __fastcall IsSame(LPCSTR v, LPCSTR t);
    int __fastcall IsCompatible(int PollType, int MakerIndex); //AA6YQ 1.6.6
	void __fastcall SetMaker(void);
	void __fastcall UpdateUI(void);
	void __fastcall SetVFOList(void);
	int __fastcall GetVFOType(LPCSTR pKey);
	LPCSTR __fastcall GetVFOName(int r);
	int				m_Maker;
	int				m_PollType;
	int				m_InitWidth;
	CMMList			m_MMList;
public:
	virtual __fastcall TRADIOSetDlg(TComponent* AOwner);
	int __fastcall Execute(void);

	void __fastcall DisplayHint(TObject *Sender);
};
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
#endif    
