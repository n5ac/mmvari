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



/*=============================================================================
  MFSK16の実装	2004/JUL/28に作成開始  by Mako JE3HHT
=============================================================================*/
#include <vcl.h>
#pragma hdrstop
#include <stdlib.h>
#include <math.h>
#include <limits.h>

#include "mfsk.h"
#include "dsp.h"

#if DEBUG && 0		// InterLeaverのデバッグ
#define	TEST_BITS	5	// MFSK8 = 5, MFSK16 = 4, MFSK31 = 3
//--------------------------------------------------------------------------
int IVTest(void)
{

	CInterLeaver	IVE, IVD;

	BYTE syms[256];

    LPCSTR p = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyz"
    		   "--------------------------------------------------------------"
    		   "--------------------------------------------------------------"
    		   "--------------------------------------------------------------";
    FILE *fp = fopen("TEST.TXT", "wt");

	int i;
    IVE.Init(TEST_BITS);
    IVD.Init(TEST_BITS);
	int stage = 1;
	while(*p){
		LPCSTR pp = p;
		fprintf(fp, "%2u ", stage++);
		for( i = 0; i < TEST_BITS; i++ ){
			fprintf(fp, "%c", *pp);
            if( *pp ) pp++;
        }
		fprintf(fp, "\t");
	    for( i = 0; i < TEST_BITS; i++ ){
			if( *p ){
				syms[i] = *p++;
	        }
    	    else {
				syms[i] = '-';
	        }
        }
        IVE.EncodeSyms(syms);
		for( i = 0; i < TEST_BITS; i++ ){
			fprintf(fp, "%c", syms[i] ? syms[i] : ' ');
	    }
        IVD.DecodeSyms(syms);
        fprintf(fp, "\t");
		for( i = 0; i < TEST_BITS; i++ ){
			fprintf(fp, "%c", syms[i] ? syms[i] : ' ');
	    }
    	fprintf(fp, "\n");
    }
    fclose(fp);
    return 0;
}
int n = IVTest();
#endif


#if DEBUG && 0		// g_tGray2Bin[], g_tBin2Gray, g_tParityOddの生成
/*=============================================================================
  g_tGray2Bin[], g_tBin2Gray, g_tParity
=============================================================================*/
//---------------------------------------------------------------------------
static int __fastcall GetParity(UINT c)
{
	int i, n;
	for( i = n = 0; i < 32; i++, c = c >> 1 ){
		if( c & 1 ) n++;
    }
    return n;
}
static int CreateTable(void)
{
	int tbl[32];
	int i;

	FILE *fp = fopen("test.txt", "wt");
    fprintf(fp, "const BYTE g_tBin2Gray[]={\t//Decode");
	for( i = 0; i < MFSK_MAXTONES; i++ ){
		tbl[i] = (i >> 1) ^ i;				// グレーコードに変換
		if( !(i % 8) ) fprintf(fp, "\n\t");
		fprintf(fp, "0x%02X,", tbl[i]);
    }
    fprintf(fp, "\n};\nconst BYTE g_tGray2Bin[]={\t//Encode");
    for( i = 0; i < MFSK_MAXTONES; i++ ){
		if( !(i % 8) ) fprintf(fp, "\n\t");
		for( int j = 0; j < 32; j++ ){
			if( i == tbl[j] ) fprintf(fp, "0x%02X,", j);
        }
    }
    fprintf(fp, "\n};\n");
											// パリティテーブルを作成
    fprintf(fp, "const BYTE g_tParityOdd[]={\t//Odd parity");
    for( i = 0; i < 256; i++ ){
		if( !(i % 32) ) fprintf(fp, "\n\t");
		fprintf(fp, "%d,", GetParity(i) & 1);
    }
    fprintf(fp, "\n};\n");
    fclose(fp);
    return 0;
}
int n = CreateTable();
#endif

