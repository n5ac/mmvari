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



///----------------------------------------------------------
///  ÇqÇrÇQÇRÇQÇbí êMÉNÉâÉX
///
///  (C) JE3HHT Makoto.Mori
///
//---------------------------------------------------------------------------
#ifndef CommH
#define CommH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <mmsystem.h>
//---------------------------------------------------------------------------
#define	CR	0x0d
#define	LF	0x0a
#define	COMM_CLOSE	1
//---------------------------------------------------------------------------

#define	MeasureAccuracy	FALSE
//---------------------------------------------------------------------------
class CFSK {
private:
	volatile HANDLE	m_hPort;

	volatile BYTE	m_bPortD;

	volatile int	m_BLen;

	volatile int	m_Sequence;
	volatile int	m_Count;
	volatile int	m_BCount;
	volatile int	m_NowD;
	volatile int	m_StgD;

	volatile int	m_Idle;

	volatile int	m_invFSK;
	volatile int	m_oFSK;
	volatile int	m_aFSK;

#if MeasureAccuracy
	LARGE_INTEGER	m_liFreq;
	LARGE_INTEGER	m_liPCur, m_liPOld;
	DWORDLONG		m_dlDiff;
#endif
public:
	inline int __fastcall IsOpen(void){ return m_hPort != INVALID_HANDLE_VALUE;};

public:
	__fastcall CFSK(void);
	void __fastcall Init(void);
	void __fastcall Timer(void);
	void __fastcall SetPara(LONG para);
	inline void __fastcall Disable(void){m_hPort = INVALID_HANDLE_VALUE;};
	void __fastcall SetHandle(HANDLE hPort);
	void __fastcall PutByte(BYTE c){m_StgD = c;};
	void __fastcall SetPort(int sw);
	void __fastcall ClearPort(void){
		SetPort(m_invFSK ? m_oFSK : !m_oFSK);
    }
	inline int __fastcall IsBusy(void){
		return (m_StgD != -1) ? TRUE : FALSE;
	};
	inline void __fastcall SetInvFSK(int inv){
		m_invFSK = inv;
		m_aFSK = -1;
	};
	inline void __fastcall SetDelay(int n){m_Count = n;};
#if MeasureAccuracy
	inline DWORDLONG __fastcall GetPDiff(void){return m_dlDiff;};
	inline DWORDLONG __fastcall GetPFreq(void){return m_liFreq.QuadPart;};
#endif
};

//Added by JA7UDE (April 17, 2010)
#define EXTFSK_CHECK_INTERVAL 22
//Till here

#define	QUEMAX	1024
class CQUE {
private:
	int		m_R;
	int		m_W;
	int		m_C;
	BYTE	m_tFifo[QUEMAX];
public:
	CQUE(void);
	void __fastcall Clear(void);
	void __fastcall Push(BYTE c);
	BYTE __fastcall Pop(void);

	inline BOOL IsEmpty(void){return m_C == 0;};
};

typedef struct {
	int	change;

	int Baud;
	int	BitLen;
	int	Stop;
	int	Parity;
}COMMPARA;
extern	COMMPARA	COMM;

typedef long (__stdcall *extfskOpen)(long para);
typedef void (__stdcall *extfskClose)(void);
typedef long (__stdcall *extfskIsTxBusy)(void);
typedef void (__stdcall *extfskPutChar)(BYTE c);
typedef void (__stdcall *extfskSetPTT)(long tx);

class CEXTFSK
{
private:
	//HANDLE			m_hLib;	//ja7ude 0522
	HINSTANCE		m_hLib;
	extfskOpen		fextfskOpen;
	extfskClose		fextfskClose;
	extfskIsTxBusy	fextfskIsTxBusy;
	extfskPutChar	fextfskPutChar;
	extfskSetPTT	fextfskSetPTT;
private:
	FARPROC GetProc(LPCSTR pName);

public:
	__fastcall CEXTFSK(LPCSTR pName);
	__fastcall ~CEXTFSK();
	long __fastcall IsLib(void){return m_hLib != NULL;};
	long __fastcall Open(long para);
	void __fastcall Close(void);
	long __fastcall IsTxBusy(void);
	void __fastcall PutChar(BYTE c);
	void __fastcall SetPTT(long tx);
};

#define	COMM_TXBUFSIZE		MODBUFMAX
class CComm
{
public:
	BOOL	m_CreateON;		// ÉNÉäÉGÉCÉgÉtÉâÉO
	volatile	int	m_Command;
	DCB		m_dcb;			// ÇcÇbÇa
	HANDLE	m_fHnd;			// ÉtÉ@ÉCÉãÉnÉìÉhÉã

	int     m_ptt;
	int     m_scan;
	CEXTFSK	*m_pEXT;

	int		m_Baud;
    int		m_bFSKOUT;
	int		m_bINVFSK;

	int		m_cGuard;
	int		m_Fig;
	int		m_FigOut;
	int		m_Diddle;
	int		m_fSendChar;
	int		m_bDiddleEnabled;
    CQUE	m_QueueExtfsk;
	TTimer	*m_pReadTimerExtfsk;

	TIMECAPS	m_TimeCaps;
    UINT	m_uMMTimerID;
	CFSK	m_FSK;
protected:
private:
	int __fastcall PTTOpen(void);
	void __fastcall CreateTimer(void);
	void __fastcall DeleteTimer(void);

public:
	__fastcall CComm(void);
	__fastcall ~CComm(){
		Close();
	};
	inline BOOL __fastcall IsOpen(void){
		return m_CreateON;
	};
	BOOL __fastcall Open(LPCTSTR PortName);
	void __fastcall Close(void);
	int __fastcall SetPTT(void);
	void __fastcall SetPTT(int sw);

	void __fastcall SetFSK(int bFSK, int bINV);
	int __fastcall GetPutChar(void);
	void __fastcall PutCharExtfsk( TObject *Sender ); //Added by JA7UDE on April 5, 2010
    void __fastcall SetSendChar(BOOL f){m_fSendChar = f;};	// Added by JE3HHT on Sep.1010
    void __fastcall SetDiddle(int d){m_Diddle = d;};		// Added by JE3HHT on Sep.1010
	BOOL __fastcall IsBaudChange(double speed){return int(speed) != m_Baud;};
    void __fastcall SetMark(void){m_bDiddleEnabled = FALSE;};
};

void __fastcall InitCOMMPara(void);
#endif






