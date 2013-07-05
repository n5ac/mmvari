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
#ifndef DSPH
#define DSPH
#include <stdio.h>
#include <math.h>

#define	DECFIR	FALSE

#include "ComLib.h"
#include "mfsk.h"
//---------------------------------------------------------------------------
extern int  SAMPBASE;
extern int	SAMPTYPE;
extern double  SAMPFREQ;
extern double  DEMSAMPFREQ;
extern double  SAMPTXOFFSET;
extern int g_tBpfTaps[];
extern const DWORD _tBitData[];

inline double ATAN2(double x, double y){
	return y ? atan2(x, y) : 0.0;
}
void __fastcall DoAvg(double &av, double in, double factor);

enum {
	MODE_GMSK,
    MODE_FSK,
	MODE_FSKW,
    MODE_BPSK,
    MODE_N_BPSK,
    MODE_RTTY,
    MODE_U_RTTY,
    MODE_mfsk_L,
    MODE_mfsk_U,
	MODE_qpsk_L,
    MODE_qpsk_U,
    MODE_END,
};
//---------------------------------------------------------------------------
BOOL __fastcall IsRTTY(int m);
BOOL __fastcall Is170(int m);
BOOL __fastcall IsBPSK(int m);
BOOL __fastcall IsFSK(int m);
BOOL __fastcall IsMFSK(int m);
BOOL __fastcall IsQPSK(int m);
BOOL __fastcall IsPSK(int m);
#define	MIN_SAMP	5000
#define	MAX_SAMP	51000
//---------------------------------------------------------------------------
#define	CARRIERFREQ		1750
#define	SPEED		31.25
#define	MIN_SPEED	15
#define	MAX_SPEED	300
//---------------------------------------------------------------------------
#define	PSK_OUTFAC	0.70710678118654752440084436210485
//---------------------------------------------------------------------------
#define	MIN_CARRIER	250
//#define	MAX_CARRIER	2700
//---------------------------------------------------------------------------
#define	PI			3.1415926535897932384626433832795
#define	MULI		0.000030517578125
#define	ABS(c)		(((c)<0)?(-(c)):(c))
#define	CIRCULATE(A,B)	((A)%(B))
//--------------------------------------------------------------------------
inline double __fastcall Limit(double x, double min, double max)
{
	if( x < min ) return min;
    if( x > max ) return max;
    return x;
}
//---------------------------------------------------------------------------
#define	FIFOMAX	256
class CFifo
{
private:
	int		m_rc;
	int		m_wc;
	int		m_cc;
	int		m_bf[FIFOMAX];
	int		m_BusyLimit;

public:
	CFifo(void){
		m_cc = m_rc = m_wc = 0;
		m_BusyLimit = FIFOMAX / 2;
	};
	void __fastcall Clear(void){
		m_cc = m_rc = m_wc = 0;
	};
	void __fastcall PutData(int c){
		m_bf[m_wc] = c;
		m_wc++;
		if( m_wc >= FIFOMAX ) m_wc = 0;
		m_cc++;
	};
	int __fastcall GetData(void){
		int c = m_bf[m_rc];
		m_rc++;
		if( m_rc >= FIFOMAX ) m_rc = 0;
		m_cc--;
		return c;
	};
	int __fastcall GetCount(void){
		return m_cc;
	}
	int __fastcall IsBusy(void){
		return (m_cc < m_BusyLimit) ? FALSE : TRUE;
	}
};
//---------------------------------------------------------------------------
#define	TAPMAX		512
enum {
	ffLPF,
	ffHPF,
	ffBPF,
	ffBEF,
	ffGAUSSIAN,
};
typedef struct {
	int		n;
	int		typ;
	double	fs;
	double	fcl;
	double	fch;
	double	att;
	double	gain;
	double	fc;
	double	hp[TAPMAX+1];		/* 係数配列		*/
}FIR;
//---------------------------------------------------------------------------
class CSinTable
{
public:
	double	*m_tSin;
	int		m_Size;
public:
	__fastcall CSinTable();
    __fastcall ~CSinTable();
};
//---------------------------------------------------------------------------
class CVARICODE
{
public:
	int		m_TMax;
    int		m_Max;
	DWORD	*m_tEncode;
    BYTE	m_tIndex2Mbcs[128];
public:
	UINT __fastcall Index2Mbcs(int index, BOOL fJA);
	int __fastcall Mbcs2Index(UINT mbcs, BOOL fJA);

public:
	__fastcall CVARICODE();
    __fastcall ~CVARICODE();
	void __fastcall Init(void);
	void __fastcall Create(int max);
    DWORD __fastcall Encode(int &n, DWORD d);
    int __fastcall Decode(DWORD d);

	void __fastcall SaveTable(LPCSTR pName);
	void __fastcall SaveSource(LPCSTR pName);
    void __fastcall SaveBin(LPCSTR pName);
	BOOL __fastcall LoadBin(LPCSTR pName);

	int __fastcall DecodeMFSK(DWORD d);
	int __fastcall EncodeMFSK(BYTE c);
};
//---------------------------------------------------------------------------
class CBAUDOT
{
private:
	int		m_OutFig;
    int		m_CodeB4;
private:
	int __fastcall GetOneCode(int &fig, int code);
	int __fastcall DblCode(int code);

public:
	__fastcall CBAUDOT();
	__fastcall ~CBAUDOT();

	int __fastcall GetCode(int &count, int code);
	inline void __fastcall Reset(void){m_OutFig = -1; m_CodeB4 = 0;};
};
//---------------------------------------------------------------------------
#pragma option -a-	// パックの指示
typedef struct {
	BYTE	Code;
	BYTE	Fig;
}BCODETBL;
#pragma option -a.	// パック解除の指示

class CRTTY
{
private:
	int m_outfig;
	BCODETBL	m_TBL[24*4];

public:
	int	m_CodeSet;
	int	m_fig;
	int	m_uos;

