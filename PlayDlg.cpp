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

#include "PlayDlg.h"
#include "ComLib.h"
#include "dsp.h"
#include "Main.h"
//--------------------------------------------------------------------- 
#pragma resource "*.dfm"
//TPlayDlgBox *PlayDlgBox;
//---------------------------------------------------------------------
__fastcall TPlayDlgBox::TPlayDlgBox(TComponent* AOwner)
	: TForm(AOwner)
{
	FormStyle = ((TForm *)AOwner)->FormStyle;
	Font->Name = ((TForm *)AOwner)->Font->Name;
	Font->Charset = ((TForm *)AOwner)->Font->Charset;

	pWave = NULL;
	m_DisEvent = 0;

	if( sys.m_MsgEng ){
		Caption = "Adjust Play position";
		CloseBtn->Caption = "Hide";
		StopBtn->Caption = "Stop";
		LRec->Caption = "RecTime";
		LNow->Caption = "NowTime";
	}
    FormCenter(this);
}
//---------------------------------------------------------------------
void __fastcall TPlayDlgBox::UpdateItem(void)
{
	char bf[32];

	int h = int(pWave->m_length * 0.5 / SAMPFREQ);
    int s = h % 60;
    h /= 60;
    int m = h % 60;
    h /= 60;
	sprintf(bf, "%u:%02u:%02u", h, m, s);
	LTime->Caption = bf;
	h = int(pWave->GetPos() * 0.5 / SAMPFREQ);
    s = h % 60;
    h /= 60;
    m = h % 60;
    h /= 60;
	sprintf(bf, "%u:%02u:%02u", h, m, s);
	LPos->Caption = bf;
	m_DisEvent++;
	ScrollBar->Max = int(pWave->m_length * 0.5 / SAMPFREQ);
	ScrollBar->Position = int(pWave->GetPos() * 0.5 / SAMPFREQ);
	CheckPause->Checked = pWave->m_pause;
	m_DisEvent--;
}
//---------------------------------------------------------------------
void __fastcall TPlayDlgBox::Execute(CWaveFile *wp)
{
	pWave = wp;
	pWave->m_autopause = 1;
	LName->Caption = pWave->m_FileName;
	UpdateItem();
//	pWave->m_autopause = 0;
    Visible = TRUE;
}
//---------------------------------------------------------------------
void __fastcall TPlayDlgBox::ScrollBarChange(TObject *Sender)
{
	if( m_DisEvent ) return;

	int pos = ScrollBar->Position;
	pWave->Seek(pos * SAMPFREQ*2);
	if( !CheckPause->Checked ) pWave->m_pause = 0;

	char bf[128];
	int h = pos;
    int s = h % 60;
    h /= 60;
    int m = h % 60;
    h /= 60;
	sprintf(bf, "%u:%02u:%02u", h, m, s);
	LPos->Caption = bf;
}
//---------------------------------------------------------------------------
void __fastcall TPlayDlgBox::CheckPauseClick(TObject *Sender)
{
	if( m_DisEvent ) return;

	pWave->m_pause = CheckPause->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TPlayDlgBox::StopBtnClick(TObject *Sender)
{
    ::PostMessage(MainVARI->Handle, WM_WAVE, wavePlayDlg, DWORD(this));
}
//---------------------------------------------------------------------------
void __fastcall TPlayDlgBox::CloseBtnClick(TObject *Sender)
{
	Visible = FALSE;
}
//---------------------------------------------------------------------------
void __fastcall TPlayDlgBox::FormClose(TObject *Sender,
      TCloseAction &Action)
{
	if( Action == caHide ){
		if( !pWave->m_mode ){
			StopBtnClick(NULL);
        }
    }
}
//---------------------------------------------------------------------------
