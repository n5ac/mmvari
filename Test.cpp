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

#include "Test.h"
#include "ComLib.h"
//--------------------------------------------------------------------- 
#pragma resource "*.dfm"
//TTestDlg *TestDlg;
//---------------------------------------------------------------------
__fastcall TTestDlg::TTestDlg(TComponent* AOwner)
	: TForm(AOwner)
{
	UdCarrier1->Max = short(sys.m_MaxCarrier);
#if DEBUG
	OnWave();
#endif
}
//---------------------------------------------------------------------
int __fastcall TTestDlg::Execute(void)
{
#if DEBUG
	UdSN->Position = (short)sys.m_testSN;
    EFile->Text = sys.m_testName;
    SBDo->Down = sys.m_test;
    UdCarrier1->Position = short(sys.m_testCarrier1);
    UdCarrier2->Position = short(sys.m_testCarrier2);
    UdDB2->Position = short(sys.m_testDB2);
    CBQSBTime->Text = sys.m_testQSBTime;
    UdQSBDB->Position = short(sys.m_testQSBDB);
    RG500->ItemIndex = sys.m_test500;
    CBPhase->Checked = sys.m_testPhase;
    UDClockErr->Position = short(sys.m_testClockErr);
	OnWave();
	int r = ShowModal();
    if( (r == IDOK) || (r == IDYES) ){
        sys.m_testSN = UdSN->Position;
        sys.m_testName = EFile->Text;
		sys.m_testCarrier1 = UdCarrier1->Position;
		sys.m_testCarrier2 = UdCarrier2->Position;
		sys.m_testDB2 = UdDB2->Position;
        sscanf(AnsiString(CBQSBTime->Text).c_str(), "%u", &sys.m_testQSBTime);	//JA7UDE 0428
        sys.m_testQSBDB = UdQSBDB->Position;
	    sys.m_test500 = RG500->ItemIndex;
		sys.m_testPhase = CBPhase->Checked;
	    sys.m_testClockErr = UDClockErr->Position;
		if( r == IDYES ){
			return 2 + SBDo->Down;
        }
        else {
			return 1;
        }
    }
#endif
    return 0;
}
//---------------------------------------------------------------------
void __fastcall TTestDlg::SBFClick(TObject *Sender)
{
#if DEBUG
	TOpenDialog *pBox = new TOpenDialog(this);
	pBox->Options >> ofCreatePrompt;
	pBox->Options << ofFileMustExist;
	if( sys.m_MsgEng ){
		pBox->Title = "text file";
		pBox->Filter = "Text Files(*.txt)|*.txt|";
	}
	else {
		pBox->Title = "擬似受信テキスト";
		pBox->Filter = "テキストファイル(*.txt)|*.txt|";
	}
	pBox->FileName = EFile->Text;
	pBox->DefaultExt = "txt";
	pBox->InitialDir = sys.m_BgnDir;
	OnWave();
	if( pBox->Execute() == TRUE ){
        EFile->Text = pBox->FileName;
	}
	delete pBox;
#endif
}
//---------------------------------------------------------------------------
void __fastcall TTestDlg::SBDoClick(TObject *Sender)
{
	ModalResult = mrYes;
}
//---------------------------------------------------------------------------

void __fastcall TTestDlg::SBClrClick(TObject *Sender)
{
	EFile->Text = "";	
}
//---------------------------------------------------------------------------