	int	m_txuos;
	CRTTY();
	char ConvAscii(int d);
	int ConvRTTY(char d);
	int ConvRTTY(BYTE *t, LPCSTR p);
	inline int IsChar(int d){
		d &= 0x000000ff;
		if( !d ) return 0;
		if( d == 0x000000ff ) return 0;
		return 1;
	};
	inline void ClearTX(void){m_outfig = 3;};
	inline void ClearRX(void){m_fig = 0;};

	int GetShift(char d);
	char InvShift(char c);
	void SetCodeSet(void);
};

//---------------------------------------------------------------------------
class CBFifo
{
private:
	int		m_WP;
    int		m_RP;
    int		m_CNT;
    int		m_MAX;
    BOOL	*m_pBase;
    BOOL	m_D;
public:
	__fastcall CBFifo();
	__fastcall ~CBFifo();

    void __fastcall Create(int max);
	void __fastcall PutFlag(BOOL f);
    BOOL __fastcall GetFlag(void);
};


//---------------------------------------------------------------------------
class CVCO
{
private:
	double	m_c1;	// VCOの利得
	double	m_c2;	// フリーランニング周波数
	double	m_z;

	double	m_FreeFreq;
	double	m_SampleFreq;
	int		m_TableSize;
    double	m_TableCOS;
public:
	__fastcall CVCO();
	__fastcall ~CVCO();
	void __fastcall InitPhase(void);
	void __fastcall SetGain(double gain);
	void __fastcall SetSampleFreq(double f);
	void __fastcall SetFreeFreq(double f);
	double __fastcall Do(void);
	double __fastcall Do(double d);
	double __fastcall DoCos(void);
};

//---------------------------------------------------------------------------
#define	IIRMAX	16
#define	IIRVLIMIT	(1.0e-37)
class CIIR
{
public:
	double	m_rp;
	double	m_A[IIRMAX*2];
	double	m_B[IIRMAX*3];
	int		m_order;
	int		m_bc;
private:
	double	m_Z[IIRMAX*3];
    int		m_orderH;
public:
	__fastcall CIIR();
	__fastcall ~CIIR();
	void __fastcall Create(int type, double fc, double fs, int order, int bc, double rp);
	double __fastcall Do(double d);
	void __fastcall Clear(void);
};

//---------------------------------------------------------------------------
class CIIRTANK
{
private:
	double	z1, z2;
public:
	double	a0;
	double	b1, b2;

    double	m_Freq;
    double	m_BW;
    double	m_SampleFreq;
private:
	void __fastcall SetFreq(double f, double smp, double bw);
public:
	__fastcall CIIRTANK();
	inline void __fastcall SetSampleFreq(double f){m_SampleFreq = f; Create();};
	inline void __fastcall SetFreq(double f){m_Freq = f; Create();};
	void __fastcall Create(void);
	double __fastcall Do(double d);
};

class CIIRTANK2
{
public:
    double	m_Freq;
    double	m_BW;
    double	m_SampleFreq;
private:
	CIIRTANK	m_Tank1;
    CIIRTANK	m_Tank2;

public:
	__fastcall CIIRTANK2();
	void __fastcall SetSampleFreq(double f);
	void __fastcall Create(void);
    double __fastcall Do(double d);
    inline CIIRTANK *__fastcall GetTank1(void){return &m_Tank1;};
};
//---------------------------------------------------------------------------
class CFIR
{
private:
	int		m_Tap;
	double	*m_pZ;
    double	*m_pH;
public:
	__fastcall CFIR();
    __fastcall ~CFIR();
	void __fastcall Create(int tap, int type, double fs, double fcl, double fch, double att, double gain);
	double __fastcall Do(double d);
    void __fastcall SaveCoef(LPCSTR pName);
    inline double __fastcall GetHD(int n){return m_pH[n];};
    inline double *__fastcall GetHP(void){return m_pH;};
    inline int __fastcall GetTap(void){return m_Tap;};
};

//---------------------------------------------------------------------------
// ダブルバッファによるFIRフィルタ
class CFIR2
{
private:
	int		m_Tap;
    int		m_TapHalf;
	double	*m_pZ;
    double	*m_pH;
	double	*m_pZP;

    int		m_W;
    double	m_fs;
public:
	__fastcall CFIR2();
    __fastcall ~CFIR2();
	void __fastcall Delete(void);
	void __fastcall Clear(void);
	void __fastcall Create(int tap, int type, double fs, double fcl, double fch, double att, double gain);
	void __fastcall Create(int tap, double fs, double fcl, double fch);
	void __fastcall CreateSamp(int tap, double fs, const double *pSmpFQ);
	double __fastcall Do(double d);
	double __fastcall Do(double *hp);
	void __fastcall Do(CLX &z, double d);

    inline double __fastcall GetHD(int n){return m_pH[n];};
    inline double *__fastcall GetHP(void){return m_pH;};
    inline int __fastcall GetTap(void){return m_Tap;};
    inline double __fastcall GetSampleFreq(void){return m_fs;};
};
//---------------------------------------------------------------------------
// ダブルバッファによるFIRフィルタ(複素数用)
class CFIRX
{
private:
	int		m_Tap;
    int		m_TapHalf;
	CLX		*m_pZ;
    double	*m_pH;
	CLX		*m_pZP;

    int		m_W;
    double	m_fs;
public:
	__fastcall CFIRX();
    __fastcall ~CFIRX();
	void __fastcall Clear(void);
	void __fastcall Create(int tap, int type, double fs, double fcl, double fch, double att, double gain);
	void __fastcall CreateSamp(int tap, double fs, const double *pSmpFQ, int wDB);
	void __fastcall Do(CLX &d);

