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
#ifndef TermH
#define TermH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
#include "Dump.h"
//---------------------------------------------------------------------------
class TTermView : public TForm
{
__published:	// IDE 管理のコンポーネント
	TPanel *PCB;
	TScrollBar *SBar;
	TPanel *PC;
	TPaintBox *PB;
	TSplitter *Splitter1;
	TPanel *PCEB;
	TPanel *PCE;
	TScrollBar *SBarE;
	TPaintBox *PBE;
    void __fastcall FormKeyPress(TObject *Sender, char &Key);
	void __fastcall PBPaint(TObject *Sender);
	
	
	void __fastcall PCResize(TObject *Sender);
	void __fastcall SBarChange(TObject *Sender);
	void __fastcall PBClick(TObject *Sender);
	void __fastcall PCEnter(TObject *Sender);
	void __fastcall PCExit(TObject *Sender);
	
	void __fastcall PBEPaint(TObject *Sender);
	void __fastcall PCEEnter(TObject *Sender);
	void __fastcall PCEExit(TObject *Sender);
	void __fastcall SBarEChange(TObject *Sender);
	void __fastcall PBEClick(TObject *Sender);
	void __fastcall PCEResize(TObject *Sender);
	void __fastcall FormActivate(TObject *Sender);
	void __fastcall FormDeactivate(TObject *Sender);
private:	// ユーザー宣言
public:
    CDump	m_Dump;
    CDump	m_Edit;
private:
	void __fastcall SetCompositionWindowPos();

public:		// ユーザー宣言
	__fastcall TTermView(TComponent* Owner);

	void PutChar(int c);

};
//---------------------------------------------------------------------------
//extern PACKAGE TTermView *TermView;
//---------------------------------------------------------------------------
#endif
