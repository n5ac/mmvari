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
#ifndef MacEditH
#define MacEditH
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
//----------------------------------------------------------------------------
#include "Main.h"
#include <Dialogs.hpp>
#include "MacroKey.h"
#include <Menus.hpp>
//----------------------------------------------------------------------------
class TMacEditDlg : public TForm
{
__published:
	TButton *OKBtn;
	TButton *CancelBtn;
	TMemo *Memo;
	TEdit *ELabel;
	TLabel *L1;
	TColorDialog *ColorDialog;
	TPanel *PC;
	TSpeedButton *SBMac;
	TSpeedButton *SBClear;
	TSpeedButton *SBB;
	TSpeedButton *SBI;
	TSpeedButton *SBU;
	TSpeedButton *SBCond;
	TPanel *PB;
	TMainMenu *Menu;
	TMenuItem *KE;
	TMenuItem *KEC;
	TMenuItem *KECP;
	TMenuItem *KEP;
	TMenuItem *KEU;
	TMenuItem *KEA;
	TMenuItem *N1;
	TMenuItem *N2;


	TMenuItem *KH;
	TMenuItem *KHO;
	TMenuItem *KHS;
	TMenuItem *N3;
	TMenuItem *KEIM;
	TMenuItem *KEIC;
	TMenuItem *KF;
	TMenuItem *KFS;
	TMenuItem *KFL;
	TMenuItem *N4;
	void __fastcall PCClick(TObject *Sender);
	void __fastcall SBMacClick(TObject *Sender);
	void __fastcall SBClearClick(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	
	void __fastcall FormResize(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	
	void __fastcall KEClick(TObject *Sender);
	void __fastcall KEUClick(TObject *Sender);
	void __fastcall KEAClick(TObject *Sender);
	void __fastcall KECClick(TObject *Sender);
	void __fastcall KECPClick(TObject *Sender);
	void __fastcall KEPClick(TObject *Sender);
	void __fastcall KHOClick(TObject *Sender);


	void __fastcall KFSClick(TObject *Sender);
	void __fastcall KFLClick(TObject *Sender);
	void __fastcall KFClick(TObject *Sender);

	
private:
	AnsiString	m_Name;
    AnsiString	m_FullName;
    AnsiString	m_MacroDir;
	int			m_InsCount;
	TMacroKeyDlg *m_pKeyDlg;
	TMenuItem	*m_pOverWrite;

	void __fastcall SaveBounds(void);

public:
	virtual __fastcall TMacEditDlg(TComponent* AOwner);

	int __fastcall Execute(MACBUTTON *pList, int n);
	int __fastcall Execute(AnsiString &as, LPCSTR pTitle);
	int __fastcall Execute(LPCSTR pName);

	void __fastcall OnInsertText(LPCSTR pText);
    void __fastcall Undo(void);
};
//----------------------------------------------------------------------------
//extern PACKAGE TMacEditDlg *MacEditDlg;
//----------------------------------------------------------------------------
#endif