/*
const BYTE g_tBin2Gray[]={	//Decode
	0x00,0x01,0x03,0x02,0x06,0x07,0x05,0x04,
	0x0C,0x0D,0x0F,0x0E,0x0A,0x0B,0x09,0x08,
	0x18,0x19,0x1B,0x1A,0x1E,0x1F,0x1D,0x1C,
	0x14,0x15,0x17,0x16,0x12,0x13,0x11,0x10,
};
const BYTE g_tGray2Bin[]={	//Encode
	0x00,0x01,0x03,0x02,0x07,0x06,0x04,0x05,
	0x0F,0x0E,0x0C,0x0D,0x08,0x09,0x0B,0x0A,
	0x1F,0x1E,0x1C,0x1D,0x18,0x19,0x1B,0x1A,
	0x10,0x11,0x13,0x12,0x17,0x16,0x14,0x15,
};
*/


/*=============================================================================
	MFSK type ヘルパ関数
=============================================================================*/
//--------------------------------------------------------------------------
int __fastcall MFSK_Speed2Type(double speed)
{
	if( speed < 5.0 ){
		return typMFSK4;
    }
	else if( speed < 10.0 ){
		return typMFSK8;
    }
	else if( speed < 13.0 ){
		return typMFSK11;
    }
	else if( speed < 20.0 ){
		return typMFSK16;
    }
    else if( speed < 28.0 ){
		return typMFSK22;
    }
	else if( speed < 32.0 ){
		return typMFSK31;
    }
	else if( speed < 50.0 ){
		return typMFSK32;
    }
    else {
		return typMFSK64;
    }
}
//--------------------------------------------------------------------------
void __fastcall MFSK_SetPara(int type, int *pTones, double *pSpeed, int *pBits)
{
	switch(type){
        case typMFSK16:
			if( pTones ) *pTones = 16;
			if( pSpeed ) *pSpeed = 15.625;
			if( pBits ) *pBits = 4;
        	break;
		case typMFSK8:
			if( pTones ) *pTones = 32;
			if( pSpeed ) *pSpeed = 7.8125;
			if( pBits ) *pBits = 5;
        	break;
        case typMFSK31:
			if( pTones ) *pTones = 8;
			if( pSpeed ) *pSpeed = 31.25;
			if( pBits ) *pBits = 3;
        	break;
        case typMFSK11:
			if( pTones ) *pTones = 16;
			if( pSpeed ) *pSpeed = 11025.0/1024.0;
			if( pBits ) *pBits = 4;
        	break;
        case typMFSK22:
			if( pTones ) *pTones = 16;
			if( pSpeed ) *pSpeed = 11025.0/512.0;
			if( pBits ) *pBits = 4;
        	break;
        case typMFSK32:
			if( pTones ) *pTones = 16;
			if( pSpeed ) *pSpeed = 31.25;
			if( pBits ) *pBits = 4;
        	break;
        case typMFSK64:
			if( pTones ) *pTones = 16;
			if( pSpeed ) *pSpeed = 62.5;
			if( pBits ) *pBits = 4;
        	break;
		case typMFSK4:
			if( pTones ) *pTones = 32;
			if( pSpeed ) *pSpeed = 3.90625;
			if( pBits ) *pBits = 5;
        	break;
    }
}
/*=============================================================================
	CViterbiEncodeクラス
=============================================================================*/
const BYTE g_tParityOdd[]={	//Odd parity
	0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0,1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1,
	1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1,0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0,
	1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1,0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0,
	0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0,1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1,
	1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1,0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0,
	0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0,1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1,
	0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0,1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1,
	1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1,0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0,
};
#define	POLY2(d,p1,p2)	((g_tParityOdd[d&p1]<<1)|g_tParityOdd[d&p2])