    inline double __fastcall GetHD(int n){return m_pH[n];};
    inline double *__fastcall GetHP(void){return m_pH;};
    inline int __fastcall GetTap(void){return m_Tap;};
    inline double __fastcall GetSampleFreq(void){return m_fs;};
};
//---------------------------------------------------------------------------
// Copy from MMTTY by JE3HHT on Sep.2010
class CSmooz{
private:
	double	*bp;
	int	Wp;
	int	Max;
	int	Cnt;
	void __fastcall IncWp(void){
		Wp++;
		if( Wp >= Max ) Wp = 0;
	};
	double __fastcall Avg(void){
		double	d = 0.0;
		int		i;
		for( i = 0; i < Cnt; i++ ){
			d += bp[i];
		}
		if( Cnt ){
			return d/double(Cnt);
		}
		else {
			return 0;
		}
	};
public:
	__fastcall CSmooz(int max = 70){
		Max = max;
		bp = new double[max];
		Cnt = 0;
		Wp = 0;
	};
	__fastcall ~CSmooz(void){
		delete bp;
	};
	void __fastcall SetCount(int n){
		double *np = new double[n];
		Max = n;
		Cnt = Wp = 0;
		double *op = bp;
		bp = np;
		delete op;
	};
	double __fastcall Avg(double d){
		bp[Wp] = d;
		IncWp();
		if( Cnt < Max ){
			Cnt++;
		}
		return Avg();
	};
};
//---------------------------------------------------------------------------
// スライディングFFT
class CSlideFFT
{
private:
	int 	m_Length;
	int 	m_Base;
    int		m_Tones;

	CLX		m_tData[MFSK_MAXTONES];
	CLX 	m_tWindow[MFSK_MAXTONES];

    CLX		*m_pCur;
    CLX		*m_pEnd;
	CLX		*m_pBase;

	double	m_kWindow;
private:
public:
	__fastcall CSlideFFT();
	__fastcall ~CSlideFFT();

	void __fastcall Create(int len, int base, int tones);
	CLX* __fastcall Do(const CLX &zIn);
};
//---------------------------------------------------------------------------
// 遅延器
class CDelay
{
private:
	int		m_Delay;
	int		m_CurPnt;
    int		m_Count;
    double	*m_pStg;

public:
	__fastcall CDelay(void);
    __fastcall ~CDelay();

	void __fastcall Create(int delay);
	void __fastcall Reset(void);
	double __fastcall Do(const double &d);
	double __fastcall GetData(void);
    inline BOOL __fastcall IsFull(void){return m_Count >= m_Delay;};
};
//---------------------------------------------------------------------------
// CICフィルタ
class CCIC
{
private:
	int		m_N;
	double	m_K;
	CDelay	m_Com;
    double	m_Z;
public:
	__fastcall CCIC(void);

	void __fastcall Create(int n);
	double __fastcall Do(double d);
};
//---------------------------------------------------------------------------
// 高次のCICフィルタ
class CCICM
{
private:
	int		m_N;
    CCIC	m_tCIC[16];
public:
	__fastcall CCICM(void);
	__fastcall ~CCICM();

    void __fastcall Create(int stages, int N);
    double __fastcall Do(double d);
};
//---------------------------------------------------------------------------
class CFAVG
{
private:
    double	m_Sum;
    double	m_Avg;
    double	m_Mul;
	int		m_Max;
    int		m_Cnt;
public:
	__fastcall CFAVG();
	void __fastcall Reset(void);
	void __fastcall Reset(double d);
    void __fastcall Create(int max);
	double __fastcall DoZ(double d);
	double __fastcall Do(double d);
    inline double __fastcall GetAvg(void){return m_Avg;};
    inline BOOL __fastcall IsFull(void){return m_Max == m_Cnt;};
    inline BOOL __fastcall IsHalf(void){return m_Max <= (m_Cnt*2);};
    inline int __fastcall GetCount(void){return m_Cnt;};
};
//---------------------------------------------------------------------------
class CAMPCONT
{
private:
	double	m_Max;
    double	m_Cnt;
    double	m_ADD;
    int		m_S;
    int		m_iMax;
public:
	__fastcall CAMPCONT();
	void __fastcall SetMax(int max);
    void __fastcall Reset(void);
    double __fastcall Do(int d);
};
//---------------------------------------------------------------------------
class CAVG
{
private:
	int		m_Max;
    double	m_Sum;
    double	m_Avg;
    double	*m_pZ;
    int		m_W;
    int		m_Cnt;

public:
	__fastcall CAVG();
    __fastcall ~CAVG();
	void __fastcall Create(int max);
	double __fastcall Do(double d);
    inline BOOL __fastcall IsFull(void){return m_Cnt >= m_Max;};
    void __fastcall Reset(double d);
    void __fastcall Reset(void);
    inline double __fastcall GetAvg(void){return m_Avg;};
    inline BOOL __fastcall IsCreate(void){return m_pZ != NULL;};
    inline BOOL __fastcall IsHalf(void){return m_Max <= (m_Cnt*2);};
};
//---------------------------------------------------------------------------
class CDECM2{
private:
    double	m_O;
	int		m_Count;
#if DECFIR
	CFIR2	m_FIR;
#else
    CIIR	m_IIR;
#endif
public:
    int		m_Type;
	double	m_SampleFreq;
public:
	__fastcall CDECM2();
    void __fastcall SetSampleFreq(int type, double f);
    BOOL __fastcall Do(double d);
    inline double __fastcall GetOut(void){return m_O;};
#if DECFIR
    inline CFIR2* __fastcall GetFIR(void){return &m_FIR;};
#else
    inline CIIR* __fastcall GetIIR(void){return &m_IIR;};
#endif
    inline void __fastcall SetOut(double d){m_O = d;};
};

//--------------------------------------------------------
// CAGCクラス
class CAGC
{
private:
	double  m_Max;
	double  m_Min;
	double  m_d;
	double  m_agc;

	double	m_fc;
	double	m_Gain;
    CIIR	m_LPF;
//    CIIR	m_Level;
    int		m_Count;
    int		m_TLimit;
    CFAVG	m_AvgOver;
public:
    double	m_LimitGain;
	double	m_MonitorFreq;
	double	m_CarrierFreq;
    double	m_SampleFreq;
public:
	__fastcall CAGC();
    void __fastcall Create(void);
	inline void __fastcall SetSampleFreq(double f){m_SampleFreq = f; Create();};
	void __fastcall SetCarrierFreq(double f);
    void __fastcall SetFC(double fc);
    void __fastcall Reset(void);
    double __fastcall Do(double d);
	inline void __fastcall ResetOver(void){m_AvgOver.Reset(1.0);};
    BOOL __fastcall GetOver(void);
};

