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
#ifndef OCBH
#define OCBH
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
#include <Dialogs.hpp>
#include <ComCtrls.hpp>
//----------------------------------------------------------------------------
class TTestDlg : public TForm
{
__published:
	TButton *OKBtn;
	TButton *CancelBtn;
	TLabel *L1;
	TComboBox *CBSigSN;
	TLabel *L2;
	TLabel *L3;
	TEdit *EFile;
	TSpeedButton *SBF;
	TSpeedButton *SBDo;
	TUpDown *UdSN;
	TComboBox *CBCarrier1;
	TComboBox *CBCarrier2;
	TUpDown *UdCarrier1;
	TUpDown *UdCarrier2;
	TEdit *EDB2;
	TUpDown *UdDB2;
	TComboBox *CBQSBTime;
	TLabel *L5;
	TComboBox *CBQSBDB;
	TUpDown *UdQSBDB;
	TLabel *L6;
	TLabel *L7;
	TSpeedButton *SBClr;
	TCheckBox *CBPhase;
	TRadioGroup *RG500;
	TLabel *Label1;
	TComboBox *CBClockErr;
	TUpDown *UDClockErr;
	void __fastcall SBFClick(TObject *Sender);
	
	void __fastcall SBDoClick(TObject *Sender);
	void __fastcall SBClrClick(TObject *Sender);
private:
public:
	virtual __fastcall TTestDlg(TComponent* AOwner);

	int __fastcall Execute(void);
};
//----------------------------------------------------------------------------
//extern PACKAGE TTestDlg *TestDlg;
//----------------------------------------------------------------------------
#endif
