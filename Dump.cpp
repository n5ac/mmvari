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

#include <mbstring.h>
#include "Dump.h"
#include "ComLib.h"
#include "Dsp.h"

#define	LXW(c)	((c*4)/5)

#if DEBUG
//---------------------------------------------------------------------------
void __fastcall CDump::Debug(LPSTR t)
{
	sprintf(t, "DT:%d,WL:%d,ML:%d,RL:%d,WC:%d,MC:%d,RC:%d,Last:%d,Window:%d,Count:%d",
    	m_DTop,
        m_WLine, m_MLine, m_RLine,
        m_WCol, m_MCol, m_RCol,
        m_fLastCP, m_fWindow, GetCharCount(TRUE)
    );
}
#endif
//---------------------------------------------------------------------------
__fastcall CDump::CDump(void)
{
	m_fDisEvent = FALSE;
	m_pPanel = NULL;
	m_pBase = NULL;
	m_pPaintBox = NULL;
    m_pFont = NULL;
    m_pCanvas = NULL;
    m_pScrollBar = NULL;
    m_ScreenColMax = DUMPCOLMAX;
	m_wfp = NULL;
	m_hWnd = NULL;
	m_pMBCS = NULL;

    m_Color[0].c = clWhite;
    m_Color[1].c = clBlack;
    m_Color[2].c = clBlue;
    m_Color[3].c = clRed;
	m_Color[4].c = TColor(RGB(224,224,224));

    m_CursorColor = TColor(128,128,128);
    m_CursorColor = clBlue;
	m_fConvAlpha = FALSE;
	m_fShowCtrlCode = FALSE;

    m_CursorType = csCARET;
    m_Cursor = 0;
    m_fActive = FALSE;
	m_CRCount = 0;

	m_MouseWheelCount = 0;
	m_fRTTY = FALSE;

    m_ScreenXW = m_ScreenYW = 0;
    memset(&m_LogFont, 0, sizeof(m_LogFont));
}

//---------------------------------------------------------------------------
__fastcall CDump::~CDump()
{
	DeleteStg();
}
//---------------------------------------------------------------------------
void __fastcall CDump::DeleteStg(void)
{
	if( m_pBase ){
		int i;
		for( i = 0; i < m_StgLineMax; i++ ){
			delete m_pBase[i];
        }
        delete m_pBase;
        m_pBase = NULL;
    }
}