#if DEBUG
//---------------------------------------------------------------------------
class CQSB
{
private:
	int		m_vMax, m_vMin;
	BOOL	m_fPhaseError;
    CVCO	m_VCO;
    CVCO	m_VCOP;
    int		m_tap;
    double	m_Z[TAPMAX];
public:
	__fastcall CQSB();
	__fastcall ~CQSB();
    void __fastcall Create(int min, int max, int msec, BOOL perr);
	int __fastcall Do(void);
	double __fastcall Do(double d);
};

//---------------------------------------------------------------------------
class CNoise		// M系列ノイズ N=22 (Tap=1)
{
private:
    double	m_SampleFreq;

	DWORD	m_reg;
	CIIR	m_LPF;
    CIIR	m_HPF;
public:
	__fastcall CNoise();
	void __fastcall SetSampleFreq(double f);
	void __fastcall Create(double f1, double f2);
	DWORD __fastcall Do(void);
    double __fastcall DoLPF(void);
    double __fastcall DoHPF(void);
};
#endif

/*=============================================================================
  CPHASEXクラス Added by JE3HHT on Sep.2010 for the new RTTY demodualtor
=============================================================================*/
#define	CPHASEX_BASEFREQ	0.0
class CPHASEX
{
public:
//	CFIRX	m_LPF;
private:
	CVCO	m_VCO;
    CFIR2	m_Hilbert;
    CAGC	m_AGC;

    FILE	*m_fp;
public:
    CLX		m_sig;
	CSlideFFT	m_SlideFFT;

    int			m_TONES;
	double		m_SHIFT;
    double		m_fftSHIFT;
	int			m_BASEPOINT;

	double	m_MixerFreq;
    double	m_SymbolLen;
	double	m_CarrierFreq;
	double	m_SampleFreq;

    double	m_dm;
    double	m_ds;
private:
//	void __fastcall CreateFilter(void);
public:
	__fastcall CPHASEX();
    void __fastcall Create(void);
    void __fastcall SetSampleFreq(double f);
	void __fastcall SetCarrierFreq(double f);
	inline void __fastcall AddCarrierFreq(double f){
		SetCarrierFreq(m_CarrierFreq + f);
    };
	CLX* __fastcall Do(double d);
    inline CFIR2* __fastcall GetFIR(void){return &m_Hilbert;};

	void __fastcall DoFSK(double d);
    void __fastcall SetShift(double f);
	void __fastcall ShowPara(void);

    inline void SetTones(int n){
		m_TONES = n;
        Create();
    }
};

//---------------------------------------------------------------------------
// CLMSクラス
#define LMSTAP  192
class CLMS
{
private:
	double	Z[LMSTAP+1];		// FIR Z Application
	double	D[LMSTAP+1];		// LMS Delay;

	double	m_lmsADJSC;			// スケール調整値
	double  m_D;
public:
	int		m_Tap;
	int		m_lmsDelay;			// LMS Delay
	int		m_lmsAGC;			// LMS AGC
	double	m_lmsMU2;			// LMS 2μ
	double	m_lmsGM;			// LMS γ
	double	H[LMSTAP+1];		// アプリケーションフィルタの係数
public:
	__fastcall CLMS();

	double __fastcall Do(double d);
};

class CAFC
{
private:
public:
	BOOL	m_fChange;
    int		m_Count;
    int		m_LCount1;
    int		m_LCount2;

	CAVG	m_Avg;
	double	m_d;
	double	m_min;
    double	m_max;
    double	m_fq;
    CIIR	m_LPF;
public:
	double	m_Max, m_Min;
    double	m_A;
    double	m_B;
	double	m_Speed;
	double	m_SampleFreq;
public:
	__fastcall CAFC();
	void __fastcall SetPara(double a, double b);
	void __fastcall SetTap(int tap);
	void __fastcall SetTone(double fq);
    BOOL __fastcall Do(double d);
    double __fastcall GetFreq(void){return m_fq;};
};

//---------------------------------------------------------------------------
enum {
    diddleLTR,
    diddleBLK,
};
class CENCODE
{
public:
	DWORD	m_Z[32];

	int		m_Type;
	int		m_Mode;
    int		m_cData;
    int		m_cBCount;
	int		m_out;
    int		m_outbit;
	int		m_sync;
    int		m_Idle;
	BOOL	m_fJA;
	BOOL	m_fTWO;

    int		m_Cnt;
	double	m_dCW;
    double	m_dTW;
    double	m_dNow;
    double	m_dNext;

    double	m_Speed;
    double	m_SampleFreq;

    BOOL	m_fCW;
	BOOL	m_Mark;
    BOOL	m_fReqRX;

    int		m_Code;

	int		m_rttyDiddle;
    int		m_rttyCWait;
    int		m_rttyDWait;
    CBAUDOT	m_BAUDOT;

    int		m_Buff[1024];
	int		m_WP;
    int		m_RP;
    int		m_CC;

    int __fastcall (*m_pFunc)(void);
    CVCO	*m_pVCO;

    BOOL	m_fChar;

    // Added for MFSK
	int		m_MFSK_TYPE;
	int		m_MFSK_TONES;
	double	m_MFSK_SPEED;
	int		m_MFSK_BITS;
    UINT	m_MFSK_MASK;

	CViterbiEncode	m_Viterbi;
	UINT	m_MFSK_SHDATA;
	int 	m_MFSK_SHCount;
    CFifo	m_Fifo;
	CInterLeaver	m_InterLeaver;

private:
    int __fastcall GetChar(void);

	void __fastcall MFSKSendBit(BOOL bit);
	void __fastcall MFSKSendPair(BYTE pair);
	void __fastcall MFSKSendSymBit(BOOL bit);
	void __fastcall MFSKSendChar(int c);
	void __fastcall MFSKSendIdle(int n);
	void __fastcall MFSKSendIdle(void);

