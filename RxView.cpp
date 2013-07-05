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

#include "RxView.h"
#include "Main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//TRxViewDlg *RxViewDlg;
LPCSTR g_tm[]={
"GMSK\tMMVARI code",
"FSK(fm=1.0)\tMMVARI code",
"FSK-W(170Hz)\tMMVARI code",
"BPSK\tMMVARI code",
"bpsk\tStandard code",
"rtty-LSB\tBAUDOT code",
"rtty-USB\tBAUDOT code",
"mfsk-LSB\tMFSK standard code",
"mfsk-USB\tMFSK standard code",
"qpsk-LSB\tStandard code",
"qpsk-USB\tStandard code",
};
//---------------------------------------------------------------------------
__fastcall TRxViewDlg::TRxViewDlg(TComponent* Owner)
	: TForm(Owner)
{
	Font->Name = ((TForm *)Owner)->Font->Name;
	Font->Charset = ((TForm *)Owner)->Font->Charset;
	if( sys.m_MsgEng ){
		KFQSW->Caption = "Exchange with Main channel";
		KRIIR->Caption = "IIR demodulator";
		KRFFT->Caption = "FFT demodulator";
    }
	m_pRxSet = NULL;
    m_pBitmapLevel = NULL;
    m_pBitmapFFT = NULL;
    m_levelXW = m_levelYW = 0;
	m_WaterWidth = 400;
    m_FFTWindow = (m_WaterWidth-1) * FFT_SIZE / MainVARI->m_FFTSampFreq;
    m_WaterLowFQ = 1750 - (m_WaterWidth/2);
	m_WaterHighFQ = 1750 + (m_WaterWidth/2);
	m_fDisEvent = TRUE;
	PC->Font->Assign(MainVARI->PCRX->Font);
    m_Dump.Create(PC->Handle, PC, PBox, PC->Font, SBar, 256);
    m_Dump.SetCursorType(csSTATIC);
    m_fDisEvent = FALSE;
    m_MouseDown = FALSE;
    m_ShowTiming = FALSE;
	AddModeMenu();
    AddToneMenu();
}
//---------------------------------------------------------------------------
void __fastcall TRxViewDlg::OnUpdateFont(void)
{
	if( !m_pRxSet ) return;

    m_Dump.SetFont(PC->Font);
	m_pRxSet->m_fJA = (PC->Font->Charset == SHIFTJIS_CHARSET);
	m_pRxSet->m_MBCS.Create(PC->Font->Charset);

	if( m_fDisEvent ) return;
    StatusBar->Invalidate();

//        m_Dump.SetFont(PC->Font);
//		m_pRxSet->m_fJA = (PC->Font->Charset == SHIFTJIS_CHARSET);
//        StatusBar->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TRxViewDlg::PBoxPaint(TObject *Sender)
{
	if( m_fDisEvent ) return;

	m_Dump.Paint();
}
//---------------------------------------------------------------------------
void __fastcall TRxViewDlg::FormClose(TObject *Sender,
      TCloseAction &Action)
{
	if( Action == caHide ){
		::PostMessage(MainVARI->Handle, WM_WAVE, waveCloseRxView, DWORD(this));
    }
}
//---------------------------------------------------------------------------
void __fastcall TRxViewDlg::DrawMode(void)
{
	int sync = (m_pRxSet->m_SQ && m_pRxSet->m_pDem->GetSyncState() && m_pRxSet->m_pDem->m_Lock);
	TCanvas *pCanvas = StatusBar->Canvas;
	pCanvas->Brush->Color = sync ? TColor(RGB(0,255,0)) : clBtnFace;
//	pCanvas->FillRect(m_rcBar[sbMODE]);
	pCanvas->TextRect(m_rcBar[sbMODE], m_rcBar[sbMODE].Left+1, m_rcBar[sbMODE].Top+1, g_tDispModeTable[m_pRxSet->m_Mode]);
}
//---------------------------------------------------------------------------
void __fastcall TRxViewDlg::DrawSN(void)
{
	TCanvas *pCanvas = StatusBar->Canvas;
	pCanvas->Brush->Color = clBtnFace;
//	pCanvas->FillRect(m_rcBar[sbSN]);
	char bf[256];
	if( m_ShowTiming ){
		if( m_pRxSet->IsRTTY() ){
			if( m_pRxSet->m_pDem->m_Decode.IsRTTYTmg() ){
				sprintf(bf, "%.2lfms", m_pRxSet->m_pDem->m_Decode.GetRTTYTmg());
      }
      else {
				strcpy(bf, "***");
      }
    }
    else {
		  sprintf(bf, "%dppm", m_pRxSet->m_pDem->GetClockError());
    }
  }
  else {
	  sprintf(bf, "S/N=%ddB", m_pRxSet->m_StgFFT.DispSig);
  }
	pCanvas->TextRect(m_rcBar[sbSN], m_rcBar[sbSN].Left+1, m_rcBar[sbSN].Top+1, bf);
}
//---------------------------------------------------------------------------
void __fastcall TRxViewDlg::DrawFREQ(void)
{
	TCanvas *pCanvas = StatusBar->Canvas;
	pCanvas->Brush->Color = m_pRxSet->m_fAFC ? clBtnFace : clBlue;
	if( !m_pRxSet->m_fAFC ) pCanvas->Font->Color = clYellow;
//	pCanvas->FillRect(m_rcBar[sbFREQ]);
	char bf[256];
    sprintf(bf, "%uHz", int(m_pRxSet->m_pDem->m_CarrierFreq + 0.5));
	pCanvas->TextRect(m_rcBar[sbFREQ], m_rcBar[sbFREQ].Left+1, m_rcBar[sbFREQ].Top+1, bf);
	if( !m_pRxSet->m_fAFC ) pCanvas->Font->Color = clBlack;
}
//---------------------------------------------------------------------------
void __fastcall TRxViewDlg::CalcWaterCenter(void)
{
	if( !m_pRxSet ) return;
	int fo = m_pRxSet->m_pDem->m_CarrierFreq;
    int fw = (m_WaterWidth - m_pRxSet->m_pDem->m_MFSK_BW)/2;
	if( fw < 30 ) fw = 30;
	if( !sys.m_MFSK_Center && m_pRxSet->IsMFSK() ){
		if( m_pRxSet->m_Mode == MODE_mfsk_U ){
       	    m_WaterLowFQ = fo - fw;
           	m_WaterHighFQ = m_WaterLowFQ + m_WaterWidth;
        }
        else {
       	    m_WaterHighFQ = fo + fw;
           	m_WaterLowFQ = m_WaterHighFQ - m_WaterWidth;
        }
    }
    else {
	    m_WaterLowFQ = fo - m_WaterWidth/2;
		m_WaterHighFQ = fo + m_WaterWidth/2;
    }
	if( m_WaterLowFQ < 0 ){
		m_WaterLowFQ = 0;
        m_WaterHighFQ = m_WaterLowFQ + m_WaterWidth;
    }
    else if( m_WaterHighFQ > 3000 ){
		m_WaterHighFQ = 3000;
        m_WaterLowFQ = m_WaterHighFQ - m_WaterWidth;
	}
}
//---------------------------------------------------------------------------
void __fastcall TRxViewDlg::UpdateWaterWidth(void)
{
	if( !m_pRxSet ) return;

	m_WaterWidth = m_pRxSet->m_WaterW;
    m_FFTWindow = (m_WaterWidth-1) * FFT_SIZE / MainVARI->m_FFTSampFreq;
	CalcWaterCenter();
}
//---------------------------------------------------------------------------
void __fastcall TRxViewDlg::UpdateStatus(void)
{
	if( !m_pRxSet ) return;

	DrawMode();
    DrawSN();
    DrawFREQ();
	if( m_pBitmapLevel ){
	    DrawLevel();
		StatusBar->Canvas->Draw(m_rcBar[sbLEVEL].Left, m_rcBar[sbLEVEL].Top, m_pBitmapLevel);
    }
	if( m_pBitmapFFT ){
	    DrawWater(FALSE);
		StatusBar->Canvas->Draw(m_rcBar[sbWATER].Left, m_rcBar[sbWATER].Top, m_pBitmapFFT);
        DrawWaterCursor();
    }
}
//---------------------------------------------------------------------------
void __fastcall TRxViewDlg::FormResize(TObject *Sender)
{
	if( m_fDisEvent ) return;

	m_Dump.Resize();
}
//---------------------------------------------------------------------------
void __fastcall TRxViewDlg::DrawLevel(void)
{
	if( m_pRxSet == NULL ) return;
	if( !m_pBitmapLevel ) return;

    TCanvas *pCanvas = m_pBitmapLevel->Canvas;
    TRect rc;
    rc.Left = 0; rc.Top = 0;
    rc.Right = m_levelXW; rc.Bottom = m_levelYW;
    pCanvas->Brush->Color = clBlack;
    pCanvas->Pen->Color = clBlack;
    pCanvas->FillRect(rc);

//	pCanvas->Pen->Color = clYellow;
	int d = m_pRxSet->m_StgFFT.Sig - 500;
	if( m_pRxSet->IsMFSK() && sys.m_MFSK_SQ_Metric ){
		d = m_pRxSet->m_pDem->GetMFSKMetric(0);
	}
   	if( d > LEVELMAX ) d = LEVELMAX;

	if( m_pRxSet->m_pDem->m_AGC.GetOver() && !MainVARI->m_TX ){
		pCanvas->Brush->Color = clRed;
    }
	else if( !m_pRxSet->m_pDem->m_Lock ){
		pCanvas->Brush->Color = m_pRxSet->m_SQ ? clBlue : clGray;
    }
    else {
		pCanvas->Brush->Color = m_pRxSet->m_SQ ? TColor(RGB(0,255,0)) : clGray;
    }
  	rc.Right = (d * m_levelXW / LEVELMAX);
   	pCanvas->FillRect(rc);
    pCanvas->Pen->Color = m_pRxSet->m_SQ ? clBlack : clWhite;
	d = (m_pRxSet->m_SQLevel * m_levelXW / LEVELMAX);
	pCanvas->MoveTo(d, 0); pCanvas->LineTo(d, m_levelYW);
}
//---------------------------------------------------------------------------
void __fastcall TRxViewDlg::InitWater(void)
{
	if( !m_pRxSet ) return;

	if( m_pRxSet->m_pFFT->m_FFTGain ){
        m_StgWater.Sum = 0;
   	    m_StgWater.Max = 5120;
       	m_StgWater.VW = 4800;
    }
    else {
        m_StgWater.Sum = 4800;
   	    m_StgWater.Max = 8000;
       	m_StgWater.VW = 4800;
    }
}
//---------------------------------------------------------------------------
void __fastcall TRxViewDlg::DrawWater(BOOL fClear)
{
	if( m_pRxSet == NULL ) return;
	if( !m_pBitmapFFT ) return;

    TCanvas *pCanvas = m_pBitmapFFT->Canvas;

    int fo = m_pRxSet->m_pDem->m_CarrierFreq;
	if( !m_MouseDown ){
		int ww = m_WaterWidth / ((m_pRxSet->Is170()||m_pRxSet->IsMFSK()) ? 4 : 8);
		int cfo = fo;
        if( m_pRxSet->IsMFSK() ){
			int off = m_pRxSet->m_pDem->m_MFSK_BW/2;
			if( m_pRxSet->m_Mode == MODE_mfsk_L ){
				cfo -= off;
            }
            else {
				cfo += off;
            }
        }
		if( (cfo < (m_WaterLowFQ+ww)) || (cfo > (m_WaterHighFQ-ww)) ){
			CalcWaterCenter();
	    }
    }

    TRect rc;
   	rc.Left = 0; rc.Top = 0; rc.Right = m_fftXW; rc.Bottom = m_fftYW;
	if( fClear ){
	   	pCanvas->Brush->Color = MainVARI->m_tWaterColset[0].c;
   		pCanvas->Pen->Color = MainVARI->m_tWaterColset[0].c;
   		pCanvas->FillRect(rc);
        InitWater();
    }

	TRect trc = rc;
    rc.Bottom--;
	trc.Top++;
	pCanvas->CopyRect(trc, pCanvas, rc);
	int xo = ((m_WaterLowFQ+(MainVARI->m_FFTSampFreq/FFT_SIZE))*FFT_SIZE/MainVARI->m_FFTSampFreq) + 0.5;
	int xl = 0.5 + (fo - m_pRxSet->m_Speed - m_WaterLowFQ) * m_fftXW / m_WaterWidth;
	int xh = 0.5 + (fo + m_pRxSet->m_Speed - m_WaterLowFQ) * m_fftXW / m_WaterWidth;
   	int x, y, xx;
	int sum = 0;
    int max = 0;
    int wmax = 0;
    double k = 256.0 / m_StgWater.VW;
   	for( x = 0; x < m_fftXW; x++ ){
		xx = xo + (x * m_FFTWindow / m_fftXW);
        y = MainVARI->m_fftout[xx];
        if( max < y ) max = y;
		if( (x >= xl) && (x <= xh) ){
            if( wmax < y ) wmax = y;
        }
		sum += y;

        y = (y - m_StgWater.Sum) * k;
		if( y < 0 ) y = 0;
        if( y >= 256 ) y = 255;

        pCanvas->Pixels[x][0] = MainVARI->m_tWaterColors[y];
   	}
    sum /= m_fftXW;
	if( (wmax-sum) >= 320 ){
       	max = wmax;
    }
	m_StgWater.Sum = (m_StgWater.Sum + sum) / 2;
    m_StgWater.Max = (m_StgWater.Max + max) / 2;
    m_StgWater.VW = m_StgWater.Max - m_StgWater.Sum;
	int low, high;
	if( m_pRxSet->m_pFFT->m_FFTGain ){
       	low = 5120; high = 7000;
    }
    else {
       	low = 6000; high = 6000;
    }
    if( MainVARI->m_TX == txINTERNAL ) high = 100000;
    if( m_StgWater.VW < low ) m_StgWater.VW = low;
    if( m_StgWater.VW > high ) m_StgWater.VW = high;
}
//---------------------------------------------------------------------------
void __fastcall TRxViewDlg::DrawWaterCursor(void)
{
	if( !m_pRxSet ) return;

    int fo = m_pRxSet->m_pDem->m_CarrierFreq;
	int xx = m_rcBar[sbWATER].Left + 0.5 + (fo - m_WaterLowFQ) * m_fftXW / m_WaterWidth;
	if( xx < m_rcBar[sbWATER].Left + 4 ) return;
    if( xx > m_rcBar[sbWATER].Right - 4 ) return;
    TCanvas *pCanvas = StatusBar->Canvas;
	int fb = m_rcBar[sbWATER].Bottom - 1;
#if 0
	if( (m_pRxSet->m_Mode == MODE_RTTY) || (m_pRxSet->m_Mode == MODE_U_RTTY) ){
		pCanvas->Pen->Width = 3;
		pCanvas->Pen->Color = MainVARI->m_tWaterColset[4].c;
		int x = m_rcBar[sbWATER].Left + 0.5 + (fo - m_WaterLowFQ - 85.5) * m_fftXW / m_WaterWidth;
        pCanvas->MoveTo(x, fb-4);
        pCanvas->LineTo(x, fb-1);
		x = m_rcBar[sbWATER].Left + 0.5 + (fo - m_WaterLowFQ + 85.5) * m_fftXW / m_WaterWidth;
        pCanvas->LineTo(x, fb-1);
        pCanvas->LineTo(x, fb-5);
		pCanvas->Pen->Width = 1;
    }
#endif
	POINT pt[3];
    pt[0].x = xx - 4; pt[0].y = fb;
    pt[1].x = xx + 4; pt[1].y = fb;
    pt[2].x = xx; pt[2].y = fb - 6;

	pCanvas->Pen->Color = MainVARI->m_tWaterColset[3].c;
	pCanvas->Brush->Color = MainVARI->m_tWaterColset[4].c;
	pCanvas->Polygon(pt, 2);
}
//---------------------------------------------------------------------------
void __fastcall TRxViewDlg::StatusBarDrawPanel(TStatusBar *StatusBar,
      TStatusPanel *Panel, const TRect &Rect)
{
	if( !m_pRxSet ) return;

	if( Panel == StatusBar->Panels->Items[sbMODE] ){
		m_rcBar[sbMODE] = Rect;
        DrawMode();
	}
	else if( Panel == StatusBar->Panels->Items[sbSN] ){
		m_rcBar[sbSN] = Rect;
        DrawSN();
	}
	else if( Panel == StatusBar->Panels->Items[sbFREQ] ){
		m_rcBar[sbFREQ] = Rect;
        DrawFREQ();
	}
	else if( Panel == StatusBar->Panels->Items[sbLEVEL] ){
		if( !m_pBitmapLevel || ((Rect.Right-Rect.Left) != m_pBitmapLevel->Width) ){
			m_rcBar[sbLEVEL] = Rect;
			m_levelXW = Rect.Right - Rect.Left;
	    	m_levelYW = Rect.Bottom - Rect.Top;
			if( !m_pBitmapLevel ){
				m_pBitmapLevel = new Graphics::TBitmap;
	        }
    	    m_pBitmapLevel->Width = m_levelXW;
        	m_pBitmapLevel->Height = m_levelYW;
	    }
		StatusBar->Canvas->Draw(m_rcBar[sbLEVEL].Left, m_rcBar[sbLEVEL].Top, m_pBitmapLevel);
    }
	else if( Panel == StatusBar->Panels->Items[sbWATER] ){
		if( !m_pBitmapFFT || ((Rect.Right-Rect.Left) != m_pBitmapFFT->Width) ){
			m_rcBar[sbWATER] = Rect;
			m_fftXW = Rect.Right - Rect.Left;
	    	m_fftYW = Rect.Bottom - Rect.Top;
			if( !m_pBitmapFFT ){
				m_pBitmapFFT = new Graphics::TBitmap;
	        }
    	    m_pBitmapFFT->Width = m_fftXW;
        	m_pBitmapFFT->Height = m_fftYW;
            DrawWater(TRUE);
	    }
		StatusBar->Canvas->Draw(m_rcBar[sbWATER].Left, m_rcBar[sbWATER].Top, m_pBitmapFFT);
        DrawWaterCursor();
    }
	else if( Panel == StatusBar->Panels->Items[sbEND] ){
		AnsiString as = PC->Font->Name;
//		TFontStyles fs = PC->Font->Style;
//	    AddStyle(as, PC->Font->Charset, FontStyle2Code(fs));
	    AddStyle(as, PC->Font->Charset, 0);
        StatusBar->Canvas->TextRect(Rect, Rect.Left + 1, Rect.Top + 1, as.c_str());
    }
}
//---------------------------------------------------------------------------
void __fastcall TRxViewDlg::FormDestroy(TObject *Sender)
{
	if( m_pBitmapLevel ){
		delete m_pBitmapLevel;
        m_pBitmapLevel = NULL;
    }
}
//---------------------------------------------------------------------------
void __fastcall TRxViewDlg::StatusBarMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
    m_MouseDown = FALSE;
	if( !m_pRxSet ) return;
	if( (Button != mbLeft) && (Button != mbRight) ) return;

	int N = -1;
	int xl = 0;
    int xr = 0;
	for( int i = 0; i <= sbEND; i++ ){
		xr += StatusBar->Panels->Items[i]->Width;
		if( i == sbEND ) xr += StatusBar->Width;
		if( (X > (xl+4)) && (X < (xr-4)) ){
			N = i;
            break;
        }
        xl = xr;
    }
	switch(N){
		case sbMODE:
			PupMode->Popup(Left+X, Top+StatusBar->Top + Y);
            break;
		case sbSN:
			PupS->Popup(Left+X, Top+StatusBar->Top + Y);
            break;
		case sbFREQ:
			Pup->Popup(Left+X, Top+StatusBar->Top + Y);
            break;
		case sbLEVEL:
			if( Button == mbLeft ){
				int x = X - m_rcBar[sbLEVEL].Left;
				x = (x * LEVELMAX) / m_levelXW;
				m_pRxSet->m_SQLevel = ((x + 5) / 10) * 10;
                m_MouseDown = sbLEVEL;
		    }
            else if( Button == mbRight ){
				MainVARI->PupSQ->PopupComponent = this;
				MainVARI->PupSQ->Popup(Left+X, Top+StatusBar->Top + Y);
            }
        	break;
		case sbWATER:
			if( Button == mbLeft ){
				int x = X - m_rcBar[sbWATER].Left;
				int fo = m_WaterLowFQ + x * m_WaterWidth / m_fftXW;
				if( m_WaterWidth >= 400 ){
					fo = MainVARI->GetSignalFreq(fo, m_WaterWidth >= 800 ? 50 : 32, m_pRxSet);
                }
				m_pRxSet->m_pDem->m_Decode.Reset();
				m_pRxSet->m_pDem->ResetMeasMFSK();
                m_pRxSet->m_AFCTimerMFSK = 0;
                m_pRxSet->SetCarrierFreq(fo);
                m_pRxSet->m_pDem->m_fAFC = m_pRxSet->m_fAFC;
                m_pRxSet->m_SQTimer = 0;
                m_MouseDown = sbWATER;
			}
            else if( Button == mbRight ){
				PupW->Popup(Left+X, Top+StatusBar->Top + Y);
            }
        	break;
        case sbEND:
			if( Button == mbRight ){
				MainVARI->PupCharset->PopupComponent = StatusBar;
				MainVARI->PupCharset->Popup(Left+X, Top+StatusBar->Top + Y);
            }
            else {
				ChangeFont();
            }
        	break;
        default:
        	break;
    }
}
//---------------------------------------------------------------------------
void __fastcall TRxViewDlg::ChangeFont(void)
{
	TFontDialog *pBox = new TFontDialog(this);
	pBox->Font = PC->Font;
    if( pBox->Execute() ){
		PC->Font = pBox->Font;
        OnUpdateFont();
    }
    delete pBox;
}
//---------------------------------------------------------------------------
void __fastcall TRxViewDlg::SBarChange(TObject *Sender)
{
	m_Dump.OnScrollBarChange();
}
//---------------------------------------------------------------------------
void __fastcall TRxViewDlg::KFQSWClick(TObject *Sender)
{
	::PostMessage(MainVARI->Handle, WM_WAVE, waveSwapRxView, DWORD(this));
//
}
//---------------------------------------------------------------------------

