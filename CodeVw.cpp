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

#include "CodeVw.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//TCodeView *CodeView;
//---------------------------------------------------------------------------
__fastcall TCodeView::TCodeView(TComponent* Owner)
	: TForm(Owner)
{
	if( sys.m_MsgEng ){
		Font->Name = "Arial";
		Font->Charset = ANSI_CHARSET;
        SBClose->Caption = "Close";
	}
    SBUS->Hint = "English(ANSI)";
    SBJA->Hint = sys.m_MsgEng ? "Japanese(Shift-JIS)" : "日本語(シフトJIS)";
    SBHL->Hint = sys.m_MsgEng ? "Korean(Hangul)" : "韓国語(ハングル)";
    SBBY->Hint = sys.m_MsgEng ? "Chinese(GB2312)" : "中国語(簡体)";
    SBBV->Hint = sys.m_MsgEng ? "Chinese(BIG5)" : "中国語(繁体)";
	m_hWnd = NULL;
    m_pFont = NULL;
	m_Base = 0;
    m_MouseDown = 0;
	CheckFontCharset();     
	SBJA->Enabled = sys.m_tFontFam[fmJA];
	SBHL->Enabled = sys.m_tFontFam[fmHL] || sys.m_tFontFam[fmJOHAB];
	SBBV->Enabled = sys.m_tFontFam[fmBV];
	SBBY->Enabled = sys.m_tFontFam[fmBY];

	UpdateXW();
#if 0
    int YW = GetSystemMetrics(SM_CYFULLSCREEN);
    Left = sys.m_CodeLeft;
    Top = sys.m_CodeTop;
    if( Left < 0 ) Left = 0;
    if( Top + Height > YW ) Top = YW - Height;
#endif
    ShowHint = sys.m_WinNT;
    FormCenter(this);
}
//---------------------------------------------------------------------------
WORD __fastcall TCodeView::GetEUDC(int y, int x)
{
const WORD _tOut[]={0xf040, 0xf050, 0xf060, 0xf070,
					0xf080, 0xf090, 0xf0a0, 0xf0b0,
                    0xf0c0, 0xf0d0, 0xf0e0, 0xf0f0,
                    0xf140, 0xf150, 0xf160, 0xf170,
};
const WORD _tOutHL[]={0xc9a0, 0xc9b0, 0xc9c0, 0xc9d0,
					0xc9e0, 0xc9f0, 0xfea0, 0xfeb0,
                    0xfec0, 0xfed0, 0xfee0, 0xfef0,
                    0, 0, 0, 0,
};
const WORD _tOutBV[]={0xfa40, 0xfa50, 0xfa60, 0xfa70,
					0xfaa0, 0xfab0, 0xfac0, 0xfad0,
                    0xfae0, 0xfaf0, 0xfb40, 0xfb50,
                    0xfb60, 0xfb70, 0xfba0, 0xfbb0,
};
const WORD _tOutBY[]={0xaaa0, 0xaab0, 0xaac0, 0xaad0,
					0xaae0, 0xaaf0, 0xaba0, 0xabb0,
                    0xabc0, 0xabd0, 0xabe0, 0xabf0,
                    0xaca0, 0xacb0, 0xacc0, 0xacd0,
};
	int c;
	switch(m_pFont->Charset){
		case SHIFTJIS_CHARSET:
			c = _tOut[y] + x;
			if( (c==0xf07f)||(c==0xf0fd)||(c==0xf0fe)||(c==0xf0ff)||(c==0xf17f) ) c = 0;
            break;
		case HANGEUL_CHARSET:
			c = _tOutHL[y];
            if( c ){
	            c += x;
    	        if( (c==0xc9a0)||(c==0xfea0)||(c==0xc9ff)||(c==0xfeff) ) c = 0;
            }
            break;
		case CHINESEBIG5_CHARSET:   // 台湾
			c = _tOutBV[y] + x;
            if( (c==0xfa7f)||(c==0xfaa0)||(c==0xfaff)||(c==0xfb7f)||(c==0xfba0) ) c = 0;
            break;
		case 134:                   // 中国語簡略
			c = _tOutBY[y] + x;
			if( (c==0xaaa0)||(c==0xaaff)||(c==0xaba0)||(c==0xabff)||(c==0xaca0) ) c = 0;
            break;
        default:
        	c = 0;
            break;
    }
    return WORD(c);
}
//---------------------------------------------------------------------------
void __fastcall TCodeView::UpdateTitle(void)
{
	AnsiString as = sys.m_MsgEng ? "Character code list" : "Character Code list"; //AA6YQ 2013-05-25 removed JA characters
    as += " [";
	as += m_pFont->Name;
    AddStyle(as, m_pFont->Charset, 0);
    as += "]";
	Caption = as;
}
//---------------------------------------------------------------------------
void __fastcall TCodeView::UpdateXW(void)
{
	if( m_Base ){
		m_XW = PBox->Width / 17;
        m_XW--;
    	m_YW1 = m_YW = PBox->Height / 17;
        m_XW1 = PBox->Width - (m_XW * 16);
        UDMB->Visible = m_Base ? TRUE : FALSE;
    }
    else {
		m_XW1 = m_XW = PBox->Width / 17;
    	m_YW1 = m_YW = PBox->Height / 17;
        UDMB->Visible = FALSE;
    }
}
//---------------------------------------------------------------------------
void __fastcall TCodeView::SetPBoxFont(TCanvas *pCanvas)
{
	pCanvas->Font = m_pFont;
   	pCanvas->Font->Height = -(m_YW-5);
   	TFontStyles fs;
   	fs = Code2FontStyle(0);
   	pCanvas->Font->Style = fs;
}
//---------------------------------------------------------------------------
void __fastcall TCodeView::DrawChar(TCanvas *pCanvas, int c, BOOL f)
{

	int x = c % 16;
    int y = c / 16;
	char bf[32];
	LPSTR t = bf;
    if( c ){
		c += m_Base;
    }
	if( !c ) return;
	if( c & 0x00ff00 ) *t++ = BYTE(c>>8);
    *t++ = BYTE(c);
    *t = 0;
    pCanvas->Font->Color = f ? clWhite : clBlack;
	int fw = pCanvas->TextWidth(bf);
	int fh = pCanvas->TextHeight(bf);
	int xx = m_XW1 + x * m_XW + (m_XW-fw)/2;
	int yy = m_YW1 + y * m_YW + (m_YW-fh)/2;
	::SetBkMode(pCanvas->Handle, TRANSPARENT);
	pCanvas->TextOut(xx, yy, bf);
}
//---------------------------------------------------------------------------
void __fastcall TCodeView::DrawCursor(TCanvas *pCanvas, int c, BOOL f)
{
	TRect rc;

	int x = c % 16;
    int y = c / 16;
    rc.Left = 1 + m_XW1 + x * m_XW;
	rc.Top = 1 + m_YW1 + y * m_YW;
    rc.Right = rc.Left + m_XW - 1;
    rc.Bottom = rc.Top + m_YW - 1;
	TColor col;
	if( f ){
		col = clBlue;
    }
    else if( (!m_Base) && m_tPfx[c] ){
		col = clGray;
    }
    else {
		col = clWhite;
    }
    pCanvas->Pen->Color = col;
    pCanvas->Brush->Color = col;
    pCanvas->FillRect(rc);
    DrawChar(pCanvas, c, f);
}
//---------------------------------------------------------------------------
void __fastcall TCodeView::PBoxPaint(TObject *Sender)
{
	if( !m_pFont ) return;

	CWaitCursor w;
	Graphics::TBitmap *pBmp = new Graphics::TBitmap;
    pBmp->Width = PBox->Width;
    pBmp->Height = PBox->Height;
	TCanvas *pCanvas = pBmp->Canvas;
	if( !m_Base ) memset(m_tPfx, 0, sizeof(m_tPfx));
	SetPBoxFont(pCanvas);
	OnWave();
	SetMBCP(m_pFont->Charset);
	int x, y, xx, yy, fw, fh;
    char bf[16];
	int c = 0;
	for( y = 0; y < 16; y++ ){
		OnWave();
		for( x = 0; x < 16; x++ ){
			if( !m_Base && (_mbsbtype((const unsigned char *)&c, 0) == _MBC_LEAD) ){
				m_tPfx[c] = TRUE;
   	        	DrawCursor(pCanvas, c, FALSE);
       	    }
           	else {
				DrawChar(pCanvas, c, FALSE);
            }
           	c++;
        }
    }
	pCanvas->Font = Font;
    pCanvas->Pen->Color = clBlack;
	::SetBkMode(pCanvas->Handle, TRANSPARENT);
    for( y = 0; y < 16; y++ ){
		wsprintf(bf, "%X", y);
		fw = pCanvas->TextWidth(bf);
        fh = pCanvas->TextHeight(bf);
        xx = m_XW1 + y*m_XW + (m_XW-fw)/2;
        yy = (m_YW1-fh)/2;
        pCanvas->TextOut(xx, yy, bf);

		if( m_Base ){
	        wsprintf(bf, "%04x", y*16 + m_Base);
        }
        else {
	        wsprintf(bf, "%02X", y*16);
        }
		fw = pCanvas->TextWidth(bf);
        fh = pCanvas->TextHeight(bf);
        xx = (m_XW1-fw)/2;
        yy = m_YW1 + y*m_YW + (m_YW-fh)/2;
        pCanvas->TextOut(xx, yy, bf);

		pCanvas->Pen->Width = y ? 1 : 2;
		pCanvas->MoveTo(0, m_YW1 + y*m_YW);
        pCanvas->LineTo(PBox->Width, m_YW1 + y*m_YW);
		pCanvas->MoveTo(m_XW1 + y*m_XW, 0);
        pCanvas->LineTo(m_XW1 + y*m_XW, PBox->Height);
    }
    PBox->Canvas->Draw(0, 0, pBmp);
    delete pBmp;
	m_pfxCount = 0;
    for( x = 0; x < 256; x++ ){
		if( m_tPfx[x] ){
			m_tPfxIdx[m_pfxCount] = x;
        	m_pfxCount++;
        }
    }
	if( m_pfxCount != UDMB->Max ) UDMB->Max = short(m_pfxCount - 1);
}
//---------------------------------------------------------------------------
void __fastcall TCodeView::UpdatePfx(void)
{
	memset(m_tPfx, 0, sizeof(m_tPfx));
    int c;
	for( c = 0; c < 256; c++ ){
		if( _mbsbtype((const unsigned char *)&c, 0) == _MBC_LEAD ){
			m_tPfx[c] = TRUE;
        }
    }
	m_pfxCount = 0;
    for( c = 0; c < 256; c++ ){
		if( m_tPfx[c] ){
			m_tPfxIdx[m_pfxCount] = c;
        	m_pfxCount++;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TCodeView::Execute(HWND hWnd, UINT uMsg, int wParam, TFont *pFont)
{
	m_hWnd = hWnd;
    m_uMsg = uMsg;
    m_wParam = wParam;
    m_pFont = pFont;
    m_Base = 0;
	UpdateXW();
    UpdateTitle();
	ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TCodeView::SBCloseClick(TObject *Sender)
{
	Close();
}
//---------------------------------------------------------------------------
void __fastcall TCodeView::PBoxMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if( (Button == mbRight) && (m_Base) ){
		if( m_Base ){
			m_Base = 0;
        }
        UpdateXW(); PBoxPaint(NULL);
        return;
    }
	if( Button != mbLeft ) return;

    X -= m_XW1;
    Y -= m_YW1;
    if( X < 0 ) return;
    if( Y < 0 ) return;
    X /= m_XW;
    Y /= m_YW;
	if( X >= 16 ) X = 15;
    if( Y >= 16 ) Y = 15;
	int code = (Y * 16) + X;
    m_MouseDown = code + 1;
	TCanvas *pCanvas = PBox->Canvas;
	SetPBoxFont(pCanvas);
    DrawCursor(pCanvas, code, TRUE);

	if( m_Base ){
		code |= m_Base | 0x00010000;
    }
	else if( m_tPfx[code] ){
		for( int i = 0; i < m_pfxCount; i++ ){
			if( m_tPfxIdx[i] == code ){
				UDMB->Position = short(i);
				break;
            }
        }
		m_Base = (code << 8);
        UpdateXW(); PBoxPaint(NULL);
        m_MouseDown = FALSE;
        return;
    }

	if( m_hWnd ){
		m_Code = code & 0x0000ffff;
		::PostMessage(m_hWnd, m_uMsg, m_wParam, m_Code);
        Timer->Interval = 500;
        Timer->Enabled = TRUE;
    }
}
//---------------------------------------------------------------------------
void __fastcall TCodeView::PBoxMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if( !m_MouseDown ) return;

	m_MouseDown--;
	TCanvas *pCanvas = PBox->Canvas;
	SetPBoxFont(pCanvas);
    DrawCursor(pCanvas, m_MouseDown, FALSE);
    m_MouseDown = 0;
    Timer->Enabled = FALSE;
}
//---------------------------------------------------------------------------
void __fastcall TCodeView::TimerTimer(TObject *Sender)
{
	if( m_MouseDown ){
		::PostMessage(m_hWnd, m_uMsg, m_wParam, m_Code);
        Timer->Interval = 100;
    }
}
//---------------------------------------------------------------------------
void __fastcall TCodeView::SBUSClick(TObject *Sender)
{
	WORD wLang;
    if( Sender == SBJA ){
		wLang = 0x0411;
    }
    else if( Sender == SBHL ){
		wLang = 0x0412;
    }
    else if( Sender == SBBY ){
		wLang = 0x0804;
    }
    else if( Sender == SBBV ){
		wLang = 0x0404;
    }
    else {
		wLang = 0;
    }
	SetLangFont(m_pFont, wLang);
	PBox->Invalidate();
    UpdateTitle();
    if( m_hWnd ){
		::PostMessage(m_hWnd, m_uMsg, m_wParam, 0x10000000);
    }
	SetMBCP(m_pFont->Charset);
    UpdatePfx();
}
//---------------------------------------------------------------------------
void __fastcall TCodeView::UDMBClick(TObject *Sender, TUDBtnType Button)
{
	m_Base = m_tPfxIdx[UDMB->Position] << 8;
    UpdateXW(); PBoxPaint(NULL);
}
//---------------------------------------------------------------------------