	void __fastcall MFSKSendIdleChar(void);

public:
	__fastcall CENCODE();
	inline void __fastcall SetVCO(CVCO *pVCO){m_pVCO = pVCO;};
	void __fastcall SetSampleFreq(double f){m_SampleFreq = f; Create();};
	void __fastcall Create(void);
	void __fastcall SetSpeed(double b){m_Speed = b; Create();};
    void __fastcall Reset(BOOL fCW);
    int __fastcall Do(void);
    void __fastcall SetTmg(double d);
	void __fastcall SetCW(int f);

	void __fastcall PutChar(int c);
    BOOL __fastcall IsBuffFull(void);
    inline int __fastcall GetBuffCount(void){return m_CC;};
    inline void __fastcall SetType(int type){m_Type = type;};
    inline void __fastcall SetDiddle(int diddle){m_rttyDiddle = diddle;};
    inline int __fastcall GetDiddle(void){return m_rttyDiddle;};
    inline void __fastcall SetMark(void){m_Mark = TRUE;};
    inline void __fastcall SetWaitC(int n){m_rttyCWait = n;};
    inline void __fastcall SetWaitD(int n){m_rttyDWait = n;};
	inline int __fastcall GetWaitC(void){return m_rttyCWait;};
	inline int __fastcall GetWaitD(void){return m_rttyDWait;};
	void __fastcall SetMFSKType(int type);
};

//---------------------------------------------------------------------------
// CDECFSKクラス
class CDECFSK
{
private:
public:
	int		m_Type;

	int		m_cData;
    int		m_cBCount;
    int		m_cTCount;
    int		m_cMode;
	int		m_s;
	int		m_a;

    double	m_dTW;
    double	m_dTWH;
    double	m_dBTW;
    double	m_dBTWL;
    double	m_dBTWH;
    double	m_dNow;
    double	m_dNext;
    double	m_dFree;

	int		m_cBWave;
	int		m_T;
    int		m_T2;
    int		m_T3;
    int		m_Error;

    int		m_ATCCounter;
    int		m_ATCLimitH;
    int		m_ATCLimitL;

	double	m_Threshold;
	double	m_GainA;
    double	m_GainB;
    CAVG	m_LPF;
    CAVG	m_LPFI;
    double	m_A;

	BOOL	m_fSync;
    CRTTY	m_BAUDOT;
	CFAVG	m_AvgRTTY;

	BOOL	m_fMeasClock;
    int		m_MeasCounter;
    int		m_MeasClock;
	int		m_MeasError;
    int		m_Meas1st;
public:
	int		m_Lock;
	double	m_dTmg3;
	double	m_dTmg2;
	double	m_dTmg;
	int		m_Tmg;
	int		m_Data;
	double	m_dAdj;			// タイミング補正(ppm)
    BOOL	m_fATC;
    int		m_ATCSpeed;
    int		m_ATCLimit;

	double	m_Speed;		// 伝送速度(baud)
    double	m_SampleFreq;	// Hz
private:
	BOOL __fastcall DoBAUDOT(int s, BOOL sq);

public:
	__fastcall CDECFSK();
    void __fastcall Create(void);
	inline void __fastcall SetSpeed(double b){m_Speed = b; Create();};
    BOOL __fastcall Do(double d, BOOL sq, BOOL fATC);
    inline void __fastcall SetSampleFreq(double f){
		m_SampleFreq = f;
        Create();
    };
	void __fastcall ClearLPF(void);
	void __fastcall Reset(void);
	void __fastcall SetMeasClock(BOOL f);
    void __fastcall SetTmg(int ppm);
    inline int __fastcall GetData(void){int r = m_Data; m_Data = -1; return r;};
    inline int __fastcall GetTmg(void){ return m_Tmg ? 1 : -1;};
	void __fastcall CalcIntval(double d);
    BOOL __fastcall GetSyncState(void);
	void __fastcall SetATCSpeed(int atcspeed);
	void __fastcall SetATCLimit(int atclimit);

    inline void __fastcall SetType(int type){m_Type = type;};
	inline void __fastcall SetUOS(int type){
		if( type == 2 ){
			m_BAUDOT.m_uos = !m_BAUDOT.m_uos;
        }
        else {
			m_BAUDOT.m_uos = type;
        }
    };
    inline BOOL __fastcall GetUOS(void){return m_BAUDOT.m_uos;};
	inline BOOL __fastcall IsRTTYTmg(void){return m_AvgRTTY.IsHalf();};
    inline double __fastcall GetRTTYTmg(void){
		return m_AvgRTTY.GetAvg() * 1000.0 / m_SampleFreq;
    };
    inline void __fastcall ResetMeasRTTY(void){m_AvgRTTY.Reset();};
};

/*=============================================================================
 CViterbiQPSKクラス Added by JE3HHT on Sep.2010 for QPSK
=============================================================================*/
class CViterbiQPSK {
private:
	double	m_tPath[16];
   	int		m_tBit[16];
public:
	CViterbiQPSK(void);
	void __fastcall Reset(void);
	BOOL __fastcall Do(double angle, double *pMetric);
};
/*=============================================================================
  CDecPSKクラス Added by JE3HHT on Sep.2010 for QPSK
=============================================================================*/
#define	QPSK_SUBFACTOR	4	// サブサンプリングファクター
#define	QPSK_LPF2K		0.70710678118654752440084436210485	// sqrt(2)/2
#define	PSK_AMPSTGMAX	int(9000.0/(MIN_SPEED*QPSK_SUBFACTOR))
enum {
	QPSK_ROT_0,
	QPSK_ROT_90,
	QPSK_ROT_180,
	QPSK_ROT_270,
};
class CDecPSK {
public:
	CFIRX		m_LPF1;				// デシメータ
	CFIRX		m_LPF2;				// 信号用フィルタ
	CLX			m_Z;
private:
    CLX			m_sig;
	CVCO		m_VCO;
    CFIR2		m_Hilbert;