void __fastcall TRxViewDlg::PBoxMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	BOOL f = m_Dump.ClearAllSelect(FALSE);
	int r = m_Dump.GetWindowText(MainVARI->m_StrSel, X, Y);
	if( f || r ) m_Dump.Paint();
    if( r ){
		MainVARI->m_UStrSel = jstrupr(MainVARI->m_StrSel.c_str());
		if( (Button == mbLeft)||(Button == mbRight) ){
			RECT rc;
			::GetWindowRect(PC->Handle, &rc);
			MainVARI->PupRX->PopupComponent = this;
			MainVARI->PupRX->Popup(rc.left+X, rc.top+Y);
		}
    }
    else if( Button == mbRight ){
		RECT rc;
		::GetWindowRect(PC->Handle, &rc);
		MainVARI->PupRXW->PopupComponent = StatusBar;
		MainVARI->PupRXW->Popup(rc.left+X, rc.top+Y);
    }
}
//---------------------------------------------------------------------------
void __fastcall TRxViewDlg::StatusBarMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
	if( m_MouseDown == sbLEVEL ){
		int x = X - m_rcBar[sbLEVEL].Left;
		x = (x * LEVELMAX) / m_levelXW;
        if( (x >= 0) && (x <= LEVELMAX) ){
			m_pRxSet->m_SQLevel = x;
        }
    }
	else if( m_MouseDown == sbWATER ){
		int x = X - m_rcBar[sbWATER].Left;
		int fo = m_WaterLowFQ + x * m_WaterWidth / m_fftXW;
        if( fo < MIN_CARRIER ) fo = MIN_CARRIER;
        if( fo > sys.m_MaxCarrier ) fo = sys.m_MaxCarrier;
		m_pRxSet->m_pDem->m_Decode.Reset();
		m_pRxSet->m_pDem->ResetMeasMFSK();
        m_pRxSet->SetCarrierFreq(fo);
    }
}
//---------------------------------------------------------------------------

