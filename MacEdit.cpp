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



//---------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "MacEdit.h"
#include "MacroKey.h"
//---------------------------------------------------------------------
#pragma resource "*.dfm"
//---------------------------------------------------------------------
__fastcall TMacEditDlg::TMacEditDlg(TComponent* AOwner)
	: TForm(AOwner)
{
	FormStyle = ((TForm *)AOwner)->FormStyle;
	Font->Name = ((TForm *)AOwner)->Font->Name;
	Font->Charset = ((TForm *)AOwner)->Font->Charset;
//	Memo->Font->Charset = MainGMSK->PCTX->Font->Charset;
	if( sys.m_MsgEng ){
//		Caption = "Edit button";
		L1->Caption = "Button";
        SBMac->Caption = "Macro";
        SBCond->Caption = "Cond.";
        SBClear->Caption = "Clear";
        CancelBtn->Caption = "Cancel";

        ELabel->Hint = "Button name";
        SBB->Hint = "Bold";
        SBI->Hint = "Italic";
        SBU->Hint = "Under line";
        SBMac->Hint = "Choose macro-command";
        SBCond->Hint = "Choose condition-command";
        SBClear->Hint = "All clear";

		KE->Caption = "&Edit";
		KEU->Caption = "&Undo";
		KEC->Caption = "Cu&T";
		KECP->Caption = "&Copy";
		KEP->Caption = "&Paste";
		KEA->Caption = "Select AL&L";

        KFL->Caption = "&Load text...";
        KFS->Caption = "Save &As...";
    }
    KEIM->Caption = SBMac->Hint;
    KEIC->Caption = SBCond->Hint;
	KF->Caption = MainVARI->KF->Caption;
	KH->Caption = MainVARI->KH->Caption;
	KHO->Caption = MainVARI->KHO->Caption;
	KHS->Caption = MainVARI->KHS->Caption;
	m_pKeyDlg = NULL;
    m_pOverWrite = NULL;
    m_InsCount = 0;
	OnWave();
    if( sys.m_PosMacEdit.right ){
		Position = poDesigned;
		if( sys.m_PosMacEdit.bottom >= 0 ){
			SetBounds(sys.m_PosMacEdit.left, sys.m_PosMacEdit.top, sys.m_PosMacEdit.right, sys.m_PosMacEdit.bottom);
        }
        else {
			WindowState = wsMaximized;
        }
    }
    else {
		FormCenter(this);
		SaveBounds();
    }
    m_MacroDir = sys.m_MacroDir;
}
//---------------------------------------------------------------------
void __fastcall TMacEditDlg::SaveBounds(void)
{
	if( WindowState == wsNormal ){
		sys.m_PosMacEdit.left = Left;
		sys.m_PosMacEdit.top = Top;
		sys.m_PosMacEdit.right = Width;
		sys.m_PosMacEdit.bottom = Height;
    }
    else {
		sys.m_PosMacEdit.bottom = -1;
    }
}
//---------------------------------------------------------------------
int __fastcall TMacEditDlg::Execute(MACBUTTON *pList, int n)
{
	char bf[128];
	sprintf(bf, sys.m_MsgEng ? "Edit button (No.%u)" : "ボタンの編集 (No.%u)", n+1);
    Caption = bf;
	sprintf(bf, "Button%d", n+1);
    m_Name = bf;
	Memo->Text = pList->Text;
    ELabel->Text = pList->Name;
    PC->Color = pList->Color;
    SBB->Down = pList->Style & FSBOLD;
    SBI->Down = pList->Style & FSITALIC;
    SBU->Down = pList->Style & FSUNDERLINE;
	Memo->SelStart = strlen(pList->Text.c_str());
    OnWave();
    int r = ShowModal();
	if( m_pKeyDlg ){
		delete m_pKeyDlg;
        m_pKeyDlg = NULL;
		OnWave();
    }
    if( r == IDOK ){
		pList->Text = Memo->Text;
		if( Memo->Text.IsEmpty() && !ELabel->Text.IsEmpty() ){
			sprintf(bf, "M%d", n+1);
			pList->Name = bf;
        }
        else {
	        pList->Name = ELabel->Text;
        }
        pList->Color = PC->Color;
        pList->Style = 0;
        if( SBB->Down ) pList->Style |= FSBOLD;
        if( SBI->Down ) pList->Style |= FSITALIC;
		if( SBU->Down ) pList->Style |= FSUNDERLINE;
        return TRUE;
    }
    else {
		return FALSE;
    }
}
//---------------------------------------------------------------------
int __fastcall TMacEditDlg::Execute(AnsiString &as, LPCSTR pTitle)
{
	L1->Enabled = FALSE;
    ELabel->Enabled = FALSE;
    PC->Enabled = FALSE;
    SBB->Enabled = FALSE;
    SBI->Enabled = FALSE;
    SBU->Enabled = FALSE;

	AnsiString ts = sys.m_MsgEng ? "Edit macro" : "マクロ文の編集";
	if( pTitle ){
		ts += " - ";
        ts += pTitle;
    }
    Caption = ts;
	m_Name = pTitle;
	Memo->Text = as.c_str();
    ELabel->Text = "";
	Memo->SelStart = strlen(as.c_str());
    OnWave();
    int r = ShowModal();
	if( m_pKeyDlg ){
		delete m_pKeyDlg;
        m_pKeyDlg = NULL;
		OnWave();
    }
    if( r == IDOK ){
		as = Memo->Text;
        return TRUE;
    }
    else {
		return FALSE;
    }
}
//---------------------------------------------------------------------
int __fastcall TMacEditDlg::Execute(LPCSTR pName)
{
	CWaitCursor w;
   	mkdir(sys.m_MacroDir);

	m_pOverWrite = new TMenuItem(this);
	m_pOverWrite->Caption = sys.m_MsgEng ? "&Over write":"上書き保存(&O)";
    m_pOverWrite->OnClick = KFSClick;
    KF->Insert(KF->IndexOf(KFS), m_pOverWrite);

	L1->Enabled = FALSE;
    ELabel->Enabled = FALSE;
    PC->Enabled = FALSE;
    SBB->Enabled = FALSE;
    SBI->Enabled = FALSE;
    SBU->Enabled = FALSE;

	GetFullPathName(m_FullName, pName, sys.m_MacroDir);
	GetFileName(m_Name, pName);
	char bf[256];
    SetDirName(bf, m_FullName.c_str());
    m_MacroDir = bf;
	AnsiString ts = sys.m_MsgEng ? "Edit macro" : "マクロ文の編集";
	ts += " - ";
	ts += m_FullName;
    Caption = ts;
	if( IsFile(m_FullName.c_str()) ){
		try {
			Memo->Lines->LoadFromFile(m_FullName);
	    }
    	catch(...){
	    }
    }
    ELabel->Text = "";
    OnWave();
    int r = ShowModal();
	if( m_pKeyDlg ){
		delete m_pKeyDlg;
        m_pKeyDlg = NULL;
		OnWave();
    }
    if( r == IDOK ){
		CWaitCursor w;
		Memo->Lines->SaveToFile(m_FullName);
        return TRUE;
    }
    else {
		return FALSE;
    }
}
//---------------------------------------------------------------------
void __fastcall TMacEditDlg::PCClick(TObject *Sender)
{
	ColorDialog->Color = PC->Color;
    if( ColorDialog->Execute() ){
		PC->Color = ColorDialog->Color;
    }
}
//---------------------------------------------------------------------------
void __fastcall TMacEditDlg::SBMacClick(TObject *Sender)
{
	int type = ((Sender == SBMac) || (Sender == KEIM)) ? 0 : 1;
	m_InsCount = 0;
	if( !m_pKeyDlg ){
    	m_pKeyDlg = new TMacroKeyDlg(this);
	    m_pKeyDlg->Execute(Left+48, Top+60, this, type);
    }
    else {
	    m_pKeyDlg->Execute(-1, -1, this, type);
    }
    if( m_pKeyDlg ) m_pKeyDlg->UpdateUndo(m_InsCount);
}
//---------------------------------------------------------------------------
void __fastcall TMacEditDlg::SBClearClick(TObject *Sender)
{
	Memo->Clear();
}
//---------------------------------------------------------------------
void __fastcall TMacEditDlg::Undo(void)
{
	if( m_InsCount ){
		for( ; m_InsCount; m_InsCount-- ){
			::PostMessage(Memo->Handle, WM_CHAR, '\b', 0);
        }
	    if( m_pKeyDlg ) m_pKeyDlg->UpdateUndo(m_InsCount);
    }
}
//---------------------------------------------------------------------
void __fastcall TMacEditDlg::OnInsertText(LPCSTR pText)
{
	m_InsCount = 0;
	for( LPCSTR p = pText; *p; p++ ){
		::PostMessage(Memo->Handle, WM_CHAR, *p, 0);
        m_InsCount++;
    }
    if( m_pKeyDlg ) m_pKeyDlg->UpdateUndo(m_InsCount);
}
//---------------------------------------------------------------------------
void __fastcall TMacEditDlg::FormDestroy(TObject *Sender)
{
	if( m_pKeyDlg ){
		delete m_pKeyDlg;
        m_pKeyDlg = NULL;
    }
}
//---------------------------------------------------------------------------
void __fastcall TMacEditDlg::FormResize(TObject *Sender)
{
	CancelBtn->Left = PB->Width - CancelBtn->Width - 1;
    OKBtn->Left = CancelBtn->Left - OKBtn->Width - 1;
    SaveBounds();
}
//---------------------------------------------------------------------------
void __fastcall TMacEditDlg::FormClose(TObject *Sender,
      TCloseAction &Action)
{
	SaveBounds();
}
//---------------------------------------------------------------------------
void __fastcall TMacEditDlg::KEClick(TObject *Sender)
{
	KEU->Enabled = Memo->Modified;
	KEC->Enabled = Memo->SelLength;
	KECP->Enabled = KEC->Enabled;
	KEP->Enabled = ::IsClipboardFormatAvailable(CF_TEXT);
}
//---------------------------------------------------------------------------
void __fastcall TMacEditDlg::KEUClick(TObject *Sender)
{
	if( Memo->HandleAllocated() ){
		SendMessage(Memo->Handle, EM_UNDO, 0, 0);
	}
}
//---------------------------------------------------------------------------
void __fastcall TMacEditDlg::KECClick(TObject *Sender)
{
	Memo->CutToClipboard();
}
//---------------------------------------------------------------------------
void __fastcall TMacEditDlg::KECPClick(TObject *Sender)
{
	Memo->CopyToClipboard();
}
//---------------------------------------------------------------------------
void __fastcall TMacEditDlg::KEPClick(TObject *Sender)
{
	Memo->PasteFromClipboard();
}
//---------------------------------------------------------------------------
void __fastcall TMacEditDlg::KEAClick(TObject *Sender)
{
	Memo->SelectAll();
}
//---------------------------------------------------------------------------
void __fastcall TMacEditDlg::KHOClick(TObject *Sender)
{
	if( Sender == KHO ){
		MainVARI->ShowManual();
    }
    else {
		MainVARI->ShowMacroSample();
	}
}
//---------------------------------------------------------------------------
void __fastcall TMacEditDlg::KFSClick(TObject *Sender)
{
   	mkdir(sys.m_MacroDir);
	if( Sender == KFS ){
		TSaveDialog *pBox = new TSaveDialog(this);
		pBox->Options << ofOverwritePrompt;
	    pBox->Options << ofNoReadOnlyReturn;
		if( sys.m_MsgEng ){
			pBox->Title = "Save macro text";
		}
		else {
			pBox->Title = "テキストファイルにセーブ";
		}
		pBox->Filter = "Text Files(*.txt)|*.txt";
		pBox->FileName = m_Name;
		pBox->DefaultExt = "txt";
		pBox->InitialDir = m_MacroDir;
		OnWave();
		if( pBox->Execute() == TRUE ){
			OnWave();
			Memo->Lines->SaveToFile(pBox->FileName);
		}
		delete pBox;
    }
    else {
		Memo->Lines->SaveToFile(m_FullName);
        Memo->Modified = FALSE;
    }
}
//---------------------------------------------------------------------------
void __fastcall TMacEditDlg::KFLClick(TObject *Sender)
{
   	mkdir(sys.m_MacroDir);
	TOpenDialog *pBox = new TOpenDialog(this);
	pBox->Options >> ofCreatePrompt;
	pBox->Options << ofFileMustExist;
	if( sys.m_MsgEng ){
		pBox->Title = "Load macro text";
	}
	else {
		pBox->Title = "テキストファイルからロード";
	}
	pBox->Filter = "Text Files(*.txt)|*.txt";
	pBox->FileName = m_Name;
	pBox->DefaultExt = "txt";
	pBox->InitialDir = m_MacroDir;
	OnWave();
	if( pBox->Execute() == TRUE ){
		OnWave();
		Memo->Lines->LoadFromFile(pBox->FileName);
        Memo->Modified = TRUE;
	}
	delete pBox;
}
//---------------------------------------------------------------------------
void __fastcall TMacEditDlg::KFClick(TObject *Sender)
{
	if( m_pOverWrite ) m_pOverWrite->Enabled = Memo->Modified;
	KFS->Enabled = !Memo->Text.IsEmpty();
}
//---------------------------------------------------------------------------

