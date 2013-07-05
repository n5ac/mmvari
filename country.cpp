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



/************************************************************************
	カントリ識別処理モジュール

	Copyright (C) JE3HHT 1993-2000.
************************************************************************/
//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "ComLib.h"
#include "Country.h"
#include "string.h"
#include "stdlib.h"

CCountry	Cty;
/*#$%
========================================================
	文字列から指定の文字をクリップする
--------------------------------------------------------
	s : 文字列のポインタ
--------------------------------------------------------
--------------------------------------------------------
========================================================
*/
static char *__fastcall _delchr(char *s, char c)
{
	char	*p;

	for( p = s; *p; p++ ){
		if( *p == c ){
			strcpy(p, p+1);
			p--;
		}
	}
	return(s);
}

/*#$%
======================================================
	異なる文字列のポインタを返す
------------------------------------------------------
	n : 範囲数の格納位置のポインタ
	t : プリフィックスのポインタ
	p : プリフィックスのポインタ
------------------------------------------------------
------------------------------------------------------
======================================================
*/
static LPSTR __fastcall lcmpp(int *n, LPSTR s, LPSTR p)
{
	LPSTR	t;

	for( t = s; *p && *t; p++, t++ ){
		if( *p != *t ){
			*n = (*p - *t) + 1;
			if( *n > 26 ) *n = 26;
			return(t);
		}
	}
	*n = 0;
	return(s);
}

/*#$%
======================================================
	文字列の比較を行う
------------------------------------------------------
	t : 対象文字列
	s : 基準文字列
------------------------------------------------------
------------------------------------------------------
======================================================
*/
static int __fastcall strcmpv(LPCSTR t, LPCSTR s)
{
	for( ; *s; s++, t++ ){
		if( *t != *s ) return(1);
	}
	return(0);
}

static	int		svf;		/* 完全一致フラグ	*/
/*#$%
================================================================
	プリフィックス検索用文字列比較
----------------------------------------------------------------
----------------------------------------------------------------
----------------------------------------------------------------
================================================================
*/
static int __fastcall _strcmp(LPCSTR t, LPCSTR v)
{
/*	printf( "<%s>-<%s>\n", v, t );	*/
	for( ; *v; v++, t++ ){
		if( *v == '*' ){
			for( v++; *t && strcmpv(t, v); t++ );
			if( !*t ) return(1);
		}
		else if( *v == '\\' ){
			if( *t ) return(1);
		}
		else if( (*v!='?') && (*v != *t) ){
			return(1);
		}
	}
	if( svf && *t ) return(1);
	return(0);
}

/*#$%
======================================================
	コンストラクター
------------------------------------------------------
------------------------------------------------------
------------------------------------------------------
======================================================
*/
__fastcall CCountry::CCountry()
{
	Init();
}

__fastcall CCountry::~CCountry(void)
{
	Free();
}

/*#$%
======================================================
	ＤＸＣＣ定義領域を開放する
------------------------------------------------------
------------------------------------------------------
------------------------------------------------------
======================================================
*/
void __fastcall CCountry::Init(void)
{
	cmax = 0;
	memset(ctl, 0, sizeof(ctl));
}

/*#$%
======================================================
	ＤＸＣＣ定義領域を開放する
------------------------------------------------------
------------------------------------------------------
------------------------------------------------------
======================================================
*/
void __fastcall CCountry::Free(void)
{
	int i;
	CTL	*cp;

	for( cp = ctl, i = 0; i < cmax; i++, cp++ ){
		if( cp->Name != NULL ) delete cp->Name;
		if( cp->Code != NULL ) delete cp->Code;
		if( cp->QTH != NULL ) delete cp->QTH;
		if( cp->Cont != NULL ) delete cp->Cont;
		if( cp->TD != NULL ) delete cp->TD;
	}
	Init();
}


