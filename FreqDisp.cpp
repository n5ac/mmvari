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

#include "ComLib.h"
#include "FreqDisp.h"
//---------------------------------------------------------------------
#pragma resource "*.dfm"
//TFreqDispDlg *FreqDispDlg;
//---------------------------------------------------------------------
__fastcall TFreqDispDlg::TFreqDispDlg(TComponent* AOwner)
	: TForm(AOwner)
{
	FormStyle = ((TForm *)AOwner)->FormStyle;
	Font->Name = ((TForm *)AOwner)->Font->Name;
	Font->Charset = ((TForm *)AOwner)->Font->Charset;

	m_H1 = NULL;
	m_Tap1 = 0;
	m_H2 = NULL;
	m_Tap2 = 0;
	m_Max = 0;
    m_SampleFreq = SAMPFREQ;

	pBitmap = new Graphics::TBitmap();
	pBitmap->Width = PaintBox->Width;
	pBitmap->Height = PaintBox->Height;
	if( sys.m_MsgEng ){
		CancelBtn->Caption = "Close";
		Caption = "frequency characteristic";
	}
    FormCenter(this);
}
__fastcall TFreqDispDlg::~TFreqDispDlg()
{
	delete pBitmap;
}
//---------------------------------------------------------------------
void __fastcall TFreqDispDlg::Execute(CFIR2 *pFIR, int max, double SampleFreq)
{
	m_type = 0;
	m_H1 = pFIR->GetHP();
	m_Tap1 = pFIR->GetTap();
	m_Tap2 = 0;
    m_Max = max;
    m_SampleFreq = SampleFreq;
    MakeBitmap();
	ShowModal();
}
//---------------------------------------------------------------------
void __fastcall TFreqDispDlg::Execute(CFIRX *pFIR, int max, double SampleFreq)
{
	m_type = 0;
	m_H1 = pFIR->GetHP();
	m_Tap1 = pFIR->GetTap();
	m_Tap2 = 0;
    m_Max = max;
    m_SampleFreq = SampleFreq;
    MakeBitmap();
	ShowModal();
}
//---------------------------------------------------------------------
void __fastcall TFreqDispDlg::Execute(const double *H, int Tap, int max, double SampleFreq)
{
	m_type = 0;
	m_H1 = H;
	m_Tap1 = Tap ? Tap : 1;
	m_Tap2 = 0;
	m_Max = max;
    m_SampleFreq = SampleFreq;
    MakeBitmap();
	ShowModal();
}
//---------------------------------------------------------------------
void __fastcall TFreqDispDlg::Execute(double a10, double b11, double b12, double a20, double b21, double b22, double SampleFreq)
{
	m_type = 1;
	m_a10 = a10;
	m_b11 = b11;
	m_b12 = b12;
	m_a20 = a20;
	m_b21 = b21;
	m_b22 = b22;
    m_SampleFreq = SampleFreq;
    MakeBitmap();
	ShowModal();
}
//---------------------------------------------------------------------
void __fastcall TFreqDispDlg::Execute(CIIRTANK &tank, double SampleFreq)
{
	m_type = 4;
	m_a10 = tank.a0;
	m_b11 = tank.b1;
	m_b12 = tank.b2;
    m_SampleFreq = SampleFreq;
    MakeBitmap();
	ShowModal();
}
//---------------------------------------------------------------------
void __fastcall TFreqDispDlg::Execute(CIIR *ip, double max, double SampleFreq)
{
	m_type = 2;
	m_piir = ip;
	m_Max = max;
    m_SampleFreq = SampleFreq;
    MakeBitmap();
	ShowModal();
}
//---------------------------------------------------------------------
void __fastcall TFreqDispDlg::MakeBitmap(void)
{
	OnWave();
	if( m_type == 0 ){		// FIR
		DrawGraph(pBitmap, m_H1, m_Tap1, m_Max, 1, clBlue, m_SampleFreq);
		if( m_Tap2 ){
			DrawGraph(pBitmap, m_H2, m_Tap2, m_Max, 0, clRed, m_SampleFreq);
		}
	}
	else if( m_type == 1 ){	// IIR Tank
		DrawGraphIIR(pBitmap, m_a10, 0, 0, -m_b11, -m_b12, m_Max, 1, clBlue, m_SampleFreq);
		DrawGraphIIR(pBitmap, m_a20, 0, 0, -m_b21, -m_b22, m_Max, 0, clRed, m_SampleFreq);
	}
	else if( m_type == 2 ){	// IIR
		DrawGraphIIR(pBitmap, m_piir, m_Max, 1, clBlue, m_SampleFreq);
	}
	else if( m_type == 3 ){	// FIR-Avg
		DrawGraph(pBitmap, m_H1, m_Tap1, m_Max, 1, clBlue, m_SampleFreq);
	}
	else if( m_type == 4 ){
		DrawGraphIIR(pBitmap, m_a10, 0, 0, -m_b11, -m_b12, m_Max, 1, clBlue, m_SampleFreq);
    }
    OnWave();
}
//---------------------------------------------------------------------
void __fastcall TFreqDispDlg::PaintBoxPaint(TObject *Sender)
{
	PaintBox->Canvas->Draw(0, 0, (TGraphic*)pBitmap);
}
//---------------------------------------------------------------------------
