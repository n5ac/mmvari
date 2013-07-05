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

#include "ClockAdj.h"
#include "Main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//TClockAdjDlg *ClockAdjDlg;
//---------------------------------------------------------------------------
__fastcall TClockAdjDlg::TClockAdjDlg(TComponent* Owner)
	: TForm(Owner)
{
	m_fDisEvent = TRUE;
	Font->Name = ((TForm *)Owner)->Font->Name;
	Font->Charset = ((TForm *)Owner)->Font->Charset;
	if( sys.m_MsgEng ){
		Caption = "Calibrating the SoundCard (Clock-RX) with a Time Standard Broadcast Station";
		SBClose->Caption = "Cancel";
    }
    else {
		char bf[256];
		sprintf(bf, "サウンドカード(%uHz系)の受信クロック値の較正", SAMPBASE);
        Caption = bf;
    }
	m_SampFreq = SAMPFREQ;
    m_ToneFreq = 1000.0;
    m_Clock.Create(PBoxT->Width);
    m_Clock.SetSampleFreq(m_SampFreq);
    m_Clock.SetToneFreq(m_ToneFreq);
    m_pBitmapFFT = new Graphics::TBitmap;
    m_pBitmapFFT->Width = PBoxFFT->Width;
    m_pBitmapFFT->Height = PBoxFFT->Height;
    m_pBitmapT = new Graphics::TBitmap;
//    m_pBitmapT->Width = PBoxT->Width;
//    m_pBitmapT->Height = PBoxT->Height;

    m_FFTWindow = (3000-1) * FFT_SIZE / MainVARI->m_FFTSampFreq;

	m_StgT.Sum = 0;
	m_StgT.Max = 16384;
    m_StgT.VW = 16384;

	m_MouseDown = 0;
    m_Point = 0;
    m_PointRX = 0;
    m_CursorX = 640;
	CreateWaterColors();
    CBTone->Text = m_ToneFreq;

    m_fDisEvent = FALSE;
}
//---------------------------------------------------------------------------
void __fastcall TClockAdjDlg::FormDestroy(TObject *Sender)
{
	if( m_pBitmapFFT ) delete m_pBitmapFFT;
	if( m_pBitmapT ) delete m_pBitmapT;
}
//---------------------------------------------------------------------------
void __fastcall TClockAdjDlg::FormCloseQuery(TObject *Sender,
      bool &CanClose)
{
	CanClose = TRUE;
	double d = SAMPFREQ - m_SampFreq;
    d = fabs(d);
    if( d >= 0.005 ){
		if( YesNoMB(sys.m_MsgEng ? "The clock value (%.2lfHz) is not applied, are you sure?":"クロック値(%.2lfHz)は適用されません. よろしいですか?", m_SampFreq) == IDNO ){
			CanClose = FALSE;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TClockAdjDlg::SBCloseClick(TObject *Sender)
{
	Close();
}
//---------------------------------------------------------------------------
void __fastcall TClockAdjDlg::FormClose(TObject *Sender,
      TCloseAction &Action)
{
	if( Action == caHide ){
		::PostMessage(MainVARI->Handle, WM_WAVE, waveClockAdj, DWORD(this));
    }
}
//---------------------------------------------------------------------------
void __fastcall TClockAdjDlg::Execute(void)
{
	char bf[256];
	sprintf(bf, "%.2lf", m_SampFreq);
	EditClock->Text = bf;
    UdClock->Position = 0;
    UpdatePPM();
	Visible = TRUE;
}
//---------------------------------------------------------------------
void __fastcall TClockAdjDlg::UpdatePPM(void)
{
	double dd = ((m_SampFreq - SAMPBASE) * 1000000.0 / SAMPBASE);

	char bf[64];
	sprintf(bf, "(%+dppm)", int(dd+0.5));
	LPPM->Caption = bf;
}
//---------------------------------------------------------------------------
void __fastcall TClockAdjDlg::DrawMsg(void)
{
	TCanvas *cp = m_pBitmapT->Canvas;

	cp->Font->Name = Font->Name;
	cp->Font->Charset = Font->Charset;
	cp->Font->Color = clWhite;
	cp->Font->Size = 10;
	int FH = cp->TextHeight("A");
	int Y = 5;
	if( sys.m_MsgEng ){
		cp->TextOut(10, Y, "1) Receive standard radio wave (e.g. BPM on 5MHz, 10MHz, 15MHz, 20MHz)."); Y+=FH;
		cp->TextOut(10, Y, "2) Tune into the tick sound."); Y+=FH;
		cp->TextOut(10, Y, "3) Continue listening to the sound for a while.  You have a vertical line."); Y+=FH;
		cp->TextOut(10, Y, "4) Click the lower point of the line."); Y+=FH;
		cp->TextOut(10, Y, "5) Click the upper point of the line."); Y+=FH;
		Y+=FH;
		cp->TextOut(10, Y, "You could use FAX broadcasting instead of WWV, but be sure it has"); Y+=FH;
		cp->TextOut(10, Y, "exact timing.");
	}
	else {
		cp->TextOut(10, Y, "1.BPM(WWV)をAMまたはSSBで受信し、この画面の下にあるToneに1000Hz"); Y+=FH;
		cp->TextOut(10, Y, "  または1800Hzを設定します. (放送周波数 = 5MHz, 10MHz, 15MHz, 20MHz)"); Y+=FH;
		cp->TextOut(10, Y, "2.SSBの場合は１秒チック音がToneに重なるように受信周波数を調整します."); Y+=FH;
		cp->TextOut(10, Y, "3.しばらく受信して縦または斜めの帯線が表示されるのを待ちます（数分）."); Y+=FH;
		cp->TextOut(10, Y, "4.表示された帯線の下の端点をクリックします."); Y+=FH;
		cp->TextOut(10, Y, "5.更に同じ帯線の上の端点をクリックするとクロック値が自動的に"); Y+=FH;
		cp->TextOut(10, Y, "  設定されます.（2点間の距離が長いほうが正確です）"); Y+=FH;
		cp->TextOut(10, Y, "6.最終的に帯線が垂直にまっすぐ表示されるようになればOKです."); Y+=FH+FH;
		cp->TextOut(10, Y, "- BPM(WWV)の代わりに、精度は劣りますが、JMH,JMGなどのFAX放送の0.5秒毎の"); Y+=FH;
		cp->TextOut(10, Y, "  デッドセクタを利用してもOKのようです."); Y+=FH;
		cp->TextOut(10, Y, "- NTTの時報サービスを利用することもできます."); Y+=FH+FH;
		cp->TextOut(10, Y, "左クリック - 調整の開始"); Y+=FH;
		cp->TextOut(10, Y, "右クリック - 垂直カーソル移動");
	}
}
//---------------------------------------------------------------------------
void __fastcall TClockAdjDlg::FormResize(TObject *Sender)
{
	SBClose->Left = PCC->ClientWidth - SBClose->Width - 2;
	SBSet->Left = SBClose->Left - SBSet->Width - 2;
}
//---------------------------------------------------------------------------
void __fastcall TClockAdjDlg::Do(short ds)
{
	if( m_Clock.Do(ds) ){
		TCanvas *pCanvas = m_pBitmapT->Canvas;
        TRect src;
        src.Left = 0; src.Right = m_pBitmapT->Width;
        src.Top = 0; src.Bottom = m_pBitmapT->Height;
        TRect trc = src;
		src.Bottom--;
        trc.Top++;
		pCanvas->CopyRect(trc, pCanvas, src);

		int sum = 0;
        int max = 0;
        int mp;
		int d;
        for( int i = 0; i < src.Right; i++ ){
			d = m_Clock.GetData(i);
			if( max < d ){
            	max = d;
                mp = i;
            }
            sum += d;

            d = (d - m_StgT.Sum) * 256 / m_StgT.VW;
            if( d >= 256 ) d = 255;
			if( d < 0 ) d = 0;
			pCanvas->Pixels[i][0] = m_tWaterColors[d];
        }
		pCanvas->Pixels[mp][0] = clRed;

        sum /= src.Right;
		m_StgT.Sum = (m_StgT.Sum + sum) / 2;
        m_StgT.Max = (m_StgT.Max + max) / 2;
        m_StgT.VW = m_StgT.Max - m_StgT.Sum;
        if( m_StgT.VW < 128 ) m_StgT.VW = 128;
		m_PointY++;
        m_PointRX++;
        if( m_PointRX >= PBoxT->Height ) m_PointRX = PBoxT->Height;
        PBoxTPaint(NULL);
    }
}
//---------------------------------------------------------------------------
void __fastcall TClockAdjDlg::DrawFFT(BOOL fClear)
{
	if( !m_pBitmapFFT ) return;

    TCanvas *pCanvas = m_pBitmapFFT->Canvas;
    TRect rc;
    rc.Left = 0; rc.Top = 0;
    rc.Right = m_fftXW; rc.Bottom = m_fftYW;
    pCanvas->Brush->Color = MainVARI->m_tFFTColset[0].c;
    pCanvas->Pen->Color = MainVARI->m_tFFTColset[0].c;
    pCanvas->FillRect(rc);

	pCanvas->Pen->Style = psDot;
    int xx, y;
    int fh = pCanvas->TextHeight("A");

	switch(MainVARI->m_FFTVType){
		case 0:
			pCanvas->Pen->Color = MainVARI->m_tFFTColset[3].c;
			for( y = 6; y <= 86; y += 20 ){
				xx = y * m_fftYW / 100;
    			pCanvas->MoveTo(0, xx); pCanvas->LineTo(m_fftXW, xx);
    		}
        	break;
        case 1:
			pCanvas->Pen->Color = MainVARI->m_tFFTColset[3].c;
			for( y = 6; y <= 66; y += 20 ){
				xx = y * m_fftYW / 60;
    			pCanvas->MoveTo(0, xx); pCanvas->LineTo(m_fftXW, xx);
    		}
        	break;
    }

#if 1
	pCanvas->Font->Height = -12;
	pCanvas->Font->Color = MainVARI->m_tFFTColset[2].c;
	pCanvas->Pen->Color = MainVARI->m_tFFTColset[3].c;
	MainVARI->DrawFreqScale(pCanvas, m_fftXW, m_fftYW, 0, 3000, fh, FALSE);
#else
	int A, L;
	A = 500; L = 500;
	f = 0;
    int f, fw;
	char bf[128];
	pCanvas->Font->Height = -12;
	pCanvas->Font->Color = MainVARI->m_tFFTColset[2].c;
	pCanvas->Pen->Color = MainVARI->m_tFFTColset[3].c;
    y = 0;
	for( ; f < 3000; f += A ){
	    xx = f * m_fftXW / 3000;
        if( xx >= m_fftXW ) break;
		if( !(f % 1000) || !(f % L) ){
			sprintf(bf, "%u", f);
            fw = pCanvas->TextWidth(bf)/2;
            if( (xx - fw) > 0 ) pCanvas->TextOut(xx - fw, y, bf);
//    		pCanvas->MoveTo(xx, y ? 0 : fh); pCanvas->LineTo(xx, y ? m_fftYW-fh : m_fftYW);
    		pCanvas->MoveTo(xx, fh); pCanvas->LineTo(xx, m_fftYW);
        }
        else {
    		pCanvas->MoveTo(xx, 0); pCanvas->LineTo(xx, m_fftYW);
        }
    }
#endif
	pCanvas->Pen->Color = clGreen;
	xx = (DEMSAMPFREQ*0.5) * m_fftXW / 3000;
   	pCanvas->MoveTo(xx, fh); pCanvas->LineTo(xx, m_fftYW);
	int rfo = m_ToneFreq;
	pCanvas->Pen->Style = psSolid;
	pCanvas->Pen->Color = MainVARI->m_tFFTColset[4].c;
	xx = 0.5 + rfo * m_fftXW / 3000;
   	pCanvas->MoveTo(xx, fh); pCanvas->LineTo(xx, m_fftYW);
	POINT pt[4];
	pt[0].x = xx; pt[0].y = fh;
    pt[1].x = xx - 3; pt[1].y = fh+3;
    pt[2].x = xx; pt[2].y = fh + 6;
    pt[3].x = xx + 3; pt[3].y = fh+3;
	pCanvas->Brush->Color = MainVARI->m_tFFTColset[4].c;
	pCanvas->Polygon(pt, 3);

    if( !fClear ){
		pCanvas->Pen->Style = psSolid;
		pCanvas->Pen->Color = MainVARI->m_tFFTColset[1].c;

		double k = double(PBoxFFT->Height)/double(MainVARI->PBoxFFT->Height);
		int *pFFT = MainVARI->m_fftout;
		int xo = 0;
    	int x, y;
		if( MainVARI->m_FFTVType == 1 ){
			int yo = m_fftYW * 40;
	    	for( x = 0; x < m_fftXW; x++ ){
				xx = xo + (x * m_FFTWindow / m_fftXW);
        	    y = pFFT[xx] * k;
				y = m_fftYW - ((y-yo)/60);
				if( x ){
	    		    pCanvas->LineTo(x, y);
            	}
            	else {
					pCanvas->MoveTo(x, y);
            	}
    		}
        }
        else {
	    	for( x = 0; x < m_fftXW; x++ ){
				xx = xo + (x * m_FFTWindow / m_fftXW);
        	    y = pFFT[xx] * k;
				y = m_fftYW - (y/100);
				if( x ){
	    		    pCanvas->LineTo(x, y);
            	}
            	else {
					pCanvas->MoveTo(x, y);
            	}
    		}
		}
    }
}
//---------------------------------------------------------------------------
void __fastcall TClockAdjDlg::UpdateFFT(void)
{
	DrawFFT(FALSE);
    PBoxFFTPaint(NULL);
}
//---------------------------------------------------------------------------
void __fastcall TClockAdjDlg::PBoxTPaint(TObject *Sender)
{
	PBoxT->Canvas->Draw(0, 0, m_pBitmapT);
    DrawCursor();
}
//---------------------------------------------------------------------------
void __fastcall TClockAdjDlg::PBoxFFTPaint(TObject *Sender)
{
	PBoxFFT->Canvas->Draw(0, 0, m_pBitmapFFT);
	MainVARI->DrawErrorMsg(PBoxFFT->Canvas, m_fftXW, m_fftYW, FALSE);
}
//---------------------------------------------------------------------------
void __fastcall TClockAdjDlg::PCFFTResize(TObject *Sender)
{
    m_pBitmapFFT->Width = PBoxFFT->Width;
    m_pBitmapFFT->Height = PBoxFFT->Height;
	TCanvas *pCanvas = m_pBitmapFFT->Canvas;
   	pCanvas->Brush->Color = MainVARI->m_tFFTColset[0].c;
	TRect rc;
    rc.Left = 0; rc.Top = 0;
    rc.Right = m_pBitmapFFT->Width;
    rc.Bottom = m_pBitmapFFT->Height;
    pCanvas->FillRect(rc);
	m_fftXW = m_pBitmapFFT->Width;
    m_fftYW = m_pBitmapFFT->Height;
    DrawFFT(TRUE);
}
//---------------------------------------------------------------------------
void __fastcall TClockAdjDlg::PCTResize(TObject *Sender)
{
	BOOL f = m_pBitmapT->Width != PBoxT->Width;

    m_pBitmapT->Width = PBoxT->Width;
    m_pBitmapT->Height = PBoxT->Height;
	if( f ){
		TCanvas *pCanvas = m_pBitmapT->Canvas;
   		pCanvas->Brush->Color = clBlack;
		TRect rc;
    	rc.Left = 0; rc.Top = 0;
	    rc.Right = m_pBitmapT->Width;
	    rc.Bottom = m_pBitmapT->Height;
	    pCanvas->FillRect(rc);

    	m_Clock.Create(PBoxT->Width);
        DrawMsg();
		m_PointRX = 0;
    }
}
//---------------------------------------------------------------------------
void __fastcall TClockAdjDlg::SBSetClick(TObject *Sender)
{
	MainVARI->SetSampleFreq(m_SampFreq, FALSE);
    Close();
}
//---------------------------------------------------------------------------
void __fastcall TClockAdjDlg::UdClockClick(TObject *Sender,
      TUDBtnType Button)
{
	double f;
	sscanf(AnsiString(EditClock->Text).c_str(), "%lf", &f);	//JA7UDE 0428
	if( Button == Comctrls::btNext ){
		f += 0.01;
	}
	else {
		f -= 0.01;
	}
	char bf[128];
    sprintf(bf, "%.2lf", f);
    EditClock->Text = bf;
    m_SampFreq = f;
    m_Clock.SetSampleFreq(f);
    UpdatePPM();
}
//---------------------------------------------------------------------------
void __fastcall TClockAdjDlg::DrawMessage(LPCSTR p)
{
	TCanvas *pCanvas = PBoxT->Canvas;
	int xr = pCanvas->TextWidth(p);
	int xl = (PBoxT->Width - xr)/2;
	xr += xl;
	int FH = pCanvas->TextHeight(p);
	int VC = PBoxT->Height - FH;
	pCanvas->Pen->Color = clWhite;
	pCanvas->Brush->Color = clBlack;
	pCanvas->RoundRect(xl-10, VC-FH, xr+10, VC+FH, 10, 10);
	pCanvas->Font->Color = clWhite;
	pCanvas->TextOut(xl, VC-FH/2, p);
}
//---------------------------------------------------------------------------
double __fastcall TClockAdjDlg::GetPointSamp(void)
{
	if( m_PointY == m_PointY2 ) return m_SampFreq;
	double d = (m_PointX2 - m_PointX);
	d = d * m_SampFreq / PBoxT->Width;
	d /= (m_PointY - m_PointY2);
	d += m_SampFreq;
    d = int(d * 100 + 0.5) * 0.01;
	return d;
}
//---------------------------------------------------------------------------
void __fastcall TClockAdjDlg::DrawCursor(void)
{
	TCanvas *pCanvas = PBoxT->Canvas;
	pCanvas->Pen->Color = clLime;
	pCanvas->Pen->Style = psDot;
	pCanvas->MoveTo(m_CursorX, 0);
	int rop = ::SetROP2(pCanvas->Handle, R2_MASKPENNOT);
	pCanvas->LineTo(m_CursorX, PBoxT->Height - 1);
	::SetROP2(pCanvas->Handle, rop);
	if( m_Point ){
		pCanvas->Pen->Color = clYellow;
		pCanvas->Pen->Style = psSolid;
		pCanvas->MoveTo(m_PointX, m_PointY);
		::SetROP2(pCanvas->Handle, R2_MASKPENNOT);
		pCanvas->LineTo(m_PointX2, m_PointY2);
		::SetROP2(pCanvas->Handle, rop);
		char bf[256];
		if( sys.m_MsgEng ){
			sprintf( bf, "OK : Left button,  Cancel : Right button   Clock=%.2lf", GetPointSamp());
		}
		else {
			sprintf( bf, "決定 : 左ボタン,  中止 : 右ボタン   Clock=%.2lf", GetPointSamp());
		}
		DrawMessage(bf);
		if( PBoxT->Cursor != crCross ) PBoxT->Cursor = crCross;
	}
	else {
		if( PBoxT->Cursor != crDefault ) PBoxT->Cursor = crDefault;
	}
}
//---------------------------------------------------------------------------
void __fastcall TClockAdjDlg::PBoxTMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if( Button == mbLeft ){
		if( m_Point ){
			m_Point = 0;
			if( m_PointY2 != m_PointY ){
				m_SampFreq = GetPointSamp();
				char bf[256];
				sprintf(bf, "%.2lf", m_SampFreq);
				EditClock->Text = bf;
                m_Clock.SetSampleFreq(m_SampFreq);
			    UpdatePPM();

				TRect rc;
                rc.Left = 0; rc.Right = m_pBitmapT->Width;
                rc.Top = 0;
                rc.Bottom = m_PointRX;
                TCanvas *pCanvas = m_pBitmapT->Canvas;
                pCanvas->Brush->Color = clBlack;
                pCanvas->FillRect(rc);
                m_PointRX = 0;
			}
		}
		else {
			m_PointX2 = m_PointX = X;
			m_PointY2 = m_PointY = Y;
			m_Point = 1;
		}
	}
	else if( m_Point ){
		m_Point = 0;
	}
	else {
		m_CursorX = X;
	}
	PBoxTPaint(NULL);
}
//---------------------------------------------------------------------------
void __fastcall TClockAdjDlg::PBoxTMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
	if( m_Point ){
		m_PointX2 = X;
		m_PointY2 = Y;
		PBoxTPaint(NULL);
	}
}
//---------------------------------------------------------------------------
void __fastcall TClockAdjDlg::CreateWaterColors(void)
{
	UCOL col[3];
    col[0].c = clBlack;
    col[1].c = clWhite;
    col[2].c = clYellow;
	int i;
    UCOL c;
	for( i = 0; i < 160; i++ ){
		c = GetGrade2(col, i, 160);
        m_tWaterColors[i] = c.c;
    }
    for( i = 0; i < 96; i++ ){
		c = GetGrade2(&col[1], i, 96);
        m_tWaterColors[i+160] = c.c;
    }
}
//---------------------------------------------------------------------------

void __fastcall TClockAdjDlg::CBToneChange(TObject *Sender)
{
	if( m_fDisEvent ) return;

	int f;
    sscanf(AnsiString(CBTone->Text).c_str(), "%d", &f);	//JA7UDE 0428
    if( (f >= 250) && (f <= 2700) ){
		SetToneFreq(f);
    }
}
//---------------------------------------------------------------------------
void __fastcall TClockAdjDlg::SetToneFreq(int f)
{
    if( f < 250 ) f = 250;
    if( f > 2700 ) f = 2700;
	m_ToneFreq = f;
    m_Clock.SetToneFreq(m_ToneFreq);
    m_fDisEvent++;
	CBTone->Text = m_ToneFreq;
    m_fDisEvent--;
}
//---------------------------------------------------------------------------

void __fastcall TClockAdjDlg::PBoxFFTMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if( Button != mbLeft ) return;

	SetToneFreq((X * 3000) / PBoxFFT->Width);
    m_MouseDown = 1;
}
//---------------------------------------------------------------------------

void __fastcall TClockAdjDlg::PBoxFFTMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
	if( m_MouseDown ){
		SetToneFreq((X * 3000) / PBoxFFT->Width);
    }
}
//---------------------------------------------------------------------------

void __fastcall TClockAdjDlg::PBoxFFTMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	m_MouseDown = FALSE;
}
//---------------------------------------------------------------------------