	CLX			m_StgZ[4];
	CViterbiQPSK	m_Viterbi;
	double		m_Metric[4];
	double		m_Angle;
	int			m_Bits;
	int			m_SN;

	int			m_nSubCounter;		// サブサンプリング用カウンタ

	int			m_iW;
    int			m_iWM;
	float		m_AmpStg[PSK_AMPSTGMAX];

	UINT		m_SHDATA;

	double		m_dNow;
    double		m_dTW;
    double		m_dBTW;
    double		m_dBTWL, m_dBTWH;

    BOOL		m_fSQ;

	CFAVG		m_AvgClock;
    int			m_MeasClock;
    int			m_MeasCounter;
	int			m_MeasStage;

	double		m_dblAvgAFC;
	double		m_vAvg;

public:
	BOOL		m_SyncState;
	BOOL		m_Tmg;
    BOOL		m_TmgLock;
	CFifo		m_RxData;

	double		m_Speed;
	BOOL		m_bQPSK;
	BOOL		m_fLSB;
    BOOL		m_bATC;

	double		m_NextCarrierFreq;
	double		m_CarrierFreq;
	double		m_SampleFreq;
private:
	void __fastcall RxBit(int bit);
	void __fastcall DoSym(const CLX &z);
	void __fastcall DoSync(CLX &z);
	void __fastcall DoATC(BOOL fATC);
	void __fastcall DoAFC(void);

public:
	__fastcall CDecPSK(void);
    __fastcall ~CDecPSK();

	void __fastcall SetSpeed(double d);
	void __fastcall SetCarrierFreq(double f);
	void __fastcall ResetRX(void);
	void __fastcall Reset(void);
	void __fastcall SetSampleFreq(double f);
	void __fastcall ResetATC(void);
	int __fastcall Do(double d, BOOL fSQ, BOOL fATC);
	int __fastcall GetData(void);

    inline BOOL __fastcall GetSyncState(void){return m_SyncState;};
    inline BOOL __fastcall GetTmgLock(void){return TRUE;};
    inline BOOL __fastcall GetTmg(void){return m_Tmg;};
	int __fastcall GetD(void);
    inline int __fastcall GetS(void){
    	return (m_Angle - PI) * 32768.0 * 0.5 / PI;
//		return m_zSymStg[0].Abs();                                                          1.
    };

    int	__fastcall GetClockError(void);
	void __fastcall SetTmg(int ppm);
    void __fastcall SetSN(int SN){
		m_SN = SN;
    }
};

/*=============================================================================
  CPHASEクラス
=============================================================================*/
#define	MFSK_BASEFREQ	1000.0
class CPHASE
{
private:
	CVCO	m_VCO;
    CFIR2	m_Hilbert;
public:
    CLX		m_sig;
	CSlideFFT	m_SlideFFT;

    int			m_MFSK_TONES;
	double		m_MFSK_SPEED;
	int			m_MFSK_BASEPOINT;

	double	m_MixerFreq;
    double	m_SymbolLen;
	double	m_CarrierFreq;
	double	m_SampleFreq;
private:
public:
	__fastcall CPHASE();
    void __fastcall Create(void);
    void __fastcall SetSampleFreq(double f);
	void __fastcall SetCarrierFreq(double f);
	inline void __fastcall AddCarrierFreq(double f){
		SetCarrierFreq(m_CarrierFreq + f);
    };
	CLX* __fastcall Do(double d);
    inline CFIR2* __fastcall GetFIR(void){return &m_Hilbert;};

	void __fastcall SetMFSKType(int type);
};

/*=============================================================================
  CDecMFSKクラス
=============================================================================*/
#define	AMPSTGMAX	int(9000.0*2.0/MFSK_MINSPEED)	// (MaxDemSamp + ClockAdjust) * 2 / MFSK8_MINSPEED
class CDecMFSK {
private:
	int			m_AmpStgMax;
	int			m_AmpStgPtr;
	double		m_AmpStg[AMPSTGMAX][MFSK_MAXTONES];

	CInterLeaver	m_InterLeaver;
	UINT		m_SHDATA;

	BYTE		m_SymStg[3];

	double		m_AvgMetric;

	double		m_dNow;
    double		m_dTW;
    double		m_dBTW;
    double		m_dBTWL, m_dBTWH;

	CViterbi	m_Viterbi1;
	CViterbi	m_Viterbi2;
	double		m_Metric1;
	double		m_Metric2;
	BYTE		m_ViterbiPair[2];
	UINT 		m_ViterbiPhase;

    BOOL		m_fSQ;
    BOOL		m_fDelaySQ;
	int			m_SQDelayCount;

	CFAVG		m_AvgClock;
    int			m_MeasClock;
    int			m_MeasCounter;
	int			m_MeasStage;

	CFAVG		m_AvgAFC;
    CLX			m_PrevZ;
	double		m_AFCWidth;
    double		m_kAFC;

public:
	BOOL		m_SyncState;
	BOOL		m_Tmg;
    BOOL		m_TmgLock;
	CPHASE		m_Phase;
	CFifo		m_RxData;

	int			m_MFSK_TYPE;
    int			m_MFSK_TONES;
	double		m_MFSK_SPEED;
	int			m_MFSK_BITS;
	double		m_MFSK_BW;

	BOOL		m_fLSB;
	double		m_SampleFreq;
private:
	double __fastcall GetFactor(void);
	void __fastcall StreamSym(BYTE sym);
	void __fastcall RecSym(const double *pAmp);
	BYTE __fastcall DecodeSym(CLX *pFFT, double *pAmp);
	void __fastcall DoSync(void);
	void __fastcall DoMeas(void);
	void __fastcall DoAFC(CLX *pFFT);
public:
	__fastcall CDecMFSK();
	void __fastcall SetSampleFreq(double f);
	void __fastcall SetCarrierFreq(double f);

	int __fastcall Do(CLX *pFFT, BOOL fSQ);
	int __fastcall GetData(void);
    void __fastcall Reset(void);
	void __fastcall ResetMeas(void);

