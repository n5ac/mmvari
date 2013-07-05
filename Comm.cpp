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

#include "Comm.h"
#include "ComLib.h"
#include "Dsp.h"

#define	WAITSTAT	0

#define	DEFFSOUND	3

COMMPARA	COMM;
void __fastcall InitCOMMPara(void)
{
	COMM.change = 1;
}

//---------------------------------------------------------------------------
void CALLBACK MMTimeProc(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	CComm *pComm = (CComm *)dwUser;
	if( uID != pComm->m_uMMTimerID ) return;

	pComm->m_FSK.Timer();
}

//***************************************************************************
// CFSK class
__fastcall CFSK::CFSK(void)
{
	m_bPortD = 0;

	m_BLen = 5;
	Init();
#if MeasureAccuracy
	QueryPerformanceFrequency(&m_liFreq);
	m_liPOld.u.HighPart = -1;
#endif
}
//---------------------------------------------------------------------------
void __fastcall CFSK::Init(void)
{
	m_hPort = INVALID_HANDLE_VALUE;
	m_StgD = -1;
	m_Sequence = 0;
	m_Count = 0;
	m_oFSK = 1;
	m_aFSK = -1;
}
//---------------------------------------------------------------------------
void __fastcall CFSK::SetHandle(HANDLE hPort)
{
	m_hPort = hPort;
	m_StgD = -1;
	m_Sequence = 0;
	m_Count = 0;
	m_oFSK = 1;			// mark signal
	m_aFSK = -1;
}
//---------------------------------------------------------------------------
//	para:	Upper16bits	Speed(eg. 45)
//			Lower16bits	b1-b0	Stop (0-1, 1-1.5, 2-2)
//						b5-b2	Length
void __fastcall CFSK::SetPara(LONG para)
{
	m_BLen = (para >> 2) & 0x000f;
}
//---------------------------------------------------------------------------
// This function is called from the TimeProc(). and according to
//MSDN,  it may be an illegal operation.  MSDN said, Applications
//should not call any system-defined functions from inside a
//callback function,  except for several functions.
// However, the EscapeCommFunction() seems to be no problem on my
//PCs with Windows 2000 and Windows XP, but I am not sure if it
//works on every PC.
// BTW, EnterCriticalSection() and LeaveCriticalSection() had problem
//on this, and I gave up to use them....
//
void __fastcall CFSK::SetPort(int sw)
{
	::EscapeCommFunction(m_hPort, sw ? SETBREAK : CLRBREAK);
}
//---------------------------------------------------------------------------
// 11ms interval
void __fastcall CFSK::Timer(void)
{
	if( m_Count <= 0 ){
		switch(m_Sequence){
			case 1:			// output data
				m_oFSK = (m_NowD & 1) ? 1 : 0;
				m_NowD = m_NowD >> 1;
				m_BCount--;
				if( !m_BCount ){
					m_Sequence++;
				}
				m_Count = 1;
				break;
			case 2:			// output stop-bit
				m_oFSK = 1;
				m_Count = 2;
				m_Sequence = 0;
				break;
			default:
				if( m_StgD != -1 ){
					m_NowD = m_StgD;
					m_StgD = -1;
					m_BCount = m_BLen;
					m_oFSK = 0;		// output start-bit
					m_Sequence = 1;
					m_Count = 1;
					m_Idle = 0;
#if MeasureAccuracy
					if( QueryPerformanceCounter(&m_liPCur) ){
						if( m_liPOld.u.HighPart != -1 ){
							m_dlDiff = m_liPCur.QuadPart - m_liPOld.QuadPart;
						}
						m_liPOld = m_liPCur;
					}
#endif
				}
				else {
					m_oFSK = 1;		// output mark signal.
					m_Idle = 1;
#if MeasureAccuracy
					m_liPOld.u.HighPart = -1;
#endif
				}
				break;
		}
	}
	else {
		m_Count--;
	}

	if( !IsOpen() ) return;

	if( m_oFSK != m_aFSK ){
		m_aFSK = m_oFSK;
		SetPort(m_invFSK ? m_oFSK : !m_oFSK);
	}
}

//---------------------------------------------------------------------------
__fastcall CComm::CComm(void)
{
	m_CreateON = FALSE;	// クリエイトフラグ
	m_Command = 0;
	m_fHnd = INVALID_HANDLE_VALUE;		// ファイルハンドル
	m_ptt = m_scan = 0;
	m_pEXT = NULL;

	m_Baud = 45;
	m_bFSKOUT = FALSE;
	m_bINVFSK = FALSE;

	m_pReadTimerExtfsk = NULL;
	m_fSendChar = 0;
    m_Diddle = diddleLTR;
    m_Fig = FALSE;
    m_FigOut = FALSE;
	m_bDiddleEnabled = TRUE;
    m_cGuard = 0;

	m_uMMTimerID = 0;
}

