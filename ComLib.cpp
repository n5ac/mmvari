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

#include "ComLib.h"
#include "LogFile.h"
#include "main.h"
//---------------------------------------------------------------------------
//char    g_MMLogDir[256];

SYSSET  	sys;		// システム共通データ

//LPCSTR g_tLogModeTable[]={"GMSK", "FSK", "FSK", "PSK", "PSK", "PSK", "PSK", "RTTY", "RTTY", "MFSK", "MFSK", NULL};
//LPCSTR g_tDispModeTable[]={"GMSK", "FSK", "FSK-W", "BPSK", "2PSK-L", "2PSK-U", "bpsk", "rtty-L", "rtty-U", "mfsk-L", "mfsk-U", NULL};
const LPCSTR g_tLogModeTable[]={"GMSK", "FSK", "FSK", "PSK", "PSK", "RTTY", "RTTY", "MFSK", "MFSK", "QPSK", "QPSK", NULL};
const LPCSTR g_tDispModeTable[]={"GMSK", "FSK", "FSK-W", "BPSK", "bpsk", "rtty-L", "rtty-U", "mfsk-L", "mfsk-U", "qpsk-L", "qpsk-U", NULL};
const LPCSTR g_tMacEvent[]={"OnTimer","OnPTT","OnQSO","OnFind", "OnBand", "OnStart", "OnExit", "OnMode", "OnSpeed", "OnClick", "OnFFTScale"};
const LPCSTR g_tSoundCH[]={"MONO","LEFT","RIGHT"};
const LPCSTR g_tOnOff[]={"OFF", "ON", "ONOFF"};
//---------------------------------------------------------------------------
void __fastcall OnWave(void)
{
	MainVARI->OnWave();
}
//---------------------------------------------------------------------------
int __fastcall FindStringTableStrictly(const LPCSTR _tt[], LPCSTR pName, int max)
{
	int n;
    for( n = 0; (n < max) && (_tt[n]!=NULL); n++ ){
		if( !strcmp(_tt[n], pName) ) return n;
    }
    return -1;
}
//---------------------------------------------------------------------------
int __fastcall FindStringTable(const LPCSTR _tt[], LPCSTR pName, int max)
{
	int n;
    for( n = 0; (n < max) && (_tt[n]!=NULL); n++ ){
		if( !strcmpi(_tt[n], pName) ) return n;
    }
    return -1;
}
//---------------------------------------------------------------------------
int __fastcall GetModeIndex(LPCSTR pName)
{
	int m;
	for( m = 0; g_tDispModeTable[m] && (m < MODE_END); m++ ){
		if( !strcmp(g_tDispModeTable[m], pName) ) return m;
    }
    if( sscanf(pName, "%d", &m) == 1 ){
		if( m < 0 ) m = 0;
        if( m >= MODE_END ) m = MODE_END - 1;
    }
    else {
		m = 0;
    }
    return m;
}

const DEFKEYTBL KEYTBL[]={
	{ VK_F1, "F1" },
	{ VK_F2, "F2" },
	{ VK_F3, "F3" },
	{ VK_F4, "F4" },
	{ VK_F5, "F5" },
	{ VK_F6, "F6" },
	{ VK_F7, "F7" },
	{ VK_F8, "F8" },
	{ VK_F9, "F9" },
	{ VK_F10, "F10" },
	{ VK_F11, "F11" },
	{ VK_F12, "F12" },

	{ VK_LEFT, "←"},
	{ VK_RIGHT, "→"},
	{ VK_UP, "↑"},
	{ VK_DOWN, "↓"},
	{ VK_PRIOR, "PageUp"},
	{ VK_NEXT, "PageDown"},
	{ VK_HOME, "Home" },
	{ VK_END, "End" },
	{ VK_INSERT, "Insert" },
	{ VK_DELETE, "Delete" },
	{ VK_HELP, "Help" },
	{ VK_ESCAPE, "ESC" },
	{ VK_PAUSE, "PAUSE" },


	{ VK_F1 | 0x400, "Shift+F1" },
	{ VK_F2 | 0x400, "Shift+F2" },
	{ VK_F3 | 0x400, "Shift+F3" },
	{ VK_F4 | 0x400, "Shift+F4" },
	{ VK_F5 | 0x400, "Shift+F5" },
	{ VK_F6 | 0x400, "Shift+F6" },
	{ VK_F7 | 0x400, "Shift+F7" },
	{ VK_F8 | 0x400, "Shift+F8" },
	{ VK_F9 | 0x400, "Shift+F9" },
	{ VK_F10 | 0x400, "Shift+F10" },
	{ VK_F11 | 0x400, "Shift+F11" },
	{ VK_F12 | 0x400, "Shift+F12" },

	{ VK_LEFT | 0x400, "Shift+←"},
	{ VK_RIGHT | 0x400, "Shift+→"},
	{ VK_UP | 0x400, "Shift+↑"},
	{ VK_DOWN | 0x400, "Shift+↓"},
	{ VK_PRIOR | 0x400, "Shift+PageUp"},
	{ VK_NEXT | 0x400, "Shift+PageDown"},
	{ VK_HOME | 0x400, "Shift+Home" },
	{ VK_END | 0x400, "Shift+End" },
	{ VK_INSERT | 0x400, "Shift+Insert" },
	{ VK_DELETE | 0x400, "Shift+Delete" },
	{ VK_HELP | 0x400, "Shift+Help" },
	{ VK_ESCAPE | 0x400, "Shift+ESC" },
	{ VK_PAUSE | 0x400, "Shift+PAUSE" },

	{ VK_F1 | 0x100, "Ctrl+F1" },
	{ VK_F2 | 0x100, "Ctrl+F2" },
	{ VK_F3 | 0x100, "Ctrl+F3" },
	{ VK_F4 | 0x100, "Ctrl+F4" },
	{ VK_F5 | 0x100, "Ctrl+F5" },
	{ VK_F6 | 0x100, "Ctrl+F6" },
	{ VK_F7 | 0x100, "Ctrl+F7" },
	{ VK_F8 | 0x100, "Ctrl+F8" },
	{ VK_F9 | 0x100, "Ctrl+F9" },
	{ VK_F10 | 0x100, "Ctrl+F10" },
	{ VK_F11 | 0x100, "Ctrl+F11" },
	{ VK_F12 | 0x100, "Ctrl+F12" },
	{ '1' | 0x100, "Ctrl+1" },
	{ '2' | 0x100, "Ctrl+2" },
	{ '3' | 0x100, "Ctrl+3" },
	{ '4' | 0x100, "Ctrl+4" },
	{ '5' | 0x100, "Ctrl+5" },
	{ '6' | 0x100, "Ctrl+6" },
	{ '7' | 0x100, "Ctrl+7" },
	{ '8' | 0x100, "Ctrl+8" },
	{ '9' | 0x100, "Ctrl+9" },
	{ '0' | 0x100, "Ctrl+0" },
	{ 'A' | 0x100, "Ctrl+A" },
	{ 'B' | 0x100, "Ctrl+B" },
	{ 'C' | 0x100, "Ctrl+C" },
	{ 'D' | 0x100, "Ctrl+D" },
	{ 'E' | 0x100, "Ctrl+E" },
	{ 'F' | 0x100, "Ctrl+F" },
	{ 'G' | 0x100, "Ctrl+G" },
//    { 'H' | 0x100, "Ctrl+H" },
//    { 'I' | 0x100, "Ctrl+I" },
	{ 'K' | 0x100, "Ctrl+K" },
	{ 'L' | 0x100, "Ctrl+L" },
//    { 'M' | 0x100, "Ctrl+M" },
	{ 'N' | 0x100, "Ctrl+N" },
	{ 'O' | 0x100, "Ctrl+O" },
	{ 'P' | 0x100, "Ctrl+P" },
	{ 'Q' | 0x100, "Ctrl+Q" },
	{ 'R' | 0x100, "Ctrl+R" },
	{ 'S' | 0x100, "Ctrl+S" },
	{ 'T' | 0x100, "Ctrl+T" },
	{ 'U' | 0x100, "Ctrl+U" },
	{ 'V' | 0x100, "Ctrl+V" },
	{ 'W' | 0x100, "Ctrl+W" },
	{ 'X' | 0x100, "Ctrl+X" },
	{ 'Y' | 0x100, "Ctrl+Y" },
	{ 'Z' | 0x100, "Ctrl+Z" },

	{ VK_LEFT | 0x100, "Ctrl+←"},
	{ VK_RIGHT | 0x100, "Ctrl+→"},
	{ VK_UP | 0x100, "Ctrl+↑"},
	{ VK_DOWN | 0x100, "Ctrl+↓"},
	{ VK_PRIOR | 0x100, "Ctrl+PageUp"},
	{ VK_NEXT | 0x100, "Ctrl+PageDown"},
	{ VK_HOME | 0x100, "Ctrl+Home" },
	{ VK_END | 0x100, "Ctrl+End" },
	{ VK_INSERT | 0x100, "Ctrl+Insert" },
	{ VK_DELETE | 0x100, "Ctrl+Delete" },
	{ VK_HELP | 0x100, "Ctrl+Help" },
	{ VK_ESCAPE | 0x100, "Ctrl+ESC" },

	{ VK_F1 | 0x200, "Alt+F1" },
	{ VK_F2 | 0x200, "Alt+F2" },
	{ VK_F3 | 0x200, "Alt+F3" },
	{ VK_F4 | 0x200, "Alt+F4" },
	{ VK_F5 | 0x200, "Alt+F5" },
	{ VK_F6 | 0x200, "Alt+F6" },
	{ VK_F7 | 0x200, "Alt+F7" },
	{ VK_F8 | 0x200, "Alt+F8" },
	{ VK_F9 | 0x200, "Alt+F9" },
	{ VK_F10 | 0x200, "Alt+F10" },
	{ VK_F11 | 0x200, "Alt+F11" },
	{ VK_F12 | 0x200, "Alt+F12" },
	{ '1' | 0x200, "Alt+1" },
	{ '2' | 0x200, "Alt+2" },
	{ '3' | 0x200, "Alt+3" },
	{ '4' | 0x200, "Alt+4" },
	{ '5' | 0x200, "Alt+5" },
	{ '6' | 0x200, "Alt+6" },
	{ '7' | 0x200, "Alt+7" },
	{ '8' | 0x200, "Alt+8" },
	{ '9' | 0x200, "Alt+9" },
	{ '0' | 0x200, "Alt+0" },
	{ 'A' | 0x200, "Alt+A" },
	{ 'B' | 0x200, "Alt+B" },
	{ 'C' | 0x200, "Alt+C" },
	{ 'D' | 0x200, "Alt+D" },
	{ 'E' | 0x200, "Alt+E" },
	{ 'F' | 0x200, "Alt+F" },
	{ 'G' | 0x200, "Alt+G" },
	{ 'H' | 0x200, "Alt+H" },
	{ 'I' | 0x200, "Alt+I" },
	{ 'K' | 0x200, "Alt+K" },
	{ 'L' | 0x200, "Alt+L" },
	{ 'M' | 0x200, "Alt+M" },
	{ 'N' | 0x200, "Alt+N" },
	{ 'O' | 0x200, "Alt+O" },
	{ 'P' | 0x200, "Alt+P" },
	{ 'Q' | 0x200, "Alt+Q" },
	{ 'R' | 0x200, "Alt+R" },
	{ 'S' | 0x200, "Alt+S" },
	{ 'T' | 0x200, "Alt+T" },
	{ 'U' | 0x200, "Alt+U" },
	{ 'V' | 0x200, "Alt+V" },
	{ 'W' | 0x200, "Alt+W" },
	{ 'X' | 0x200, "Alt+X" },
	{ 'Y' | 0x200, "Alt+Y" },
	{ 'Z' | 0x200, "Alt+Z" },

	{ VK_LEFT | 0x200, "Alt+←"},
	{ VK_RIGHT | 0x200, "Alt+→"},
	{ VK_UP | 0x200, "Alt+↑"},
	{ VK_DOWN | 0x200, "Alt+↓"},
	{ VK_PRIOR | 0x200, "Alt+PageUp"},
	{ VK_NEXT | 0x200, "Alt+PageDown"},
	{ VK_HOME | 0x200, "Alt+Home" },
	{ VK_END | 0x200, "Alt+End" },
	{ VK_INSERT | 0x200, "Alt+Insert" },
	{ VK_DELETE | 0x200, "Alt+Delete" },
	{ VK_HELP | 0x200, "Alt+Help" },
	{ VK_ESCAPE | 0x200, "Alt+ESC" },
	{ VK_PAUSE | 0x200, "Alt+PAUSE" },
	{ 0, "" },
};
//---------------------------------------------------------------------------
LPCSTR __fastcall ToDXKey(LPCSTR s)
{
	if( sys.m_MsgEng && *s ){
		LPSTR		p;
		static char bf[32];
		strcpy(bf, s);
		if( (p = strstr(bf, "←"))!=NULL ){
			strcpy(p, "ArrowLeft");
			return bf;
		}
		else if( (p = strstr(bf, "→"))!=NULL ){
			strcpy(p, "ArrowRight");
			return bf;
		}
		else if( (p = strstr(bf, "↓"))!=NULL ){
			strcpy(p, "ArrowDown");
			return bf;
		}
		else if( (p = strstr(bf, "↑"))!=NULL ){
			strcpy(p, "ArrowUp");
			return bf;
		}
	}
	return s;
}
//---------------------------------------------------------------------------
LPCSTR __fastcall ToJAKey(LPCSTR s)
{
	if( sys.m_MsgEng && *s ){
		LPSTR		p;
		static char bf[32];
		strcpy(bf, s);
		if( (p = strstr(bf, "ArrowLeft"))!=NULL ){
			strcpy(p, "←");
			return bf;
		}
		else if( (p = strstr(bf, "ArrowRight"))!=NULL ){
			strcpy(p, "→");
			return bf;
		}
		else if( (p = strstr(bf, "ArrowDown"))!=NULL ){
			strcpy(p, "↓");
			return bf;
		}
		else if( (p = strstr(bf, "ArrowUp"))!=NULL ){
			strcpy(p, "↑");
			return bf;
		}
	}
	return s;
}
//---------------------------------------------------------------------------
LPCSTR __fastcall GetKeyName(WORD Key)
{
	int i;
	for( i = 0; KEYTBL[i].Key; i++ ){
		if( Key == KEYTBL[i].Key ){
			break;
		}
	}
	return ToDXKey(KEYTBL[i].pName);
}
//---------------------------------------------------------------------------
WORD __fastcall GetKeyCode(LPCSTR pName)
{
	LPCSTR p = ToJAKey(pName);
	int i;
	for( i = 0; KEYTBL[i].Key; i++ ){
		if( !strcmp(p, KEYTBL[i].pName) ) break;
	}
	return KEYTBL[i].Key;
}

