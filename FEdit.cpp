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

#include "FEdit.h"
#include "ComLib.h"
#include "Main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//TFileEdit *FileEdit;
//---------------------------------------------------------------------------
__fastcall TFileEdit::TFileEdit(TComponent* Owner)
	: TForm(Owner)
{
	m_hWnd = NULL;
    m_uMsg = 0;
	m_FileName = "";
	REdit->Font->Pitch = fpFixed;
	if( sys.m_MsgEng ){
		REdit->Font->Name = "Courier New";
		REdit->Font->Charset = ANSI_CHARSET;
	}
	else {
		REdit->Font->Name = "ＭＳ ゴシック";
		REdit->Font->Charset = SHIFTJIS_CHARSET;

		KF->Caption = "ファイル(&F)";
		KFN->Caption = "メモ帳に切り替え(&N)";
		KFX->Caption = "終了(&X)";
		KE->Caption = "編集(&E)";
		KFA->Caption = "名前を付けて保存(&A)...";
		KEU->Caption = "元に戻す(&U)";
		KEC->Caption = "切り取り(&T)";
		KECP->Caption = "コピー(&C)";
		KEP->Caption = "貼り付け(&P)";
		KEA->Caption = "すべて選択(&L)";
		KS->Caption = "検索(&S)";
		KSS->Caption = "検索(&F)...";
		KSN->Caption = "次を検索(&N)";
	}
	for( int i = 0; i < KE->Count; i++ ){
		TMenuItem *pMenu = new TMenuItem(this);
		pMenu->Caption = KE->Items[i]->Caption;
		pMenu->OnClick = KE->Items[i]->OnClick;
		PopupMenu->Items->Add(pMenu);
	}
	m_fDelFile = FALSE;
    m_InitFirst = TRUE;
}
//---------------------------------------------------------------------------
void __fastcall TFileEdit::Execute(LPCSTR pName, BOOL fReadOnly)
{
	DWORD dw = ::GetFileAttributes(pName);
	if( (dw != 0xffffffff) && (dw & FILE_ATTRIBUTE_READONLY) ){
		fReadOnly = TRUE;
	}
	REdit->ReadOnly = fReadOnly;
	AnsiString as = pName;
    if( fReadOnly ) as += sys.m_MsgEng ? " (Read only)" : " (読み取り専用)";
	Caption = as;
	CWaitCursor tw;
	m_FileName = pName;
	OnWave();
	try {
		REdit->Lines->LoadFromFile(pName);
    }
    catch(...){
		FILE *fp;
        if( (fp = fopen(pName, "rt")) != NULL ){
			OnWave();
			char bf[2048];
			REdit->Lines->Clear();
            while(!feof(fp)){
				if( fgets(bf, sizeof(bf), fp) ){
					ClipLF(bf);
					REdit->Lines->Add(bf);
                }
            }
			fclose(fp);
            REdit->ReadOnly = TRUE;
        }
		else {
			ErrorMB(sys.m_MsgEng ? "Sorry, could not open <%s>...":"<%s>をオープンできませんでした", pName);
			Close();
        	return;
        }
    }
	if( m_fDelFile ){
		m_fDelFile = FALSE;
    	unlink(pName);
    }
	REdit->Modified = FALSE;
    m_InitFirst = FALSE;
	OnWave();
    AdjustTop();
	Show();
	if( !::IsWindowEnabled(Handle) ) ::EnableWindow(Handle, TRUE);
}
//---------------------------------------------------------------------------
void __fastcall TFileEdit::Execute(AnsiString &as, LPCSTR pTitle, LPCSTR pName)
{
	Caption = pTitle;
	m_FileName = pName;
	CWaitCursor tw;
	REdit->Text = as;
	REdit->ReadOnly = FALSE;
	REdit->Modified = FALSE;
    m_InitFirst = TRUE;
    OnWave();
	Show();
}
//---------------------------------------------------------------------
void __fastcall TFileEdit::FormCloseQuery(TObject *Sender, bool &CanClose)
{
	if( REdit->Modified ){
		int r = YesNoCancelMB(sys.m_MsgEng ? "Overwrite to the file (%s) ?" : "ファイル (%s) に上書きしますか?", m_FileName.c_str());
		switch(r){
			case IDYES:
				{
					CWaitCursor w;
					REdit->Lines->SaveToFile(m_FileName);
				}
				break;
			case IDCANCEL:
				CanClose = FALSE;
				return;
		}
	}
	CanClose = TRUE;
}
//---------------------------------------------------------------------------
void __fastcall TFileEdit::KECClick(TObject *Sender)
{
	REdit->CutToClipboard();
}
//---------------------------------------------------------------------------
void __fastcall TFileEdit::KECPClick(TObject *Sender)
{
	REdit->CopyToClipboard();
}
//---------------------------------------------------------------------------
void __fastcall TFileEdit::KEPClick(TObject *Sender)
{
	REdit->PasteFromClipboard();
}
//---------------------------------------------------------------------------
void __fastcall TFileEdit::KEUClick(TObject *Sender)
{
	if( REdit->HandleAllocated() ){
		SendMessage(REdit->Handle, EM_UNDO, 0, 0);
	}
}
//---------------------------------------------------------------------------
void __fastcall TFileEdit::KEAClick(TObject *Sender)
{
	REdit->SelectAll();
}
//---------------------------------------------------------------------------
void __fastcall TFileEdit::KEClick(TObject *Sender)
{
	BOOL f = !REdit->ReadOnly;
	KEU->Enabled = f && REdit->Modified;
	KECP->Enabled = REdit->SelLength;
	KEC->Enabled = f && KECP->Enabled;
	KEP->Enabled = f && ::IsClipboardFormatAvailable(CF_TEXT);
}
//---------------------------------------------------------------------------
void __fastcall TFileEdit::KFAClick(TObject *Sender)
{
	AnsiString  FileName = m_FileName.c_str();
	SetEXT(FileName.c_str(), "");
	LPCSTR pEXT = GetEXT(m_FileName.c_str());
	SaveDialog->FileName = FileName;
	SaveDialog->DefaultExt = pEXT;
	if( SaveDialog->Execute() ) {
		// Options + OverwritePrompt = True thus no need to check.
		REdit->Lines->SaveToFile(SaveDialog->FileName);
		m_FileName = SaveDialog->FileName;
		REdit->Modified = FALSE;
	 }
}
//---------------------------------------------------------------------------
void __fastcall TFileEdit::KFXClick(TObject *Sender)
{
	Close();
}
//---------------------------------------------------------------------------
void __fastcall TFileEdit::PopupMenuPopup(TObject *Sender)
{
	KEClick(NULL);
	for( int i = 0; i < PopupMenu->Items->Count; i++ ){
		PopupMenu->Items->Items[i]->Enabled = KE->Items[i]->Enabled;
	}
}
//---------------------------------------------------------------------------
void __fastcall TFileEdit::KSSClick(TObject *Sender)
{
	FindDialog->Execute();
}
//---------------------------------------------------------------------------
void __fastcall TFileEdit::FindDialogFind(TObject *Sender)
{
	int FoundAt, StartPos, ToEnd;
	  // 検索は現在選択されている場所から開始される
	  // 何も選択されていない場合は，
	  // テキストの最初から開始される
	if(REdit->SelLength){
		StartPos = REdit->SelStart + REdit->SelLength;
	}
	else {
		StartPos = 0;
	}
	// ToEnd は検索範囲の文字数を示す
	ToEnd = REdit->Text.Length() - StartPos;


	TSearchTypes stype;
	if( FindDialog->Options.Contains(frMatchCase) ) stype << stMatchCase;
	if( FindDialog->Options.Contains(frWholeWord) ) stype << stWholeWord;
	FoundAt = REdit->FindText(FindDialog->FindText, StartPos, ToEnd, stype);
	if( FoundAt != -1 ){
		REdit->SetFocus();
		REdit->SelStart = FoundAt;
		REdit->SelLength = FindDialog->FindText.Length();
		keybd_event(VK_RIGHT, 0, 0, 0);
		keybd_event(VK_RIGHT, 0, KEYEVENTF_KEYUP, 0);
        Application->ProcessMessages();
		REdit->SelStart = FoundAt;
		REdit->SelLength = FindDialog->FindText.Length();
	}
}
//---------------------------------------------------------------------------
void __fastcall TFileEdit::KSClick(TObject *Sender)
{
	KSN->Enabled = !FindDialog->FindText.IsEmpty();
}
//---------------------------------------------------------------------------
void __fastcall TFileEdit::KFNClick(TObject *Sender)
{
	CWaitCursor w;
	REdit->Lines->SaveToFile(m_FileName);
	REdit->Modified = FALSE;

	Close();

	char bf[256];
	sprintf(bf, "NOTEPAD %s", m_FileName.c_str());
	WinExec(bf, SW_SHOWDEFAULT);
}
//---------------------------------------------------------------------------
void __fastcall TFileEdit::FormClose(TObject *Sender, TCloseAction &Action)
{
	if( (Action == caHide) && m_hWnd && m_uMsg && m_wParam){
	    ::PostMessage(m_hWnd, m_uMsg, m_wParam, DWORD(this));
    }
}
//---------------------------------------------------------------------------
void __fastcall TFileEdit::SetEvent(HWND hWnd, UINT uMsg, DWORD wParam)
{
	m_hWnd = hWnd;
    m_uMsg = uMsg;
    m_wParam = wParam;
}
//---------------------------------------------------------------------------
void __fastcall TFileEdit::FormPaint(TObject *Sender)
{
	if( m_InitFirst ){
		m_InitFirst = FALSE;
		const short _tt[]={
			VK_CONTROL, VK_END, VK_END|0x8000, VK_CONTROL|0x8000,
			0
		};
		KeyEvent(_tt);
        AdjustTop();
    }
}
//---------------------------------------------------------------------------
void __fastcall TFileEdit::AdjustTop(void)
{
    int ot = MainVARI->GetOverlayTop();
	if( ot > Top ){
		Top = ot;
    }
}
//---------------------------------------------------------------------------


  