//-----------------------------------------------------------------
int __fastcall CComm::GetPutChar(void)
{
	int c;

   	if( m_QueueExtfsk.IsEmpty() ){
		if( !m_bDiddleEnabled ) return -1;
		switch(m_Diddle){
			case diddleLTR:
				c = 0x1f;
				if( m_Fig ) m_FigOut = TRUE;
               	break;
			default:
				c = 0x00;
               	break;
        }
    }
	else if( m_FigOut ){
		c = 0x1b;
        m_FigOut = FALSE;
    }
    else {
		c = m_QueueExtfsk.Pop();
        switch(c){
			case 0x1f:		// 11111 LTR
				m_Fig = FALSE;
               	break;
            case 0x1b:		// 11011 FIG
               	m_Fig = TRUE;
                break;
		}
	}
    return c;
}
//-----------------------------------------------------------------
void __fastcall CComm::PutCharExtfsk( TObject *Sender )
{
	if( m_cGuard > 0 ){
    	m_cGuard--;
        return;
    }
	if( m_pEXT != NULL ){
		if( !m_pEXT->IsTxBusy() ){
			int c = GetPutChar();
            if( c != -1 ) m_pEXT->PutChar(BYTE(c));
		}
	}
    else if( m_FSK.IsOpen() ){
		if( !m_FSK.IsBusy() ){
			int c = GetPutChar();
			if( c != -1 ) m_FSK.PutByte(BYTE(c));
        }
    }
}

//-----------------------------------------------------------------
void __fastcall CComm::CreateTimer(void)
{
	if( !m_bFSKOUT ) return;
	if( !m_fSendChar ) return;
	if( !m_pEXT && (m_fHnd == INVALID_HANDLE_VALUE) ) return;

    m_Fig = FALSE;
    m_FigOut = FALSE;

	m_QueueExtfsk.Clear();
	if( !m_pReadTimerExtfsk ) m_pReadTimerExtfsk = new TTimer(NULL);
	int interval = (1000.0 / m_Baud);
	if( interval < 10 ) interval = 10;
	m_pReadTimerExtfsk->Interval = interval;
	m_pReadTimerExtfsk->Enabled = true;
	m_pReadTimerExtfsk->OnTimer = PutCharExtfsk;
	m_bDiddleEnabled = TRUE;
    m_cGuard = 8;

	if( !m_pEXT && (m_fHnd != INVALID_HANDLE_VALUE) ){
		m_FSK.SetHandle(m_fHnd);
		m_FSK.ClearPort();

		m_uMMTimerID = 0;
		if( ::timeGetDevCaps(&m_TimeCaps, sizeof(m_TimeCaps)) == TIMERR_NOERROR ){
			::timeBeginPeriod(m_TimeCaps.wPeriodMin);
			m_uMMTimerID = ::timeSetEvent(11, 0, MMTimeProc, DWORD(this), TIME_PERIODIC);
		}
    }
}
//-----------------------------------------------------------------
void __fastcall CComm::DeleteTimer(void)
{
	if( m_pReadTimerExtfsk ){
		delete m_pReadTimerExtfsk;
        m_pReadTimerExtfsk = NULL;
    }
	if( m_uMMTimerID ){
		m_FSK.Disable();

		::timeKillEvent(m_uMMTimerID);
		m_uMMTimerID = 0;
		::timeEndPeriod(m_TimeCaps.wPeriodMin);
	}
}

void __fastcall CComm::SetFSK(int bFSK, int bINV)
{
	m_bFSKOUT = bFSK;
    m_bINVFSK = bINV;
	m_FSK.SetInvFSK(m_bINVFSK);
    if( m_ptt && !m_pReadTimerExtfsk ) CreateTimer();
}

/*#$%
==============================================================
	通信回線をクローズする
--------------------------------------------------------------
--------------------------------------------------------------
--------------------------------------------------------------
==============================================================
*/
void __fastcall CComm::Close(void)
{
	if( m_CreateON ){
		DeleteTimer();
		if( m_pEXT != NULL ){
			delete m_pEXT;
			m_pEXT = NULL;
		}
		else if( m_fHnd != INVALID_HANDLE_VALUE ){
			::CloseHandle(m_fHnd);
			m_fHnd = INVALID_HANDLE_VALUE;
		}
		m_CreateON = FALSE;
	}
}