    inline BOOL __fastcall GetSyncState(void){return m_SyncState;};
    inline BOOL __fastcall GetTmgLock(void){return TRUE;};
    inline BOOL __fastcall GetTmg(void){return m_Tmg;};
    inline int __fastcall GetS(void){return m_SymStg[2];};

    int	__fastcall GetClockError(void);
    inline CFIR2* __fastcall GetFIR(void){return m_Phase.GetFIR();};
    inline BOOL __fastcall IsMetSQ(void){
		return (m_AvgMetric >= 100);
    };
    int __fastcall GetMetric(int sw);
	void __fastcall SetTmg(int ppm);

	void __fastcall SetMFSKType(int type);
    BOOL __fastcall GetAFCShift(double &fq);
};


//---------------------------------------------------------------------------
// CDEMFSKクラス
#define	DEMAFCLIMIT		(10*11025/2048)
class CDEMFSK
{
#if DEBUG
public:
	int		m_DEBUG;
#endif
//private:
public:
	double	m_d;
	double	m_err;
    double	m_out;
    double	m_PreBPFFC;
	CFIR2	*m_pBPF;
public:
	CFIR2	m_inBPF;
	CIIR	m_LPF1;
//    CIIR	m_LPF2;
    CIIR	m_LoopLPF;
    CIIR	m_OutLPF;
    CVCO	m_VCO;
    CAGC	m_AGC;
	CAFC	m_AFC;
	CDECFSK	m_Decode;
	CFAVG	m_FAVG;
	CIIRTANK	m_TankL;
    CIIRTANK	m_TankH;
	BOOL	m_fRTTYTANK;
	UINT	m_AFCCount;

	BOOL	m_fRTTYFFT;
	CPHASEX	m_PhaseX;
	CSmooz	m_SmoozM;
	CSmooz	m_SmoozS;

    double	m_BPFLimit;
//	CCICM	m_CIC;

	BOOL	m_fMFSK;
	CDecMFSK	*m_pMFSK;
	BOOL	m_fQPSK;
    CDecPSK	m_DecPSK;
    double	m_CarrierQPSK;
    double	m_RxFreqQPSK;

public:
	int		m_Type;
	double	m_RxFreq;
	int		m_Lock;
	int		m_LockTh;		// 連続キャリア検出のスレッシュホールド
    double	m_FreqErr;
	double	m_DemLevel;

    double	m_Gain;
	double	m_SW;

    BOOL	m_fEnableAFC;
	BOOL	m_fAFC;

	double	m_Speed;
	double	m_CarrierFreq;
    double	m_SampleFreq;
	int		m_PreBPFTaps;
	double	m_RTTYShift;

	int			m_MFSK_TYPE;
    int			m_MFSK_TONES;
	double		m_MFSK_SPEED;
	double		m_MFSK_BW;
	int			m_MFSK_BITS;

#if MEASIMD

public:
	double		m_imd;
	double		m_max;
    double		m_min;
    double		m_amax;
    double		m_amin;
	void __fastcall CalcIMD(void);
#endif
private:
	void __fastcall DoAFC(double d);
	void __fastcall DoAFCPSK(double d);
	void __fastcall DoAFCQPSK(double d);
	void __fastcall DoAFCRTTY(double d);
public:
	__fastcall CDEMFSK();
	__fastcall ~CDEMFSK();
	inline void __fastcall SetSampleFreq(double f){
		m_SampleFreq = f; Create();
    };
	inline void __fastcall SetType(int type){m_Type = type; Create();};
	void __fastcall MakeBPF(int taps);
	void __fastcall UpdateBPF(void);
	void __fastcall SetCarrierFreq(double f);
	inline void __fastcall SetSpeed(double b){m_Speed = b; Create();};
    inline void __fastcall SetRTTYShift(double s){
    	m_RTTYShift = s;
		if( Is170(m_Type) ) Create();
    };
	void __fastcall Create(void);
    int __fastcall Do(double);
	int __fastcall Do(double d, BOOL fSQ, BOOL fATC);
	int __fastcall GetData(void);
	int __fastcall GetTmg(void);
	int __fastcall GetTmgLock(void);
	int __fastcall GetS(void);
	void __fastcall ResetMFSK(void);
	void __fastcall ResetMeasMFSK(void);
	BOOL __fastcall GetSyncState(void);
	int __fastcall GetClockError(void);
	void __fastcall SetTmg(int ppm);

	double __fastcall GetFreqErr(void);
#if 0
    inline double __fastcall GetMFSKAFC(void){
		if( m_pMFSK && m_pMFSK->m_AvgAFC.IsFull() ){
			return m_pMFSK->m_AvgAFC.GetAvg();
        }
        else {
			return 0;
        }
    };
    inline void __fastcall ClearMFSKAFC(void){
		if( m_pMFSK ){
			m_pMFSK->m_AvgAFC.Reset();
        }
    }

#endif
	inline CFIR2* __fastcall GetMFSKHIL(void){
		if( m_pMFSK ){
			return m_pMFSK->GetFIR();
        }
        else {
			return NULL;
        }
    };
    inline BOOL __fastcall IsMFSKSQ(void){
		if( m_pMFSK ){
			return m_pMFSK->IsMetSQ();
        }
        else {
			return FALSE;
        }
    }
    inline int __fastcall GetMFSKMetric(int sw){
		if( m_pMFSK ){
			return m_pMFSK->GetMetric(sw);
        }
        else {
			return 0;
        }
    }

	void __fastcall SetMFSKType(int type);
};

#define	MODTABLEMAX	(MAX_SAMP/MIN_SPEED)
/*
enum {
    MOD_CZ2P,
    MOD_VZ2P,
    MOD_RZ2P,
    MOD_LZ2P,
};
*/
enum {
    MOD_P2M,
    MOD_M2P,
    MOD_Z2P,
    MOD_Z2M,
};
//---------------------------------------------------------------------------
// CDEMFSKクラス
class CMODFSK
{
#if DEBUG
public:
	int		m_DEBUG;
#endif
private:
	int		m_s;
    int		m_Cnt;
    double	m_d;
    double	*m_pTbl;
    double	m_Tbl[4][MODTABLEMAX];
    CFIR2	m_BPF;
	CAMPCONT	m_AMPCW;
    CAMPCONT	m_AMPSIG;
public:
	int		m_OutVol;
	int		m_Type;
	CENCODE	m_Encode;
    CVCO	m_Carrier;
	int		m_CWSpeed;
    BOOL	m_fRTTYLPF;
	double	m_Speed;
    double	m_RTTYShift;
	double	m_CarrierFreq;
    double	m_SampleFreq;