//--------------------------------------------------------------------------
__fastcall CViterbiEncode::CViterbiEncode(void)
{
	Init(MFSK_VITERBI_K, MFSK_VITERBI_POLY1, MFSK_VITERBI_POLY2);
}
//--------------------------------------------------------------------------
void __fastcall CViterbiEncode::Init(int k, int poly1, int poly2)
{
	m_K = k;
    m_Poly1 = poly1; m_Poly2 = poly2;
    Reset();
}
//--------------------------------------------------------------------------
// b1 = 1st, b0 = 2nd
BYTE __fastcall CViterbiEncode::Do(BOOL bit)
{
	m_EncData = m_EncData << 1;
    if( bit ) m_EncData |= 1;
    return BYTE(POLY2(m_EncData, m_Poly1, m_Poly2));
}

/*=============================================================================
  CViterbiクラス
=============================================================================*/
//--------------------------------------------------------------------------
__fastcall CViterbi::CViterbi(void)
{
	m_BlockSize = 1;
	m_TraceBack = 45;
	m_States = 0;

	m_pPoly = NULL;

	memset(m_pMetrics, 0, sizeof(m_pMetrics));
	memset(m_pHistory, 0, sizeof(m_pHistory));
	memset(m_tSeq, 0, sizeof(m_tSeq));
	memset(m_tMetrics, 0, sizeof(m_tMetrics));

	m_CurPtr = NULL;
}
//--------------------------------------------------------------------------
void __fastcall CViterbi::Free(void)
{
	if( m_pPoly ){
    	delete m_pPoly; m_pPoly = NULL;
    }
    for( int i = 0; i < VITERBI_PATHMAX; i++ ){
		delete m_pMetrics[i]; m_pMetrics[i] = NULL;
		delete m_pHistory[i]; m_pHistory[i] = NULL;
    }
}
//--------------------------------------------------------------------------
void __fastcall CViterbi::Init(int k, int poly1, int poly2)
{
	int i;

	if( m_pPoly ){
#if DEBUG
		Application->MainForm->Caption = "Init duplication in CViterbi";
#endif
		Free();
    }
	int size = _tBitData[k] * 2;
#if DEBUG
	if( size > 256 ){
		Application->MainForm->Caption = "Over size in CViterbi";
        size = 256;
    }
#endif
	m_States = size / 2;
	// 高速化のために展開データをあらかじめ作成しておく
	m_pPoly = new int[size];
	for( i = 0; i < size; i++ ){
		m_pPoly[i] = POLY2(i, poly1, poly2);
    }
	for( i = 0; i < VITERBI_PATHMAX; i++ ){
		m_pMetrics[i] = new int[m_States];
		m_pHistory[i] = new int[m_States];
	}
	for( i = 0; i < 256; i++ ){			// s = 0 to 255
		m_tMetrics[0][i] = 128 - i;		// 128 to -127
		m_tMetrics[1][i] = i - 128;		// -128 to 127
	}
	m_CurPtr = 0;
    Reset();
}
//--------------------------------------------------------------------------
BOOL __fastcall CViterbi::SetTraceBack(int traceback)

