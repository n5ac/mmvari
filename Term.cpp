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
#include <vcl.h>
#pragma hdrstop

#include <mbstring.h>
//#include "Main.h"
#include "Term.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//TTermView *TermView;
//---------------------------------------------------------------------------
__fastcall TTermView::TTermView(TComponent* Owner)
	: TForm(Owner)
{
    m_Dump.Create(PC->Handle, PB, PC->Font, SBar, 20);
    m_Dump.SetCursorType(csSTATIC);

    m_Edit.Create(PCE->Handle, PBE, PCE->Font, SBarE, 20);
    m_Edit.SetCursorType(csCARET);

}
//---------------------------------------------------------------------------
void TTermView::PutChar(int c)
{
	m_Dump.PutChar(c, 1);
//    Caption = m_Dump.Debug();
}
//---------------------------------------------------------------------------
void __fastcall TTermView::FormKeyPress(TObject *Sender, char &Key)
{
	if( Key == '\b' ){
//		if( m_Edit.GetSendCharCount() ){
//        	DPLLWnd->SendChar('\b');
//        }
    }
	m_Edit.PutKey(Key, 1);
}
//---------------------------------------------------------------------------
void __fastcall TTermView::PBPaint(TObject *Sender)
{
	m_Dump.Paint();
}
//---------------------------------------------------------------------------
void __fastcall TTermView::PCResize(TObject *Sender)
{
	m_Dump.Resize();
}
//---------------------------------------------------------------------------
void __fastcall TTermView::SBarChange(TObject *Sender)
{
	m_Dump.OnScrollBarChange();
}
//---------------------------------------------------------------------------
void __fastcall TTermView::PBClick(TObject *Sender)
{
	PCE->SetFocus();
//    m_Dump.Cursor(TRUE);
}
//---------------------------------------------------------------------------
void __fastcall TTermView::PCEnter(TObject *Sender)
{
	m_Dump.CreateCaret();
}
//---------------------------------------------------------------------------
void __fastcall TTermView::PCExit(TObject *Sender)
{
	m_Dump.DestroyCaret();
}
//---------------------------------------------------------------------------
void __fastcall TTermView::PBEPaint(TObject *Sender)
{
	m_Edit.Paint();
}
//---------------------------------------------------------------------------
void __fastcall TTermView::PCEEnter(TObject *Sender)
{
	m_Edit.CreateCaret();
}
//---------------------------------------------------------------------------
void __fastcall TTermView::PCEExit(TObject *Sender)
{
	m_Edit.DestroyCaret();
}
//---------------------------------------------------------------------------
void __fastcall TTermView::SBarEChange(TObject *Sender)
{
	m_Edit.OnScrollBarChange();
}
//---------------------------------------------------------------------------
void __fastcall TTermView::PBEClick(TObject *Sender)
{
	PCE->SetFocus();
    m_Edit.Cursor(TRUE);
}
//---------------------------------------------------------------------------
void __fastcall TTermView::PCEResize(TObject *Sender)
{
	m_Edit.Resize();
}
//---------------------------------------------------------------------------
void __fastcall TTermView::FormActivate(TObject *Sender)
{
	PCE->SetFocus();
	m_Edit.CreateCaret();
}
//---------------------------------------------------------------------------
void __fastcall TTermView::FormDeactivate(TObject *Sender)
{
	m_Edit.DestroyCaret();
}
//---------------------------------------------------------------------------