//---------------------------------------------------------------------------
void __fastcall CDump::Create(HWND hWnd, TPanel *pPanel, TPaintBox *pPaintBox, TFont *pFont, TScrollBar *pBar, int line)
{
	m_hWnd = hWnd;
	m_pPanel = pPanel;
	m_pPaintBox = pPaintBox;
	m_pFont = pFont;
    m_pScrollBar = pBar;
	m_pCanvas = pPaintBox->Canvas;
    m_ScreenYW = pPaintBox->Height;
    m_ScreenXW = pPaintBox->Width;

    m_rcScreen.Left = 0;
    m_rcScreen.Top = 0;
    m_rcScreen.Right = m_ScreenXW;
    m_rcScreen.Bottom = m_ScreenYW;

    if( !m_pBase || (m_StgLineMax != line) ){
		DeleteStg();
        m_pBase = new int *[line];
        for( int i = 0; i < line; i++ ){
			int *ip = new int[DUMPCOLMAX+1];
            memset(ip, 0, sizeof(int)*(DUMPCOLMAX+1));
            m_pBase[i] = ip;
        }
    }
	m_StgLineMax = line;

	m_fLastCP = TRUE;
    m_MLine = m_WLine = m_WCol = m_DTop = 0;
    m_MCol = m_RLine = m_RCol = 0;
    m_X = m_Y = 0;
    m_RX = m_RY = 0;
	m_fKanji = FALSE;
	m_fWindow = TRUE;
	m_fCR = FALSE;
	m_CRCount = 0;

	m_fSelText = FALSE;
	m_fSelect = FALSE;
	m_SLine = m_SCol = 0;
	m_FLine = 0;

    m_pCanvas->Font = pFont;
    m_FontXW = m_pCanvas->TextWidth("W");
    m_FontYW = m_pCanvas->TextHeight("|");
	if( !m_FontXW ) m_FontXW = 8;
    if( !m_FontYW ) m_FontYW = 8;
    m_ScreenLXW = LXW(m_ScreenXW);
    m_ScreenLineMax = m_ScreenYW / m_FontYW;
    if( m_ScreenLineMax >= m_StgLineMax ) m_ScreenLineMax = m_StgLineMax - 1;
    GetLogFont(&m_LogFont, pFont);
    SetScrollBar();
}
//---------------------------------------------------------------------------
void __fastcall CDump::SetFont(TFont *pFont)
{
	if( !m_hWnd ) return;

    m_pCanvas->Font = pFont;
    m_FontXW = m_pCanvas->TextWidth("W");
    m_FontYW = m_pCanvas->TextHeight("|");
	if( !m_FontXW ) m_FontXW = 8;
    if( !m_FontYW ) m_FontYW = 8;
    m_ScreenLXW = LXW(m_ScreenXW);
    GetLogFont(&m_LogFont, pFont);
    Resize();
    int *ip = m_pBase[m_WLine];
	m_X = 0;
    for( ; *ip; ip++ ){
		m_X += CharWidth(*ip);
    }
    ip = m_pBase[m_RLine];
	int col = m_RCol;
	m_RX = 0;
    for( ; *ip && col; ip++, col-- ){
		m_RX += CharWidth(*ip);
    }
}
//---------------------------------------------------------------------------
void __fastcall CDump::Resize(void)
{
	if( !m_hWnd ) return;

	Cursor(FALSE);
	m_ScreenXW = m_pPaintBox->Width;
    m_ScreenYW = m_pPaintBox->Height;
    int lmax = m_ScreenYW / m_FontYW;
	if( !lmax ) lmax++;
	if( lmax >= m_StgLineMax ) lmax = m_StgLineMax - 1;
    if( lmax != m_ScreenLineMax ){
		m_ScreenLineMax = lmax;
		if( m_WLine >= (lmax-1) ){
			m_DTop = m_WLine - lmax + 1;
            if( m_DTop < 0 ) m_DTop = 0;
        }
        else {
			m_DTop = 0;
        }
		m_Y = (m_WLine - m_DTop) * m_FontYW;
		m_RY = (m_RLine - m_DTop) * m_FontYW;
    }
	SetScrollBar();
	SetCursorPos();
   	m_fWindow = TRUE;
    Cursor(TRUE);
}
//---------------------------------------------------------------------------
void __fastcall CDump::OpenSelect(void)
{
	if( !m_fSelect && !m_fSelText ){
    	m_SLine = m_WLine;
	    m_SCol = m_WCol;
    }
	m_fSelect = TRUE;
}
//---------------------------------------------------------------------------
void __fastcall CDump::CloseSelect(void)
{
	m_fSelect = FALSE;
}
//---------------------------------------------------------------------------
BOOL __fastcall CDump::ClearSelect(BOOL fPaint)
{
	BOOL r = FALSE;
	if( m_fSelText ){
		r = ClearAllSelect(fPaint);
    }
    return r;
}
//---------------------------------------------------------------------------
void __fastcall CDump::UpdateSelect(void)
{
	if( !m_fSelect ) return;

	ClearSelect(FALSE);
    int sno = m_SLine * DUMPCOLMAX + m_SCol;
    int wno = m_WLine * DUMPCOLMAX + m_WCol;
    int *ip;
    int i;
	int sline, wline, scol, wcol;
    if( sno < wno ){
		sline = m_SLine; wline = m_WLine;
        scol = m_SCol; wcol = m_WCol;
    }
    else {
		sline = m_WLine; wline = m_SLine;
        scol = m_WCol; wcol = m_SCol;
    }
    BOOL f = FALSE;
	for( i = sline; i <= wline; i++ ){
		ip = m_pBase[i];
        int col = 0;
		for( ; *ip; ip++, col++ ){
        	if( (i == sline) && (col < scol) ){
				continue;
            }
            else if( (i == wline) && (col >= wcol) ){
				continue;
            }
            else {
				f = TRUE;
				*ip |= 0x80000000;
            }
        }
    }
    m_fSelText = f;
}
//---------------------------------------------------------------------------
BOOL __fastcall CDump::GetSelText(AnsiString &as)
{
	BOOL f = FALSE;
	as = "";
	if( !m_fSelText ) return f;

	int i, c;
	int *ip;
	for( i = 0; i < m_StgLineMax; i++ ){
		ip = m_pBase[i];
        for( ; *ip; ip++ ){
			if( *ip & 0x80000000 ){
				f = TRUE;
				c = *ip & 0x0000ffff;
                if( c >= 0x8100 ){
					as += char(c>>8);
                }
                as += char(c);
                if( c == '\r' ) as += '\n';
            }
        }
    }
    return f;
}
//---------------------------------------------------------------------------
BOOL __fastcall CDump::DeleteSelText(void)
{
	if( !m_fSelText ) return FALSE;
    if( !CanEdit() ) return FALSE;

//	if( m_RLine > m_SLine ) return FALSE;
//   	if( (m_RLine == m_SLine) && (m_RCol >= m_SCol) ) return FALSE;

	int i, col;
	int *ip;
	for( i = 0; i < m_StgLineMax; i++ ){
		ip = m_pBase[i];
		col = 0;
        for( ; *ip; ip++, col++ ){
			if( *ip & 0x80000000 ){
            	m_WLine = i;
                m_WCol = col;
                int *bp = CreateContP(i, col, 0);
                for( ip = bp; *ip; ip++ ){
					if( !(*ip & 0x80000000) ) break;
                }
                CloseContP(i, col, bp, ip - bp);
				m_Y = (m_WLine - m_DTop) * m_FontYW;
                m_X = 0;
                col = 0;
                ip = m_pBase[m_WLine];
                for( ; *ip && (col < m_WCol); ip++, col++ ){
                	m_X += CharWidth(*ip);
                }
				SetCursorPos();
                m_fSelText = FALSE;
				return TRUE;
            }
        }
    }
	m_fSelText = FALSE;
    return FALSE;
}
//---------------------------------------------------------------------------
void __fastcall CDump::MoveCursor(int SX, int SY, BOOL fDown)
{
	Cursor(FALSE);
	BOOL fPaint = ClearSelect(FALSE);
	int line = SY/m_FontYW + m_DTop;
    if( line > m_MLine ){
	    if( fPaint ) Paint();
    	MoveCursor(dmpMoveLAST);
        return;
	}

    int *bp = m_pBase[line];
    int *ip = bp;
	int x, xw;
    int col = 0;
    for( x = 0; *ip; ip++ ){
		xw = CharWidth(*ip);
        if( (x + (xw/2)) >= SX ) break;
		x += xw;
        col++;
    }
	if( fDown && !*ip ){
	    if( fPaint ) Paint();
    	MoveCursor(dmpMoveLAST);
        return;
    }
	m_WLine = line;
	m_WCol = col;
	m_X = x;
	m_Y = (m_WLine - m_DTop) * m_FontYW;
	if( m_fSelect ){
		UpdateSelect();
        fPaint = TRUE;
    }
	m_fWindow = (m_WLine >= m_DTop) && (m_WLine < (m_ScreenLineMax + m_DTop));
	m_fLastCP = (m_WLine == m_MLine) && (m_WCol == m_MCol);
    SetCursorPos();
    Cursor(TRUE);
    if( fPaint ) Paint();
}
//---------------------------------------------------------------------------
void __fastcall CDump::MoveCursor(int Key)
{
	Cursor(FALSE);
    switch(Key){
		case VK_LEFT:
			if( m_WCol ){
				m_WCol--;
				int *ip = m_pBase[m_WLine] + m_WCol;
				int xw = CharWidth(*ip);
		        m_X -= xw;
		    }
		    else if( m_WLine ){
				m_WLine--;
				m_Y -= m_FontYW;
		        int *ip = m_pBase[m_WLine];
				m_X = 0;
		        for( ; *ip; ip++ ){
					m_X += CharWidth(*ip);
		            m_WCol++;
		        }
			}
        	break;
		case VK_RIGHT:
			{
				int *ip = m_pBase[m_WLine] + m_WCol;
                if( *ip && ((*ip & 0x0000ffff) != '\r') ){
					m_WCol++;
					m_X += CharWidth(*ip);
                }
                else if( m_WLine < m_MLine ){
					m_WLine++;
					m_Y += m_FontYW;
                    m_WCol = 0;
                    m_X = 0;
                }
            }
        	break;
        case VK_UP:
			if( m_WLine ){
				m_WLine--;
                m_Y -= m_FontYW;
                int *ip = m_pBase[m_WLine];
                m_WCol = 0;
                int x = 0;
                for( ; *ip && (x < m_X); ip++ ){
					x += CharWidth(*ip);
                    m_WCol++;
                }
                m_X = x;
            }
        	break;
        case VK_DOWN:
        	if( m_WLine < m_MLine ){
				m_WLine++;
                m_Y += m_FontYW;
                int *ip = m_pBase[m_WLine];
                m_WCol = 0;
                int x = 0;
                for( ; *ip && (x < m_X); ip++ ){
					x += CharWidth(*ip);
                    m_WCol++;
                }
                m_X = x;
            }
            break;
		case VK_HOME:
			if( m_WLine == m_RLine ){
				m_WCol = m_RCol;
                m_X = 0;
                int *ip = m_pBase[m_WLine];
                int col = 0;
                for( ; *ip && (col < m_WCol); ip++, col++ ){
					m_X += CharWidth(*ip);
                }
            }
            else {
				m_WCol = 0;
    	        m_X = 0;
            }
            break;
        case dmpMoveTOP:
		case VK_HOME+0x0100:
        	{
				m_WLine = m_RLine;
                m_WCol = m_RCol;
                m_X = 0;
                int *ip = m_pBase[m_WLine];
                int col = 0;
                for( ; *ip && (col < m_WCol); ip++, col++ ){
					m_X += CharWidth(*ip);
                }
				m_Y = (m_WLine - m_DTop) * m_FontYW;
            }
            break;
        case VK_END:
        	if( !m_fLastCP ){
				int *ip = m_pBase[m_WLine];
                m_WCol = 0;
                int x = 0;
                for( ; *ip; ip++ ){
					x += CharWidth(*ip);
                    m_WCol++;
                }
                m_X = x;
            }
            break;
		case VK_END + 0x0100:
        case dmpMoveLAST:
        	if( !m_fLastCP ){
				m_WLine = m_MLine;
				m_Y = (m_WLine - m_DTop) * m_FontYW;
				int *ip = m_pBase[m_WLine];
                m_WCol = 0;
                int x = 0;
                for( ; *ip; ip++ ){
					x += CharWidth(*ip);
                    m_WCol++;
                }
                m_X = x;
            }
            break;
    }
	if( m_WCol ){
		int *ip = m_pBase[m_WLine] + m_WCol - 1;
        if( (*ip & 0x0000ffff) == '\r' ) m_WCol--;
    }
	BOOL fPaint;
	if( m_fSelect ){
		UpdateSelect();
        fPaint = TRUE;
    }
    else {
		fPaint = ClearSelect(FALSE);
    }
    if( (m_Y < 0) || (m_WLine < m_DTop) ){
		m_DTop = m_WLine;
		m_Y = (m_WLine - m_DTop) * m_FontYW;
        fPaint = TRUE;
    }
    else if( (m_Y >= m_ScreenYW) || ((m_WLine - m_DTop) >= (m_ScreenLineMax - 1))  ){
		m_DTop = m_WLine - m_ScreenLineMax + 1;
		m_Y = (m_WLine - m_DTop) * m_FontYW;
        fPaint = TRUE;
    }
	m_RY = (m_RLine - m_DTop) * m_FontYW;
	if( fPaint ) Paint();
    SetScrollBar();
	m_fLastCP = (m_WLine == m_MLine) && (m_WCol == m_MCol);
    SetCursorPos();
    Cursor(TRUE);
}
//---------------------------------------------------------------------------
void __fastcall CDump::FollowRP(void)
{
	if( m_WLine > m_RLine ) return;
	if( m_WLine < m_RLine ){
		m_RLine = m_WLine;
        m_RCol = m_WCol;
        m_RX = m_X;
    }
    else if( m_WCol < m_RCol ){
		m_RCol = m_WCol;
        m_RX = m_X;
    }
}
//---------------------------------------------------------------------------
void __fastcall CDump::SetBuffer(int c)
{
	c &= 0x0000ffff;
    LPSTR t = m_Buffer;
	if( c == 0x0200 ){
		strcpy(t, "(i)"); t += 3;
    }
	else if( c >= 0x0020 ){
	    if( c >= 0x8140 ){
			*t++ = BYTE(c >> 8);
    	}
    	*t++ = BYTE(c);
    }
    else if( m_fShowCtrlCode && (c != '\r') ){
		sprintf(t, "[$%02X]", c); t += strlen(t);
    }
    *t = 0;
}

