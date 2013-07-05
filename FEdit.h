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
#ifndef FEditH
#define FEditH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <Menus.hpp>
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
enum TFontPitch { fpDefault, fpVariable, fpFixed };	//ja7ude 0522
//---------------------------------------------------------------------------
class TFileEdit : public TForm
{
__published:	// IDE 管理のコンポーネント
	TRichEdit *REdit;
	TMainMenu *MainMenu1;
	TMenuItem *KF;
	TMenuItem *KE;
	TMenuItem *KFA;
	TMenuItem *KEC;
	TMenuItem *KECP;
	TMenuItem *KEP;
	TSaveDialog *SaveDialog;
	TMenuItem *N1;
	TMenuItem *KEU;
	TMenuItem *N2;
	TMenuItem *KEA;
	TMenuItem *N3;
	TMenuItem *KFX;
	TPopupMenu *PopupMenu;
	TFindDialog *FindDialog;
	TMenuItem *KS;
	TMenuItem *KSS;
	TMenuItem *KSN;
	TMenuItem *KFN;
	void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);

	void __fastcall KECClick(TObject *Sender);
	void __fastcall KECPClick(TObject *Sender);
	void __fastcall KEPClick(TObject *Sender);
	void __fastcall KFAClick(TObject *Sender);
	void __fastcall KEUClick(TObject *Sender);
	void __fastcall KEAClick(TObject *Sender);

	void __fastcall KEClick(TObject *Sender);
	void __fastcall KFXClick(TObject *Sender);
	void __fastcall PopupMenuPopup(TObject *Sender);

	void __fastcall KSSClick(TObject *Sender);
	void __fastcall FindDialogFind(TObject *Sender);
	void __fastcall KSClick(TObject *Sender);
	void __fastcall KFNClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall FormPaint(TObject *Sender);
	
private:	// ユーザー宣言
	BOOL		m_InitFirst;
	AnsiString  m_FileName;
    HWND		m_hWnd;
    UINT		m_uMsg;
    DWORD		m_wParam;
    BOOL		m_fDelFile;
private:
	void __fastcall AdjustTop(void);

public:		// ユーザー宣言
	__fastcall TFileEdit(TComponent* Owner);
	void __fastcall SetEvent(HWND hWnd, UINT uMsg, DWORD wParam);
	void __fastcall Execute(LPCSTR pName, BOOL fReadOnly);
	void __fastcall Execute(AnsiString &as, LPCSTR pTitle, LPCSTR pName);

    inline void __fastcall SetWordWrap(BOOL ww){REdit->WordWrap = ww;};
    inline void __fastcall SetDelFile(BOOL fDel){m_fDelFile = fDel;};
};
//---------------------------------------------------------------------------
//extern PACKAGE TFileEdit *FileEdit;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#endif
  