/*#$%
======================================================
	プリフィックスからカントリコードを得る
------------------------------------------------------
	p : プリフィックスのポインタ
------------------------------------------------------
	カントリコード
------------------------------------------------------
======================================================
*/
int __fastcall CCountry::GetNo(LPCSTR s)
{
	LPSTR	p, pb, t, pp;
	int		i;
	int	j;

	if( !*s ) return(0);
	for( i = 0; i < cmax; i++ ){
		strcpy(wbf, ctl[i].Code);
		for( p = wbf; *p; ){
			p = StrDlm(pb, p, ',');
			if( (t = strchr(pb, '-')) != NULL ){
				*t = 0;
				t++;
				if( (*pb == *s) || (*pb != *t) ){
					for( pp = lcmpp(&j, pb, t); j; (*pp)++, j-- ){
						if( !_strcmp(s, pb) ) return(i+1);
					}
				}
			}
			else {
				if( !_strcmp(s, pb) ) return(i+1);
			}
		}
	}
	return(0);
}

/*#$%
======================================================
	コールサインからポインタを得る
------------------------------------------------------
	p : コールサインのポインタ
------------------------------------------------------
	ポインタ
------------------------------------------------------
======================================================
*/
int __fastcall CCountry::GetNoP(LPCSTR p)
{
	int		n;

	svf = 1;
	if( (n = GetNo(p))!=0 ) return(n);
	svf = 0;
	if( (n = GetNo(p))!=0 ) return(n);
	return(0);
}

/*#$%
======================================================
	コールサインからカントリ名を得る
------------------------------------------------------
	p : コールサインのポインタ
------------------------------------------------------
	カントリ名のポインタ
------------------------------------------------------
======================================================
*/
LPCSTR __fastcall CCountry::GetCountry(LPCSTR p)
{
	if( !cmax ) return "";
	int		n;

	if( !(*p) ) return("?");
	if( ((n = GetNoP(p))!=0) && (ctl[n-1].Name != NULL) ){
		strcpy(wbf, ctl[n-1].Name);
		return(wbf);
	}
	else {
		return("?");
	}
}

/*#$%
======================================================
	コールサインから大陸名を得る
------------------------------------------------------
	p : コールサインのポインタ
------------------------------------------------------
	カントリ名のポインタ
------------------------------------------------------
======================================================
*/
LPCSTR __fastcall CCountry::GetCont(LPCSTR p)
{
	if( !cmax ) return "";
	int		n;

	if( !(*p) ) return("?");
	if( ((n = GetNoP(p))!=0) && (ctl[n-1].Cont != NULL) ){
		strcpy(wbf, ctl[n-1].Cont);
		return(wbf);
	}
	else {
		return("?");
	}
}

/*#$%
======================================================
	ＤＸＣＣ定義ファイルを読み込む
------------------------------------------------------
	fm : ファイル名のポインタ
------------------------------------------------------
------------------------------------------------------
======================================================
*/
void __fastcall CCountry::Load(LPCSTR fm)
{
	FILE	*fp;
	LPCSTR	p;
	char	hbf[512];

	Free();
	if( (fp = fopen(fm, "rt"))!=NULL ){
		while( !feof(fp) ){
			if( fgets(hbf, 512, fp)!=NULL ){
				if( hbf[0] == '$' ) break;
				ClipLF(hbf);
				_delchr(hbf, TAB);
				if( hbf[0] != '!' ){
					p = StrDlmCpy(wbf, hbf, ';', 512);
					clipsp(wbf);
					ctl[cmax].Name = StrDupe(wbf);
					if( p != NULL ){
						p = StrDlmCpy(wbf, p, ';', 512);
						clipsp(wbf);
						ctl[cmax].Code = StrDupe(wbf);
					}
					if( p != NULL ){
						p = StrDlmCpy(wbf, p, ';', 512);
						clipsp(wbf);
						ctl[cmax].QTH = StrDupe(wbf);
					}
					if( p != NULL ){
						p = StrDlmCpy(wbf, p, ';', 512);
						clipsp(wbf);
						ctl[cmax].Cont = StrDupe(wbf);
					}
					if( p != NULL ){
						StrDlmCpy(wbf, p, ';', 512);
						clipsp(wbf);
						ctl[cmax].TD = StrDupe(wbf);
					}
					cmax++;
					if( cmax >= CTMAX ) break;
				}
			}
		}
		fclose(fp);
	}
	else {
		WarningMB(sys.m_MsgEng ? "'ARRL.DX' was not found.\r\n\r\nYou cannot use a DXCC function.\r\nThis is not a problem if you do not need it":"'ARRL.DX'が見つかりません.\r\n\r\nDXエンティティの自動判定機能は使用できません.");
	}
}