int __fastcall CDump::CharWidth(int c)
{
	SetBuffer(c);
    return m_Buffer[0] ? m_pCanvas->TextWidth(m_Buffer) : 0;
}

int __fastcall CDump::DrawChar(int x, int y, int c, BOOL f)
{
    SetBuffer(c);
	if( m_Buffer[0] ){
		if( f && (x>=0)&&(y>=0)&&(x<m_ScreenXW)&&(y<m_ScreenYW) ){
			int col = (c >> 16) & 3;
			if( c & 0x80000000 ){
				TRect rc;
                rc.Left = x; rc.Top = y;
                rc.Right = x + m_pCanvas->TextWidth(m_Buffer);
                rc.Bottom = y + m_FontYW;
                m_pCanvas->Brush->Color = m_Color[col].c;
                m_pCanvas->FillRect(rc);
                m_pCanvas->Brush->Color = m_fWindow ? m_Color[0].c : m_Color[4].c;
				col = 0;
            }
			m_pCanvas->Font->Color = m_Color[col].c;
			::SetBkMode(m_pCanvas->Handle, TRANSPARENT);
   			m_pCanvas->TextOut(x, y, m_Buffer);
        }
	    x += m_pCanvas->TextWidth(m_Buffer);
    }
    return x;
}

void __fastcall CDump::MemScroll(void)
{
    int i;
	int **ip = m_pBase;
    int *bp = *ip;
    for( i = 0; i < (m_StgLineMax - 1); i++, ip++ ){
		*ip = *(ip+1);
    }
    *ip = bp;
	*bp = 0;
    if( m_RLine ) m_RLine--;
    if( m_FLine ) m_FLine--;
    m_SML--;
}

void __fastcall CDump::FlushLogFile(void)
{
	if( m_wfp ) fflush(m_wfp);
}

