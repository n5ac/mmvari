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
#ifndef MacroKeyH
#define MacroKeyH
//----------------------------------------------------------------------------
// JA7UDE 0428
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

#include <Grids.hpp>
//----------------------------------------------------------------------------
#define	MACLISTMAX		288
//----------------------------------------------------------------------------
#pragma option -a-	// パックの指示
typedef struct {
	BYTE	r;
	LPCSTR	pKey;
	LPCSTR	pJpn;
	LPCSTR	pEng;
}MACKEY;
#pragma option -a.	// パック解除の指示
class TMacEditDlg;
class TMacroKeyDlg : public TForm
{
__published:
	TButton *CancelBtn;
	TStringGrid *Grid;
	TSpeedButton *SBIns;
	TSpeedButton *SBSpace;
	TSpeedButton *SBCR;
	TSpeedButton *SBUndo;
	TSpeedButton *SBFind;
	TSpeedButton *SBPrint;
	void __fastcall GridDrawCell(TObject *Sender, int Col, int Row,
		  TRect &Rect, TGridDrawState State);
	void __fastcall GridSelectCell(TObject *Sender, int Col, int Row,
		  bool &CanSelect);
	void __fastcall CancelBtnClick(TObject *Sender);
	void __fastcall SBInsClick(TObject *Sender);
	void __fastcall SBSpaceClick(TObject *Sender);
	void __fastcall SBCRClick(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall SBUndoClick(TObject *Sender);
	void __fastcall GridMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall GridDblClick(TObject *Sender);
	void __fastcall FormKeyPress(TObject *Sender, char &Key);
	void __fastcall SBFindClick(TObject *Sender);
	
	void __fastcall SBPrintClick(TObject *Sender);
	void __fastcall SBPrintMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
private:
	int		m_Type;
    BOOL	m_fSort;
    int		m_fSortType;
    BOOL	m_fDisDblClick;
	MACKEY	mackey[MACLISTMAX];
	int __fastcall AddMacKey(MACKEY *mp, int n);
	LPCSTR __fastcall GetComment(const MACKEY *mp);
	void __fastcall UpdateUI(int row);
	void __fastcall Sort(BOOL fSort, int type);

	TMacEditDlg *m_pDlg;

public:
	virtual __fastcall TMacroKeyDlg(TComponent* AOwner);

	void __fastcall Execute(int x, int y, TMacEditDlg *pDlg, int type);
	void __fastcall UpdateUndo(BOOL undo);
};
//----------------------------------------------------------------------------
//extern TMacroKeyDlg *MacroKeyDlg;
//----------------------------------------------------------------------------
#endif    