{
	if( (traceback < 0) || (traceback > (VITERBI_PATHMAX - 1)) ) return FALSE;
	m_TraceBack = traceback;
	return TRUE;
}
//--------------------------------------------------------------------------
BOOL __fastcall CViterbi::SetBlockSize(int blocksize)
{
	if( (blocksize < 1) || (blocksize > m_TraceBack) ) return FALSE;
	m_BlockSize = blocksize;
	return TRUE;
}
//--------------------------------------------------------------------------
void __fastcall CViterbi::Reset(void)
{
	if( !m_pMetrics[0] ) return;

	memset(m_tSeq, 0, sizeof(m_tSeq));
	int i;
	for( i = 0; i < VITERBI_PATHMAX; i++) {
		memset(m_pMetrics[i], 0, m_States * sizeof(int));
		memset(m_pHistory[i], 0, m_States * sizeof(int));
	}
	m_CurPtr = 0;
}
//--------------------------------------------------------------------------
int __fastcall CViterbi::TraceBack(int *pMetric)
{
	int i;

	UINT pn = CIRCULATE(m_CurPtr - 1, VITERBI_PATHMAX);
	int *ip = m_pMetrics[pn];
	int MaxMetric = INT_MIN;
	int MaxState = 0;
	for( i = 0; i < m_States; i++, ip++ ){
		if( *ip > MaxMetric ){
			MaxState = i;
			MaxMetric = *ip;
		}
	}

	m_tSeq[pn] = MaxState;
	for( i = 0; i < m_TraceBack; i++ ){
		UINT prev = CIRCULATE(pn - 1, VITERBI_PATHMAX);
		m_tSeq[prev] = m_pHistory[pn][m_tSeq[pn]];
		pn = prev;
	}
	if( pMetric ) *pMetric = m_pMetrics[pn][m_tSeq[pn]];

	UINT c = 0;
	for( i = 0; i < m_BlockSize; i++ ){
		c = (c << 1) | (m_tSeq[pn] & 1);
		pn = CIRCULATE(pn + 1, VITERBI_PATHMAX);
	}
	if( pMetric ) *pMetric = m_pMetrics[pn][m_tSeq[pn]] - *pMetric;

	return c;
}
//--------------------------------------------------------------------------
// s[0] = 1st, s[1] = 2nd
int __fastcall CViterbi::Decode(int *pMetric, BYTE s[2])
{
	int 	i, j, m[4], n;

	UINT curp = m_CurPtr;
	UINT prevp = CIRCULATE(curp - 1, VITERBI_PATHMAX);

	m[0] = m_tMetrics[0][s[0]] + m_tMetrics[0][s[1]];
	m[1] = m_tMetrics[0][s[0]] + m_tMetrics[1][s[1]];
	m[2] = m_tMetrics[1][s[0]] + m_tMetrics[0][s[1]];
	m[3] = m_tMetrics[1][s[0]] + m_tMetrics[1][s[1]];

	int *mp = m_pMetrics[curp];
    int *hp = m_pHistory[curp];
	for( n = 0; n < m_States; n++ ){
		int p0, p1, s0, s1, m0, m1;

		s0 = n; s1 = n + m_States;
		p0 = s0 >> 1; p1 = s1 >> 1;
		m0 = m_pMetrics[prevp][p0] + m[m_pPoly[s0]];
		m1 = m_pMetrics[prevp][p1] + m[m_pPoly[s1]];
		if( m0 > m1 ){
			*mp++ = m0;
			*hp++ = p0;
		}
        else {
			*mp++ = m1;
			*hp++ = p1;
		}
	}
	m_CurPtr = CIRCULATE(m_CurPtr + 1, VITERBI_PATHMAX);
	if( !(m_CurPtr % m_BlockSize) ) return TraceBack(pMetric);

	if( m_pMetrics[curp][0] > (INT_MAX / 2) ){
		for( i = 0; i < VITERBI_PATHMAX; i++ ){
			mp = m_pMetrics[i];
			for( j = 0; j < m_States; j++ ){
				*mp++ -= INT_MAX / 2;
            }
        }
	}
	if( m_pMetrics[curp][0] < (INT_MIN / 2) ){
		for( i = 0; i < VITERBI_PATHMAX; i++){
			mp = m_pMetrics[i];
			for( j = 0; j < m_States; j++){
				*mp++ += INT_MIN / 2;
            }
        }
	}
	return -1;
}