/*#$%
==============================================================
	通信回線をオープンする
--------------------------------------------------------------
PortName : 回線の名前
pCP		 : COMMPARAのポインタ（ヌルの時はデフォルトで初期化）
RBufSize : 受信バッファのサイズ(default=2048)
TBufSize : 送信バッファのサイズ(default=2048)
--------------------------------------------------------------
TRUE/FALSE
--------------------------------------------------------------
==============================================================
*/
BOOL __fastcall CComm::Open(LPCTSTR PortName)
{
	if( m_CreateON ) Close();
	m_fHnd = ::CreateFile( PortName, GENERIC_READ | GENERIC_WRITE,
						0, NULL,
						OPEN_EXISTING,
						FILE_ATTRIBUTE_NORMAL,
						NULL
	);
	if( m_fHnd == INVALID_HANDLE_VALUE ){
		AnsiString as = "\\\\.\\";
        as += PortName;
		m_fHnd = ::CreateFile( as.c_str(), GENERIC_READ | GENERIC_WRITE,
							0, NULL,
							OPEN_EXISTING,
							FILE_ATTRIBUTE_NORMAL,
							NULL
		);
	}
	if( m_fHnd == INVALID_HANDLE_VALUE ){
		m_pEXT = new CEXTFSK(PortName);
		if( m_pEXT->IsLib() ){
			LONG para;
			para = (m_Baud << 16) | (5 << 2);
			m_pEXT->Open(para);
			m_CreateON = TRUE;
			return TRUE;
		}
		else {
			delete m_pEXT;
			m_pEXT = NULL;
		}
		return FALSE;
	}
	// setup device buffers
	if( ::SetupComm( m_fHnd, DWORD(1024), DWORD(2) ) == FALSE ){
		::CloseHandle(m_fHnd);
        m_fHnd = INVALID_HANDLE_VALUE;
		return FALSE;
	}

	// purge any information in the buffer
	::PurgeComm( m_fHnd, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR );

	// set up for overlapped I/O
	COMMTIMEOUTS TimeOut;

	TimeOut.ReadIntervalTimeout = 0xffffffff;
	TimeOut.ReadTotalTimeoutMultiplier = 0;
	TimeOut.ReadTotalTimeoutConstant = 0;
	TimeOut.WriteTotalTimeoutMultiplier = 0;
	TimeOut.WriteTotalTimeoutConstant = 20000;
//	TimeOut.WriteTotalTimeoutConstant = 1;
	if( !::SetCommTimeouts( m_fHnd, &TimeOut ) ){
		::CloseHandle( m_fHnd );
        m_fHnd = INVALID_HANDLE_VALUE;
		return FALSE;
	}
	::GetCommState( m_fHnd, &m_dcb );
	m_dcb.BaudRate = 9600;
	m_dcb.fBinary = TRUE;
	m_dcb.ByteSize = 8;
	m_dcb.Parity = NOPARITY;
	m_dcb.StopBits = ONESTOPBIT;
	m_dcb.XonChar = 0x11;	// XON
	m_dcb.XoffChar = 0x13;	// XOFF
	m_dcb.fParity = 0;
	m_dcb.fOutxCtsFlow = FALSE;
	m_dcb.fInX = m_dcb.fOutX = FALSE;
	m_dcb.fOutxDsrFlow = FALSE;
	m_dcb.EvtChar = 0x0d;

	m_dcb.fRtsControl = RTS_CONTROL_DISABLE;		// 送信禁止
	m_dcb.fDtrControl = DTR_CONTROL_DISABLE;		// 送信禁止

//	m_dcb.fTXContinueOnXoff = TRUE;
	m_dcb.XonLim = USHORT(1024/4);			// 1/4 of RBufSize
	m_dcb.XoffLim = USHORT(1024*3/4);		// 3/4 of RBufSize
	m_dcb.DCBlength = sizeof( DCB );

	if( !::SetCommState( m_fHnd, &m_dcb ) ){
		::CloseHandle( m_fHnd );
        m_fHnd = INVALID_HANDLE_VALUE;
		return FALSE;
	}

	// get any early notifications
	if( !::SetCommMask( m_fHnd, EV_RXFLAG ) ){
		::CloseHandle(m_fHnd);
        m_fHnd = INVALID_HANDLE_VALUE;
		return FALSE;
	}
	m_CreateON = TRUE;
	return TRUE;
}
//-----------------------------------------------------------------
// PTT切り替え用
int __fastcall CComm::PTTOpen(void)
{
	if( !m_CreateON ){
		if( !strcmpi(sys.m_PTTCOM.c_str(), "NONE") ) return FALSE;
		Open(sys.m_PTTCOM.c_str());
		if( !m_CreateON ) return FALSE;
	}
	return TRUE;
}
//-----------------------------------------------------------------
// PTT切り替え用
int __fastcall CComm::SetPTT(void)
{
	if( m_pEXT != NULL ){
		m_pEXT->SetPTT(m_ptt);
	}
	else if( m_fHnd != INVALID_HANDLE_VALUE ){
		::EscapeCommFunction(m_fHnd, m_ptt ? SETRTS : CLRRTS);
		::EscapeCommFunction(m_fHnd, m_ptt ? SETDTR : CLRDTR);
	}
	if( m_ptt ){
		CreateTimer();
    }
    else {
		DeleteTimer();
    }
	return m_ptt;
}
//-----------------------------------------------------------------
// PTT切り替え用
void __fastcall CComm::SetPTT(int sw)
{
	m_ptt = sw;

	if( !PTTOpen() ) return;
	if( !SetPTT() && (!sys.m_PTTLock) ) Close();
}

