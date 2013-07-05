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



#ifndef WAVE_H
#define WAVE_H
//---------------------------------------------------------------------------
#include <mmsystem.h>
#include "mmw.h"
#include "ComLib.h"
//---------------------------------------------------------------------------
#ifndef PROC
#define PROC(Key) ((t##Key)GetProc("_" #Key))
#endif
//---------------------------------------------------------------------------
class CXWave
{
private:
	AnsiString	m_ItemName;
	//HANDLE	m_hLib;	//ja7ude 0522
	HINSTANCE	m_hLib;
	UINT	m_nTimerID;

	tmmwPumpMessages		fmmwPumpMessages;
	tmmwGetTimeout			fmmwGetTimeout;
	tmmwSetPTT				fmmwSetPTT;

	tmmwInOpen				fmmwInOpen;
	tmmwInClose				fmmwInClose;
	tmmwInRead				fmmwInRead;

	tmmwGetInExist			fmmwGetInExist;
	tmmwIsInCritical		fmmwIsInCritical;

	tmmwOutOpen				fmmwOutOpen;
	tmmwOutAbort			fmmwOutAbort;
	tmmwOutFlush			fmmwOutFlush;
	tmmwOutWrite			fmmwOutWrite;

	tmmwIsOutCritical		fmmwIsOutCritical;
	tmmwIsOutFull			fmmwIsOutFull;
	tmmwGetOutRemaining		fmmwGetOutRemaining;
	tmmwGetOutCounter		fmmwGetOutCounter;
	tmmwSetOutCounter		fmmwSetOutCounter;

private:
	FARPROC __fastcall GetProc(LPCSTR pName);
	void __fastcall FreeLib(void);

public:
	__fastcall CXWave(LPCSTR pName);
	__fastcall ~CXWave(void);
	inline BOOL __fastcall IsLib(void){return m_hLib != NULL;};
	BOOL __fastcall IsNameChange(void);
public:
	BOOL __fastcall InOpen(int sampfreq, int size);
	void __fastcall InClose(void);
	BOOL __fastcall InRead(SHORT *p);

	int __fastcall GetInBC(void);
	int __fastcall IsInBufCritical(void);

	BOOL __fastcall OutOpen(int sampfreq, int size);
	void __fastcall OutFlush(void);
	void __fastcall OutAbort(void);
	BOOL __fastcall OutWrite(const SHORT *p);
//    inline void __fastcall OutClose(void){OutFlush(); OutAbort();};

	int __fastcall GetOutBC(void);
	int __fastcall GetOutBCC(void);
	void __fastcall SetOutBCC(int count);
	int __fastcall IsOutBufCritical(void);
	int __fastcall IsOutBufFull(void);

	void __fastcall SetPTT(int tx);
    int __fastcall GetTimeout(void);
    void __fastcall PumpMessages(void);
};
//---------------------------------------------------------------------------
//#define	WM_WAVE		WM_USER+400
#define WAVE_TIMEOUT_EVENT  2000		// バッファリクエストのタイムアウト
#define	WAVE_FIFO_MAX	    32			// 循環バッファの最大個数
//---------------------------------------------------------------------------
class CWave
{
	friend void CALLBACK WaveInProc(HWAVE m_hin, UINT uMsg, CWave* pWave, DWORD dwParam1, DWORD dwParam2 );
	friend void CALLBACK WaveOutProc(HWAVE m_hout, UINT uMsg, CWave* pWave, DWORD dwParam1, DWORD dwParam2 );

public:
	HWND	m_hWnd;

	int     m_InFifoSize;
	int     m_OutFifoSize;
	int     m_SoundStereo;
	int		m_SoundID;
    int		m_SoundTxID;

	int			m_InDevs;
    int			m_OutDevs;
    AnsiString	m_tInDevName[16];
    AnsiString	m_tOutDevName[16];

private:
	volatile int     m_InBC;
	volatile int     m_OutBC;
	volatile int     m_OutBCC;

private:
	LPWAVEHDR    m_pInBuff[WAVE_FIFO_MAX];	// 入力バッファ
	LPWAVEHDR    m_pOutBuff[WAVE_FIFO_MAX];	// 出力バッファ
	WAVEFORMATEX m_OWFX;		// 出力フォーマット
	WAVEFORMATEX m_IWFX;		// 入力フォーマット

	CRITICAL_SECTION m_InCS;
	CRITICAL_SECTION m_OutCS;

	HANDLE	m_InEvent;
	HANDLE	m_OutEvent;
	BOOL	m_InWait;
	BOOL	m_InOver;
	BOOL	m_OutWait;
	BOOL	m_OutUnder;

	int		m_InWP;
	int		m_InRP;

	int		m_OutWP;
	int		m_OutRP;

	int		m_Error;
	BOOL	m_InOpen;
	BOOL	m_OutOpen;