void __fastcall TRxViewDlg::StatusBarMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if( m_MouseDown == sbWATER ){
		if( m_WaterWidth >= 400 ){
			int fo = MainVARI->GetSignalFreq(m_pRxSet->m_pDem->m_CarrierFreq, m_WaterWidth >= 800 ? 50 : 32, m_pRxSet);
			m_pRxSet->m_pDem->m_Decode.Reset();
			m_pRxSet->m_pDem->ResetMeasMFSK();
    	    m_pRxSet->SetCarrierFreq(fo);
        }
    }
	m_MouseDown = FALSE;
}
//---------------------------------------------------------------------------
void __fastcall TRxViewDlg::SetMode(int mode)
{
    if( m_pRxSet->m_Mode != mode ){
		double bw = m_pRxSet->GetBandWidth();
		m_pRxSet->SetMode(mode);
		m_Dump.SetRTTY(m_pRxSet->IsRTTY());
		if( bw != m_pRxSet->GetBandWidth() ){
        	CalcWaterCenter();
	        StatusBar->Invalidate();
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TRxViewDlg::SetSpeed(double spd)
{
	if( m_pRxSet->IsMFSK() ){
		m_pRxSet->SetMFSKType(MFSK_Speed2Type(spd));
    }
    else if( m_pRxSet->m_Speed != spd ){
		m_pRxSet->SetSpeed(spd);
    }
}
//---------------------------------------------------------------------------
void __fastcall TRxViewDlg::PupModePopup(TObject *Sender)
{
	PupMode->Items->Items[m_pRxSet->m_pDem->m_Type]->Checked = TRUE;
	BOOL f = m_pRxSet->Is170();
    NR->Visible = f;
    KRFFT->Visible = f;
    KRIIR->Visible = f;
	if( f ){
		if( m_pRxSet->m_RTTYFFT ){
	    	KRFFT->Checked = TRUE;
	    }
	    else {
	    	KRIIR->Checked = TRUE;
	    }
    }
	int spd = (m_pRxSet->m_Speed * 1000) + 0.5;
    f = FALSE;
	if( m_pRxSet->IsMFSK() ){
		switch(m_pRxSet->m_MFSK_TYPE){
			case typMFSK4:
				KS0->Checked = TRUE;
            	break;
			case typMFSK8:
				KS1->Checked = TRUE;
            	break;
			case typMFSK11:
				KS2->Checked = TRUE;
            	break;
            case typMFSK16:
				KS3->Checked = TRUE;
            	break;
            case typMFSK22:
				KS4->Checked = TRUE;
            	break;
            case typMFSK31:
				KS5->Checked = TRUE;
            	break;
            case typMFSK32:
				KS6->Checked = TRUE;
            	break;
            case typMFSK64:
				KS7->Checked = TRUE;
            	break;
        }
    }
	else if( m_pRxSet->IsRTTY() ){
		switch(spd){
			case 45450:
				KS0->Checked = TRUE;
            	break;
            case 75000:
				KS1->Checked = TRUE;
            	break;
            case 110000:
				KS2->Checked = TRUE;
            	break;
        }
    }
    else {
	switch(spd){
		case 15625:
        	KS0->Checked = TRUE;
            break;
		case 20000:
			KS1->Checked = TRUE;
            break;
        case 31250:
			KS2->Checked = TRUE;
            break;
        case 62500:
			KS3->Checked = TRUE;
            break;
        case 93750:
			KS4->Checked = TRUE;
            break;
        case 125000:
			KS5->Checked = TRUE;
            break;
        case 250000:
			KS6->Checked = TRUE;
            break;
        default:
			char bf[256];
            StrDbl(bf, m_pRxSet->m_Speed);
            strcat(bf, " bps");
			KS7->Caption = bf;
        	KS7->Checked = TRUE;
            f = TRUE;
            break;
    }
    }
    if( m_pRxSet->IsMFSK() ){
		KS0->Caption = "3.9063 baud, 32tones\tmfsk4";
		KS1->Caption = "7.8125 baud, 32tones\tmfsk8";
		KS2->Caption = "10.767 baud, 16tones\tmfsk11";
		KS3->Caption = "15.625 baud, 16tones\tmfsk16 (Standard)";
		KS4->Caption = "21.533 baud, 16tones\tmfsk22";
		KS5->Caption = "31.25 baud, 8tones\tmfsk31";
		KS6->Caption = "31.25 baud, 16tones\tmfsk32";
		KS7->Caption = "62.5 baud, 16tones\tmfsk64";
		KS3->Visible = TRUE;
		KS4->Visible = TRUE;
		KS5->Visible = TRUE;
		KS6->Visible = TRUE;
        KS7->Visible = TRUE;
    }
	else if( m_pRxSet->IsRTTY() ){
		KS0->Caption = "45.45 baud\tStandard RTTY";
		KS1->Caption = "75.0 baud";
		KS2->Caption = "110.0 baud";
		KS3->Visible = FALSE;
		KS4->Visible = FALSE;
		KS5->Visible = FALSE;
		KS6->Visible = FALSE;
	    KS7->Visible = f;
    }
    else {
		KS0->Caption = "15.625 bps";
		KS1->Caption = "20.0 bps";
		KS2->Caption = "31.25 bps";
		KS3->Caption = "62.5 bps";
        KS4->Caption = "93.75 bps";
        KS5->Caption = "125.0 bps";
        KS6->Caption = "250.0 bps";
		KS3->Visible = TRUE;
		KS4->Visible = TRUE;
		KS5->Visible = TRUE;
		KS6->Visible = TRUE;
	    KS7->Visible = f;
    }
}
//---------------------------------------------------------------------------
void __fastcall TRxViewDlg::OnMouseWheel(TMessage Message)
{
	if( !sys.m_EnableMouseWheel ) return;
	if( m_Dump.OnMouseWheel(Message.WParam >> 16) ){
		MainVARI->OpenWheelTimer(&m_Dump);
    }
}
//---------------------------------------------------------------------------
void __fastcall TRxViewDlg::PupWPopup(TObject *Sender)
{
	switch(m_WaterWidth){
		case 200:
			KW1->Checked = TRUE;
        	break;
        case 400:
			KW2->Checked = TRUE;
        	break;
        default:
			KW3->Checked = TRUE;
        	break;
    }
}
//---------------------------------------------------------------------------
void __fastcall TRxViewDlg::KW1Click(TObject *Sender)
{
	int fw = 800;
	if( Sender == KW1 ){
		fw = 200;
    }
    else if( Sender == KW2 ){
		fw = 400;
    }
    if( !m_pRxSet ) return;
    m_pRxSet->m_WaterW = fw;
    UpdateWaterWidth();
}
//---------------------------------------------------------------------------
void __fastcall TRxViewDlg::KS0Click(TObject *Sender)
{
	if( m_pRxSet->IsMFSK() ){
	    SetSpeed(3.90625);
    }
    else if( m_pRxSet->IsRTTY() ){
		SetSpeed(45.45);
    }
    else {
		SetSpeed(15.625);
    }
}
//---------------------------------------------------------------------------
void __fastcall TRxViewDlg::KS1Click(TObject *Sender)
{
	if( m_pRxSet->IsMFSK() ){
	    SetSpeed(7.8125);
    }
    else if( m_pRxSet->IsRTTY() ){
		SetSpeed(75.0);
    }
    else {
		SetSpeed(20.0);
    }
}
//---------------------------------------------------------------------------
void __fastcall TRxViewDlg::KS2Click(TObject *Sender)
{
	if( m_pRxSet->IsMFSK() ){
	    SetSpeed(10.767);
    }
	else if( m_pRxSet->IsRTTY() ){
		SetSpeed(110.0);
    }
	else {
	    SetSpeed(31.25);
    }
}
//---------------------------------------------------------------------------
void __fastcall TRxViewDlg::KS3Click(TObject *Sender)
{
	if( m_pRxSet->IsMFSK() ){
	    SetSpeed(15.625);
    }
	else {
	    SetSpeed(62.5);
    }
}
//---------------------------------------------------------------------------
void __fastcall TRxViewDlg::KS4Click(TObject *Sender)
{
	if( m_pRxSet->IsMFSK() ){
	    SetSpeed(21.533);
    }
	else {
	    SetSpeed(93.75);
    }
}
//---------------------------------------------------------------------------
void __fastcall TRxViewDlg::KS5Click(TObject *Sender)
{
	if( m_pRxSet->IsMFSK() ){
	    SetSpeed(31.25);
    }
	else {
	    SetSpeed(125.0);
    }
}
//---------------------------------------------------------------------------
void __fastcall TRxViewDlg::KS6Click(TObject *Sender)
{
	if( m_pRxSet->IsMFSK() ){
	    SetSpeed(32.0);
    }
	else {
	    SetSpeed(250.0);
    }
}
//---------------------------------------------------------------------------
void __fastcall TRxViewDlg::KS7Click(TObject *Sender)
{
	if( m_pRxSet->IsMFSK() ){
	    SetSpeed(62.5);
    }
}
//---------------------------------------------------------------------------
void __fastcall TRxViewDlg::PupSPopup(TObject *Sender)
{
	KSTM->Caption = m_pRxSet->IsRTTY() ? "Timing (ms)" : "Timing (ppm)";
	if( m_ShowTiming ){
		KSTM->Checked = TRUE;
    }
    else {
		KSSN->Checked = TRUE;
    }
}
//---------------------------------------------------------------------------
void __fastcall TRxViewDlg::KSSNClick(TObject *Sender)
{
	m_ShowTiming = (Sender != KSSN);
    StatusBar->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TRxViewDlg::KFAFCClick(TObject *Sender)
{
	if( !m_pRxSet ) return;

	InvMenu(KFAFC);
	m_pRxSet->m_pDem->m_fAFC = m_pRxSet->m_fAFC = KFAFC->Checked;
    StatusBar->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TRxViewDlg::PupPopup(TObject *Sender)
{
	if( !m_pRxSet ) return;

	KFAFC->Checked = m_pRxSet->m_fAFC;
}
//---------------------------------------------------------------------------
void __fastcall TRxViewDlg::KRIIRClick(TObject *Sender)
{
	if( !m_pRxSet ) return;

	m_pRxSet->m_RTTYFFT = FALSE;
	m_pRxSet->m_pDem->m_fRTTYFFT = FALSE;
}
//---------------------------------------------------------------------------
void __fastcall TRxViewDlg::KRFFTClick(TObject *Sender)
{
	if( !m_pRxSet ) return;

	m_pRxSet->m_RTTYFFT = TRUE;
	m_pRxSet->m_pDem->m_fRTTYFFT = TRUE;
}
void __fastcall TRxViewDlg::AddModeMenu(void)
{
	for( int i = 0; i < AN(g_tm); i++ ){
        TMenuItem *pm = new TMenuItem(this);
        pm->Caption = g_tm[i];
		pm->GroupIndex = 1;
        pm->RadioItem = TRUE;
        pm->OnClick = KMClick;
		PupMode->Items->Insert(i, pm);
    }
}
//---------------------------------------------------------------------------
void __fastcall TRxViewDlg::KMClick(TObject *Sender)
{
	int N = PupMode->Items->IndexOf((TMenuItem *)Sender);
    if( (N >= 0) && (N < MODE_END) ){
	    SetMode(N);
    }
}
//---------------------------------------------------------------------------
static int g_tf[]={1000, 1200, 1500, 1750, 2000, 2210};
void __fastcall TRxViewDlg::AddToneMenu(void)
{
	for( int i = 0; i < AN(g_tf); i++ ){
		char bf[32];
        sprintf(bf, "%d", g_tf[i]);
        TMenuItem *pm = new TMenuItem(this);
        pm->Caption = bf;
        pm->OnClick = KFQClick;
		Pup->Items->Insert(i, pm);
    }
}
//---------------------------------------------------------------------------
void __fastcall TRxViewDlg::KFQClick(TObject *Sender)
{
	if( !m_pRxSet ) return;
	for( int i = 0; i < AN(g_tf); i++ ){
		if( Sender == (TObject *)Pup->Items->Items[i] ){
			m_pRxSet->SetCarrierFreq(g_tf[i]);
            break;
        }
    }
}
//---------------------------------------------------------------------------