	BOOL		m_fMFSK;
	int			m_MFSK_TYPE;
    int			m_MFSK_TONES;
	double		m_MFSK_SPEED;
	double		m_MFSK_BW;

	double		m_MFSK_K;

	BOOL			m_fQPSK;
	CLX				m_Sym;
	CLX				m_PrevSym;
	BOOL			m_fLSB;
	UINT			m_SHDATA;
private:
	void __fastcall CreateGMSK(void);
	void __fastcall CreatePSK(void);
	void __fastcall CreateRTTY(void);

public:
	__fastcall CMODFSK();
	__fastcall ~CMODFSK();
	inline void __fastcall SetSampleFreq(double f){
		m_SampleFreq = f; Create();
    };
	void __fastcall SetCarrierFreq(double f);
	void __fastcall SetCWSpeed(int d);
    void __fastcall Reset(void);
	inline int __fastcall GetCWSpeed(void){return m_CWSpeed;};
	inline void __fastcall SetSpeed(double b){ m_Speed = b; Create();};
	inline void __fastcall SetType(int type){m_Type = type; Create();};
    inline void __fastcall SetRTTYShift(double s){
    	m_RTTYShift = s;
		if( Is170(m_Type) ) Create();
    };
	void __fastcall Create(void);
	//inline void Reset(void){m_Encode.Reset(FALSE);};

    double __fastcall Do(void);
    double __fastcall DoCarrier(void);

    inline CFIR2* __fastcall GetFIR(void){return &m_BPF;};
	void __fastcall SetMFSKType(int type);
};

//---------------------------------------------------------------------------
class CCOLLECT
{
public:
	int		m_Max;
    int		m_Cnt;
    double	*m_pZ;
public:
	__fastcall CCOLLECT();
	__fastcall ~CCOLLECT();
	void __fastcall Create(int max);
    inline void __fastcall Clear(void){m_Cnt = 0;};
    void __fastcall Do(double d);
	inline double *__fastcall GetZP(void){return m_pZ;};
    inline int __fastcall GetMax(void){return m_Max;};
    inline int __fastcall GetCount(void){return m_Cnt;};
    inline BOOL __fastcall IsFull(void){return m_Cnt >= m_Max;};
};
//---------------------------------------------------------------------------
class CClock
{
public:
	int			m_Width;
    int			*m_pData;
    double		m_dNow;
    double		m_dAdd;

    double		m_ToneFreq;
	double		m_SampleFreq;
    CIIRTANK2	m_BPF;
	CIIR		m_LPF;
public:
	__fastcall CClock();
	__fastcall ~CClock();
    void __fastcall Create(int max);
	void __fastcall SetSampleFreq(double f);
	void __fastcall SetToneFreq(double f);
    BOOL __fastcall Do(short ds);
    inline int __fastcall GetData(int n){return m_pData[n];};
};
/*=============================================================================
  CPlayBackクラス
=============================================================================*/
class CPlayBack
{
private:
	int		m_StgMax;
    int		m_StgWidth;

    int		m_StgCnt;
    int		m_StgW;

	int		m_WTimer;
	int		m_StgRCnt;
    int		m_StgR;
	short	*m_pStg;
public:
	__fastcall CPlayBack();
	__fastcall ~CPlayBack();
	void __fastcall Delete(void);
	void __fastcall Init(int wSize, int SampBase);
    void __fastcall Clear(void);
	void __fastcall Write(const short *p);
	BOOL __fastcall Read(short *p);
    void __fastcall StopPlaying(void);
	BOOL __fastcall StartPlaying(int s);

    BOOL __fastcall IsActive(void){return m_StgMax;};
    BOOL __fastcall IsPlaying(void){return m_StgRCnt;};
};
/*=============================================================================
  CNotchesクラス
=============================================================================*/
typedef struct {
	int		Freq;
    int		m_MX;
    int		m_MY;
}NOTCHCTR;

class CNotches
{
public:
	int		m_Count;
    int		m_Max;
    NOTCHCTR	*m_pBase;

    CFIR2	m_FIR;

    int		m_NotchWidth;
	int		m_nBaseTaps;
	int		m_nTaps;
	double	m_SampleFreq;

private:
	void __fastcall Alloc(int nIndex);

public:
	__fastcall CNotches(void);
    __fastcall ~CNotches();
	void __fastcall Delete(void);
	void __fastcall Create(void);
	int __fastcall Find(int Freq);
	void __fastcall Add(int Freq);
	void __fastcall SetFreq(int nIndex, int Freq);
	void __fastcall Delete(int nIndex);
};
//---------------------------------------------------------------------------
double __fastcall DoFIR(double *hp, double *zp, double d, int tap);
void __fastcall DrawGraph(Graphics::TBitmap *pBitmap, const double *H, int Tap, int &nmax, int init, TColor col, double SampFreq);
void __fastcall DrawGraphIIR(Graphics::TBitmap *pBitmap, double a0, double a1, double a2, double b1, double b2, int &nmax, int init, TColor col, double SampFreq);
void __fastcall DrawGraphIIR(Graphics::TBitmap *pBitmap, CIIR *ip, int &nmax, int init, TColor col, double SampFreq);
void __fastcall MakeHilbert(double *H, int N, double fs, double fc1, double fc2);
void __fastcall MakeGaussian(double *H, int N, double fc, double fs, double B);
void __fastcall AddGaussian(short *pData, int n, double gain);
double __fastcall GetRTTYBW(int taps);
double __fastcall GetMFSKBW(int taps);
extern CVARICODE	g_VariCode;
#endif