/*******************************************************************
	EXTFSK.DLL
*******************************************************************/
__fastcall CEXTFSK::CEXTFSK(LPCSTR pName)
{
	char Name[128];
	sprintf(Name, "%s.%s", pName, strcmpi(pName, "EXTFSK") ? "fsk" : "dll");

	fextfskOpen	= NULL;
	fextfskClose = NULL;
	fextfskIsTxBusy = NULL;
	fextfskPutChar = NULL;
	fextfskSetPTT = NULL;

	m_hLib = ::LoadLibrary(Name);
	if( m_hLib != NULL ){
		fextfskOpen	= (extfskOpen)GetProc("_extfskOpen");
		fextfskClose = (extfskClose)GetProc("_extfskClose");
		fextfskIsTxBusy = (extfskIsTxBusy)GetProc("_extfskIsTxBusy");
		fextfskPutChar = (extfskPutChar)GetProc("_extfskPutChar");
		fextfskSetPTT = (extfskSetPTT)GetProc("_extfskSetPTT");
	}
}

//---------------------------------------------------------------------
__fastcall CEXTFSK::~CEXTFSK()
{
	if( m_hLib != NULL ){
		Close();
		::FreeLibrary(m_hLib);
		m_hLib = NULL;
	}
}

//---------------------------------------------------------------------
FARPROC CEXTFSK::GetProc(LPCSTR pName)
{
	FARPROC fn = ::GetProcAddress(m_hLib, pName+1);
	if( fn == NULL ){
		fn = ::GetProcAddress(m_hLib, pName);
	}
	return fn;
}

long __fastcall CEXTFSK::Open(long para)
{
	if( !m_hLib || !fextfskOpen ) return FALSE;
	return fextfskOpen(para);
}

void __fastcall CEXTFSK::Close(void)
{
	if( !m_hLib || !fextfskClose ) return;
	fextfskClose();
}

long __fastcall CEXTFSK::IsTxBusy(void)
{
	if( !m_hLib || !fextfskIsTxBusy ) return FALSE;
	return fextfskIsTxBusy();
}

void __fastcall CEXTFSK::PutChar(BYTE c)
{
	if( !m_hLib || !fextfskPutChar ) return;
	fextfskPutChar(c);
}

void __fastcall CEXTFSK::SetPTT(long tx)
{
	if( !m_hLib || !fextfskSetPTT ) return;
	fextfskSetPTT(tx);
}

//--------------------------------------------------------------------------------------------------
CQUE::CQUE(void)
{
	Clear();
}

//--------------------------------------------------------------------------------------------------
void __fastcall CQUE::Clear(void)
{
	m_R = 0;
	m_W = 0;
	m_C = 0;
}
//--------------------------------------------------------------------------------------------------
void __fastcall CQUE::Push(BYTE c)
{
	if( m_C < QUEMAX ){
		m_tFifo[m_W++] = c;
		if( m_W >= QUEMAX ) m_W = 0;
		m_C++;
	}
}

//--------------------------------------------------------------------------------------------------
BYTE __fastcall CQUE::Pop(void)
{
	if( !m_C ) return 0;

	BYTE c = m_tFifo[m_R++];
	if( m_R >= QUEMAX ) m_R = 0;
	m_C--;
	return c;
}