	LPSTR   m_pInBase;
	LPSTR   m_pOutBase;
	int		m_InLen;
	int		m_InBuffSize;
	int		m_InMemSize;
	int     m_InAllocSize;
	int		m_OutLen;
	int		m_OutBuffSize;
	int		m_OutMemSize;
	int     m_OutAllocSize;
	BOOL	m_OutFirst;
	//HWAVE	m_hin;
	//HWAVE	m_hout;	//ja7ude 0522
	HWAVEIN		m_hin;
	HWAVEOUT	m_hout;

    BOOL	m_fPTT;
	CXWave	*m_pDLL;
private:

public:
	__fastcall CWave(void);
	__fastcall ~CWave();
//	BOOL __fastcall IsFormatSupported(LPWAVEFORMATEX pWFX, UINT IDDevice);
	BOOL __fastcall InOpen(LPWAVEFORMATEX pWFX, UINT IDDevice, DWORD Size);
	inline __fastcall BOOL IsInOpen(){return m_InOpen;};
	BOOL __fastcall InRead(SHORT *pData);
	void __fastcall InClose();
	BOOL __fastcall OutOpen(LPWAVEFORMATEX pWFX, UINT IDDevice, DWORD Size);
	inline __fastcall BOOL IsOutOpen(){return m_OutOpen;};
	BOOL __fastcall OutWrite(const SHORT *pData);
	void __fastcall OutFlush();
	void __fastcall OutAbort();
	void __fastcall OutClose(){OutFlush();OutAbort();};
//	DWORD __fastcall GetOutVolume(void);
//	BOOL __fastcall SetOutVolume(DWORD vol);
#if 0
	inline int __fastcall IsInBufCritical(void){
		return ( m_InBC >= (m_InFifoSize/2) ) ? 1 : 0;
	}
	inline int __fastcall IsOutBufCritical(void){
		return (m_OutBC <= (m_OutFifoSize/2)) ? 1 : 0;
	}
	inline int __fastcall IsOutBufFull(void){
		return (m_OutBC >= m_OutFifoSize) ? 1 : 0;
	};
    inline void __fastcall SetOutBCC(int count){
		m_OutBCC = count;
    };
    inline int __fastcall GetOutBCC(void){
		return m_OutBCC;
    };
    inline int __fastcall GetOutBC(void){
		return m_OutBC;
    };
#else
    BOOL __fastcall IsOutFirst(void);
	inline int __fastcall IsInBufNull(void){return !GetInBC();};
	int __fastcall GetInBC(void);
	int __fastcall GetOutBC(void);
	int __fastcall GetOutBCC(void);
	void __fastcall SetOutBCC(int count);
	int __fastcall IsInBufCritical(void);
	int __fastcall IsOutBufCritical(void);
	int __fastcall IsOutBufFull(void);
	void __fastcall SetPTT(LONG tx);
	int __fastcall GetTimeout(void);
	void __fastcall GetDeviceList(void);
	void __fastcall SetSoundID(int &rxID, int &txID);
	void __fastcall SetSoundID(void){
		SetSoundID(m_SoundID, m_SoundTxID);
    }
	void __fastcall UpdateDevice(int ID);
	void __fastcall PumpMessages(void);
#endif
	inline BOOL __fastcall IsPTT(void){return m_fPTT;};
    void __fastcall GetInDevName(AnsiString &as);
    void __fastcall GetOutDevName(AnsiString &as);
};

//---------------------------------------------------------------------------
class CWaveFile
{
private:
	HMMIO	m_Handle;
	int		m_dis;
	BYTE	m_Head[4];
private:
	void __fastcall SetName(void);

public:
	long	m_length;
	long	m_pos;
	int		m_mode;
	int		m_pause;
	int		m_autopause;
	AnsiString	m_FileName;
    AnsiString	m_Name;
	int		m_SampType;
public:
	__fastcall CWaveFile();
	__fastcall ~CWaveFile();
	void __fastcall FileClose(void);
	void __fastcall Rec(LPCSTR pName);
	BOOL __fastcall Play(LPCSTR pName);
	void __fastcall Rewind(void);
	void __fastcall ReadWrite(short *s, int size);
	long __fastcall GetPos(void);
	void __fastcall Seek(int n);
	int __fastcall ChangeSampFreq(LPCSTR tName, LPCSTR pName, int sSamp);
};
//---------------------------------------------------------------------------
class CWaveStrage
{
private:
	HMMIO	m_Handle;
	AnsiString  m_FileName;

public:
	long	m_pos;
	long    m_wpos;
	long    m_rpos;
	short   *pData;
	short   *pSync;

	__fastcall CWaveStrage();
	__fastcall ~CWaveStrage(){Close();};
	void __fastcall Close(void);
	void __fastcall Open(void);
	void __fastcall RInit(void);
	void __fastcall WInit(void);
	int __fastcall IsOpen(void){return m_Handle != NULL ? TRUE : FALSE;};
	void __fastcall Read(short *sp, int size);
	void __fastcall Write(short *sp, int size);
	void __fastcall Seek(int n);
};
//---------------------------------------------------------------------------
#endif