//---------------------------------------------------------------------------
int __fastcall IsFile(LPCSTR pName)
{
	if( !pName || !*pName ) return 0;
	FILE *fp;
	fp = fopen(pName, "rb");
	if( fp != NULL ){
		fclose(fp);
		return 1;
	}
	else {
		return 0;
	}
}
//---------------------------------------------------------------------------
int __fastcall SetTimeOffsetInfo(int &Hour, int &Min)
{
	TIME_ZONE_INFORMATION tz;

	int off;
	switch(GetTimeZoneInformation(&tz)){
		case TIME_ZONE_ID_STANDARD:
			off = tz.StandardBias;
			break;
		case TIME_ZONE_ID_DAYLIGHT:
			off = tz.DaylightBias;
			break;
		default:
			return FALSE;
	}
	if( off >= 0 ){
		Hour = -(off / 60);
		Min = (off % 60);
	}
	else {
		off = -off;
		Hour = off / 60;
		Min = off % 60;
	}
	return TRUE;
}
//---------------------------------------------------------------------------
void __fastcall AddjustOffset(SYSTEMTIME *tp)
{
	if( sys.m_TimeOffset || sys.m_TimeOffsetMin ){
//typedef struct _SYSTEMTIME {  /* st */
//    WORD wYear;
//    WORD wMonth;
//    WORD wDayOfWeek;
//    WORD wDay;
//    WORD wHour;
//    WORD wMinute;
//    WORD wSecond;
//    WORD wMilliseconds;
//} SYSTEMTIME;


		LPCSTR	mt;
		int Hour = tp->wHour;
		int Min = tp->wMinute;
		Min += sys.m_TimeOffsetMin;
		if( Min >= 60 ){
			Hour++;
			Min -= 60;
		}
		else if( Min < 0 ){
			Hour--;
			Min += 60;
		}
		tp->wMinute = WORD(Min);
		Hour += sys.m_TimeOffset;
		if( Hour >= 24 ){
			tp->wHour = WORD(Hour - 24);
			tp->wDay++;
			if( tp->wYear % 4 ){
				mt = MONN;
			}
			else {
				mt = MONU;
			}
			if( tp->wDay > mt[tp->wMonth] ){
				tp->wDay = 1;
				tp->wMonth++;
				if( tp->wMonth > 12 ){
					tp->wMonth = 1;
					tp->wYear++;
				}
			}
		}
		else if( Hour < 0 ){
			tp->wHour = WORD(Hour + 24);
			tp->wDay--;
			if( tp->wDay < 1 ){
				tp->wMonth--;
				if( tp->wMonth < 1 ){
					tp->wMonth = 12;
					tp->wYear--;
				}
				if( tp->wYear % 4 ){
					tp->wDay = MONN[tp->wMonth];
				}
				else {
					tp->wDay = MONU[tp->wMonth];
				}
			}
		}
		else {
			tp->wHour = WORD(Hour);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall GetUTC(SYSTEMTIME *tp)
{
	::GetSystemTime(tp);
	AddjustOffset(tp);
}
//---------------------------------------------------------------------------
void __fastcall GetLocal(SYSTEMTIME *tp)
{
	::GetLocalTime(tp);
	AddjustOffset(tp);
}
//---------------------------------------------------------------------------
void __fastcall LimitInt(int *pInt, int min, int max)
{
	if( *pInt < min ) *pInt = min;
    if( *pInt > max ) *pInt = max;
}
//---------------------------------------------------------------------------
void __fastcall LimitDbl(double *pInt, double min, double max)
{
	if( *pInt < min ) *pInt = min;
    if( *pInt > max ) *pInt = max;
}
//---------------------------------------------------------------------------
LPCSTR __fastcall StrDbl(LPSTR bf, double d)
{
	LPSTR	p;

	sprintf(bf, "%lf", d);
	for( p = &bf[strlen(bf)-1]; p >= bf; p-- ){
		if( (*p == '0')&&(*(p-1)!='.') ){
			*p = 0;
		}
		else {
			break;
		}
	}
	return bf;
}
//---------------------------------------------------------------------------
void __fastcall AdjustAS(AnsiString *pAS)
{
	LPCSTR p;
    BOOL f = TRUE;
	for( p = pAS->c_str(); *p; p++ ){
		if( (*p != '\r') && (*p != '\n') ) f = FALSE;
    }
    if( f ) *pAS = "";
}
//---------------------------------------------------------------------------
// ビットマップへのメッセージの描画
void __fastcall DrawMessage(TCanvas *pCanvas, int XW, int YW, LPCSTR p, int Pos)
{
	if( !*p ) return;

	int xr, xl, vc;
	int fh = pCanvas->TextHeight(p);
    int fw = pCanvas->TextWidth(p);
	switch(Pos){
		case 0:     // 中央
			xr = fw;
			xl = (XW - xr)/2;
			xr += xl;
			vc = YW / 2;
			break;
		case 1:     // 左上
			xl = 5;
			xr = xl + fw;
			vc = fh*2/3;
			break;
        case 2:		// 右上
        	xr = XW - 8;
			xl = xr - fw;
			vc = fh*2/3;
            break;
        case 3:		// 右下
			xr = XW - 8;
            xl = xr - fw;
            vc = YW - fh;
            break;
		case 4:     // 左下
			xl = 5;
			xr = xl + fw;
            vc = YW - fh;
			break;
        default:
        	xl = Pos - fw/2;
            xr = xl + fw;
            vc = fh*2/3;
        	break;
	}
	pCanvas->Pen->Style = psSolid;
	pCanvas->Pen->Color = clBlack;
	pCanvas->Brush->Color = clWhite;
    int yo = fh * 2/3;
	pCanvas->RoundRect(xl-5, vc-yo, xr+5, vc+yo, 10, 10);
	pCanvas->Font->Color = clBlack;
	pCanvas->TextOut(xl, vc-fh/2, p);
}
//---------------------------------------------------------------------------
LPSTR __fastcall StrDupe(LPCSTR s)
{
	LPSTR p = new char[strlen(s)+1];
	strcpy(p, s);
	return p;
}


LPUSTR __fastcall jstrupr(LPUSTR s)
{
	LPUSTR	p = s;
	int kf;

	for( kf = 0; *p; p++ ){
		if( kf ){
			kf = 0;
		}
		else if( _mbsbtype((unsigned char *)p, 0) == _MBC_LEAD ){
			kf = 1;
		}
		else {
			*p = (unsigned char)toupper(*p);
		}
	}
	return s;
}
LPUSTR __fastcall jstrlwr(LPUSTR s)
{
	LPUSTR	p = s;
	int kf;

	for( kf = 0; *p; p++ ){
		if( kf ){
			kf = 0;
		}
		else if( _mbsbtype((unsigned char *)p, 0) == _MBC_LEAD ){
			kf = 1;
		}
		else {
			*p = (unsigned char)tolower(*p);
		}
	}
	return s;
}
//---------------------------------------------------------------------------
LPCSTR __fastcall ConvAndChar(LPSTR t, LPCSTR p)
{
	LPSTR s = t;

    while(*p){
		if( *p == '&' ){
			*t++ = *p;
        }
		*t++ = *p++;
    }
    *t = 0;
    return s;
}
/*#$%
===============================================================
	時刻を調整する
---------------------------------------------------------------
	t : 時刻(UTC)
	c : 時差コード
---------------------------------------------------------------
	ローカルタイム
---------------------------------------------------------------
	A-Z
	a-z 	+30min
===============================================================
*/
WORD __fastcall AdjustRolTimeUTC(WORD tim, char c)
{
	const	char	tdf[]={
/*		A B C D E F G H I J K  L  M  N  O  P  Q  R  S  T  U  V  W  X  Y  Z	*/
		1,2,3,4,5,6,7,8,9,9,10,11,12,23,22,21,20,19,18,17,16,15,14,13,12,0
	};
	int	cc;

	cc = toupper(c);
	if( (cc >= 'A')&&(cc <= 'Z') ){
		// JST to UTC
#if 0
		if( tim >= (9*60*30) ){
			tim -= WORD(9 * 60 * 30);
		}
		else {
			tim += WORD(15 * 60 * 30);
		}
#endif
		tim /= WORD(30);
		tim += WORD(tdf[cc-'A'] * 60);
		if( c >= 'a' ) tim += WORD(30);
		while( tim >= (24 * 60) ){
			tim -= WORD(24 * 60);
		}
		tim *= WORD(30);
		if( !tim ) tim++;
	}
	else {
		tim = 0;
	}
	return tim;
}
///----------------------------------------------------------------
///	ウィンドウをクライアントセンターにする
void __fastcall FormCenter(TForm *tp, int XW, int YW)
{
	int top = (YW - tp->Height)/2;
	int left = (XW - tp->Width)/2;
	if( top < 0 ) top = 0;
	if( left < 0 ) left = 0;
	tp->Position = poDesigned;
    tp->SetBounds(left, top, tp->Width, tp->Height);
}
///----------------------------------------------------------------
///	ウィンドウをクライアントセンターにする
void __fastcall FormCenter(TForm *tp, TForm *pOwner)
{
	int top = (pOwner->Height - tp->Height)/2;
	int left = (pOwner->Width - tp->Width)/2;
	if( top < 0 ) top = 0;
	if( left < 0 ) left = 0;
	tp->Position = poDesigned;
    tp->SetBounds(pOwner->Left + left, pOwner->Top + top, tp->Width, tp->Height);
}
///----------------------------------------------------------------
///	ウィンドウをクライアントセンターにする
void __fastcall FormCenter(TForm *tp)
{
	FormCenter(tp, MainVARI);
}
/*#$%
========================================================
	最後の文字コードを返す
--------------------------------------------------------
	p : 文字列のポインタ
--------------------------------------------------------
	文字コード
--------------------------------------------------------
========================================================
*/
char *__fastcall lastp(char *p)
{
	if( *p ){
		for( ; *p; p++ );
		p--;
		return(p);
	}
	else {
		return(p);
	}
}
/*#$%
========================================================
	末尾のスペースとＴＡＢを取る
--------------------------------------------------------
	s : 文字列のポインタ
--------------------------------------------------------
	文字列のポインタ
--------------------------------------------------------
========================================================
*/
char *__fastcall clipsp(char *s)
{
	char	*p;

	if( *s ){
		for( p = lastp(s); p >= s; p-- ){
			if( (*p == ' ')||(*p == 0x09) ){
				*p = 0;
			}
			else {
				break;
			}
		}
	}
	return(s);
}
//---------------------------------------------------------------------------
LPCSTR __fastcall _strdmcpy(LPSTR t, LPCSTR p, char c)
{
	for( ; *p && (*p != c); p++, t++ ) *t = *p;
	*t = 0;
	if( *p == c ) p++;
	return(p);
}
const char *__fastcall StrDlmCpy(char *t, const char *p, char Dlm, int len)
{
	const char _tt1[]="[{(｢<";
	const char _tt2[]="]})｣>";
	const char	*pp;
	int			r = FALSE;

	char	Key;
	if( (pp = strchr(_tt2, Dlm))!=NULL ){
		Key = _tt1[pp - _tt2];
	}
	else {
		Key = 0;
	}
	int	f, k;
	for( f = k = 0; *p;  p++ ){
		if( k ){															// 漢字２バイト目
			k = 0;
		}
		else if(  _mbsbtype((const unsigned char *)p, 0) == _MBC_LEAD ){	// 漢字１バイト目
			k = 1;
		}
		else if( *p == Key ){
			f++;
		}
		else if( *p == Dlm ){
			if( f ){
				f--;
			}
			else {
				r = TRUE;
				p++;
				break;
			}
		}
		if( len ){
			*t++ = *p;
			len--;
		}
	}
	*t = 0;
	return (r == TRUE) ? p : NULL;
}

const char *__fastcall StrDlmCpyK(char *t, const char *p, char Dlm, int len)
{
	const char _tt1[]="[{(｢<";
	const char _tt2[]="]})｣>";
	const char	*pp;
	int			r = FALSE;

	char	Key;
	if( (pp = strchr(_tt2, Dlm))!=NULL ){
		Key = _tt1[pp - _tt2];
	}
	else {
		Key = 0;
	}
	int	f, k;
	for( f = k = 0; *p;  p++ ){
		if( k ){															// 漢字２バイト目
			k = 0;
		}
		else if(  _mbsbtype((const unsigned char *)p, 0) == _MBC_LEAD ){	// 漢字１バイト目
			k = 1;
		}
		else if( (pp = strchr(_tt1, *p))!=NULL ){
			Key = _tt2[pp - _tt1];
			f++;
		}
		else if( f && (*p == Key) ){
			f--;
			Key = 0;
		}
		else if( *p == Dlm ){
			if( !f ){
				r = TRUE;
				p++;
				break;
			}
		}
		if( len ){
			*t++ = *p;
			len--;
		}
	}
	*t = 0;
	return (r == TRUE) ? p : NULL;
}
//---------------------------------------------------------------------------
void __fastcall StrCopy(LPSTR t, LPCSTR s, int n)
{
	for( ; *s && n; n--, s++, t++){
		*t = *s;
	}
	*t = 0;
}
//---------------------------------------------------------------------------
char __fastcall LastC(LPCSTR p)
{
	char c = 0;

	if( *p ){
		c = *(p + strlen(p) - 1);
	}
	return c;
}

/*#$%
========================================================
	拡張子を得る
--------------------------------------------------------
	p : 文字列のポインタ
--------------------------------------------------------
	文字コード
--------------------------------------------------------
========================================================
*/
LPCSTR __fastcall GetEXT(LPCSTR Fname)
{
	if( *Fname ){
		LPCSTR p = Fname + (strlen(Fname) - 1);
		for( ; p > Fname; p-- ){
			if( *p == '.' ) return p+1;
		}
	}
	return "";
}

void __fastcall SetEXT(LPSTR pName, LPSTR pExt)
{
	if( *pName ){
		LPSTR	p;

		for( p = &pName[strlen(pName)-1]; p >= pName; p-- ){
			if( *p == '.' ){
				strcpy(p, pExt);
				return;
			}
		}
		strcat(pName, pExt);
	}
}

BOOL __fastcall CheckEXT(LPCSTR pName, LPCSTR pExt)
{
	return !strcmpi(GetEXT(pName), pExt);
}

void __fastcall SetCurDir(LPSTR t, int size)
{
	if( !::GetCurrentDirectory(size-1, t) ){
		*t = 0;
	}
	else {
		if( LastC(t) != '\\' ){
			strcat(t, "\\");
		}
	}
}

void __fastcall SetDirName(LPSTR t, LPCSTR pName)
{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char name[_MAX_FNAME];
	char ext[_MAX_EXT];
	AnsiString	Dir;

	::_splitpath( pName, drive, dir, name, ext );
	Dir = drive;
	Dir += dir;
	strncpy(t, Dir.c_str(), 128);
}

///----------------------------------------------------------------
void __fastcall GetFileName(AnsiString &Name, LPCSTR pName)
{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char name[_MAX_FNAME];
	char ext[_MAX_EXT];
	::_splitpath( pName, drive, dir, name, ext );
    Name = name;
    Name += ext;
}
//---------------------------------------------------------------------------
void __fastcall GetFullPathName(AnsiString &as, LPCSTR pName)
{
	GetFullPathName(as, pName, sys.m_BgnDir);
}
//---------------------------------------------------------------------------
void __fastcall GetFullPathName(AnsiString &as, LPCSTR pName, LPCSTR pFolder)
{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char name[_MAX_FNAME];
	char ext[_MAX_EXT];

	::_splitpath( pName, drive, dir, name, ext );
	if( !drive[0] || !dir[0] ){
		char fname[512];
		sprintf(fname, "%s%s", pFolder, pName);
        as = fname;
    }
    else {
		as = pName;
    }
}
///----------------------------------------------------------------
///  CR/LFを削除
///
void __fastcall ClipLF(LPSTR sp)
{
	for( ; *sp; sp++ ){
		if( (*sp == LF)||(*sp == CR) ){
			*sp = 0;
			break;
		}
	}
}
///----------------------------------------------------------------
///  CRを削除
///
void __fastcall DelCR(AnsiString &ws, LPCSTR s)
{
	ws = "";
	for( ; *s; s++ ){
		if( *s != CR ){
			ws += *s;
		}
	}
}
///----------------------------------------------------------------
///  コメントを削除（スペースおよびＴＡＢも削除）
///
void __fastcall DeleteComment(LPSTR bf)
{
	LPSTR sp, tp;

	for( sp = tp = bf; *sp; sp++ ){
		if( (*sp == ';')||(*sp == CR)||(*sp == LF) ){
			break;
		}
		else if( (*sp!=' ')&&(*sp!=TAB) ){
			*tp++ = *sp;
		}
	}
	*tp = 0;
}

LPSTR __fastcall FillSpace(LPSTR s, int n)
{
	LPSTR p = s;
	int i = 0;
	for( ; *p && (i < n); i++, p++ );
	for( ; i < n; i++, p++ ){
		*p = ' ';
	}
	*p = 0;
	return s;
}
///----------------------------------------------------------------
LPCSTR __fastcall SkipToValue(LPCSTR sp)
{
	while(*sp){
		if( isdigit(*sp) ) break;
        if( (*sp == '+') || (*sp == '-') || (*sp == '.') ) break;
        sp++;
    }
    return sp;
}
///----------------------------------------------------------------
///  ホワイトスペースのスキップ
///
LPSTR __fastcall SkipSpace(LPSTR sp)
{
	for( ; *sp; sp++ ){
		if( (*sp != ' ')&&(*sp != TAB) ) break;
	}
	return sp;
}

LPCSTR __fastcall SkipSpace(LPCSTR sp)
{
	for( ; *sp; sp++ ){
		if( (*sp != ' ')&&(*sp != TAB) ) break;
	}
	return sp;
}
///----------------------------------------------------------------
LPSTR __fastcall DelLastSpace(LPSTR t)
{
	int l = strlen(t);
	if( l ){
		LPSTR p;
		for( p = t + l - 1; p >= t; p-- ){
			if( (*p == ' ') || (*p == TAB) ){
				*p = 0;
			}
			else {
				break;
			}
		}
	}
	return t;
}
///----------------------------------------------------------------
///  デリミッタ分解を行う
///
LPSTR __fastcall StrDlm(LPSTR &t, LPSTR p)
{
	return StrDlm(t, p, ',');
}

///----------------------------------------------------------------
///  デリミッタ分解を行う
///
LPSTR __fastcall StrDlm(LPSTR &t, LPSTR p, char c)
{
	int		f, k;
	LPSTR	d1=NULL;
	LPSTR	d2=NULL;

	t = p;
	f = k = 0;
	while(*p){
		if( k ){															// 漢字２バイト目
			if( (c == -1) && !sys.m_MsgEng ){
				k |= *p & 0x00ff;
                if( k == 0x8140 ){
					p--;
					*p = 0;
					p+=2;
					break;
                }
            }
			k = 0;
		}
		else if(  _mbsbtype((const unsigned char *)p, 0) == _MBC_LEAD ){	// 漢字１バイト目
			k = *p & 0x00ff;
            k = k << 8;
		}
		else if( *p == 0x22 ){
			if( !f ){
				if( d1 == NULL ) d1 = p+1;
				f++;
			}
			else {
				d2 = p;
				f--;
			}
		}
		else if( !f && ((*p == c) || ((c == -1) && strchr(" /,;;.", *p))) ){
			*p = 0;
			p++;
			break;
		}
		p++;
	}
	if( (d1!=NULL)&&(d2!=NULL) ){
		if( ((t+1)==d1) && ( ((p-2)==d2)||((p-1)==d2) ) ){
			t = d1;
			*d2 = 0;
		}
	}
	return(p);
}

///----------------------------------------------------------------
///
void __fastcall ChgString(LPSTR t, char a, char b)
{
	for( ; *t; t++ ){
		if( *t == a ) *t = b;
	}
}

///----------------------------------------------------------------
///
void __fastcall DelChar(LPSTR t, char a)
{
	for( ; *t; t++ ){
		if( *t == a ){
			strcpy(t, t+1);
			t--;
		}
	}
}

/*#$%
========================================================
	デシマルアスキーを数値に変換する
--------------------------------------------------------
	p : 文字列のポインタ
	n : 変換桁数
--------------------------------------------------------
	数値
--------------------------------------------------------
========================================================
*/
int __fastcall atoin(const char *p, int n)
{
	int	d;

	for( d = 0; *p && n; p++, n-- ){
		d *= 10;
		d += (*p & 0x0f);
	}
	return(d);
}
/*#$%
========================================================
	１６進アスキーを数値に変換する
--------------------------------------------------------
	p : 文字列のポインタ
	n : 変換桁数
--------------------------------------------------------
	数値
--------------------------------------------------------
========================================================
*/
int __fastcall htoin(const char *p, int n)
{
	if( *p == 'x' ) return 0;
	int	d;

	for( d = 0; *p && n; p++, n-- ){
		d = d << 4;
		d += (*p & 0x0f);
		if( *p >= 'A' ) d += 9;
	}
	return(d);
}
//---------------------------------------------------------------------------
int __fastcall FontStyle2Code(TFontStyles style)
{
	int code = 0;

	TFontStyles fa;
	TFontStyles fb;

	fa << fsBold;
	fb << fsBold;
	fa *= style;
	if( fa == fb ) code |= FSBOLD;

	fa >> fsBold;
	fb >> fsBold;
	fa << fsItalic;
	fb << fsItalic;
	fa *= style;
	if( fa == fb ) code |= FSITALIC;

	fa >> fsItalic;
	fb >> fsItalic;
	fa << fsUnderline;
	fb << fsUnderline;
	fa *= style;
	if( fa == fb ) code |= FSUNDERLINE;

	fa >> fsUnderline;
	fb >> fsUnderline;
	fa << fsStrikeOut;
	fb << fsStrikeOut;
	fa *= style;
	if( fa == fb ) code |= FSSTRIKEOUT;

	return code;
}

TFontStyles __fastcall Code2FontStyle(int code)
{
	TFontStyles fs;

	if( code & FSBOLD ) fs << fsBold;
	if( code & FSITALIC ) fs << fsItalic;
	if( code & FSUNDERLINE ) fs << fsUnderline;
	if( code & FSSTRIKEOUT ) fs << fsStrikeOut;
	return fs;
}
//---------------------------------------------------------------------------
void __fastcall Font2FontData(FONTDATA *pData, TFont *pFont)
{
	pData->m_Name = pFont->Name;
    pData->m_Charset = pFont->Charset;
    pData->m_Height = pFont->Height;
	TFontStyles ts = pFont->Style;
	pData->m_Style = FontStyle2Code(ts);
}
//---------------------------------------------------------------------------
void __fastcall FontData2Font(TFont *pFont, FONTDATA *pData)
{
	pFont->Name = pData->m_Name;
	pFont->Charset = pData->m_Charset;
    pFont->Height = pData->m_Height;
	TFontStyles ts;
	ts = Code2FontStyle(pData->m_Style);
    pFont->Style = ts;
}
//---------------------------------------------------------------------------
void __fastcall LoadFontFromInifile(FONTDATA *pData, LPCSTR pSect, TMemIniFile *pIniFile)
{
	if( pData == NULL ) return;

	pData->m_Name = pIniFile->ReadString(pSect, "FontName", pData->m_Name);
	pData->m_Charset = (BYTE)pIniFile->ReadInteger(pSect, "FontSet", pData->m_Charset);
	pData->m_Height = pIniFile->ReadInteger(pSect, "FontSize", pData->m_Height);
    pData->m_Style = pIniFile->ReadInteger(pSect, "FontStyle", pData->m_Style);
}
//---------------------------------------------------------------------------
void __fastcall SaveFontToInifile(FONTDATA *pData, LPCSTR pSect, TMemIniFile *pIniFile)
{
	if( pData == NULL ) return;

	pIniFile->WriteString(pSect, "FontName", pData->m_Name);
	pIniFile->WriteInteger(pSect, "FontSet", pData->m_Charset);
	pIniFile->WriteInteger(pSect, "FontSize", pData->m_Height);
    pIniFile->WriteInteger(pSect, "FontStyle", pData->m_Style);
}
//---------------------------------------------------------------------------
void __fastcall LoadFontFromInifile(TFont *pFont, LPCSTR pSect, TMemIniFile *pIniFile)
{
	if( pFont == NULL ) return;

	pFont->Name = pIniFile->ReadString(pSect, "FontName", pFont->Name);
	pFont->Charset = (BYTE)pIniFile->ReadInteger(pSect, "FontSet", pFont->Charset);
	pFont->Height = pIniFile->ReadInteger(pSect, "FontSize", pFont->Height);
	TFontStyles ts = pFont->Style;
	DWORD d = FontStyle2Code(ts);
	d = pIniFile->ReadInteger(pSect, "FontStyle", d);
	ts = Code2FontStyle(d);
	pFont->Style = ts;
}
//---------------------------------------------------------------------------
void __fastcall SaveFontToInifile(TFont *pFont, LPCSTR pSect, TMemIniFile *pIniFile)
{
	if( pFont == NULL ) return;

	pIniFile->WriteString(pSect, "FontName", pFont->Name);
	pIniFile->WriteInteger(pSect, "FontSet", pFont->Charset);
	pIniFile->WriteInteger(pSect, "FontSize", pFont->Height);
	TFontStyles ts = pFont->Style;
	DWORD d = FontStyle2Code(ts);
	pIniFile->WriteInteger(pSect, "FontStyle", d);
}
//---------------------------------------------------------------------------
void __fastcall SetMBCP(BYTE charset)
{
	sys.m_Charset = charset;
	UINT cp;
	switch(charset){
		case SHIFTJIS_CHARSET:
			cp = 932;
			break;
		case HANGEUL_CHARSET:
			cp = 949;
			break;
        case JOHAB_CHARSET:
			cp = 1361;
        	break;
		case CHINESEBIG5_CHARSET:   //
			cp = 950;
			break;
		case 134:       // 簡略
			cp = 936;
			break;
        case ANSI_CHARSET:
        case SYMBOL_CHARSET:
        	cp = 1252;
            break;
		default:
			cp = _MB_CP_ANSI;
			break;
	}
	if( cp != _MB_CP_ANSI ){
		CPINFO info;
		if( GetCPInfo(cp, &info) != TRUE ){
			cp = _MB_CP_ANSI;
		}
	}
	_setmbcp(cp);
}

//---------------------------------------------------------------------------
BOOL __fastcall SetLangFont(TFont *pFont, WORD wLang)
{
	BOOL r = TRUE;
	switch(wLang){
		case 0x0411:	// JA
			if( pFont ){
				pFont->Name = sys.m_MsgEng ? "MS UI Gothic" : "ＭＳ Ｐゴシック";
				pFont->Charset = SHIFTJIS_CHARSET;
            }
    	   	break;
		case 0x0412:	// HL
			if( pFont ){
				pFont->Name = "DotumChe";
				pFont->Charset = HANGEUL_CHARSET;
				if( sys.m_fFontFam && (!sys.m_tFontFam[fmHL] && sys.m_tFontFam[fmJOHAB]) ){
					pFont->Charset = JOHAB_CHARSET;
				}
            }
            break;
        case 0x0404:	// BV
			if( pFont ){
				pFont->Name = "PMingLiU";
				pFont->Charset = CHINESEBIG5_CHARSET;
            }
            break;
        case 0x0804:	// BY
			if( pFont ){
				pFont->Name = "SimHei";
				pFont->Charset = 134;
            }
            break;
		default:
			if( pFont ){
				pFont->Name = "Arial";
				pFont->Charset = ANSI_CHARSET;
            }
            r = FALSE;
            break;
    }
    return r;
}
//---------------------------------------------------------------------------
void __fastcall GetLogFont(LOGFONT *pLogfont, TFont *pFont)
{
	memset(pLogfont, 0, sizeof(LOGFONT));
	pLogfont->lfHeight = pFont->Height;
	pLogfont->lfWidth = 0;
	pLogfont->lfEscapement = 0;
	pLogfont->lfOrientation = 0;
	TFontStyles ts = pFont->Style;
	int fsw = FontStyle2Code(ts);
	pLogfont->lfWeight = fsw & FSBOLD ? 700 : 400;
	pLogfont->lfItalic = BYTE(fsw & FSITALIC ? TRUE : FALSE);
    pLogfont->lfUnderline = BYTE(fsw & FSUNDERLINE ? TRUE : FALSE);
    pLogfont->lfStrikeOut = BYTE(fsw & FSSTRIKEOUT ? TRUE : FALSE);
	pLogfont->lfCharSet = pFont->Charset;
	pLogfont->lfOutPrecision = OUT_CHARACTER_PRECIS;
	pLogfont->lfClipPrecision = CLIP_DEFAULT_PRECIS;
	pLogfont->lfQuality = NONANTIALIASED_QUALITY;
	pLogfont->lfPitchAndFamily = DEFAULT_PITCH;
	strcpy(pLogfont->lfFaceName, AnsiString(pFont->Name).c_str());	//JA7UDE 0428
}
//---------------------------------------------------------------------------
void __fastcall AddStyle(AnsiString &as, BYTE charset, DWORD style)
{
    switch(charset){
		case ANSI_CHARSET:
			as += sys.m_MsgEng ? "/ANSI" : "/欧文";
        	break;
		case SHIFTJIS_CHARSET:
			as += sys.m_MsgEng ? "/Japanese" : "/日本語";
        	break;
		case HANGEUL_CHARSET:
			as += sys.m_MsgEng ? "/Korean(Hangul)" : "/ハングル";
			break;
		case JOHAB_CHARSET:
			as += sys.m_MsgEng ? "/Korean(Johab)" : "/ハングル(Johab)";
			break;
		case CHINESEBIG5_CHARSET:   // 台湾
			as += sys.m_MsgEng ? "/Chinese(BIG5)" : "/中国語(繁体)";
			break;
		case 134:                   // 中国語簡略
			as += sys.m_MsgEng ? "/Chinese(GB2312)" : "/中国語(簡体)";
			break;
        case SYMBOL_CHARSET:
			as += sys.m_MsgEng ? "/Symbol" : "/シンボル";
        	break;
        default:
        	break;
    }
	if( style & FSBOLD ) as += sys.m_MsgEng ? "/Bold" : "/太字";
	if( style & FSITALIC ) as += sys.m_MsgEng ? "/Italic" : "/斜体";
	if( style & FSUNDERLINE ) as += sys.m_MsgEng ? "/UnderLine" : "/下線";
	if( style & FSSTRIKEOUT ) as += sys.m_MsgEng ? "/StrikeOut" : "/取消し線";
}
//---------------------------------------------------------------------------
//static int CALLBACK EnumFontFamExProc( ENUMLOGFONT FAR* lpelf, NEWTEXTMETRIC FAR* lpntm, DWORD FontType, LPARAM lParam)	//ja7ude 0522
static int CALLBACK EnumFontFamExProc( const tagLOGFONTA FAR* lpelf, const tagTEXTMETRICA FAR* lpntm, DWORD FontType, LPARAM lParam)
{
	OnWave();
	//switch(lpelf->elfLogFont.lfCharSet){
	switch(lpelf->lfCharSet){
		case SHIFTJIS_CHARSET:
			sys.m_tFontFam[fmJA] = TRUE;
        	break;
		case HANGEUL_CHARSET:
			sys.m_tFontFam[fmHL] = TRUE;
            break;
		case JOHAB_CHARSET:
			sys.m_tFontFam[fmJOHAB] = TRUE;
        	break;
		case CHINESEBIG5_CHARSET:   // 台湾
        	sys.m_tFontFam[fmBV] = TRUE;
            break;
		case 134:                   // 中国語簡略
			sys.m_tFontFam[fmBY] = TRUE;
            break;
        default:
        	break;
    }
	int r = FALSE;
    for( int i = 0; i < fmEND; i++ ){
		if( !sys.m_tFontFam[i] ) r = TRUE;
    }
	return r;
}
//---------------------------------------------------------------------------
void __fastcall CheckFontCharset(void)
{
	if( sys.m_fFontFam ) return;

	sys.m_fFontFam = TRUE;
	memset(sys.m_tFontFam, 0, sizeof(sys.m_tFontFam));
    LOGFONT logfont;
    memset(&logfont, 0, sizeof(logfont));
    logfont.lfCharSet = DEFAULT_CHARSET;
	HDC hDC = ::CreateDC("DISPLAY", NULL, NULL, NULL);
	//::EnumFontFamiliesEx(hDC, &logfont, (int (_stdcall *)())EnumFontFamExProc, 0, 0);	//ja7ude 0522
	::EnumFontFamiliesEx(hDC, &logfont, EnumFontFamExProc, 0, 0);
	::DeleteDC(hDC);
}
//---------------------------------------------------------------------------
UCOL __fastcall GetGrade2(UCOL s[2], int x, int xw)
{
	if( x < 0 ) x = 0;
	if( x > xw ) x = xw;
	UCOL    r;
	int c = s[0].b.b + ((int(s[1].b.b) - int(s[0].b.b)) * x / xw);
	if( c < 0 ) c = 0;
	if( c > 255 ) c = 255;
	r.b.b = BYTE(c);
	c = s[0].b.g + ((int(s[1].b.g) - int(s[0].b.g)) * x / xw);
	if( c < 0 ) c = 0;
	if( c > 255 ) c = 255;
	r.b.g = BYTE(c);
	c = s[0].b.r + ((int(s[1].b.r) - int(s[0].b.r)) * x / xw);
	if( c < 0 ) c = 0;
	if( c > 255 ) c = 255;
	r.b.r = BYTE(c);
	return r;
}
//---------------------------------------------------------------------------
int __fastcall GetActiveIndex(TPageControl *pp)
{
	int i;
	for( i = 0; i < pp->PageCount; i++ ){
		if( pp->ActivePage == pp->Pages[i] ) return i;
	}
	return 0;
}
//---------------------------------------------------------------------------
void __fastcall SetActiveIndex(TPageControl *pp, int page)
{
	if( (page >= 0) && (page < pp->PageCount) ){
		pp->ActivePage = pp->Pages[page];
	}
}
//---------------------------------------------------------------------------
int __fastcall InvMenu(TMenuItem *pItem)
{
	pItem->Checked = pItem->Checked ? FALSE : TRUE;
	return pItem->Checked;
}
///----------------------------------------------------------------
HWND __fastcall GetMBHandle(int &flag)
{
	HWND hd;
	if( Screen->ActiveForm != NULL ){
		hd = Screen->ActiveForm->Handle;
	}
	else {
		hd = NULL;
	}
	flag = MB_SETFOREGROUND;
	flag |= MB_TOPMOST;
	flag |= (hd == NULL) ? MB_TASKMODAL : MB_APPLMODAL;
	return hd;
}
static int MsgBoxFlag = 0;
///----------------------------------------------------------------
///  メッセージの表示
///
void InfoMB(LPCSTR fmt, ...)
{
	if( MsgBoxFlag ) return;
	MsgBoxFlag++;
	va_list	pp;
	char	bf[1024];

	va_start(pp, fmt);
	vsprintf( bf, fmt, pp );
	va_end(pp);

	int	flag;
	HWND hd = GetMBHandle(flag);
	flag |= (MB_OK | MB_ICONINFORMATION);
//	Application->NormalizeTopMosts();
	::MessageBox(hd, bf, "MMVARI", flag);
//	Application->RestoreTopMosts();
	MsgBoxFlag--;
}
///----------------------------------------------------------------
///  エラーメッセージの表示
///
void ErrorMB(LPCSTR fmt, ...)
{
	if( MsgBoxFlag ) return;
	MsgBoxFlag++;
	va_list	pp;
	char	bf[1024];

	va_start(pp, fmt);
	vsprintf( bf, fmt, pp );
	va_end(pp);

	int	flag;
	HWND hd = GetMBHandle(flag);
	flag |= (MB_OK | MB_ICONEXCLAMATION);
//	Application->NormalizeTopMosts();
	::MessageBox(hd, bf, sys.m_MsgEng ? "Error":"ｴﾗｰ", flag);
//	Application->RestoreTopMosts();
	MsgBoxFlag--;
}

///----------------------------------------------------------------
///  警告メッセージの表示
///
void WarningMB(LPCSTR fmt, ...)
{
	if( MsgBoxFlag ) return;
	MsgBoxFlag++;
	va_list	pp;
	char	bf[1024];

	va_start(pp, fmt);
	vsprintf( bf, fmt, pp );
	va_end(pp);

	int	flag;
	HWND hd = GetMBHandle(flag);
	flag |= (MB_OK | MB_ICONEXCLAMATION);
//	Application->NormalizeTopMosts();
	::MessageBox(hd, bf, sys.m_MsgEng?"Warning":"警告", flag);
//	Application->RestoreTopMosts();
	MsgBoxFlag--;
}

///----------------------------------------------------------------
///  実行選択メッセージの表示
///
int YesNoMB(LPCSTR fmt, ...)
{
	if( MsgBoxFlag ) return IDNO;
	MsgBoxFlag++;
	va_list	pp;
	char	bf[1024];

	va_start(pp, fmt);
	vsprintf( bf, fmt, pp );
	va_end(pp);

	int	flag;
	HWND hd = GetMBHandle(flag);
//	if( Screen->ActiveForm != NULL ) NormalWindow(Screen->ActiveForm);
	flag |= (MB_YESNO | MB_ICONQUESTION);
//	Application->NormalizeTopMosts();
	int r = ::MessageBox(hd, bf, "MMVARI", flag);
//	Application->RestoreTopMosts();
	MsgBoxFlag--;
	return r;
}

///----------------------------------------------------------------
///  実行選択メッセージの表示
///
int YesNoCancelMB(LPCSTR fmt, ...)
{
	if( MsgBoxFlag ) return IDCANCEL;
	MsgBoxFlag++;
	va_list	pp;
	char	bf[1024];

	va_start(pp, fmt);
	vsprintf( bf, fmt, pp );
	va_end(pp);

	int	flag;
	HWND hd = GetMBHandle(flag);
	flag |= (MB_YESNOCANCEL | MB_ICONQUESTION);
//	Application->NormalizeTopMosts();
	int r = ::MessageBox(hd, bf, "MMVARI", flag);
//	Application->RestoreTopMosts();
	MsgBoxFlag--;
	return r;
}
///----------------------------------------------------------------
///  実行選択メッセージの表示
///
int OkCancelMB(LPCSTR fmt, ...)
{
	if( MsgBoxFlag ) return IDCANCEL;
	MsgBoxFlag++;
	va_list	pp;
	char	bf[1024];

	va_start(pp, fmt);
	vsprintf( bf, fmt, pp );
	va_end(pp);

	int	flag;
	HWND hd = GetMBHandle(flag);
	flag |= (MB_OKCANCEL | MB_ICONQUESTION);
//	Application->NormalizeTopMosts();
	int r = ::MessageBox(hd, bf, "MMVARI", flag);
//	Application->RestoreTopMosts();
	MsgBoxFlag--;
	return r;
}
///----------------------------------------------------------------
///
void __fastcall ErrorFWrite(LPCSTR pName)
{
	if( GetFileAttributes(pName) & FILE_ATTRIBUTE_READONLY ){
		ErrorMB(sys.m_MsgEng ? "'%s' is read-only (fail to update)." : "'%s'が 読み取り専用 に設定されているため更新に失敗しました.", pName);
	}
	else {
		ErrorMB( "Could not update '%s'", pName );
	}
}
///----------------------------------------------------------------
///
int __fastcall RemoveL2(LPSTR t, LPSTR ss, LPCSTR pKey, int size)
{
	char	c;
	int		k;
	LPCSTR	pp;
	LPSTR	s;
	const char	_tt1[]="[{(｢<";
	const char	_tt2[]="]})｣>";

	int		len = strlen(pKey);
	char ac = ';';
	for( k = 0, s = ss; *s; s++ ){
		if( k ){															// 漢字２バイト目
			k = 0;
			ac = 0x1e;
		}
		else if(  _mbsbtype((const unsigned char *)s, 0) == _MBC_LEAD ){	// 漢字１バイト目
			k = 1;
			if( (len >= 2) && (strchr(" ,./;:*\t[{(｢<]})｣>", ac)!=NULL) && (!strnicmp(s, pKey, len)) ){
				pp = s + len;
				if( (pp = strchr(_tt1, *pp))!=NULL ){
					c = _tt2[pp - _tt1];
					if( *(s+len+1) ){
						StrDlmCpy(t, s+len+1, c, size);
						if( *t ){
							strcpy((ac == ' ')?(s-1):s, s+len+2+strlen(t));
							return TRUE;
						}
					}
				}
			}
		}
		else if( (strchr(" ,./;:*\t\x1e[{(｢<]})｣>", ac)!=NULL) && (!strnicmp(s, pKey, len)) ){
			pp = s + len;
			if( (pp = strchr(_tt1, *pp))!=NULL ){
				c = _tt2[pp - _tt1];
				if( *(s+len+1) ){
					StrDlmCpy(t, s+len+1, c, size);
					if( *t ){
						strcpy((ac == ' ')?(s-1):s, s+len+2+strlen(t));
						return TRUE;
					}
				}
			}
			ac = *s;
		}
		else {
			ac = *s;
		}
	}
	return FALSE;
}

void __fastcall AddL2(LPSTR t, LPCSTR pKey, LPCSTR s, UCHAR c1, UCHAR c2, int size)
{
	if( *s ){
		int len = strlen(t);
		int lenkey = strlen(pKey);
		int lenData = strlen(s);
		if( (len + lenkey + lenData + 2) < size ){
			t += len;
			if( len ) *t++ = ' ';
			if( *pKey ){
				strcpy(t, pKey);
				t += lenkey;
			}
			*t++ = c1;
			strcpy(t, s);
			t += lenData;
			*t++ = c2;
			*t = 0;
		}
	}
}
///----------------------------------------------------------------
void __fastcall NumCopy(LPSTR t, LPCSTR p)
{
	p = SkipSpace(p);
	for( ; *p; p++ ){
		if( isdigit(*p) ){
			*t++ = *p;
		}
		else {
			break;
		}
	}
	*t = 0;
}
///----------------------------------------------------------------
///  数字が含まれるかどうか調べる
///
int __fastcall IsNumbs(LPCSTR p)
{
	for( ; *p; p++ ){
		if( isdigit(*p) ) return 1;
	}
	return 0;
}
///----------------------------------------------------------------
///  数字が含まれるかどうか調べる
///
int __fastcall IsNumbAll(LPCSTR p)
{
	for( ; *p; p++ ){
		if( !isdigit(*p) ) return 0;
	}
	return 1;
}
///----------------------------------------------------------------
///  数字が含まれるかどうか調べる
///
int __fastcall IsAlphas(LPCSTR p)
{
	for( ; *p; p++ ){
		if( isalpha(*p) ) return 1;
	}
	return 0;
}
///----------------------------------------------------------------
int __fastcall IsAlphaAll(LPCSTR p)
{
	for( ; *p; p++ ){
		if( !isalpha(*p) ) return FALSE;
	}
	return TRUE;
}
///----------------------------------------------------------------
///  RSTかどうか調べる
///
int __fastcall IsRST(LPCSTR p)
{
	if( strlen(p) < 3 ) return 0;
	if( (*p < '1') || (*p > '5') ) return 0;
	p++;
	if( (*p < '1') || (*p > '9') ) return 0;
	p++;
	if( (*p < '1') || (*p > '9') ) return 0;
	return 1;
}
///----------------------------------------------------------------
int __fastcall IsCallChar(char c)
{
	if( !isalpha(c) && !isdigit(c) && (c != '/') ) return 0;
	if( islower(c) ) return 0;
	return 1;
}
///----------------------------------------------------------------
///  コールサインかどうか調べる
///
int __fastcall IsCall(LPCSTR p)
{
	int l = strlen(p);
	if( l > 16 ) return 0;
	if( l < 3 ) return 0;
	if( isdigit(*p) ){					// 先頭が数字
		if( l <= 3 ) return 0;				// 3文字以下の時NG
		if( isdigit(*(p+1)) ) return 0;		// 2文字目が数字の時NG
	}
	if( isdigit(LastC(p)) ){			// 最後が数字
		if( l <= 4 ) return 0;				// ４文字以下の時NG
//		if( !strchr(p, '/')==NULL ) return 0;	// /が含まれていない時NG
//		if( p[l-2] != '/' ) return 0;		// 最後の１つ前が/以外の時NG
	}
	int i;
	for( i = 0; i < l-1; i++, p++ ){
		if( isdigit(*p) ) return 1;		// 数字が含まれていればOK
	}
	return 0;
}
/*#$%
======================================================
	ＪＡとＤＸの区別をする
------------------------------------------------------
	s : コールサイン文字列のポインタ
------------------------------------------------------
	1 : JA
	0 : DX (JD1 INCLUDEED)
------------------------------------------------------
======================================================
*/
int __fastcall IsJA(const char *s)
{
	if( (!memcmp(s, "8J1R", 4) && (strlen(s) == 5))||(strstr(s, "/JD1")!=NULL) ){
		return(0);
	}
	else if( strchr(s, '/') ){
		char	bf[MLCALL+1];
		StrCopy(bf, s, MLCALL);
		char	*p;
		char	*t;
		for( p = bf; *p; ){
			p = StrDlm(t, p, '/');
			if( *t ){
				if( (strlen(t) >= 2) && isdigit((unsigned char)LastC(t)) ){
					if( *t == 'J' ){
						t++;
						if( *t == 'D' ) return(0);
						if( (*t >= 'A')&&(*t <= 'S' ) ) return(1);
					}
					else if( *t == '7' ){
						t++;
						if( (*t >= 'J')&&(*t <= 'N' ) ) return(1);
					}
					else if( *t == '8' ){
						t++;
						if( (*t >= 'J')&&(*t <= 'N' ) ) return(1);
					}
					else {
						return 0;
					}
				}
			}
		}
	}
	if( *s == 'J' ){
		s++;
		if( *s == 'D' ) return(0);
		if( (*s >= 'A')&&(*s <= 'S' ) ) return(1);
	}
	else if( *s == '7' ){
		s++;
		if( (*s >= 'J')&&(*s <= 'N' ) ) return(1);
	}
	else if( *s == '8' ){
		s++;
		if( (*s >= 'J')&&(*s <= 'N' ) ) return(1);
	}
	return(0);
}
/*#$%
======================================================
	コールサインをクリップする
------------------------------------------------------
	s : コールサイン文字列のポインタ
------------------------------------------------------
	クリップコールのポインタ
------------------------------------------------------
======================================================
*/
LPCSTR __fastcall ClipCall(LPCSTR s)
{
	static	char	bf[MLCALL+1];
	LPCSTR	p1, p2;

	if( (p1=strchr(s, '/'))!=NULL ){
		if( (p2=strchr(p1+1, '/'))!=NULL ){	/* ３分割	*/
			if( (int(strlen(p2+1)) < int((p2 - p1)+1)) || (!IsCall(p2+1)) ){		/* 最後より途中が長い	*/
				if( ((p2-p1) < (p1-s))||(!IsCall(p1+1)) ){	/* 途中より最初が長い	*/
					StrCopy(bf, s, MLCALL);
					*strchr(bf, '/') = 0;
					return(bf);
				}
				else {
					strcpy(bf, p1+1);
					*strchr(bf, '/') = 0;
					return(bf);
				}
			}
			else if( int(strlen(p2+1)) < int((p1 - s)+1) ){	/* 最後より最初が長い	*/
				StrCopy(bf, s, MLCALL);
				*strchr(bf, '/') = 0;
				return(bf);
			}
			else {
				return(p2+1);
			}
		}
		else {								/* ２分割	*/
			if( (int(strlen(p1+1)) < int((p1 - s)+1)) || (!IsCall(p1+1)) ){
				StrCopy(bf, s, MLCALL);
				*strchr(bf, '/') = 0;
				return(bf);
			}
			else {
				return(p1+1);
			}
		}
	}
	else {
		return(s);
	}
}

/*#$%
======================================================
	ポータブル表記を調べる
------------------------------------------------------
	p : コールサイン文字列のポインタ
------------------------------------------------------
	数値のポインタ
------------------------------------------------------
======================================================
*/
static LPSTR __fastcall chkptb(LPSTR p)
{
	if( *p ){
		p = lastp(p);
		if( isdigit(*p) && ((*(p-1))=='/') ){
			return(p);
		}
	}
	return(NULL);
}

/*#$%
======================================================
	ポータブル表記の入れ替え
------------------------------------------------------
	s : コールサイン文字列のポインタ
------------------------------------------------------
------------------------------------------------------
	元の文字列を破壊する
======================================================
*/
void __fastcall chgptb(LPSTR s)
{
	LPSTR	p, t;

	if( (p = chkptb(s))!=NULL ){
		t = p;
		for( p--; p > s; p-- ){
			if( isdigit(*p) ){
				*p = *t;
				t--;
				*t = 0;
				break;
			}
		}
	}
}

/*#$%
======================================================
	コールサインからカントリの元をクリップする
------------------------------------------------------
	s : コールサイン文字列のポインタ
------------------------------------------------------
	カントリのポインタ
------------------------------------------------------
======================================================
*/
LPCSTR __fastcall ClipCC(LPCSTR s)
{
	static	char	bf[MLCALL+1];
	LPSTR	p, t;

	StrCopy(bf, s, MLCALL);
	chgptb(bf);			/* ポータブル表記の入れ替え	*/
	for( p = bf; *p; ){
		if( *p ){
			p = StrDlm(t, p, '/');
			if( (strlen(t) > 1) && (isdigit(*t) || isdigit(LastC(t))) ) return(t);
		}
	}
	for( p = bf; *p; ){
		if( *p ){
			p = StrDlm(t, p, '/');
			if( (strlen(t) > 1) && (*t!='Q') && strcmp(t, "MM") ) return(t);
		}
	}
	return ClipCall(s);
}
//---------------------------------------------------------------------------
// コンボＢＯＸに文字列を設定する
void __fastcall SetComboBox(TComboBox *pCombo, LPCSTR pList)
{
	pCombo->Items->Clear();
	LPSTR s = strdup(pList);
	LPSTR p = s;
	LPSTR t;
	while(*p){
		p = StrDlm(t, p);
		clipsp(t);
		t = SkipSpace(t);
		if( *t ) pCombo->Items->Add(t);
	}
	free(s);
    pCombo->DropDownCount = pCombo->Items->Count;
}
//---------------------------------------------------------------------------
void __fastcall GetComboBox(AnsiString &as, TComboBox *pCombo)
{
	as = "";
	int i;
	for( i = 0; i < pCombo->Items->Count; i++ ){
		if( i ) as += ',';
		as += pCombo->Items->Strings[i];
	}
}
///----------------------------------------------------------------
///  文字列変換
///
void __fastcall Yen2CrLf(AnsiString &ws, AnsiString &cs)
{
	ws = "";
	LPCSTR	p;
	int		f;
	int		dlm = 0;

	p = cs.c_str();
	if( *p == 0x22 ){
		p++;
		dlm++;
	}
	for( f = 0; *p; p++ ){
		if( f ){
			f = 0;
			ws += *p;
		}
		else if(  _mbsbtype((const unsigned char *)p, 0) == _MBC_LEAD ){
			f = 1;
			ws += *p;
		}
		else if( *p == '\\' ){
			switch(*(p+1)){
			case 'r':
				ws += "\r";
				p++;
				break;
			case 'n':
				ws += "\n";
				p++;
				break;
			case 't':
				ws += "\t";
				p++;
				break;
			case '\\':
				ws += "\\";
				p++;
				break;
			default:
				p++;
				ws += *p;
				break;
			}
		}
		else if( !dlm || (*p!=0x22) || *(p+1) ){
			ws += *p;
		}
	}
}

void __fastcall CrLf2Yen(AnsiString &ws, AnsiString &cs)
{
	ws = "\x22";
	LPCSTR	p;
	int f = 0;

	for( p = cs.c_str(); *p; p++ ){
		if( f ){
			f = 0;
			ws += *p;
		}
		else if(  _mbsbtype((const unsigned char *)p, 0) == _MBC_LEAD ){
			f = 1;
			ws += *p;
		}
		else if( *p == 0x0d ){
			ws += "\\r";
		}
		else if( *p == 0x0a ){
			ws += "\\n";
		}
		else if( *p == TAB ){
			ws += "\\t";
		}
		else if( *p == '\\' ){
			ws += "\\\\";
		}
		else {
			ws += *p;
		}
	}
	ws += "\x22";
}


///---------------------------------------------------------
///  テキスト文字列ストリーマー
int __fastcall CTextString::LoadText(LPSTR tp, int len)
{
	char c;
	int n = 0;
	if( !(*rp) ) return FALSE;
	while(*rp){
		c = *rp++;
		if( c == LF ){
			*tp = 0;
			return TRUE;
		}
		else if( (c != CR)&&(c != 0x1a) ){
			if( n < (len-1) ){
				*tp++ = c;
				n++;
			}
		}
	}
	*tp = 0;
	return TRUE;
}
///------------------------------------------------------
///
///CWebRef クラス
///
void __fastcall MakeCommand(LPSTR t, LPCSTR s, LPCSTR p)
{
	for( ;*s; s++ ){
		if( *s == '%' ){
			s++;
			if( *s == '%' ){
				*t++ = '%';
			}
			else if( (p != NULL) && (*s == '1') ){
				strcpy(t, p);
				t += strlen(t);
				p = NULL;
			}
		}
		else {
			*t++ = *s;
		}
	}
	*t = 0;
	if( p != NULL ){
		*t++ = ' ';
		strcpy(t, p);
	}
}

__fastcall CWebRef::CWebRef()
{
	HTML = "";

	HKEY hkey=NULL;	/* オープン キーのハンドル	*/

	char    bf[512], name[512];
	ULONG   namelen, len;
	if( !RegOpenKeyEx(HKEY_CLASSES_ROOT, "http", 0, KEY_READ, &hkey) ){
		if( !RegOpenKeyEx(hkey, "shell", 0, KEY_READ, &hkey) ){
			if( !RegOpenKeyEx(hkey, "open", 0, KEY_READ, &hkey) ){
				if( !RegOpenKeyEx(hkey, "command", 0, KEY_READ, &hkey) ){
					namelen = sizeof(name);
					len = sizeof(bf);
					if( !::RegEnumValue(hkey, 0, name, &namelen, NULL,
					NULL, (LPBYTE)bf, &len) ){
						HTML = bf;
					}
				}
			}
		}
	}
	::RegCloseKey(hkey);
}

void __fastcall CWebRef::ShowHTML(LPCSTR url)
{
	char    cmd[1024];
	MakeCommand(cmd, HTML.c_str(), url);
	::WinExec(cmd, SW_SHOWDEFAULT);
}


///------------------------------------------------------
///
///CWaitCursor クラス
///
__fastcall CWaitCursor::CWaitCursor()
{
	sv = Screen->Cursor;
	Screen->Cursor = crHourGlass;
}

__fastcall CWaitCursor::~CWaitCursor()
{
	Screen->Cursor = sv;
}

void __fastcall CWaitCursor::Delete(void)
{
	Screen->Cursor = sv;
}

void __fastcall CWaitCursor::Wait(void)
{
	Screen->Cursor = crHourGlass;
}

///----------------------------------------------------------------
///  CAlignクラス
void __fastcall CAlign::InitControl(TControl *p, TControl *pB, TFont *pF /*= NULL*/)
{
	tp = p;
	OTop = p->Top;
	OLeft = p->Left;
	OWidth = p->Width;
	OHeight = p->Height;
	if( pF != NULL ){
		fp = pF;
		OFontHeight = pF->Height;
	}

	BTop = pB->Top;
	BLeft = pB->Left;
	BWidth = pB->ClientWidth;
	BHeight = pB->ClientHeight;
}

void __fastcall CAlign::InitControl(TControl *p, RECT *rp, TFont *pF /*= NULL*/)
{
	tp = p;
	OTop = p->Top;
	OLeft = p->Left;
	OWidth = p->Width;
	OHeight = p->Height;
	if( pF != NULL ){
		fp = pF;
		OFontHeight = pF->Height;
	}

	BTop = rp->top;
	BLeft = rp->left;
	BWidth = rp->right - rp->left + 1;
	BHeight = rp->bottom - rp->top + 1;
}

void __fastcall CAlign::NewAlign(TControl *pB)
{
	double Sx = double(pB->ClientWidth)/double(BWidth);
	double Sy = double(pB->ClientHeight)/double(BHeight);
	tp->SetBounds(int(OLeft * Sx), int(OTop * Sy), int(OWidth * Sx), int(OHeight * Sy));
	if( fp != NULL ){
		if( Sx > Sy ) Sx = Sy;
		fp->Height = int(OFontHeight * Sx);
		m_FontAdj = Sx;
	}
	if( tp->ClassNameIs("TComboBox") ){
		((TComboBox *)tp)->SelLength = 0;
	}
	else if( tp->ClassNameIs("TLabel") ){
		TLabel *lp = ((TLabel *)tp);
		if( lp->Alignment == taRightJustify ){
			lp->AutoSize = FALSE;
			lp->AutoSize = TRUE;
		}
	}
}

void __fastcall CAlign::NewAlign(TControl *pB, double hx)
{
	double Sx = double(pB->ClientWidth)/double(BWidth);
	double Sy = double(pB->ClientHeight)/double(BHeight);
	Sy *= hx;
	tp->SetBounds(int(OLeft * Sx), int(OTop * Sy), int(OWidth * Sx), int(OHeight * Sy));
	if( fp != NULL ){
		if( Sx > Sy ) Sx = Sy;
		fp->Height = int(OFontHeight * Sx);
		m_FontAdj = Sx;
	}
	if( tp->ClassNameIs("TComboBox") ){
		((TComboBox *)tp)->SelLength = 0;
	}
}

void __fastcall CAlign::NewFont(AnsiString &FontName, BYTE Charset, TFontStyles fs)
{
	if( fp != NULL ){
		fp->Name = FontName;
		fp->Charset = Charset;
		fp->Style = fs;
		if( tp->ClassNameIs("TComboBox") ){
			((TComboBox *)tp)->SelLength = 0;
		}
	}
}

void __fastcall CAlign::NewFixAlign(TControl *pB, int XR)
{
	double Sx = double(pB->ClientWidth - XR) / double(BWidth - XR);

	tp->SetBounds(XR + (OLeft - XR) * Sx, OTop, OWidth * Sx, OHeight);
	if( fp != NULL ){
		fp->Height = int(OFontHeight * (Sx < 1.0 ? Sx : 1.0));
		m_FontAdj = Sx;
	}
	if( tp->ClassNameIs("TComboBox") ){
		((TComboBox *)tp)->SelLength = 0;
	}
}

void __fastcall CAlign::Resume(void)
{
	if( tp != NULL ){
		tp->Top = OTop;
		tp->Left = OLeft;
		tp->Width = OWidth;
		tp->Height = OHeight;
		if( fp != NULL ){
			fp->Height = OFontHeight;
		}
	}
}
///----------------------------------------------------------------
///  CAlignListクラス
__fastcall CAlignList::CAlignList(void)
{
	Max = 0;
	Cnt = 0;
	AlignList = NULL;
}

__fastcall CAlignList::~CAlignList(){
	if( AlignList != NULL ){
		for( int i = 0; i < Cnt; i++ ){
			delete AlignList[i];
		}
		delete AlignList;
		AlignList = NULL;
	}
}

void __fastcall CAlignList::Alloc(void)
{
	if( Cnt >= Max ){
		Max = Max ? Max * 2 : 16;
		CAlign **np = (CAlign **)new BYTE[sizeof(CAlign*) * Max];
		if( AlignList != NULL ){
			for( int i = 0; i < Cnt; i++ ){
				np[i] = AlignList[i];
			}
			delete AlignList;
		}
		AlignList = np;
	}
	AlignList[Cnt] = new CAlign;
}
void __fastcall CAlignList::EntryControl(TControl *tp, TControl *pB, TFont *pF /*= NULL*/)
{
	Alloc();
	AlignList[Cnt]->InitControl(tp, pB, pF);
	Cnt++;
}

void __fastcall CAlignList::EntryControl(TControl *tp, RECT *rp, TFont *pF /*= NULL*/){
	Alloc();
	AlignList[Cnt]->InitControl(tp, rp, pF);
	Cnt++;
}

void __fastcall CAlignList::EntryControl(TControl *tp, int XW, int YW, TFont *pF /*= NULL*/){
	RECT rc;
	rc.left = rc.top = 0;
	rc.right = XW; rc.bottom = YW;
	Alloc();
	AlignList[Cnt]->InitControl(tp, &rc, pF);
	Cnt++;
}

void __fastcall CAlignList::NewAlign(TControl *pB)
{
	if( AlignList == NULL ) return;
	for( int i = 0; i < Cnt; i++ ){
		AlignList[i]->NewAlign(pB);
	}
}

double __fastcall CAlignList::GetFontAdj(TControl *pB)
{
	for( int i = 0; i < Cnt; i++ ){
		if( AlignList[i]->GetControl() == pB ){
			return AlignList[i]->GetFontAdj();
		}
	}
	return 1.0;
}

void __fastcall CAlignList::NewAlign(TControl *pB, TControl *pS, double hx)
{
	for( int i = 0; i < Cnt; i++ ){
		if( AlignList[i]->GetControl() == pS ){
			AlignList[i]->NewAlign(pB, hx);
			break;
		}
	}
}
void __fastcall CAlignList::NewFont(AnsiString &FontName, BYTE Charset, TFontStyles fs)
{
	if( AlignList == NULL ) return;
	for( int i = 0; i < Cnt; i++ ){
		AlignList[i]->NewFont(FontName, Charset, fs);
	}
}

void __fastcall CAlignList::NewFixAlign(TControl *pB, int XR)
{
	if( AlignList == NULL ) return;
	for( int i = 0; i < Cnt; i++ ){
		AlignList[i]->NewFixAlign(pB, XR);
	}
}

void __fastcall CAlignList::Resume(TControl *pB)
{
	if( AlignList == NULL ) return;
	for( int i = 0; i < Cnt; i++ ){
		if( pB == NULL ){
			AlignList[i]->Resume();
		}
		else if( pB == AlignList[i]->GetControl() ){
			AlignList[i]->Resume();
			break;
		}
	}
}

//---------------------------------------------------------------------------
void __fastcall SetGroupEnabled(TGroupBox *gp)
{
	int enb = gp->Enabled;
	TControl *tp;
	int i;
	for( i = 0; i < gp->ControlCount; i++ ){
		tp = gp->Controls[i];
		if( tp != NULL ){
			tp->Enabled = enb;
			if( tp->ClassNameIs("TGroupBox") ){
				SetGroupEnabled((TGroupBox *)tp);
			}
		}
	}
	gp->Font->Color = gp->Enabled ? clBlack : clGrayText;
	for( i = 0; i < gp->ControlCount; i++ ){
		tp = gp->Controls[i];
		if( tp != NULL ){
			if( tp->ClassNameIs("TComboBox") ){
				((TComboBox *)tp)->SelLength = 0;
			}
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall KeyEvent(const short *p)
{
	for( ; *p; p++ ){
		keybd_event(BYTE(*p), 0, *p&0x8000 ? KEYEVENTF_KEYUP : 0, 0);
	}
}
//---------------------------------------------------------------------------
// CMULTクラス
__fastcall CMULT::CMULT()
{
	m_pBase = NULL;
	m_CNT = m_MAX = 0;
}
//---------------------------------------------------------------------------
void __fastcall CMULT::Clear(void)
{
	for( int i = 0; i < m_CNT; i++ ){
		delete m_pBase[i].pStr;
	}
	delete m_pBase;
	m_pBase = NULL;
	m_CNT = m_MAX = 0;
}
//---------------------------------------------------------------------------
void __fastcall CMULT::Alloc(void)
{
	int max = m_MAX ? (m_MAX * 2) : 256;
	MULTSET *pBase = new MULTSET[max];
	if( m_pBase != NULL ){
		memcpy(pBase, m_pBase, sizeof(MULTSET) * m_CNT);
		delete m_pBase;
	}
	m_pBase = pBase;
	m_MAX = max;
}
//---------------------------------------------------------------------------
int __fastcall CMULT::Add(LPCSTR pKey)
{
	for( int i = 0; i < m_CNT; i++ ){
		if( !strcmp(m_pBase[i].pStr, pKey) ){
			m_pBase[i].Count++;
			return 0;
		}
	}
	if( m_CNT >= m_MAX ) Alloc();
	m_pBase[m_CNT].pStr = StrDupe(pKey);
	m_pBase[m_CNT].Count = 1;
	m_CNT++;
	return 1;
}
//---------------------------------------------------------------------------
int __fastcall CMULT::Set(LPCSTR pKey, int n)
{
	for( int i = 0; i < m_CNT; i++ ){
		if( !strcmp(m_pBase[i].pStr, pKey) ){
			m_pBase[i].Count = n;
			return 0;
		}
	}
	if( m_CNT >= m_MAX ) Alloc();
	m_pBase[m_CNT].pStr = StrDupe(pKey);
	m_pBase[m_CNT].Count = n;
	m_CNT++;
	return 1;
}
//---------------------------------------------------------------------------
static int CMULTcmpCall(const void *s, const void *t)
{
	MULTSET *sp = (MULTSET *)s;
	MULTSET *tp = (MULTSET *)t;
	return strcmp(sp->pStr, tp->pStr);
}
static int CMULTcmpCount(const void *s, const void *t)
{
	MULTSET *sp = (MULTSET *)s;
	MULTSET *tp = (MULTSET *)t;
	if( sp->Count == tp->Count ){
		return strcmp(sp->pStr, tp->pStr);
	}
	else {
		return tp->Count - sp->Count;
	}
}
void __fastcall CMULT::Sort(void)
{
	if( m_CNT < 2 ) return;
	qsort(m_pBase, m_CNT, sizeof(MULTSET), CMULTcmpCall);
}
void __fastcall CMULT::SortCount(void)
{
	if( m_CNT < 2 ) return;
	qsort(m_pBase, m_CNT, sizeof(MULTSET), CMULTcmpCount);
}
//---------------------------------------------------------------------------
int __fastcall CMULT::GetCount(LPCSTR pKey)
{
	for( int i = 0; i < m_CNT; i++ ){
		if( !strcmp(m_pBase[i].pStr, pKey) ) return m_pBase[i].Count;
	}
	return 0;
}
//---------------------------------------------------------------------------
int __fastcall CMULT::GetTotal(void)
{
	int sum = 0;
	for( int i = 0; i < m_CNT; i++ ){
		sum += m_pBase[i].Count;
	}
	return sum;
}
//---------------------------------------------------------------------------
BOOL __fastcall StrWindowsVer(LPSTR t)
{
	*t = 0;
	OSVERSIONINFO osvi;

	memset(&osvi, 0, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	if( !GetVersionEx(&osvi) ) return FALSE;

	switch (osvi.dwPlatformId){
		case VER_PLATFORM_WIN32_NT:		// NT, 2000, XP
			if( osvi.dwMajorVersion <= 4 ){
    	        strcat(t, "NT");
			}
	        else if( osvi.dwMajorVersion == 5 ){
			    if( osvi.dwMinorVersion == 0 ){
					strcat(t, "2000");
				}
				else if( osvi.dwMinorVersion == 1 ){
					strcat(t, "XP");
				}
			}
			break;
		case VER_PLATFORM_WIN32_WINDOWS:	// 95, 98, 98SE, ME
			if(osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0){
				strcat(t, "95");
				if( osvi.szCSDVersion[1] == 'C' || osvi.szCSDVersion[1] == 'B' ){
					strcat(t, " OSR2" );
				}
			}
			else if(osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10){
				strcat(t, "98");
				if( osvi.szCSDVersion[1] == 'A' ){
	                strcat(t, "SE" );
    			}
            }
            else if(osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90){
				strcat(t, "ME");
            }
			break;
		default:
      		break;
   }
   return TRUE;
}
///------------------------------------------------------
///
///CCond クラス
///
int __fastcall GetDataConds(const char *p, int err, TSpeedButton *pButton)
{
	return MainVARI->GetDataConds(p, err, pButton);
}
///------------------------------------------------------
__fastcall CCond::CCond(int max)
{
	m_CIFMAX = max;
    m_f = m_fc = 0;
	m_err = 1;
    m_pE = new int[max];
    m_pAF = new int[max];
    memset(m_pE, 0, sizeof(int)*max);
    memset(m_pAF, 0, sizeof(int)*max);
    m_fCond = FALSE;
}
__fastcall CCond::~CCond(void)
{
	delete m_pE;
    delete m_pAF;
}
/*
================================================================
	条件擬似命令の開始のチェック
----------------------------------------------------------------
----------------------------------------------------------------
----------------------------------------------------------------
	0 : 開始できない
================================================================
*/
int __fastcall CCond::BgnCond(void)
{
	if( m_f && (!m_pE[m_f-1]) ){
		m_fc++;
		return(0);
	}
	else if( m_f >= m_CIFMAX ){
		if( m_err ){
			m_err = 0;
			if( !sys.m_MacroError ){
				sys.m_MacroError = TRUE;
				ErrorMB(sys.m_MsgEng ? "Over the nest of condition blocks":"#if～#endifブロックのネストが制限を超えました.");
			}
		}
		return(0);
	}
	else {
		return(1);
	}
}

/*#$%
================================================================
	条件ブロックの処理を行う
----------------------------------------------------------------
	p : 文字列のポインタ
----------------------------------------------------------------
	0 : 条件が偽である
    1 : 条件が真である(行は有効)
----------------------------------------------------------------
================================================================
*/
int __fastcall CCond::CondJob(const char *p, TSpeedButton *pButton)
{
	int	d;

	p = SkipSpace(p);
	if( !strncmp(p, "#if", 3) ){
		m_fCond = TRUE;
		if( BgnCond() ){
			m_pAF[m_f] = m_pE[m_f] = GetDataConds(p+3, m_err, pButton) ? 1 : 0;
			m_f++;
		}
		return(0);
	}
	else if( !strncmp(p, "#elseif", 7) ){
		if( !m_fc ){
			if( m_f ){
				if( m_pAF[m_f-1] ){		/* 過去に真の時は偽	*/
					m_pE[m_f-1] = 0;
				}
				else {
					d = GetDataConds(p+7, m_err, pButton) ? 1 : 0;
					m_pE[m_f-1] = d;
					m_pAF[m_f-1] |= d;
				}
			}
			else if( m_err ){
				m_err = 0;
				if( !sys.m_MacroError ){
					sys.m_MacroError = TRUE;
					ErrorMB(sys.m_MsgEng ? "We need <#if> first":"#ifブロックが始まっていません");
                }
			}
		}
		return(0);
	}
	else if( (!strcmp(p, "#else")) || (!strncmp(p, "#else ", 6)) ){
		if( !m_fc ){
			if( m_f ){
				if( m_pAF[m_f-1] ){		/* 過去に真の時は偽	*/
					m_pE[m_f-1] = 0;
				}
				else {
					m_pE[m_f-1] = m_pE[m_f-1] ? 0 : 1;	/* 条件の反転	*/
					m_pAF[m_f-1] |= m_pE[m_f-1];
				}
			}
			else if( m_err ){
				m_err = 0;
				if( !sys.m_MacroError ){
					sys.m_MacroError = TRUE;
					ErrorMB(sys.m_MsgEng ? "We need <#if> first":"#ifブロックが始まっていません");
				}
			}
		}
		return(0);
	}
	else if( (!strcmp(p, "#endif")) || (!strncmp(p, "#endif ", 7)) ){
		if( m_fc ){
			m_fc--;
		}
		else if( m_f ){
			m_f--;
		}
		else if( m_err ){
			m_err = 0;
			if( !sys.m_MacroError ){
				sys.m_MacroError = TRUE;
				ErrorMB(sys.m_MsgEng ? "We need <#if> first":"#ifブロックが始まっていません");
			}
		}
		return(0);
	}

	if( m_f && (!m_pE[m_f-1]) ){
		return(0);
	}
	return(1);
}

/*#$%
================================================================
	条件ブロックの処理を行う
----------------------------------------------------------------
	p : 文字列のポインタ
----------------------------------------------------------------
	0 : 条件が偽である
    1 : 条件が真である(行は有効)
----------------------------------------------------------------
================================================================
*/
int __fastcall CCond::CondStr(AnsiString &Cond, const char *p)
{
	p = SkipSpace(p);
	if( !strncmp(p, "#if", 3) ){
		Cond = SkipSpace(p+3);
		m_f++;
		return m_f - 1;
	}
	else if( !strncmp(p, "#elseif", 7) ){
		Cond = SkipSpace(p+7);
		return m_f - 1;
	}
	else if( (!strcmp(p, "#else")) || (!strncmp(p, "#else ", 6)) ){
		if( m_f == 1 ){
			Cond = "TRUE";
		}
		return m_f - 1;
	}
	else if( (!strcmp(p, "#endif")) || (!strncmp(p, "#endif ", 7)) ){
		m_f--;
		if( !m_f ) Cond = "$End";
		return m_f;
	}
	return 1;
}

//---------------------------------------------------------------------------
#define	DUMMYMAX	64
typedef struct {
	LPCSTR	pName;
    int		Len;
    int		Order;
}_DMY_PARA;
static int _CmpDummyOrder(const void *s, const void *t)
{
	_DMY_PARA	*pSrc = (_DMY_PARA*)s;
    _DMY_PARA	*pDis = (_DMY_PARA*)t;

	if( pDis->Len != pSrc->Len ){
	    return pDis->Len - pSrc->Len;
    }
    else {
		return pSrc->Order - pDis->Order;
    }
}
void __fastcall ConvDummy(AnsiString &ws, LPCSTR p, LPCSTR pNames, LPCSTR pValues)
{
	if( pNames && *pNames ){
		_DMY_PARA *pListNames = new _DMY_PARA[DUMMYMAX];
		LPCSTR *pListValues = new LPCSTR[DUMMYMAX];

        LPSTR pp, tt;
		LPSTR pbNames = StrDupe(SkipSpace(pNames));
        pp = pbNames;
        int ncount = 0;
        _DMY_PARA *dp = pListNames;
        while(*pp && (ncount < DUMMYMAX) ){
			pp = StrDlm(tt, SkipSpace(pp));
            dp->pName = SkipSpace(tt);
            dp->Len = strlen(dp->pName);
            dp->Order = ncount++;
            dp++;
        }
        LPSTR pbValues = StrDupe(SkipSpace(pValues));
        pp = pbValues;
        int vcount = 0;
        while(*pp && (vcount < ncount) ){
			pp = StrDlm(tt, SkipSpace(pp));
            pListValues[vcount++] = SkipSpace(tt);
		}
		if( ncount >= 2 ){
			qsort(pListNames, ncount, sizeof(_DMY_PARA), _CmpDummyOrder);
        }
		int i, f;
		ws = "";
		int fAdd = 0;
		int cProc = 0;
        while(*p){
			f = FALSE;
			if( (*p == '\n')||(*p == '\r')|| !fAdd ){
				LPCSTR pp = SkipSpace(p+fAdd);
				fAdd = 1;
                if( !strncmp(pp, "#proc", 5) || !strncmp(pp, "#repeat", 7) ){
                	cProc++;
                }
                else if( !strncmp(pp, "#endp", 5) ){
					if( cProc ) cProc--;
                }
            }
			if( !cProc ){
	            dp = pListNames;
				for( i = 0; i < ncount; i++, dp++ ){
					if( !strncmp(p, dp->pName, dp->Len) ){
            	    	if( dp->Order < vcount ){
                	    	ws += pListValues[dp->Order];
                    	}
	                    p += dp->Len;
    	                f = TRUE;
        	            break;
            	    }
	            }
            }
            if( !f ){
				ws += *p;
                p++;
            }
        }
        delete pbValues;
        delete pbNames;
        delete pListValues;
        delete pListNames;
    }
    else {
		ws = p;
    }
}
//---------------------------------------------------------------------------
static WORD GetHash(LPCSTR p)
{
	WORD d = 0;
	for( ; *p; p++ ){
		d = WORD(d << 1);
		d += *p;
    }
    return d;
}
//---------------------------------------------------------------------------
__fastcall CVal::CVal()
{
	m_Max = m_Count = 0;
    m_pBase = NULL;
	m_fHandleProc = FALSE;
}
//---------------------------------------------------------------------------
void __fastcall CVal::Delete(void)
{
	if( m_pBase ){
		VALDATA *pData = m_pBase;
		for( int i = 0; i < m_Count; i++, pData++ ){
			if( pData->pName ) delete pData->pName;
            if( pData->pString ) delete pData->pString;
            if( pData->pPara ) delete pData->pPara;
	    }
        delete m_pBase;
        m_pBase = NULL;
        m_Count = m_Max = 0;
    }
	m_fHandleProc = FALSE;
}
//---------------------------------------------------------------------------
void __fastcall CVal::Alloc(void)
{
	if( m_Count >= m_Max ){
		int max = m_Max ? m_Max * 2 : 16;
		VALDATA *pNew = new VALDATA[max];
		memset(pNew, 0, sizeof(VALDATA)*max);
        if( m_pBase ) memcpy(pNew, m_pBase, sizeof(VALDATA) * m_Count);
        delete m_pBase;
        m_pBase = pNew;
        m_Max = max;
    }
}
//---------------------------------------------------------------------------
int __fastcall CVal::FindName(LPCSTR pName, BYTE mtype)
{
	if( mtype & (_VAL_NORMAL|_VAL_INIFILE) ) mtype |= _VAL_NORMAL|_VAL_INIFILE;
	int i;
    WORD h = GetHash(pName);
    VALDATA *pData = m_pBase;
    for( i = 0; i < m_Count; i++, pData++ ){
		if( (pData->hash == h) && (pData->mtype & mtype) ){
			if( !strcmp(pData->pName, pName) ) return i;
        }
    }
    return -1;
}
//---------------------------------------------------------------------------
void __fastcall CVal::Delete(BYTE mtype)
{
	int i;
    VALDATA *pData = m_pBase;
    for( i = 0; i < m_Count; i++, pData++ ){
		if( pData->mtype & mtype ){
			if( Delete(pData->pName, pData->mtype) ){
				pData--; i--;
            }
        }
    }
}
//---------------------------------------------------------------------------
BOOL __fastcall CVal::Delete(LPCSTR pName, BYTE mtype)
{
	if( !m_Count ) return FALSE;

	int r = FindName(pName, mtype);
	VALDATA *pData;
    if( r >= 0 ){
		pData = &m_pBase[r];
        delete pData->pName;
        delete pData->pString;
        if( pData->pPara ) delete pData->pPara;
        r = m_Count - r - 1;
        if( r > 0 ) memcpy(pData, &pData[1], r*sizeof(VALDATA));
        m_Count--;
#if 0
	FILE *fp = fopen("test.txt", "wt");
    for( int i = 0; i < m_Count; i++ ){
		fprintf(fp, "[%d] [%s]=[%s]\n", m_pBase[i].mtype, m_pBase[i].pName, m_pBase[i].pString);
	}
    fclose(fp);
#endif
        return TRUE;
    }
    else {
		return FALSE;
    }
}
//---------------------------------------------------------------------------
void __fastcall CVal::RegisterString(LPCSTR pName, LPCSTR pString, LPCSTR pPara, BYTE mtype)
{
	int r = FindName(pName, mtype);
	VALDATA *pData;
    if( r >= 0 ){
		pData = &m_pBase[r];
		if( pData->pString ) delete pData->pString;
	    if( pData->pPara ) delete pData->pPara;
    }
    else {
		Alloc();
        pData = &m_pBase[m_Count++];
        pData->pName = StrDupe(pName);
        pData->hash = GetHash(pName);
    }
	pData->pString = StrDupe(pString);
    pData->pPara = (pPara && *pPara) ? StrDupe(pPara) : NULL;
    pData->mtype = BYTE(mtype);
    if( (mtype == _VAL_PROC) && !strncmp(pName, "On$", 3) ){
		m_fHandleProc = TRUE;
    }
}
//---------------------------------------------------------------------------
void __fastcall CVal::WriteInifile(TMemIniFile *pIniFile, LPCSTR pSect)
{
	pIniFile->EraseSection(pSect);
	char bf1[64];
	char bf2[4096];
    AnsiString	as, rs;
	VALDATA *pData = m_pBase;
	int n = 0;
	for( int i = 0; i < m_Count; i++, pData++ ){
		if( pData->mtype == _VAL_INIFILE ){
			sprintf(bf1, "V%d", n+1);
            rs = pData->pString;
			CrLf2Yen(as, rs);
			sprintf(bf2, "%s,%s", pData->pName, as.c_str());
            pIniFile->WriteString(pSect, bf1, bf2);
            n++;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall CVal::ReadInifile(TMemIniFile *pIniFile, LPCSTR pSect)
{
	char bf1[64];
	char bf[4096];
	LPSTR p, t;
    AnsiString as, rs;
	int n = 1;
	while(1){
		sprintf(bf1, "V%d", n++);
		as = pIniFile->ReadString(pSect, bf1, "");
		if( as.IsEmpty() ) break;
        StrCopy(bf, as.c_str(), sizeof(bf)-1);
        p = StrDlm(t, bf);
        rs = SkipSpace(p);
		Yen2CrLf(as, rs);
		RegisterString(t, as.c_str(), NULL, _VAL_INIFILE);
    }
}
//---------------------------------------------------------------------------
__fastcall CMBCS::CMBCS()
{
	m_pLead = NULL;
	m_Charset = ANSI_CHARSET;
}
//---------------------------------------------------------------------------
__fastcall CMBCS::~CMBCS()
{
	if( m_pLead ) delete m_pLead;
}
//---------------------------------------------------------------------------
void __fastcall CMBCS::Create(void)
{
//	FILE *fp = fopen("DbgLog.txt", "wt");
	if( !m_pLead ) m_pLead = new BYTE[256];
    for( int i = 0; i < 256; i++ ){
		m_pLead[i] = (_mbsbtype((unsigned char *)&i, 0) == _MBC_LEAD);
//        fprintf(fp, "%02X:%d\n", i, m_pLead[i]);
    }
//    fclose(fp);
}
//---------------------------------------------------------------------------
void __fastcall CMBCS::Create(BYTE charset)
{
	BYTE bak_charset = sys.m_Charset;
	m_Charset = charset;
	SetMBCP(charset);
	Create();
    SetMBCP(bak_charset);
}
//---------------------------------------------------------------------------
BOOL __fastcall CMBCS::IsLead(BYTE c)
{
	if( !m_pLead ) return FALSE;
    return m_pLead[c];
}
//---------------------------------------------------------------------------
BOOL __fastcall CMBCS::IsLead(const unsigned char *p)
{
	return IsLead(*p);
}
//---------------------------------------------------------------------------
#define _SWAP(c) WORD((c<<8)|(c>>8))
const CONVALPHA	_tConvAlphaJA[]={
	{_SWAP('　'), ' '},
//	{_SWAP('！'), '!'},
//	{_SWAP('”'), '"'},
//	{_SWAP('＃'), '#'},
//	{_SWAP('＄'), '$'},
//	{_SWAP('％'), '%'},
//	{_SWAP('＆'), '&'},
//	{_SWAP('’'), 0x27},
//	{_SWAP('（'), '('},
//	{_SWAP('）'), ')'},
//	{_SWAP('＝'), '='},
//	{_SWAP('－'), '-'},
//	{_SWAP('＠'), '@'},
//	{_SWAP('＋'), '+'},
//	{_SWAP('＊'), '*'},
//	{_SWAP('；'), ';'},
//	{_SWAP('：'), ':'},
//	{_SWAP('＜'), '<'},
//	{_SWAP('＞'), '>'},
	{_SWAP('，'), ','},
	{_SWAP('．'), '.'},
//	{_SWAP('？'), '?'},
	{_SWAP('／'), '/'},
    {0, 0},
};
const CONVALPHA	_tConvAlphaBV[]={
	{0xa140, ' '},
	{0xa14d, ','},
	{0xa14f, '.'},
//	{0xa151, ';'},
//	{0xa152, ':'},
//	{0xa153, '?'},
//	{0xa248, '%'},
//	{0xa249, '@'},
    {0, 0},
};
const char _tInhibit[]="!\x22#$%&'()=-^~|\@`[{;+*:}]<>?_";
//---------------------------------------------------------------------------
static int __fastcall ConvChar(const CONVALPHA *pAlpha, int code)
{
	for( ; pAlpha->cMBCS; pAlpha++ ){
		if( pAlpha->cMBCS == code ) return pAlpha->cASCII;
    }
    return code;
}
//---------------------------------------------------------------------------
int __fastcall CMBCS::ConvAlpha(int code)
{
	switch(m_Charset){
		case SHIFTJIS_CHARSET:		// JA
			if( (code >= 0x8140) && (code <= 0x81ff) ){
				code = ConvChar(_tConvAlphaJA, code);
            }
            else if( (code >= 0x824f) && (code <= 0x8258) ){
				code -= (0x824f - 0x30);
            }
            else if( (code >= 0x8260) && (code <= 0x8279) ){
				code -= (0x8260 - 0x41);
            }
            else if( (code >= 0x8281) && (code <= 0x829a) ){
				code -= (0x8281 - 0x61);
            }
        	break;
		case HANGEUL_CHARSET:		// HL
		case 134:       			// BY
			if( code == 0xa1a1 ){
				code = 0x20;
            }
            else if( (code >= 0xa3a1) && (code <= 0xa3ff) ){
				int c = code - (0xa3a1 - 0x21);
				if( !strchr(_tInhibit, c) ) code = c;
            }
			break;
		case CHINESEBIG5_CHARSET:   // BV
			if( (code >= 0xa140) && (code <= 0xa249) ){
				code = ConvChar(_tConvAlphaBV, code);
            }
			else if( (code >= 0xa2af) && (code <= 0xa2b9) ){
				code -= (0xa2af - 0x30);
            }
            else if( (code >= 0xa2cf) && (code <= 0xa2e8) ){
				code -= (0xa2cf - 0x41);
            }
            else if( (code >= 0xa2e9) && (code <= 0xa2fe) ){
				code -= (0xa2cf - 0x61);
            }
            else if( (code >= 0xa340) && (code <= 0xa343) ){
				code -= (0xa340 - 'w');
            }
            break;
        default:
        	break;
    }
	return code;
}
///----------------------------------------------------------------
///  式の計算
typedef struct {
	int		pr;			/* 優先順位	*/
	char	cd;			/* 演算ｺｰﾄﾞ	*/
	double	d;
}VSS;
static int	_err;

/*#$%
========================================================
	演算子の優先順位を返す
--------------------------------------------------------
	c  : 演算子
--------------------------------------------------------
--------------------------------------------------------
========================================================
*/
static int __fastcall _cpr(char c)
{
	switch(c){
		case 0:
			return(0);
		case '+':
			return(1);
		case '-':
			return(1);
		case '*':
			return(2);
		case '/':
			return(2);
		case '%':
			return(2);
        case '&':
        case '|':
        	return(3);
		default:
			return(-1);
	}
}

/*#$%
========================================================
	演算子かどうかを返す
--------------------------------------------------------
	c  : 演算子
--------------------------------------------------------
--------------------------------------------------------
========================================================
*/
static int __fastcall _iscd(char c)
{
	if( c == 0 ) return(0);
	return( (_cpr(c) == -1) ? 0 : 1 );
}

static BOOL __fastcall IsValExp(LPCSTR p, LPCSTR v)
{
	if( (*(p-1) != 'E') && (*(p-1) != 'e') ) return FALSE;
	return isdigit(*v);
}
/*#$%
========================================================
	演算子と値を式管理データに格納する
--------------------------------------------------------
	vs : 式管理データのポインタ
	p  : 文字列のポインタ
--------------------------------------------------------
	文字列のポインタ
--------------------------------------------------------
========================================================
*/
static LPCSTR __fastcall _val(VSS *vs, LPCSTR p)
{
	char	*t, bf[80];

	BOOL f = FALSE;
	vs->cd = 0;
	vs->pr = 0;
	for( t = bf; *p; p++ ){
		if( *p == '<' ){
			*t++ = *p;
			f++;
		}
		else if( *p == '>' ){
			*t++ = *p;
			if( f ) f--;
        }
		else if( !f && _iscd(*p) && (t!=bf) && ((t==bf)||!IsValExp(p, bf) ) ){
			vs->cd = *p;
			vs->pr = _cpr(*p);
			p++;
			break;
		}
		else if( *p == ')' ){
			p++;
			break;
		}
		else if( (*p != ' ')&&(*p != TAB) ){
			*t++ = *p;
		}
	}
	*t = 0;
	if( bf[0] == 0 ){
		vs->d = 0.0;
	}
    else if( (bf[0] == '0') && (bf[1] == 'x') ){
		vs->d = htoin(&bf[2], -1);
    }
	else {
		vs->d = MainVARI->GetMacroDouble(bf);
	}
	return(p);
}

/*#$%
========================================================
	演算子に従って計算を行う
--------------------------------------------------------
	vp : 式管理データのポインタ
	vw : 式管理データのポインタ
--------------------------------------------------------
--------------------------------------------------------
========================================================
*/
static void __fastcall _cop(VSS *vs, VSS *vw)
{
	switch(vs->cd){
		case '+':
			vs->d += vw->d;
			break;
		case '-':
			vs->d -= vw->d;
			break;
		case '*':
			vs->d *= vw->d;
			break;
		case '/':
			if( vw->d ){
				vs->d /= vw->d;
			}
			else {
				vs->d = MAXDOUBLE;
			}
			break;
		case '%':
			if( vw->d ){
				vs->d = fmod(vs->d, vw->d);
			}
			else {
				vs->d = 0;
			}
			break;
        case '&':
        	vs->d = UINT(vs->d) & UINT(vw->d);
            break;
        case '|':
        	vs->d = UINT(vs->d) | UINT(vw->d);
            break;
	}
}

/*#$%
========================================================
	式計算処理
--------------------------------------------------------
	vp : 式管理データのポインタ
	p  : 文字列のポインタ
--------------------------------------------------------
	文字列のポインタ
--------------------------------------------------------
========================================================
*/
static LPCSTR __fastcall _calc(VSS *vs, LPCSTR p)
{
	VSS		vw;

	p = SkipSpace(p);
	if( *p ){
		if( *p == '(' ){
			p++;
			vw.d = 0;
			vw.pr = 0;
			vw.cd = '+';
			p = _calc(&vw, p);
			if( *p && _iscd(*p) ){
				vw.cd = *p;
				vw.pr = _cpr(*p);
				p++;
			}
		}
		else {
			p = _val(&vw, p);
		}
		if( vw.pr == 0 ){
			_cop(vs, &vw);
			p = SkipSpace(p);
			if( _iscd(*p) ){
				vs->cd = *p;
				vs->pr = _cpr(*p);
				p++;
			}
		}
		else if( vs->pr >= vw.pr ){
			_cop(vs, &vw);
			vs->pr = vw.pr;
			vs->cd = vw.cd;
			p = _calc(vs, p);
		}
		else {
			p = _calc(&vw, p);
			_cop(vs, &vw);
			vs->pr = vw.pr;
			vs->cd = vw.cd;
		}
	}
	return(p);
}

/*#$%
========================================================
	式計算処理
--------------------------------------------------------
	d  : 計算結果格納位置のポインタ
	p  : 文字列のポインタ
--------------------------------------------------------
	ERR
--------------------------------------------------------
========================================================
*/
int __fastcall Calc(double &d, LPCSTR p)
{
	VSS		vs;

	p = SkipSpace(p);
    LPSTR pBF = new char[strlen(p)+2];
	d = 0;
    LPSTR t = pBF;
    if( (*p == '-')||(*p == '+') ){
		*t++ = '0';
    }
	strcpy(t, p);
	_err = TRUE;
	vs.d = 0;
	vs.pr = 0;
	vs.cd = '+';
	_calc(&vs, pBF);
    delete pBF;
	d = vs.d;
	return(_err);
}
/*#$%
========================================================
	式計算処理
--------------------------------------------------------
	d  : 計算結果格納位置のポインタ
	p  : 文字列のポインタ
--------------------------------------------------------
	ERR
--------------------------------------------------------
========================================================
*/
int __fastcall CalcI(int &d, LPCSTR p)
{
	double	dd;
	int r = Calc(dd, p);
	if( dd >= 0 ){
		d = int(dd + 0.5);
    }
    else {
		d = int(dd - 0.5);
    }
	return r;
}
/*#$%
========================================================
	式計算処理
--------------------------------------------------------
	d  : 計算結果格納位置のポインタ
	p  : 文字列のポインタ
--------------------------------------------------------
	ERR
--------------------------------------------------------
========================================================
*/
int __fastcall CalcU(int &d, LPCSTR p)
{
	double	dd;
	int r = Calc(dd, p);
	d = int(ABS(dd)+0.5);
	return r;
}
short	mend[14] = {31,31,28,31,30,31,30,31,31,30,31,30,31,31} ;
short	summ[13] = {0,0,31,59,90,120,151,181,212,243,273,304,334};

static __fastcall int cleap(int yy)
{
	if ((yy % 4) != 0)
		return 0;
	else if (((yy % 100) == 0) && ((yy % 400) != 0))
		return 0;
	else
		return 1;
}

static __fastcall int mcleap(int y, int m)
{
	if (m == 2)
		return cleap(y) ;
	else
		return 0 ;
}

void __fastcall GPS2SystemTime(ULONG gps, SYSTEMTIME *sp)
{
	int 	s,yr,m;
	long	d;

	d = (gps / 86400) + 6 ;
	yr = 1980 ;
	while ((d > 0) && (yr <= 2060))
	{
		s = cleap(yr) + 365;
		d -= s;
		yr++ ;
	}
	if (yr >= 2060)
	{
		memset(sp, 0, sizeof(SYSTEMTIME));
		return;
	}
	else
	{
		d += s ;
		yr-- ;
	}
	m = 1 ;
	while(d > 0)
	{
		d = d - (mend[m] + mcleap(yr,m));
		m++ ;
	}
	m-- ;
	d = d + (mend[m] + mcleap(yr,m));

/*
	WORD wYear;
	WORD wMonth;
	WORD wDayOfWeek;
	WORD wDay;
	WORD wHour;
	WORD wMinute;
	WORD wSecond;
*/
	sp->wYear = WORD(yr);
	sp->wMonth = WORD(m);
	sp->wDay = WORD(d);
	sp->wHour = WORD((gps % 86400) / 3600);
	sp->wMinute = WORD((gps % 3600) / 60);
	sp->wSecond = WORD(gps % 60);
}

ULONG __fastcall SystemTime2GPS(SYSTEMTIME *sp)
{
	int 	dd;
	ULONG	gps;
/*
	WORD wYear;
	WORD wMonth;
	WORD wDayOfWeek;
	WORD wDay;
	WORD wHour;
	WORD wMinute;
	WORD wSecond;
*/

	if (sp->wYear < 1980){
		return 0;
	}
	if ((sp->wYear == 1980) && (sp->wMonth == 1) && (sp->wDay < 6)){
		return 0;
	}

	dd = (sp->wYear - 1980) * 365 + (sp->wYear - 1977) / 4 + summ[sp->wMonth]
		+ (sp->wMonth + 9) /12 * cleap(sp->wYear) + sp->wDay - 6;
	gps = dd * 86400 + (ULONG)sp->wHour * 3600 + sp->wMinute * 60 + sp->wSecond;

	return gps;
}
//***************************************************************************
//CLIBLクラス
//***************************************************************************
//
//
//---------------------------------------------------------------------------
void __fastcall CLIBL::Alloc(void)
{
	int am = m_AMax ? (m_AMax * 2) : 16;
	LIBD *pN = new LIBD[am];
	if( m_pBase != NULL ){
		memcpy(pN, m_pBase, sizeof(LIBD)*m_Count);
		delete m_pBase;
	}
	m_pBase = pN;
	m_AMax = am;
}
//---------------------------------------------------------------------------
void __fastcall CLIBL::Delete(void)
{
	if( m_pBase != NULL ){
		LIBD *cp = m_pBase;
		for( int i = 0; i < m_Count; i++, cp++ ){
			delete cp->pName;
			::FreeLibrary(cp->hLib);
		}
		delete m_pBase;
	}
	m_pBase = NULL;
	m_AMax = 0;
	m_Count = 0;
}
//---------------------------------------------------------------------------
void __fastcall CLIBL::Add(LPCSTR pName, HANDLE hLib)
{
	if( m_Count >= m_AMax ) Alloc();
	LIBD *cp = &m_pBase[m_Count];
	cp->pName = StrDupe(pName);
	cp->hLib = (HINSTANCE)hLib;
	m_Count++;
}
//---------------------------------------------------------------------------
HANDLE __fastcall CLIBL::LoadLibrary(LPCSTR pName)
{
	LIBD *cp = m_pBase;
	for( int i = 0; i < m_Count; i++, cp++ ){
		if( !strcmpi(cp->pName, pName) ){
			if( cp->hLib == NULL ){
				::SetCurrentDirectory(sys.m_BgnDir);
				cp->hLib = ::LoadLibrary(pName);
			}
			return cp->hLib;
		}
	}
	HANDLE hLib = ::LoadLibrary(pName);
	if( hLib != NULL ) Add(pName, hLib);
	return hLib;
}
//---------------------------------------------------------------------------
void __fastcall CLIBL::DeleteLibrary(HANDLE hLib)
{
	if( hLib == NULL ) return;

	LIBD *cp = m_pBase;
	for( int i = 0; i < m_Count; i++, cp++ ){
		if( cp->hLib == hLib ){
			::FreeLibrary((HINSTANCE)hLib);	//ja7ude 0522
			cp->hLib = NULL;
			break;
		}
	}
}
//---------------------------------------------------------------------------