void __fastcall CDump::OpenLogFile(LPCSTR pName)
{
	CloseLogFile();
	FILE *fp = fopen(pName, "rb");
	BOOL f = FALSE;
    if( fp ){
		fclose(fp);
        f = TRUE;
    }
    m_wfp = fopen(pName, "ab");
    if( !m_wfp ){
		ErrorMB(sys.m_MsgEng ? "Can not create '%s'":"%sを作成できません.", pName);
    }
    else if( !f ){	// 初回のオープン
		for( int i = 0; i < m_WLine; i++ ){
			WriteLine(i);
        }
    }
}

void __fastcall CDump::CloseLogFile(void)
{
	if( m_wfp ){
		fclose(m_wfp);
        m_wfp = NULL;
    }
}

void __fastcall CDump::WriteLine(AnsiString &as, int line)
{
    int *ip = m_pBase[line];
	int c;
    for( ; *ip; ip++ ){
		c = *ip & 0x0000ffff;
		if( c == '\r' ){
		    as += '\r';
		    as += '\n';
		}
        else if( c >= 0x100 ){
			as += char(c>>8);
			as += char(c);
        }
   	    else {
			as += char(c);
        }
    }
}

void __fastcall CDump::WriteLine(int line)
{
	if( !m_wfp ) return;

    int *ip = m_pBase[line];
	int c;
    for( ; *ip; ip++ ){
		c = *ip & 0x0000ffff;
		if( c == '\r' ){
		    fputc('\r', m_wfp);
		    fputc('\n', m_wfp);
		}
        else if( c >= 0x100 ){
			fputc(c>>8, m_wfp);
			fputc(c&0x00ff, m_wfp);
        }
   	    else {
			fputc(c, m_wfp);
        }
    }
}

