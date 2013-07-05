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
///  Radioコミニュケーションクラス
///
///  (C) JE3HHT Makoto.Mori
///
//---------------------------------------------------------------------------
#ifndef CradioH
#define CradioH
#include "ComLib.h"
#include "MMlink.h"
//---------------------------------------------------------------------------
#include <Classes.hpp>

typedef enum {
	rmLSB,
    rmUSB,
    rmCW,
    rmAM,
    rmFM,
    rmRTTY,
    rmPACKET,
    rmUNKNOWN,
}RIGMODE;

extern const LPCSTR g_tRadioMode[];

typedef struct {
	int		change;

	char	StrPort[32];	// ポートの名前
	int		BaudRate;		// ボーレート
	int		BitLen;			// 0-7Bit, 1-8Bit
	int		Stop;			// 0-1Bit, 1-2Bit
	int		Parity;			// 0-PN, 1-PE, 2-PO
	int		flwXON;			// Xon/Xoff ON
	int		flwCTS;			// CTS-RTS ON
	int     usePTT;         // PTT control

	long	ByteWait;		// バイト間の送信ウエイト

	int		Cmdxx;

	AnsiString	CmdInit;
	AnsiString	CmdRx;
	AnsiString	CmdTx;

	AnsiString	cmdGNR;
	int		openGNR;

	int		PollType;
	int		PollInterval;
	int		PollOffset;

	int		PollScan;
}CRADIOPARA;
extern	CRADIOPARA	RADIO;
//---------------------------------------------------------------------------
void __fastcall LoadRADIOSetup(TMemIniFile *pIniFile);
void __fastcall SaveRADIOSetup(TMemIniFile *pIniFile);
//#define	CR	0x0d
//#define	LF	0x0a
#define	CRADIO_CLOSE	1
#define	RADIO_COMBUFSIZE	4096
#define	RADIO_TXBUFSIZE		256
#define	RADIO_RXBUFSIZE		256

enum {
	RADIO_POLLNULL,
	RADIO_POLLYAESUHF,
	RADIO_POLLYAESUVU,
	RADIO_POLLICOM,
	RADIO_POLLICOMN,
	RADIO_POLLOMNIVI,
	RADIO_POLLOMNIVIN,
	RADIO_POLLKENWOOD,
	RADIO_POLLKENWOODN,
	RADIO_POLLFT1000D,
	RADIO_POLLFT920,
	RADIO_POLLJST245,
	RADIO_POLLJST245N,
    RADIO_POLLFT9000,   //1.66B AA6YQ add new radios at end of list as this value is stored in mmtty.ini
    RADIO_POLLFT2000,   //1.66B AA6YQ
    RADIO_POLLFT950,    //1.66B AA6YQ
    RADIO_POLLFT450,    //1.66B AA6YQ
};

class CCradio : public TThread
{
public:
	BOOL	m_CreateON;		// クリエイトフラグ
	DCB		m_dcb;			// ＤＣＢ
	HANDLE	m_fHnd;			// ファイルハンドル
	HWND	m_wHnd;			// 親のウインドウハンドル
	UINT	m_uMsg;
	UINT	m_ID;			// メッセージのＩＤ番号
	volatile	int	m_Command;		// スレッドへのコマンド
	BOOL	m_TxAbort;		// 送信中止フラグ
	AnsiString	Name;
	CMMRadio	*m_pRadio;

	int		m_PSKGNRId;
	int 	m_OpenGNR;

	char	m_txbuf[RADIO_TXBUFSIZE];
	int		m_txcnt;
	int		m_txwp;
	int		m_txrp;

	int		m_PollCnt;
    int		m_PollCntHalf;
	int     m_PollTimer;
    int		m_PollInhibit;
	unsigned char m_rxbuf[RADIO_RXBUFSIZE];
	int		m_rxcnt;
	int		m_FreqEvent;
	char	m_Freq[32];
	UINT	m_FreqHz;		// [Hz]
	UINT	m_RigHz;

	int		m_ScanAddr;
    int		m_CarrierFreq;
    int		m_LSB;
    int		m_RigMode;
protected:
	void virtual __fastcall Execute();
	BOOL __fastcall OpenPipe(CRADIOPARA *cp, HWND hwnd, UINT nID);
	void __fastcall CatchPoll(BYTE c);
	void __fastcall FreqYaesuHF(void);
	void __fastcall FreqYaesuVU(void);
	void __fastcall FreqYaesu9K2K(void);
	void __fastcall FreqICOM(void);
	void __fastcall FreqKenwood(void);
	void __fastcall FreqJST245(void);

public:
	__fastcall CCradio(bool CreateSuspended);
	__fastcall ~CCradio(){
		Close();
	};
	inline BOOL __fastcall IsOpen(void){
		return m_CreateON;
	};
	inline void __fastcall UpdateHandle(HWND hwnd, UINT uMsg){
		m_wHnd = hwnd; m_uMsg = uMsg;
	};
	BOOL __fastcall Open(CRADIOPARA *cp, HWND hwnd, UINT uMsg, UINT nID);
	void __fastcall Close(void);
	void __fastcall ReqClose(void);
	void __fastcall WaitClose(void);
	DWORD __fastcall RecvLen(void);
	int __fastcall TxBusy(void);
	DWORD __fastcall Read(BYTE *p, DWORD len);
	void __fastcall Write(void *p, DWORD len);
	void __fastcall PutChar(char c);
	void OutStr(LPCSTR fmt, ...);
	void OutLine(LPCSTR fmt, ...);
	void __fastcall SendCommand(LPCSTR p);
	void __fastcall SetPTT(int sw);
	void __fastcall Timer(int tx, int interval);

	inline LPCSTR __fastcall GetFreq(void){
		m_FreqEvent = 0;
		return m_Freq;
	};
	int __fastcall IsFreqChange(LPCSTR p);
	void __fastcall UpdateFreq(double freq);
    void __fastcall SetCarrierFreq(int fq);

	void __fastcall SetInternalRigMode(LPCSTR pMode);
	BOOL __fastcall IsRigLSB(void);
	double __fastcall CalcRigFreq(double freq);
	void __fastcall WaitICOM(int intval);
};

void __fastcall InitRADIOPara(void);
#endif