/*=============================================================================
  CInterLeaverクラス

           1   2   3   4   5   6   7   8   9   0   1   2   3   4
To encode: ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyz--------------------------------------------------------------------------------------
Encoded:   A   E   I   M   Q   U   Y   2   6   a   eB  iF  mJ  qN  uR  yV  -Z  -3  -7  -b  -fC -jG -nK -rO -vS -zW --0 --4 --8 --c --gD--kH--oL--sP--wT---X---1

1234567890 12
-------------
AEIMQUY26a ei
BFJNRVZ37b fj
CGKOSW048c gk
DHLPTX159d hl

=============================================================================*/
//--------------------------------------------------------------------------
__fastcall CInterLeaver::CInterLeaver(void)
{
	Init(4);
}
//--------------------------------------------------------------------------
void __fastcall CInterLeaver::Init(int bitsize)
{
#if DEBUG
	if( bitsize > MFSK_MAXBITS ){
    	Application->MainForm->Caption = "Over BitSize in InterLeaver";
		return;
    }
#endif
	m_Stages = 10;
	m_BitSize = bitsize;
    m_BitSizeM = bitsize - 1;
	if( bitsize == 5 ){			// for comminucate with Stream on MFSK8
		m_Stages = 5;
    }
    Reset();
}
//--------------------------------------------------------------------------
void __fastcall CInterLeaver::Reset(void)
{
    memset(m_tTable, 0, sizeof(m_tTable));
}
//--------------------------------------------------------------------------
void __fastcall CInterLeaver::Bits2Syms(BYTE *pSyms, int b)
{
	for( int i = 0; i < m_BitSize; i++ ){
    	*pSyms++ = BYTE(b & _tBitData[m_BitSizeM-i]);
    }
}
//--------------------------------------------------------------------------
int __fastcall CInterLeaver::Syms2Bits(BYTE *pSyms)
{
	int b = 0;
	for( int i = 0; i < m_BitSize; i++ ){
    	b = b << 1;
        if( *pSyms++ ) b |= 1;
    }
    return b;
}
//--------------------------------------------------------------------------
// MFSK16 = pSyms[4],  MFSK8 = pSyms[5],  MFSK31 = pSym[3]
void __fastcall CInterLeaver::DecodeSyms(BYTE *pSyms)
{
	for( int N = 0; N < m_Stages; N++ ){
		int i;
		for( i = 0; i < m_BitSize; i++ ){
			for( int j = 0; j < m_BitSizeM; j++ ) m_tTable[N][i][j] = m_tTable[N][i][j+1];
		}
		for(i = 0; i < m_BitSize; i++) m_tTable[N][i][m_BitSizeM] = pSyms[i];
		for(i = 0; i < m_BitSize; i++) pSyms[i] = m_tTable[N][i][i];
	}
}
//--------------------------------------------------------------------------
// MFSK16 = pSyms[4],  MFSK8 = pSyms[5],  MFSK31 = pSym[3]
void __fastcall CInterLeaver::EncodeSyms(BYTE *pSyms)
{
	for( int N = 0; N < m_Stages; N++ ){
		int i;
		for( i = 0; i < m_BitSize; i++ ){
			for( int j = 0; j < m_BitSizeM; j++ ) m_tTable[N][i][j] = m_tTable[N][i][j+1];
		}
		for(i = 0; i < m_BitSize; i++) m_tTable[N][i][m_BitSizeM] = pSyms[i];
		for(i = 0; i < m_BitSize; i++) pSyms[i] = m_tTable[N][i][m_BitSizeM-i];
	}
}
//--------------------------------------------------------------------------
// MFSK8 = b4 to b0,  MFSK16 = b3 to b0
int __fastcall CInterLeaver::DecodeBits(int b)
{
	BYTE tSyms[MFSK_MAXBITS];

	Bits2Syms(tSyms, b);
    DecodeSyms(tSyms);
    return Syms2Bits(tSyms);
}
//--------------------------------------------------------------------------
// MFSK8 = b4 to b0,  MFSK16 = b3 to b0
int __fastcall CInterLeaver::EncodeBits(int b)
{
	BYTE tSyms[MFSK_MAXBITS];

	Bits2Syms(tSyms, b);
	EncodeSyms(tSyms);
    return Syms2Bits(tSyms);
}
//--------------------------------------------------------------------------

