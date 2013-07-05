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

#include "Main.h"
#include "TrackDlg.h"
//---------------------------------------------------------------------
#pragma resource "*.dfm"
//TTrackDlgBox *TrackDlgBox;
static	int g_DlgCount=0;
//---------------------------------------------------------------------
__fastcall TTrackDlgBox::TTrackDlgBox(TComponent* AOwner)
	: TForm(AOwner)
{
	FormStyle = ((TForm *)AOwner)->FormStyle;
	Font->Name = ((TForm *)AOwner)->Font->Name;
	Font->Charset = ((TForm *)AOwner)->Font->Charset;
	if( sys.m_MsgEng ){
		CancelBtn->Caption = "Cancel";
    }
	FormCenter(this);
   	m_Changed = FALSE;
}
//---------------------------------------------------------------------
BOOL __fastcall TTrackDlgBox::Execute(LPCSTR pTitle, double &d, double min, double max, int sc, double per)
{
	if( g_DlgCount ) return FALSE;

    g_DlgCount++;
	if( pTitle && *pTitle ) Caption = pTitle;
    if( min > max ){
		double d = min;
        min = max;
        max = d;
    }
    else if( min == max ){
		max = min + 10.0;
    }
	m_Min = min;
    m_Max = max;
	char bf[256], bff[128];
    sprintf(bf, "Min(%s)", StrDbl(bff, min));
    LMin->Caption = bf;
    sprintf(bf, "Max(%s)", StrDbl(bff, max));
    LMax->Caption = bf;
	if( per < 0.0 ) per = -per;
	if( per < 1e-64 ) per = 1.0;
	m_K = per;
	int mw = ((max - min)/per) + 0.5;
    TB->Min = 0;
   	TB->Max = mw;
    TB->Position = ((d - m_Min) / m_K)+0.5;
	if( sc < 2 ){
    	sc = (max - min) / per;
        if( sc > 20 ) sc = 20;
    }
	TB->Frequency = (mw/sc)+0.5;
	ShowLV();
   	m_Changed = FALSE;
	int r = ShowModal();
    if( r == IDOK ){
		if( m_Changed ) d = (TB->Position * m_K) + m_Min;
        r = TRUE;
    }
    else {
		r = FALSE;
    }
	g_DlgCount--;
    return r;
}
//---------------------------------------------------------------------
void __fastcall TTrackDlgBox::ShowLV(void)
{
	char bf[256], bff[128];
	double d = (TB->Position * m_K) + m_Min;
    AnsiString as = StrDbl(bff, m_K);
	if( sys.m_MsgEng ){
		sprintf(bf, "Value= %s  (Step=%s)", StrDbl(bff, d), as.c_str());
    }
    else {
		sprintf(bf, "’l= %s  (½Ã¯Ìß=%s)", StrDbl(bff, d), as.c_str());
    }
    LV->Caption = bf;
}
//---------------------------------------------------------------------
void __fastcall TTrackDlgBox::TBChange(TObject *Sender)
{
	m_Changed = TRUE;
    ShowLV();
}
//---------------------------------------------------------------------------