void __fastcall CDump::PutScroll(void)
{
	if( m_wfp ) WriteLine(m_WLine);
	m_WCol = 0;
    m_X = 0;
	m_WLine++;
    if( m_WLine >= m_StgLineMax ){
		m_WLine--;
		MemScroll();
    }
	if( m_fLastCP ){
    	m_MLine = m_WLine; m_MCol = m_WCol;
    }
    m_Y += m_FontYW;
	if( m_WLine >= m_ScreenLineMax ){
    	m_Y -= m_FontYW;
    	if( m_fWindow ){
		    m_DTop++;
    	    if( m_DTop >= (m_StgLineMax - m_ScreenLineMax + 1) ){
				m_DTop--;
        	}
			m_RY = (m_RLine - m_DTop) * m_FontYW;
	        Paint();
        }
	}
    SetScrollBar();
}
//---------------------------------------------------------------------------
void __fastcall CDump::SetScrollBar(void)
{
	if( !m_pScrollBar ) return;

	int max = m_MLine - m_ScreenLineMax + 1;
    if( max < 0 ) max = 0;
	m_fDisEvent++;
    m_pScrollBar->SetParams(m_DTop, 0, max);
	m_pScrollBar->Enabled = (m_MLine >= m_ScreenLineMax);
	m_fWindow = (m_WLine >= m_DTop) && (m_WLine < (m_ScreenLineMax + m_DTop));
	m_fLastCP = (m_WLine == m_MLine) && (m_WCol == m_MCol);
    m_fDisEvent--;
}
//---------------------------------------------------------------------------
void __fastcall CDump::OnScrollBarChange(void)
{
	if( !m_pScrollBar ) return;
	if( m_fDisEvent ) return;

    if( m_DTop != m_pScrollBar->Position ){
		m_DTop = m_pScrollBar->Position;
		m_Y = (m_WLine - m_DTop) * m_FontYW;
		m_RY = (m_RLine - m_DTop) * m_FontYW;
		m_fWindow = (m_WLine >= m_DTop) && (m_WLine < (m_ScreenLineMax + m_DTop));
		m_fLastCP = (m_WLine == m_MLine) && (m_WCol == m_MCol);
	    Paint();
    }
}
//---------------------------------------------------------------------------
void CALLBACK TimeProc(UINT IDEvent, UINT uReserved, DWORD dwUser, DWORD dwReserved1, DWORD dwReserved2)
{
	if( !dwUser ) return;

	CDump *pDump = (CDump *)dwUser;
    pDump->OnTimer();
}
//---------------------------------------------------------------------------
BOOL __fastcall CDump::OnTimer(void)
{
	BOOL f = FALSE;
//	Application->MainForm->Caption = m_MouseWheelCount;
	if( m_MouseWheelCount ){
		m_MouseWheelCount -= 3;
		if( m_MouseWheelCount > 0 ){
        	f = TRUE;
        }
        else {
			m_MouseWheelCount = 0;
        }
    }
    return f;
}
//---------------------------------------------------------------------------
BOOL __fastcall CDump::OnMouseWheel(int z)
{
	if( !m_pScrollBar ) return FALSE;
    if( !m_pScrollBar->Enabled ) return FALSE;


   	m_MouseWheelCount++;
	if( m_MouseWheelCount > 6 ) m_MouseWheelCount = 6;
	int div = 120;
	if( m_MouseWheelCount >= 6 ){
		div = 20;
    }
	else if( m_MouseWheelCount >= 4 ){
		div = 30;
    }
	else if( m_MouseWheelCount >= 2 ){
		div = 60;
    }
    z /= div;
	DoWheel(z);
    return TRUE;
}
//---------------------------------------------------------------------------
void __fastcall CDump::DoWheel(int z)
{
	while( z ){
		if( z > 0 ){
			if( m_pScrollBar->Position ){
            	m_pScrollBar->Position--;
            }
            else {
				break;
            }
			z--;
		}
		else {
			if( m_pScrollBar->Position < m_pScrollBar->Max ){
            	m_pScrollBar->Position++;
            }
            else {
				break;
            }
			z++;
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall CDump::Clear(void)
{
	Cursor(FALSE);
	m_fLastCP = TRUE;
    m_MLine = m_WLine = m_WCol = m_DTop = 0;
    m_MCol = m_RLine = m_RCol = 0;
    m_X = m_Y = 0;
    m_RX = m_RY = 0;
	m_fKanji = FALSE;
	m_fWindow = TRUE;
	m_fCR = FALSE;
	m_CRCount = 0;

	m_fSelText = FALSE;
	m_fSelect = FALSE;
	m_SLine = m_SCol = 0;

    for( int i = 0; i < m_StgLineMax; i++ ){
		*m_pBase[i] = 0;
    }
    SetScrollBar();
    SetCursorPos();
	Paint();
//	Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall CDump::DeleteCR2(void)
{
	for( int i = 0; i < m_MLine; i++ ){
		int *ip = m_pBase[i];
        for( ; *ip; ip++ ){
			if( ((*ip & 0x0000ffff) == '\r') && (*(ip+1) & 0x0000ffff) ){
				int *rp = ip;
                for( ; *rp; rp++ ){
					*rp = *(rp+1);
                }
            }
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall CDump::DeleteChar(BOOL fForce)
{
	if( !fForce && !CanEdit() ) return;

    if( m_fSelText ){
		DeleteSelText();
        Invalidate();
		return;
    }
	if( m_fLastCP ) return;
	Cursor(FALSE);
    int *bp = CreateContP(m_WLine, m_WCol, 0);
    CloseContP(m_WLine, m_WCol, bp, 1);
	SetScrollBar();
    Invalidate();
    Cursor(TRUE);
}
//---------------------------------------------------------------------------
BOOL __fastcall CDump::CanEdit(void)
{
	if( m_fSelText ){
		if( m_RLine > m_SLine ) return FALSE;
   		if( (m_RLine == m_SLine) && (m_RCol > m_SCol) ) return FALSE;
    }
	if( m_RLine == m_WLine ){
		if( m_RCol <= m_WCol ) return TRUE;
    }
    else if( m_RLine < m_WLine ){
		return TRUE;
    }
    return FALSE;
}
//---------------------------------------------------------------------------
// 連続データを作成する
int *__fastcall CDump::CreateContP(int line, int col, int add)
{
	int SLine = m_MLine - line + 1;
    int *bp = new int[DUMPCOLMAX*SLine + add + 1];
    int *wp = bp;
	wp += add;
    int *ip = m_pBase[line] + col;
	while(line <= m_MLine){
		while(*ip){
			*wp++ = *ip++;
        }
        line++;
        ip = m_pBase[line];
    }
    *wp = 0;
    return bp;
}
//---------------------------------------------------------------------------
void __fastcall CDump::DupeText(int col)
{
	if( !m_WLine ) return;
	if( !CanEdit() ) return;

    int bf[DUMPCOLMAX+1];
    memcpy(bf, m_pBase[m_WLine-1], sizeof(int)*DUMPCOLMAX);
    int *ip = bf;
    for( ; *ip; ip++ ){
		PutChar(*ip & 0x0000ffff, col);
    }
}
//---------------------------------------------------------------------------
// 連続データを設定する
void __fastcall CDump::CloseContP(int line, int col, int *bp, int offset)
{
	int cc = 0;
	int x = 0;
    int *ip = m_pBase[line];
	for( ; *ip && (cc < col); ip++, cc++ ){
		x += CharWidth(*ip);
    }
    int *wp = bp + offset;
	if( !*bp ) wp = bp;
	m_MCol = col;
	while(*wp && (line < m_StgLineMax)){
		int cc = *wp++;
		*ip++ = cc;
		m_MCol++;
        x += CharWidth(cc);
		if( (cc & 0x0000ffff) == '\r' ){
			m_MCol = 0;
			x = 0;
			*ip = 0;
			line++;
        	ip = m_pBase[line];
        }
        else if( (x >= (m_ScreenXW - 32)) || (((cc&0x0000ffff)==' ') && (x >= m_ScreenLXW)) ){
			m_MCol = 0;
			x = 0;
			*ip = 0;
			line++;
        	ip = m_pBase[line];
        }
    }
//	if( m_MCol ) *ip = 0;
	*ip = 0;
    delete bp;
    m_MLine = line;
    if( m_MLine >= m_StgLineMax ) m_MLine = m_StgLineMax - 1;
    ip = m_pBase[m_MLine] + m_MCol;
    *ip = 0;

	if( m_MLine < (m_DTop + m_ScreenLineMax - 1) ){
		m_DTop = m_MLine - m_ScreenLineMax + 1;
        if( m_DTop < 0 ) m_DTop = 0;
		m_Y = (m_WLine - m_DTop) * m_FontYW;
		m_RY = (m_RLine - m_DTop) * m_FontYW;
        SetCursorPos();
    }
	m_fLastCP = (m_WLine == m_MLine) && (m_WCol == m_MCol);
}
//---------------------------------------------------------------------------
void __fastcall CDump::InsChar(int c, int col)
{
	if( !CanEdit() ) return;

	if( m_MLine >= (m_StgLineMax - 1) ){	// １行の余裕を作る
		if( !m_WLine ) return;
		MemScroll();
        if( m_WLine ) m_WLine--;
        if( m_DTop ) m_DTop--;
        m_MLine--;

    }

    int *bp = CreateContP(m_WLine, m_WCol, 1);
	*bp = c | (col << 16);
	CloseContP(m_WLine, m_WCol, bp, 0);

	m_X += CharWidth(c);
    m_WCol++;
	if( ((c & 0x0000ffff) == '\r') || (m_X >= m_ScreenXW - 32) ){
		m_X = 0;
        m_WCol = 0;
        m_WLine++;
        m_Y += m_FontYW;
    }
    if( (m_Y >= m_ScreenYW) || ((m_WLine - m_DTop) >= m_ScreenLineMax)  ){
		m_DTop++;
   	    m_Y -= m_FontYW;
		m_RY = (m_RLine - m_DTop) * m_FontYW;
    }
	m_fLastCP = (m_WLine == m_MLine) && (m_WCol == m_MCol);
	SetScrollBar();
	Invalidate();
}
//---------------------------------------------------------------------------
BOOL __fastcall CDump::PutChar(int c, int col)
{
	BOOL r = FALSE;

	if( m_pMBCS && m_fConvAlpha ) c = m_pMBCS->ConvAlpha(c);
    if( m_fSelText ) ClearSelect(TRUE);
	Cursor(FALSE);
	if( (c == '\r') || (c == '\n') ){
		if( ((c == '\r') || !m_fCR) && (m_CRCount < DUMPCRLIMIT) ){
			if( m_fLastCP ){
				m_CRCount++;
				int *ip = m_pBase[m_WLine] + m_WCol;
	    	    *ip++ = ('\r' | (col << 16));
	        	*ip = 0;
				PutScroll();
    	    }
        	else {
				InsChar(('\r' | (col << 16)), col);
	        }
        }
        m_fCR = (c == '\r') ? TRUE : FALSE;
        r = TRUE;
    }
	else if( c == '\t' ){
		m_CRCount = 0;
		int ax = m_FontXW*4;
		int xx = m_X + ax;
        xx = xx - (xx % ax);
        while(m_X < xx){
			ax = m_X;
			PutChar(' ', col);
            if( ax >= m_X ) break;
        }
        m_fCR = FALSE;
        r = TRUE;
    }
    else if( c == '\b' ){
		m_CRCount = 0;
		r = BackSpace();
		m_fCR = FALSE;
    }
//    else if( (c >= 0x0020) || (m_fShowCtrlCode && c) ){
    else if( c ){
		m_CRCount = 0;
		if( c < 0x0020 ) col = 3;
		m_fCR = FALSE;
    	if( c ){
			int *ip = m_pBase[m_WLine] + m_WCol;
			c = c | (col << 16);
			if( m_fLastCP ){
				*ip++ = c;
    		    *ip = 0;
				m_X = DrawChar(m_X, m_Y, c, m_fWindow);
    	    	m_WCol++;
	    	    if( (m_X >= (m_ScreenXW - 32) ) || (m_WCol >= DUMPCOLMAX) ){
					PutScroll();
    		    }
            	else if( ((c & 0xffff) == ' ') && (m_X >= m_ScreenLXW) ){
					PutScroll();
    	        }
            }
            else if( CanEdit() ){
				InsChar(c, col);
            }
        }
        r = TRUE;
    }
	if( m_fLastCP ){
    	m_MLine = m_WLine; m_MCol = m_WCol;
    }
	SetCursorPos();
	Cursor(TRUE);
    return r;
}
//---------------------------------------------------------------------------
BOOL __fastcall CDump::PutKey(char c, int col)
{
	BOOL r = FALSE;
	int key = BYTE(c);
	if( m_fKanji ){
		m_fKanji += key;
        r = PutChar(m_fKanji, col);
        m_fKanji = 0;
    }
	else if( m_pMBCS ){
		if( m_pMBCS->IsLead(c) ){
			m_fKanji = key << 8;
        }
        else {
			r = PutChar(key, col);
        }
    }
    else {
		if( _mbsbtype((unsigned char *)&key, 0) == _MBC_LEAD ){
			m_fKanji = key << 8;
    	}
        else {
			r = PutChar(key, col);
        }
    }
    return r;
}
//---------------------------------------------------------------------------
void CDump::PutStatus(int col, LPCSTR pFmt, ...)
{
	va_list	pp;
	char	bf[1024];
	va_start(pp, pFmt);
	vsprintf( bf, pFmt, pp );
	va_end(pp);

	if( !bf[0] ) return;

    if( m_WCol ){
//		Cursor(FALSE);
//    	PutScroll();
		PutChar('\r', col);
    }
    LPCSTR p = bf;
    for( ; *p; p++ ){
		PutKey(*p, col);
    }
}
//---------------------------------------------------------------------------
BOOL __fastcall CDump::BackSpace(void)
{
	Cursor(FALSE);
	BOOL r = FALSE;
	if( m_WCol ){
		m_WCol--;
		int *ip = m_pBase[m_WLine] + m_WCol;
		int xw = CharWidth(*ip);
        m_X -= xw;
		if( m_fWindow ){
			TRect rc;
            rc.Left = m_X; rc.Top = m_Y;
            rc.Right = m_X + xw; rc.Bottom = m_Y + m_FontYW;
            m_pCanvas->Brush->Color = m_Color[0].c;
            m_pCanvas->FillRect(rc);
        }
		if( m_fLastCP ){
			*ip = 0;
        }
        else {
			for( ; *ip; ip++ ){
				*ip = *(ip+1);
                if( !*ip ) break;
            }
            Invalidate();
        }
        r = TRUE;
    }
    else if( m_WLine ){
		if( m_fLastCP ){
			if( m_CRCount ) m_CRCount--;
			m_WLine--;
			if( m_WLine >= (m_ScreenLineMax - 1) ){
				m_DTop = m_WLine - m_ScreenLineMax + 1;
    	    	m_Y = (m_WLine - m_DTop) * m_FontYW;
				m_RY = (m_RLine - m_DTop) * m_FontYW;
	        }
    	    else {
				m_Y -= m_FontYW;
	        }
    	    int *ip = m_pBase[m_WLine];
			m_X = 0;
    	    for( ; *ip; ip++ ){
				m_X += CharWidth(*ip);
            	m_WCol++;
		        }
        	if( m_WCol ){
				ip--;
		        m_X -= CharWidth(*ip);
        	    m_WCol--;
            	*ip = 0;
	        }
        	Invalidate();
			SetScrollBar();
        }
        else {
			MoveCursor(VK_LEFT);
            DeleteChar(TRUE);
        }
    }
	SetCursorPos();
	FollowRP();
	Cursor(TRUE);
    return r;
}
//---------------------------------------------------------------------------
void __fastcall CDump::Invalidate(void)
{
	if( m_pPaintBox ) m_pPaintBox->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall CDump::Paint(void)
{
	if( !m_hWnd ) return;

	Cursor(FALSE);
    TColor cBack = m_fWindow ? m_Color[0].c : m_Color[4].c;
	if( m_pPanel && (m_Color[0].c != m_Color[4].c) ){
    	m_pPanel->Color = cBack;
    }
	m_pCanvas->Brush->Color = cBack;
	int i;
    for( i = 0; i < m_ScreenLineMax; i++ ){
    	int n = i + m_DTop;
		TRect rc;
        rc.Left = 0; rc.Right = m_ScreenXW;
        rc.Top = i * m_FontYW;
        rc.Bottom = rc.Top + m_FontYW;
        m_pCanvas->FillRect(rc);
		if( (n <= m_MLine) && (n < m_StgLineMax) ){
	    	int *ip = m_pBase[n];
    	    int x = 0;
        	while(*ip){
				if( (*ip & 0x0000ffff) != '\r' ){
					x = DrawChar(x, rc.Top, *ip++, TRUE);
                }
                else {
					ip++;
                }
        	}
        }
    }
	Cursor(TRUE);
}
//---------------------------------------------------------------------------
static int __fastcall GetCharLength(int *ip)
{
	int n = 0;
    for( ; *ip; ip++ ) n++;
    return n;
}
//---------------------------------------------------------------------------
int __fastcall CDump::GetCharCount(BOOL fTX)
{
	int line;
    int *ip;
	if( fTX ){
		if( m_WLine < m_RLine ) return 0;
		if( m_WLine == m_RLine ) return m_WCol - m_RCol;
		line = m_RLine;
	    ip = m_pBase[line++] + m_RCol;
    }
    else {
		if( !m_WLine ) return m_WCol;
        line = 0;
	    ip = m_pBase[line++];
    }
	int n = GetCharLength(ip);
	while( m_WLine >= line ){
		ip = m_pBase[line++];
		n += GetCharLength(ip);
	}
    return n;
}
//---------------------------------------------------------------------------
int __fastcall CDump::GetChar(BOOL fDraw)
{
	int r;
	if( m_fSelText ){
		if( m_RLine > m_SLine ) return -1;
    	if( (m_RLine == m_SLine) && (m_RCol >= m_SCol) ) return -1;
    }
	if( (m_WLine > m_RLine) || (m_WCol > m_RCol) ){
		int *ip = m_pBase[m_RLine] + m_RCol;
		r = *ip;
        m_RCol++;
		if( !r ){
			m_RX = 0;
            m_RCol = 0;
            m_RLine++;
		    if( m_RLine >= m_StgLineMax ) m_RLine--;
            r = -1;
        }
        else {			// Color change
			m_RY = (m_RLine - m_DTop) * m_FontYW;
            r &= 0x0000ffff;
			*ip = r | (2<<16);
			m_RX = DrawChar(m_RX, m_RY, *ip, m_fWindow && fDraw);
        }
    }
    else {
		r = -1;
    }
    return r;
}
//---------------------------------------------------------------------------
int __fastcall CDump::IsWord(void)
{
	int r = TRUE;
	if( m_fSelText ){
		if( m_RLine > m_SLine ) return TRUE;
    	if( (m_RLine == m_SLine) && (m_RCol >= m_SCol) ) return TRUE;
    }
    if( m_WLine > m_RLine ){
		return TRUE;
    }
	else if( (m_WLine == m_RLine) || (m_WCol > m_RCol) ){
		int *ip = m_pBase[m_RLine] + m_RCol;
		r = *ip;
		if( !r ){
			r = TRUE;
        }
		else if( (r & 0x0000ff00) == 0x0100 ){
			r = TRUE;
        }
        else {
			r = FALSE;
            int d;
			for( ; *ip; ip++ ){
				d = *ip & 0x0000ffff;
                if( (d == ' ') || (d == '\r') ) r = TRUE;
            }
        }
    }
    return r;
}
//---------------------------------------------------------------------------
int __fastcall CDump::GetCharNB(void)
{
	int r;
	if( (m_WLine > m_RLine) || (m_WCol > m_RCol) ){
		int *ip = m_pBase[m_RLine] + m_RCol;
        if( *ip ){
			r = *ip & 0x0000ffff;
        }
        else {
			r = -1;
        }
    }
    else {
		r = -1;
    }
    return r;
}
//---------------------------------------------------------------------------
static int __fastcall JisType(int c)
{
	if( c <= 0x0100 ){
		return 0;
    }
    else if( c < 0x8140 ){
		return -1;
    }
	else if( c == 0x815b ){						// ー
		return 3;
    }
	else if( (c >= 0x8395) && (c <= 0x8396) ){	// ｶｹ
		return 4;
    }
	else if( (c >= 0x824f) && (c <= 0x8258) ){
		return 6;								// suuji
	}
    else if( (c >= 0x829f) && (c <= 0x82f2) ){	// hiragana
		return 2;
    }
    else if( (c >= 0x8340) && (c <= 0x8396) ){	// katakana
		return 3;
    }
    else if( c >= 0x8840 ){						// kanji
		return 4;
    }
	else if( c <= 0x82ff ){
		return 5;								// kugiri
    }
    else {
		return 1;
    }
}
//---------------------------------------------------------------------------
BOOL __fastcall CDump::IsSpace(int c, int ci)
{
	c &= 0x0000ffff;
    if( c == ' ' ) return TRUE;
	if( ci >= 0x8140 ){		// 初回がShift-JISの場合
		c = JisType(c);
        if( (c == 5)||(c == -1) ) return TRUE;
		return (c != JisType(ci));
    }
    else {					// 初回が英数の場合
	    if( c == '/' ) return FALSE;
    	if( c >= 0x100 ) return TRUE;
	    return !isalnum(c);
    }
}
//---------------------------------------------------------------------------
BOOL __fastcall CDump::ClearAllSelect(BOOL fPaint)
{
	BOOL f = FALSE;
	int i;
	int *ip;
	for( i = 0; i < m_StgLineMax; i++ ){
		ip = m_pBase[i];
        for( ; *ip; ip++ ){
			if( *ip & 0x80000000 ){
				f = TRUE;
            	*ip &= 0x7fffffff;
            }
        }
    }
    if( fPaint && f ){
    	Invalidate();
    }
	m_fSelText = FALSE;
    return f;
}
//---------------------------------------------------------------------------
BOOL __fastcall CDump::GetWindowCallsign(AnsiString &as)
{
	AnsiString	MyCall = ClipCall(sys.m_CallSign.c_str());
	as = "";
    int line;
    for( line = m_WLine; line >= 0; line-- ){
	    int *bp = m_pBase[line];
    	int *ip = bp;
        if( (*ip & 0xffff0000)==0x00020000 ){	// 送信文字
			break;
		}
		int n = 0;
        char bf[256];
        for( ; *ip; ip++ ){
			int c = *ip & 0x0000fff;
			if( (c == ' ') || (c == '\r') ){
				bf[n] = 0;
                if( (n >= 3) && IsCall(bf) && strcmpi(ClipCall(bf), MyCall.c_str()) ){
					jstrupr(bf);
					as = bf;
                }
				n = 0;
            }
            else if( (c >= 0x100) || (!isalpha(c) && !isdigit(c) && (c != '/')) ){
				n = 0;
            }
            else {
				bf[n++] = char(c);
                if( n >= 250 ) n = 0;
            }
        }
        if( !as.IsEmpty() ) break;
    }
    return !as.IsEmpty();
}
//---------------------------------------------------------------------------
BOOL __fastcall CDump::IsWindowText(LPCSTR pText)
{
    int line;
    for( line = m_WLine; line >= m_FLine; line-- ){
	    int *bp = m_pBase[line];
    	int *ip = bp;
        if( (*ip & 0xffff0000)==0x00020000 ){	// 送信文字
			break;
		}
		AnsiString as;
		WriteLine(as, line);
		if( strstr(as.c_str(), pText) ) return TRUE;
    }
    return FALSE;
}
//---------------------------------------------------------------------------
void __fastcall CDump::GetWindowText(AnsiString &as)
{
	as = "";
	for( int i = 0; i <= m_MLine; i++ ){
		WriteLine(as, i);
    }
}
//---------------------------------------------------------------------------
int __fastcall CDump::GetWindowText(AnsiString &as, int SX, int SY)
{
	m_SMX = SX;
	as = "";
	int line = SY/m_FontYW + m_DTop;
    if( line > m_WLine ) return FALSE;

    m_SML = line;
    int *bp = m_pBase[line];
    int *ip = bp;
	int x;
    for( x = 0; *ip; ip++ ){
		x += CharWidth(*ip);
        if( x >= SX ) break;
    }
	if( !*ip ) return FALSE;
	int ci = *ip & 0x0000ffff;
	if( m_fRTTY ){
		if( ci == ' ' ) return FALSE;
    }
    else if( m_pFont->Charset != SHIFTJIS_CHARSET ){
		if( IsSpace(ci, 'A') ) return FALSE;
    }
    else if( IsSpace(ci, ci) ){
    	return FALSE;
    }
	/* スペースを探す */
    for( ip--; ip >= bp; ip-- ){
		if( IsSpace(*ip, ci) ){
            ip++;
            break;
        }
    }
	if( ip < bp ) ip = bp;
	/* スペースまでコピー */
	int c;
    for( ; *ip; ip++ ){
		if( IsSpace(*ip, ci) ) break;
		c = *ip & 0x0000ffff;
        if( c >= 0x0100 ){
			as += char(c>>8);
            as += char(c);
        }
        else {
	        as += char(c);
        }
        *ip |= 0x80000000;
    }
    return (ci > 0x0100) ? JisType(ci)+1 : 1;
}
//---------------------------------------------------------------------------
void __fastcall CDump::SwapLTR(void)
{
	if( (m_SML < 0) || (m_SML > m_WLine) ) return;

	CRTTY	rtty;

    int *bp = m_pBase[m_SML];
    int *ip = bp;
	int x;
    for( x = 0; *ip; ip++ ){
		x += CharWidth(*ip);
        if( x >= m_SMX ) break;
    }
	if( !*ip ) return;
	int ci = *ip & 0x0000ffff;
    if( ci == ' ' ) return;

	/* スペースを探す */
    for( ip--; ip >= bp; ip-- ){
		if( (*ip & 0x0000ffff) == ' ' ){
            ip++;
            break;
        }
    }
	if( ip < bp ) ip = bp;
	/* スペースまでコピー */
	int c;
    for( ; *ip; ip++ ){
		if( (*ip & 0x0000ffff) == ' ' ) break;
		c = *ip & 0x0000ffff;
		if( (c >= 0x20) && (c <= 0x7f) ){
			c = rtty.InvShift(char(c));
			*ip = (*ip & 0xffff0000) | c;
        }
        *ip |= 0x80000000;
    }
   	Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall CDump::SetCursorPos(void)
{
	if( !m_fActive ) return;

	if( m_CursorType == csCARET ){
    	SetCaretPos(m_X, m_Y);
	   	SetCompositionWindowPos(FALSE);
	}
}
//---------------------------------------------------------------------------
void __fastcall CDump::DrawCursor(int x, int y, int sw)
{
	TRect rc;
	rc.Left = x; rc.Top = y + m_FontYW - 2;
    rc.Right = x + m_FontXW; rc.Bottom = y + m_FontYW;
	m_pCanvas->Brush->Color = sw ? m_CursorColor : m_Color[0].c;
    m_pCanvas->FillRect(rc);
}
//---------------------------------------------------------------------------
void __fastcall CDump::SetCursorType(int type)
{
	Cursor(FALSE);
    m_CursorType = type;
    Cursor(TRUE);
}
//---------------------------------------------------------------------------
void __fastcall CDump::Cursor(int sw)
{
	if( sw && !m_fWindow ) return;

	switch(m_CursorType){
		case csCARET:		// キャレットを使ったカーソル
			if( sw ){
				if( !m_fActive ) return;

				if( !m_Cursor ){
					m_Cursor++;
				   	ShowCaret(m_hWnd);
                }
            }
            else {
				if( m_Cursor ){
					m_Cursor--;
				    HideCaret(m_hWnd);
                }
            }
        	break;
        case csSTATIC:		// 描画カーソル
        	if( sw ){
				DrawCursor(m_X, m_Y, TRUE);
            }
            else {
				DrawCursor(m_X, m_Y, FALSE);
            }
    }
}
//---------------------------------------------------------------------------
void __fastcall CDump::CreateCaret(void)
{
	if( m_fActive ) return;
	m_fActive = TRUE;

	if( m_CursorType == csCARET ){
	    ::CreateCaret(m_hWnd, 0, 2, m_FontYW);
    	::SetCaretPos(m_X, m_Y);
		::ShowCaret(m_hWnd);
        m_Cursor = TRUE;
    	SetCompositionWindowPos(TRUE);
    }
}

//---------------------------------------------------------------------------
void __fastcall CDump::DestroyCaret(void)
{
	if( !m_fActive ) return;
	m_fActive = FALSE;

	if( m_CursorType == csCARET ){
		m_Cursor = 0;
		::DestroyCaret();
    }
}
//---------------------------------------------------------------------------
void __fastcall CDump::SetCompositionWindowPos(BOOL fFont)
{
	if( !m_fActive ) return;

	HIMC hIMC;
	COMPOSITIONFORM Composition;

	if( (hIMC = ::ImmGetContext(m_hWnd))!=NULL ){
		if( fFont ){
			::ImmSetCompositionFont(hIMC, &m_LogFont);
       	}
		Composition.dwStyle = CFS_POINT;
		Composition.ptCurrentPos.x = m_X;
		Composition.ptCurrentPos.y = m_Y;
		::ImmSetCompositionWindow(hIMC, &Composition);

		::ImmReleaseContext(m_hWnd, hIMC);
	}
}
//---------------------------------------------------------------------------
void __fastcall CDump::ShowCtrl(BOOL f)
{
	m_fShowCtrlCode = f;
    if( m_pPaintBox ) m_pPaintBox->Invalidate();
}
#pragma package(smart_init)
