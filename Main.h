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
#ifndef MainH
#define MainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Menus.hpp>
#include <ComCtrls.hpp>
#include <Buttons.hpp>
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
#include "ComLib.h"
#include "wave.h"
#include "fft.h"
#include "dsp.h"
#include "dump.h"
#include "comm.h"
#include "fedit.h"
#include "PlayDlg.h"
#include "cradio.h"
#include "RxView.h"
#include "ClockAdj.h"
#include "RMenuDlg.h"
//---------------------------------------------------------------------------
#define	KEY_DIRECT	TRUE		// WM_CHARで処理
//---------------------------------------------------------------------------
#define POINT TPoint
//---------------------------------------------------------------------------
#define	MODGAIN		16384.0
#define	LEVELMAX	2048
//---------------------------------------------------------------------------
#define	STGCALLMAX	16
#define	LOSTMSGTIME	3
//---------------------------------------------------------------------------
enum {
	statusPAGE,
    statusSN,
    statusSAMP,
    statusCOM,
    statusVARI,
	statusHint,
    statusEND,
};
//---------------------------------------------------------------------------
#define	MACBUTTONMAX	144
#define	MACBUTTONWIDTH	60
#define	MACBUTTONVW		3
#define	MACBUTTONXW		12

#define	MACEXBUTTONMAX	16
#define	MACBUTTONALL	(MACBUTTONMAX+MACEXBUTTONMAX)

#define	button1ST	0x02		// Seq 1st
#define	button2ND	0xaa		// Seq 2nd
enum {
	buttonCWON = 1,
    buttonCWOFF,
    buttonMOVETOP,
    buttonMOVEEND,
    buttonIDLE,
};
#define	macTX			1
#define	macRX			2
#define	macTXOFF		4
#define	macAUTOCLEAR	8
#define macTONE			16
#define	macSEEK			32
#define	macTXRX			64
typedef struct {
	TSpeedButton *pButton;
    AnsiString	 Name;
    AnsiString	 Text;
    TColor		 Color;
    DWORD		 Style;
}MACBUTTON;

enum {
    loopINTERNAL,
    loopEXTERNAL,
};
enum {
	txRX,
    txINTERNAL,
    txEXTERNAL,
};

#define	RADIOMENUMAX	128
typedef struct {
	AnsiString	strTTL;
    AnsiString	strCMD;
}RADIOMENU;

#define	iniwMETRIC	1
#define	iniwLIMIT	2
#define	iniwBOTH	3
//---------------------------------------------------------------------------
#define	RXMAX	9
//---------------------------------------------------------------------------
class CRxSet {
public:
	int				m_Mode;
	BOOL			m_fJA;
	BOOL			m_fTWO;
    int				m_fMBCS;
	int				m_CarrierFreq;
	RECT			m_rcView;
	FONTDATA		m_FontData;
	CMBCS			m_MBCS;

	BOOL			m_fAFC;
    BOOL			m_SQ;			// スケルチの状態
    int				m_SQLevel;
    int				m_SQTimer;
	double			m_Speed;

    int				m_AFCTimerW;	// 広帯域AFCのガード時間
    int				m_AFCTimerN;	// 高精度AFCのガード時間
	int				m_AFCTimerW2;
    UINT			m_AFCTimerPSK;
	int				m_AFCFQ;
    BOOL			m_AFCSQ;

	CFAVG			m_AvgAFC;

    BOOL			m_fATC;

    int				m_PeakSig;
    CFAVG			m_AvgSig;
	FFTSTG			m_StgFFT;
    CDEMFSK			*m_pDem;
    CFFT			*m_pFFT;
    TRxViewDlg		*m_pView;

    int				m_X;
    int				m_Y;
    int				m_WaterW;		// for the sub channel

    int				m_cAutoTS1;		// 自動タイムスタンプカウンタ
    int				m_cAutoTS2;		// 自動タイムスタンプカウンタ

    int				m_MFSK_TYPE;
    UINT			m_AFCTimerMFSK;

    int				m_RTTYFFT;
    int				m_fShowed;
public:
	__fastcall CRxSet();
    __fastcall ~CRxSet();
	void __fastcall SetMFSKType(int type);
	void __fastcall SetSpeed(double b);
	void __fastcall SetMode(int mode);
	void __fastcall SetCarrierFreq(double f);
	void __fastcall Create(BOOL fView);
	void __fastcall Delete(void);
	void __fastcall InitStgFFT(void);
	void __fastcall SetSampleFreq(double f);
	void __fastcall ClearWindow(void);
	double __fastcall GetBandWidth(void);
    double __fastcall GetSpeed(void);
    inline BOOL __fastcall IsActive(void){return m_pDem != NULL;};
    inline BOOL __fastcall IsRTTY(void){return ::IsRTTY(m_Mode);};
    inline BOOL __fastcall Is170(void){return ::Is170(m_Mode);};
    inline BOOL __fastcall IsBPSK(void){return ::IsBPSK(m_Mode);};
    inline BOOL __fastcall IsFSK(void){return ::IsFSK(m_Mode);};
    inline BOOL __fastcall IsMFSK(void){return ::IsMFSK(m_Mode);};
    inline BOOL __fastcall IsQPSK(void){return ::IsQPSK(m_Mode);};
    inline BOOL __fastcall IsPSK(void){return ::IsPSK(m_Mode);};
};

//---------------------------------------------------------------------------
class TMainVARI : public TForm
{
__published:	// IDE 管理のコンポーネント
	TMainMenu *MainMenu;
	TMenuItem *KF;
	TPanel *PCont;
	TPanel *PLog;
	TPanel *PCRXB;
	TSplitter *Splitter;
	TPanel *PCTXB;
	TPanel *PCMac;
	TStatusBar *StatusBar;
	TPanel *PCRX;
	TScrollBar *SBarRX;
	TPanel *PCTX;
	TScrollBar *SBarTX;
	TPaintBox *PBoxRX;
	TPaintBox *PBoxTX;
	TSpeedButton *SBTX;
	TSpeedButton *SBTXOFF;
	TGroupBox *GBBPF;
	TSpeedButton *SBBPFW;
	TSpeedButton *SBBPFM;
	TSpeedButton *SBBPFS;
	TGroupBox *GBCarrier;
	TLabel *L1;
	TLabel *L2;
	TComboBox *CBRXCarrier;
	TComboBox *CBTxCarrier;
	TSpeedButton *SBAFC;
	TSpeedButton *SBNET;
	TPanel *PCLevel;
	TGroupBox *GBBaud;
	TPanel *PFFT;
	TSpeedButton *SBFFT;
	TSpeedButton *SBWater;
	TSpeedButton *SBWave;
	TSpeedButton *SBFFT500;
	TSpeedButton *SBFFT1K;
	TSpeedButton *SBFFT2K;
	TSpeedButton *SBFFT3K;
	TUpDown *UdRxCarrier;
	TUpDown *UdTxCarrier;
	TComboBox *CBSpeed;
	TGroupBox *GBTiming;
	TEdit *EATC;
	TUpDown *UdATC;
	TSpeedButton *SBBPFN;
	TPaintBox *PBoxFFT;
	TPaintBox *PBoxLevel;
	TMenuItem *KFS;
	TSpeedButton *SBATC;
	TLabel *L3;
	TEdit *HisCall;
	TMenuItem *KE;
	TUpDown *UdMac;
	TMenuItem *KV;
	TMenuItem *KVF;
	TMenuItem *KVF1;
	TMenuItem *KVF3;
	TMenuItem *KVFS;
	TMenuItem *KVFS2;
	TMenuItem *KVFS4;
	TMenuItem *N1;
	TMenuItem *KFX;
	TMenuItem *KO;
	TMenuItem *KEP;
	TMenuItem *NOT;
	TMenuItem *KOVO;
	TMenuItem *KOVI;
	TMenuItem *N3;
	TMenuItem *KOO;
	TMenuItem *N4;
	TMenuItem *KOAI;
	TMenuItem *KOAO;
	TLabel *L4;
	TEdit *HisName;
	TLabel *L5;
	TComboBox *HisRST;
	TLabel *L6;
	TComboBox *MyRST;
	TSpeedButton *SBInit;
	TMenuItem *KH;
	TMenuItem *KHA;
	TMenuItem *KHV;
	TMenuItem *N5;
	TMenuItem *KVFS3;
	TMenuItem *KVF2;
	TMenuItem *N6;
	TMenuItem *KVCR;
	TMenuItem *KVCT;
	TPopupMenu *PupCharset;
	TMenuItem *KANSI;
	TMenuItem *KJA;
	TMenuItem *KHL;
	TMenuItem *KBV;
	TMenuItem *KBY;
	TMenuItem *KMISC;
	TMenuItem *KOV;
	TMenuItem *KHH;
	TMenuItem *KEC;
	TMenuItem *N2;
	TMenuItem *KFWS;
	TMenuItem *KFRS;
	TMenuItem *KFES;
	TMenuItem *N7;
	TMenuItem *KVSP;
	TMenuItem *KFWST;
	TMenuItem *N8;
	TMenuItem *KVL;
	TSpeedButton *SBQSO;
	TComboBox *LogFreq;
	TMenuItem *N9;
	TMenuItem *KFLF;
	TSpeedButton *SBData;
	TSpeedButton *SBFind;
	TMenuItem *KOL;
	TMenuItem *KFLO;
	TSpeedButton *SBList;
	TPopupMenu *PupRX;
	TMenuItem *KRXQTH;
	TMenuItem *KRXMY;
	TMenuItem *KRXNAME;
	TMenuItem *KRXNOTE;
	TMenuItem *KRXCALL;
	TMenuItem *N10;
	TMenuItem *KVWA;
	TMenuItem *N11;
	TMenuItem *KVS;
	TPopupMenu *PupCalls;
	TSpeedButton *SBM;
	TMenuItem *KHO;
	TMenuItem *N12;
	TMenuItem *KRXCopy;
	TLabel *L16;
	TEdit *HisQTH;
	TLabel *L17;
	TEdit *EditNote;
	TLabel *L18;
	TEdit *EditQSL;
	TMenuItem *KVLA;
	TMenuItem *KVM;
	TMenuItem *KVM2;
	TMenuItem *KVM3;
	TMenuItem *KVM4;
	TMenuItem *N13;
	TMenuItem *KRXADDNAME;
	TMenuItem *KRXADDNOTE;
	TMenuItem *KOA;
	TMenuItem *N14;
	TMenuItem *KFL;
	TMenuItem *KVSF;
	TMenuItem *KVSFT;
	TMenuItem *KVSFR;
	TMenuItem *KOR;
	TComboBox *CBMode;
	TMenuItem *KFLR;
	TMenuItem *KECP;
	TMenuItem *KEX;
	TPopupMenu *PupTX;
	TMenuItem *N15;
	TMenuItem *KRXADDQSL;
	TMenuItem *N16;
	TMenuItem *KECall;
	TMenuItem *KEName;
	TMenuItem *KERST;
	TMenuItem *KEMyCall;
	TMenuItem *KEDear;
	TMenuItem *KESeq;
	TMenuItem *KEFinal;
	TPopupMenu *PupPage;
	TMenuItem *KPG1;
	TMenuItem *KPG2;
	TMenuItem *KPG3;
	TMenuItem *KPG4;
	TPopupMenu *PupSpec;
	TMenuItem *KSAS;
	TMenuItem *N17;
	TMenuItem *KSCan;
	TMenuItem *N18;
	TMenuItem *KFMS;
	TMenuItem *KFML;
	TPopupMenu *PupRXW;
	TMenuItem *KRWC;
	TMenuItem *N19;
	TMenuItem *KRWT;
	TMenuItem *KRWB;
	TMenuItem *KRWE;
	TMenuItem *KSW;
	TMenuItem *N20;
	TMenuItem *KVSD;
	TMenuItem *KRXADDMY;
	TMenuItem *KRXINV;
	TMenuItem *KRXN;
	TMenuItem *KORS;
	TMenuItem *KORSC;
	TMenuItem *KORSW;
	TMenuItem *KSN;
	TMenuItem *N21;
	TMenuItem *KSTX;
	TMenuItem *KSNRA;
	TMenuItem *KSS;
	TMenuItem *N23;
	TMenuItem *KSRX;
	TMenuItem *KSSR;
	TMenuItem *KSSM;
	TPanel *PF;
	TPaintBox *PBoxPF;
	TMenuItem *KVMX;
	TMenuItem *N22;
	TSpeedButton *SBP60;
	TSpeedButton *SBP30;
	TSpeedButton *SBP15;
	TMenuItem *KRSC;
	TMenuItem *N24;
	TPopupMenu *PupSQ;
	TMenuItem *KS0;
	TMenuItem *KS1;
	TMenuItem *KS2;
	TMenuItem *KS3;
	TMenuItem *KS4;
	TMenuItem *N25;
	TMenuItem *KHJ;
	TMenuItem *KHN;
	TMenuItem *KHS;
	TMenuItem *KSNR;
	TMenuItem *KR;
	TMenuItem *NR;
	TMenuItem *KRM;
	TMenuItem *N26;
	TMenuItem *KRL;
	TMenuItem *KRO;
	TMenuItem *NRE;
	TMenuItem *KRADD;
	TMenuItem *KHM;
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall PBoxFFTPaint(TObject *Sender);
	void __fastcall PBoxLevelPaint(TObject *Sender);
	void __fastcall PBoxRXPaint(TObject *Sender);
	void __fastcall PBoxTXPaint(TObject *Sender);
	void __fastcall PCTXEnter(TObject *Sender);
	void __fastcall PCTXExit(TObject *Sender);
	void __fastcall PBoxTXClick(TObject *Sender);
	void __fastcall FormKeyPress(TObject *Sender, char &Key);
	
	void __fastcall SBTXClick(TObject *Sender);
	void __fastcall PCRXResize(TObject *Sender);
	void __fastcall PCTXResize(TObject *Sender);
	void __fastcall SBFFTClick(TObject *Sender);
	void __fastcall SBBPFWClick(TObject *Sender);
	void __fastcall UdTxCarrierClick(TObject *Sender, TUDBtnType Button);
	void __fastcall UdRxCarrierClick(TObject *Sender, TUDBtnType Button);
	void __fastcall CBSpeedChange(TObject *Sender);
	void __fastcall KFSClick(TObject *Sender);
	
	void __fastcall SBFFT500Click(TObject *Sender);
	void __fastcall UdATCClick(TObject *Sender, TUDBtnType Button);
	void __fastcall SBAFCClick(TObject *Sender);
	void __fastcall SBATCClick(TObject *Sender);
	void __fastcall PBoxFFTMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall PBoxLevelMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall SBTXOFFClick(TObject *Sender);
	void __fastcall CBTxCarrierChange(TObject *Sender);
	void __fastcall FormResize(TObject *Sender);
	void __fastcall KFClick(TObject *Sender);
	void __fastcall CBRXCarrierChange(TObject *Sender);
	void __fastcall CBRXCarrierDropDown(TObject *Sender);
	void __fastcall CBRXCarrierKeyPress(TObject *Sender, char &Key);
	void __fastcall UdMacClick(TObject *Sender, TUDBtnType Button);
	
	void __fastcall PBoxRXMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);

	void __fastcall SBQSOClick(TObject *Sender);
	void __fastcall KVClick(TObject *Sender);
	void __fastcall KVF1Click(TObject *Sender);
	void __fastcall SBNETClick(TObject *Sender);
	void __fastcall KVFS2Click(TObject *Sender);
	void __fastcall KVFS4Click(TObject *Sender);
	void __fastcall KFXClick(TObject *Sender);
	void __fastcall FormActivate(TObject *Sender);
	void __fastcall FormDeactivate(TObject *Sender);
	void __fastcall KEClick(TObject *Sender);
	void __fastcall KEPClick(TObject *Sender);
	void __fastcall SBarRXChange(TObject *Sender);
	void __fastcall SBarTXChange(TObject *Sender);
	void __fastcall SBBPFWMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall KOVOClick(TObject *Sender);
	void __fastcall KOOClick(TObject *Sender);
	void __fastcall KOAIClick(TObject *Sender);
	void __fastcall KOAOClick(TObject *Sender);
	void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
	void __fastcall SBInitClick(TObject *Sender);
	void __fastcall KOClick(TObject *Sender);
	void __fastcall KHAClick(TObject *Sender);
	void __fastcall KHVClick(TObject *Sender);
	
	void __fastcall FormPaint(TObject *Sender);
	void __fastcall KVFS3Click(TObject *Sender);
	void __fastcall KVCRClick(TObject *Sender);
	void __fastcall KVCTClick(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall SBATCMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	

	void __fastcall StatusBarMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall KANSIClick(TObject *Sender);
	void __fastcall KMISCClick(TObject *Sender);
	void __fastcall PupCharsetPopup(TObject *Sender);
	void __fastcall HisRSTChange(TObject *Sender);
	void __fastcall KOVClick(TObject *Sender);
	void __fastcall KECClick(TObject *Sender);
	void __fastcall KFWSClick(TObject *Sender);
	void __fastcall KFRSClick(TObject *Sender);
	void __fastcall KFESClick(TObject *Sender);
	void __fastcall KVSPClick(TObject *Sender);
	void __fastcall KFWSTClick(TObject *Sender);

	void __fastcall KVLClick(TObject *Sender);
	void __fastcall KFLFClick(TObject *Sender);
	void __fastcall HisCallChange(TObject *Sender);
	void __fastcall SBDataClick(TObject *Sender);
	void __fastcall SBFindClick(TObject *Sender);
	void __fastcall KOLClick(TObject *Sender);
	void __fastcall KFLOClick(TObject *Sender);
	void __fastcall LogFreqChange(TObject *Sender);
	void __fastcall SBListClick(TObject *Sender);

	void __fastcall KRXMYClick(TObject *Sender);
	void __fastcall KRXQTHClick(TObject *Sender);
	void __fastcall KRXNAMEClick(TObject *Sender);
	void __fastcall KRXNOTEClick(TObject *Sender);
	void __fastcall KRXCALLClick(TObject *Sender);
	void __fastcall KVWAClick(TObject *Sender);
	void __fastcall PBoxFFTMouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y);
	void __fastcall PBoxFFTMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall PBoxLevelMouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y);
	void __fastcall PBoxLevelMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall SBMClick(TObject *Sender);
	void __fastcall KRXCopyClick(TObject *Sender);
	void __fastcall KVLAClick(TObject *Sender);
	void __fastcall PLogMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall KVM2Click(TObject *Sender);
	void __fastcall UdMacMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall KRXADDNAMEClick(TObject *Sender);
	void __fastcall KRXADDNOTEClick(TObject *Sender);
	void __fastcall KOAClick(TObject *Sender);
	void __fastcall SBDataMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall SBFindMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	
	void __fastcall KFLClick(TObject *Sender);
	
	void __fastcall KVSFTClick(TObject *Sender);
	void __fastcall KORClick(TObject *Sender);
	void __fastcall CBModeChange(TObject *Sender);
	void __fastcall KFLRClick(TObject *Sender);
	void __fastcall PBoxTXMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall KECPClick(TObject *Sender);
	void __fastcall KEXClick(TObject *Sender);
	void __fastcall PupTXPopup(TObject *Sender);
	void __fastcall PBoxTXMouseMove(TObject *Sender, TShiftState Shift, int X,
          int Y);
	void __fastcall PBoxTXMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);

	
	void __fastcall KRXADDQSLClick(TObject *Sender);
	void __fastcall KECallClick(TObject *Sender);
	void __fastcall SBTXMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall KPG1Click(TObject *Sender);
	void __fastcall PupPagePopup(TObject *Sender);
	
	void __fastcall SBWaveMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	
	
	void __fastcall KSASClick(TObject *Sender);
	void __fastcall KFMSClick(TObject *Sender);
	void __fastcall KFMLClick(TObject *Sender);
	void __fastcall KRWCClick(TObject *Sender);
	void __fastcall KRWTClick(TObject *Sender);
	void __fastcall PupRXWPopup(TObject *Sender);
	void __fastcall KRWEClick(TObject *Sender);
	void __fastcall PupSpecPopup(TObject *Sender);
	void __fastcall KSWClick(TObject *Sender);
	void __fastcall KVSDClick(TObject *Sender);
	void __fastcall StatusBarDrawPanel(TStatusBar *StatusBar,
          TStatusPanel *Panel, const TRect &Rect);
	void __fastcall StatusBarMouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y);
	void __fastcall KRXADDMYClick(TObject *Sender);
	void __fastcall KRXINVClick(TObject *Sender);
	void __fastcall PupRXPopup(TObject *Sender);
	void __fastcall KORSCClick(TObject *Sender);
	void __fastcall KSNClick(TObject *Sender);
	
	void __fastcall KSTXClick(TObject *Sender);
	void __fastcall KSNRAClick(TObject *Sender);
	void __fastcall KSRXClick(TObject *Sender);
	void __fastcall KSSRClick(TObject *Sender);
	
	void __fastcall PBoxPFPaint(TObject *Sender);
	
	void __fastcall KVMXClick(TObject *Sender);
	void __fastcall SBP60Click(TObject *Sender);
	void __fastcall KRSCClick(TObject *Sender);
	void __fastcall PupSQPopup(TObject *Sender);
	void __fastcall KS0Click(TObject *Sender);
	void __fastcall KHJClick(TObject *Sender);
	
	
	void __fastcall KHClick(TObject *Sender);
	void __fastcall KSNRClick(TObject *Sender);
	void __fastcall KRClick(TObject *Sender);
	void __fastcall KRADDClick(TObject *Sender);
	void __fastcall KRLClick(TObject *Sender);
	void __fastcall KROClick(TObject *Sender);
	void __fastcall PupCallsPopup(TObject *Sender);
private:	// ユーザー宣言
	BOOL			m_fInitFirst;
    BOOL			m_fDisEvent;
    int				m_AppErr;
public:
	TTimer			*m_pSoundTimer;
	TTimer			*m_pLogLinkTimer;
	TTimer			*m_pRadioTimer;
	TTimer			*m_pMacroTimer;
	TTimer			*m_pPlayBackTimer;
	TTimer			*m_pMacroOnTimer;
	int				m_BufferSize;
	SHORT			m_wBuffer[8192];
	WAVEFORMATEX	m_WFX;
	CWave			m_Wave;
    CWaveFile		m_WaveFile;

	int				m_SoundMsgTimer;
    int				m_RecoverSoundMode;
	int				m_LostSoundRX;
    int				m_LostSoundTX;
	int				m_fShowMsg;
	int				m_cInfoMsg[5];
    int				m_InfoMsgFlag;
	AnsiString		m_InfoMsg[5];
	int				m_cErrorMsg;
	AnsiString		m_ErrorMsg;

	TPlayDlgBox		*m_pPlayBox;

	int				m_TX;
	BOOL			m_fHPF;
	CIIR			m_HPF;

    int				m_NotchFreq;
    CNotches		m_Notches;
	int				m_MouseNotch;

	CRxSet			m_RxSet[RXMAX];		// 受信管理データ
    CMODFSK			m_ModFSK;
	int				m_ModGain;
    double			m_ModGainR;
//	double			m_Speed;

#if DEBUG
    CMODFSK			m_ModTest;	// For test
	CVCO			m_VCOTest;	// For test
    CIIR			m_TestHPF;
    CFIR2			m_BPF500;
#endif

	BOOL			m_StatusUTC;
    CDump			m_Dump;
    CDump			m_Edit[4];
	int				m_SendingEdit;
	int				m_CurrentEdit;
	int				m_SaveEditPage;
	BOOL			m_fSendChar;

	int				m_WaveBitMax;
	int				m_WaveType;
    CCOLLECT		m_Collect1;
    CCOLLECT		m_Collect2;
//	CFAVG			m_AVGWave;

#if DEBUG
    CQSB			m_QSB;
    CNoise			m_Noise;
#endif

	double			m_DecFactor;
	int				m_fDec;
    CDECM2			m_Dec2;

	int				m_RadioScaleCounter;
	int				m_ScaleAsRigFreq;
    int				m_ScaleDetails;
	int				m_FFTVType;
	int				m_FFTSmooth;
	int				m_FFTW;
    int				m_FFTB;
    int				m_FFTU;
	int				m_FFTWindow;
    double			m_FFTFactor;
    double			m_FFTSampFreq;
	double			m_fftbuf[FFT_BUFSIZE*2];
    int				m_fftout[FFT_BUFSIZE];
    CFFT			m_FFT;

	int				m_WaterNoiseL;
    int				m_WaterNoiseH;

    int				m_AFCWidth;
    int				m_AFCLevel;
    int				m_ATCLevel;
    int				m_ATCSpeed;
	int				m_ATCLimit;
	int				m_AFCKeyTimer;

	int				m_SkipTmg;
    BOOL			m_Lock;
	BOOL			m_fReqRX;
	BOOL			m_fDrop;

	DWORD			m_QSOStart;
    SYSTEMTIME		m_LocalTime;

	BOOL			m_fSubWindow;

    int				m_fftSC;
	int				m_fftMX;
	int				m_fftXW, m_fftYW;
    Graphics::TBitmap *m_pBitmapFFT;
	int				m_levelXW, m_levelYW;
    Graphics::TBitmap *m_pBitmapLevel;

	int				m_PFTimer;
    int				m_pfXW, m_pfYW, m_pfXC;
    Graphics::TBitmap *m_pBitmapPF;

	int				m_tWaterLevel[6];
    UCOL			m_tWaterColset[12];
	TColor			m_tWaterColors[256];

    UCOL			m_tFFTColset[6];

    FFTSTG			m_StgWater;

	int				m_TestTimer;
    FILE			*m_fpText;
    FILE			*m_fpTest;

    int				m_MacButtonVW;
	MACBUTTON		m_tMacButton[MACBUTTONALL];

    CComm			*m_pCom;
    CCradio			*m_pRadio;
	TFileEdit		*m_pHelp;
	TFileEdit		*m_pEdit;

    char			m_HintText[256];	// ヒント用のバッファ
    char			m_TextBuff[256];	// 汎用のバッファ

	LPCSTR			m_pCheckKey;
	AnsiString		*m_pCheckBuff;

    BOOL			m_fHintUpdate;
	AnsiString		m_HintKey;

	BOOL			m_MouseDown;
    int				m_MouseSubChannel;

	int				m_Priority;
    int				m_Dupe;

    AnsiString		m_StrSel;
    AnsiString		m_UStrSel;

	int				m_MacroMenuNo;
	TPopupMenu		*m_pMacroPopup;
    TMenuItem		*m_pCurrentMacroMenu;
    int				m_CurrentMacro;
	BOOL			m_fMacroRepeat;
    int				m_ReqMacroTimer;
	int				m_ReqAutoClear;
    int				m_ReqAutoReturn;
	int				m_ReqAutoNET;
	TMenuItem		*m_ParentMenu;

    TClockAdjDlg	*m_pClockView;

    BOOL			m_fKeyShift;
    BOOL			m_fSuspend;
    BOOL			m_fTone;

	int				m_RightX;
    int				m_RightFreq;

    AnsiString		m_MacroInput;
    CVal			m_MacroVal;

    BOOL			m_fMBCS;
    BOOL			m_fConvAlpha;
	BOOL			m_fRttyWordOut;

	AnsiString		m_strStatus[statusEND];
    TRect			m_rcStatus[statusEND];


    TTimer			*m_pWheelTimer;
    CDump			*m_pDump;

	int				m_CPUBENCHType;
    CFAVG			m_CPUBENCH;

	BYTE			m_PlayBackTime[3];
    CPlayBack		m_PlayBack;
    CWebRef			m_WebRef;
	CLIBL			m_LibDLL;

    int				m_LogBand;

    AnsiString		m_ListBAUD;
	AnsiString		m_strLogMode;

public:
	int			m_nRadioMenu;
private:
	RADIOMENU	m_RadioMenu[RADIOMENUMAX];

#if DEBUG
	TSpeedButton	*m_pDebugButton;
#endif

private:
	void __fastcall OnAppMessage(tagMSG &Msg, bool &Handled);
	void __fastcall OnActiveFormChange(TObject *Sender);
	void __fastcall SetSystemFont(void);
	void __fastcall SoundTimer(TObject *Sender);
	void __fastcall LogLinkTimer(TObject *Sender);
	void __fastcall RadioTimer(TObject *Sender);
	void __fastcall MacroTimer(TObject *Sender);
	void __fastcall WheelTimer(TObject *Sender);
	void __fastcall MacroOnTimer(TObject *Sender);

	LPCSTR __fastcall GetHintStatus(LPCSTR pHint);
	//void __fastcall DrawStatusBar(const Windows::TRect &Rect, LPCSTR pText, TColor col);
	void __fastcall DrawStatusBar(const TRect &Rect, LPCSTR pText, TColor col);	//JA7UDE 0428
	void __fastcall DrawHint(void);
	void __fastcall DrawStatus(int n, LPCSTR pText);
	void __fastcall DrawStatus(int n);
	int __fastcall GetStatusIndex(int x);
	int __fastcall GetPopupIndex(TComponent *pComponent);
	void __fastcall UpdateCharset(void);
	void __fastcall UpdateTitle(void);
	void __fastcall ReadRegister(void);
	void __fastcall WriteRegister(void);
	void __fastcall SampleStatus(void);
	void __fastcall PageStatus(void);
	BOOL __fastcall SampleFreq(double f);
	void __fastcall FFTSampleFreq(double f);
	BOOL __fastcall OpenSound(BOOL fTX);
	BOOL __fastcall ReOutOpen(void);
	void __fastcall OpenCom(void);
	void __fastcall OpenRadio(void);
	void __fastcall SetCBSpeed(void);
	void __fastcall UpdateUI(void);
	void __fastcall UpdateLogPanel(void);
	void __fastcall OnWaveIn(void);
	void __fastcall OnWaveOut(void);
	void __fastcall RemoveUselessMessage(UINT wParam);
	void __fastcall InitWFX(void);
	void __fastcall Draw(BOOL fPaint);
	void __fastcall DrawFFT(BOOL fPaint);
	void __fastcall DrawWater(BOOL fPaint, BOOL fClear);
	void __fastcall DrawWave(BOOL fPaint);
	void __fastcall DrawPF(BOOL fPaint);
	void __fastcall CalcFFTCenter(int fo);
	void __fastcall CalcFFTFreq(void);
	void __fastcall SetFFTWidth(int fw);
	void __fastcall InitStgFFT(FFTSTG *pStg);
	void __fastcall InitWater(int sw);
	double __fastcall SqrtToDB(double d);
	double __fastcall DBToSqrt(double d);
	double __fastcall AdjDB(double d);
	void __fastcall CalcStgFFT(CRxSet *pRxSet);
	void __fastcall DoAFCMFSK(CRxSet *pRxSet, int fo, BOOL fUpdate);
	void __fastcall CreateWaterColors(void);

	void __fastcall DrawLevel(BOOL fPaint);
	void __fastcall DoMod(void);
	void __fastcall PutDumpChar(int d, CRxSet *pRxSet, CDump *pDump);
	void __fastcall DoDem(double d);
	void __fastcall DoDem(void);
	void __fastcall UpdateWaveCaption(void);
	void __fastcall SetTXCaption(void);
	void __fastcall SetPTT(BOOL fTX);
	void __fastcall RxStatus(CRxSet *pRxSet, LPCSTR p);
	void __fastcall SetTXInternal(void);
	void __fastcall ToTone(void);
	void __fastcall ToTX(void);
	void __fastcall ToRX(void);
	void __fastcall DeleteSoundTimer(void);
	void __fastcall SetRxFreq(int fq);
	void __fastcall SetTxFreq(int fq);
	void __fastcall SetMacButtonMax(void);
	void __fastcall CreateMacButton(void);
	int __fastcall GetMacButtonNo(TSpeedButton *pButton);
	void __fastcall OnMacButtonClick(TObject *Sender);
	void __fastcall OnMacButtonDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall CreateMacExButton(void);
	void __fastcall DeleteMacExButton(void);
	void __fastcall AttachFocus(void);
	void __fastcall DettachFocus(void);
	TSpeedButton *__fastcall GetDraw(int n);
	int __fastcall GetDrawType(void);
	void __fastcall SetDrawType(int n);
	TSpeedButton *__fastcall GetBPF(int n);
	int __fastcall GetBPFType(void);
	void __fastcall SetBPFType(int n);
	int __fastcall GetBPFTaps(TObject *Sender);

	void __fastcall InitCheckValKey(LPCSTR pKey, AnsiString *pAS);
	BOOL __fastcall CheckKey(LPCSTR pTemplate);
	LPCSTR __fastcall CheckValKey(LPCSTR pTemplate);
	LPCSTR __fastcall CheckValKey(LPCSTR pKey, LPCSTR pTemplate);
	void __fastcall MacroDate(LPSTR t, SYSTEMTIME &now);
	void __fastcall MacroHisName(LPSTR t);
//	BOOL __fastcall CheckCond(LPCSTR p, LPCSTR v, int type, int &op, double &d, AnsiString &as);
	LPCSTR __fastcall Cond(LPCSTR p, TSpeedButton *pButton);
	BOOL __fastcall IsMBCSStr(LPCSTR p);
    BOOL __fastcall GetDataCond(LPCSTR p, int err, TSpeedButton *pButton);
	int __fastcall ConvMacro(LPSTR t, LPCSTR p, TSpeedButton *pButton);
	void __fastcall DoMacroReturn(int f);
	void __fastcall DoMacro(LPCSTR pMacro, TSpeedButton *pButton);
	void __fastcall SendButton(int n);

	void __fastcall UpdateCallsign(void);
	void __fastcall UpdateLogMode(void);
	void __fastcall UpdateLogData(void);
	void __fastcall UpdateTextData(void);
	void __fastcall FindCall(void);
	void __fastcall UpdateLogLink(void);
	void __fastcall AutoLogSave(void);
	void __fastcall WndCopyData(TMessage &Message);
	void __fastcall CheckLogLink(void);
	void __fastcall InitDefKey(void);
	void __fastcall UpdateSubWindow(void);
	void __fastcall DrawSubChannel(TPaintBox *pBox);
	void __fastcall AppException(TObject *Sender, Exception *E);
	void __fastcall ShowSubChannel(int n, int sw);
	void __fastcall MacroGreeting(LPSTR t, LPCSTR pCall, int type);

	void __fastcall KCallClick(TObject *Sender);
	void __fastcall AddCall(LPCSTR p);
	void __fastcall SetTXFocus(void);

	BOOL __fastcall DeleteMacroTimerS(void);
	void __fastcall DeleteMacroTimer(void);
	void __fastcall CreateMacroTimer(int n);

    void __fastcall UpdateLogHeight(void);
	void __fastcall UpdateMacButtonVW(int n);
	void __fastcall UpdateSpeedList(int mode);
	void __fastcall SetMode(int mode);
	void __fastcall DoSuspend(BOOL fMinimize);
	void __fastcall DoResume(void);
	void __fastcall SetEditPage(int cno);
	void __fastcall InitCollect(void);
	void __fastcall LoadMacro(LPCSTR pName);
	void __fastcall SaveMacro(LPCSTR pName);
	void __fastcall LoadMacro(TMemIniFile *pIniFile);
	void __fastcall SaveMacro(TMemIniFile *pIniFile);
	void __fastcall SetSpeedInfo(double b);
	BOOL __fastcall IsFreqErr(double d);
	int __fastcall GetMsgCount(void);

	inline BOOL __fastcall IsRTTY(void){return m_RxSet[0].IsRTTY();};
	inline BOOL __fastcall Is170(void){return m_RxSet[0].Is170();};
	inline BOOL __fastcall IsBPSK(void){return m_RxSet[0].IsBPSK();};
	inline BOOL __fastcall IsPSK(void){return m_RxSet[0].IsPSK();};

	void __fastcall OutYaesuVU(int hz);
	void __fastcall OutYaesuHF(int hz);
	void __fastcall OutCIV(int hz);
	void __fastcall OutCIV4(int hz);
	void __fastcall OutKENWOOD(int hz);
	void __fastcall OutJST245(int hz);

	void __fastcall OutModeYaesuHF(LPCSTR pMode);
	void __fastcall OutModeYaesuVU(LPCSTR pMode);
	void __fastcall OutModeCIV(LPCSTR pMode);
	void __fastcall OutModeKENWOOD(LPCSTR pMode);
	void __fastcall OutModeJST245(LPCSTR pMode);

	void __fastcall OnMacroMenuClick(TObject *Sender);
	void __fastcall WaitICOM(void);
	void __fastcall DoMacroMenu(LPCSTR pVal, TSpeedButton *pButton, BOOL fRadio);

	void __fastcall UpdatePlayBack(void);
	void __fastcall DoPlayBack(int s);
	void __fastcall StopPlayBack(void);

	void __fastcall UpdateShowCtrl(void);
	void __fastcall DoEvent(int n);
	void __fastcall UpdateMacroOnTimer(void);
	void __fastcall OnLogFreq(BOOL fLink);
	void __fastcall ShutDown(void);
	void __fastcall DoBench(LPSTR t, int type);
	void __fastcall OnXmClick(TObject *Sender);
	void __fastcall OnXClick(TObject *Sender);
	TMenuItem* __fastcall FindMenu(TMenuItem *pMainMenu, LPCSTR pTitle);
	TMenuItem* __fastcall GetMenuItem(TMenuItem *pMenu, LPCSTR pTitle);
	TMenuItem* __fastcall GetMainMenu(LPCSTR pTitle, BOOL fNew);
	TMenuItem* __fastcall GetSubMenu(LPCSTR pTitle);
	void __fastcall AddExtensionMenu(LPCSTR pMain, LPCSTR pCaption, LPCSTR pHint);
	void __fastcall InsExtensionMenu(LPCSTR pTitle, LPCSTR pPos, LPCSTR pCaption, LPCSTR pHint);
	void __fastcall ShortCutExtensionMenu(TMenuItem *pMenu, LPCSTR pKey);
	BOOL __fastcall IsXMenu(TMenuItem *pItem);
	BOOL __fastcall OnMenuProc(TMenuItem *pMenu, LPCSTR pProc, LPCSTR pPara, BOOL fStop);
	void __fastcall OnMenuProc(TMenuItem *pMenu, LPCSTR pCaption);
	void __fastcall DoParentClick(TMenuItem *pMenu);
    TMenuItem* __fastcall GetMenuArg(AnsiString &arg, LPCSTR pVal, BOOL fArg);
	void __fastcall SetRadioMenu(TMenuItem *pMenu);

	TSpeedButton* __fastcall FindButton(TComponent *pMainControl, LPCSTR pTitle, TSpeedButton *pButton, BOOL fErrMsg);
	void __fastcall ClickButton(TSpeedButton *pButton);

	void __fastcall CreateSubMenu(void);
	void __fastcall KVSClick(TObject *Sender);
	void __fastcall KSSClick(TObject *Sender);

	void __fastcall LoadRadioDef(LPCSTR pName);
	void __fastcall AdjustRadioMenu(void);
	void __fastcall KRadioCmdClick(TObject *Sender);
	void __fastcall KRadioEditClick(TObject *Sender);

#if DEBUG
	void __fastcall TestSignal(void);
#endif

public:		// ユーザー宣言
	__fastcall TMainVARI(TComponent* Owner);

	void __fastcall WndProc(TMessage &Message);
	void __fastcall DisplayHint(TObject *Sender);
  	void __fastcall OnWave(void);
	void __fastcall SetSoundCard(int ch, LPCSTR pID, LPCSTR pIDTX);
	void __fastcall SetSampleFreq(double f, BOOL fForce);
	void __fastcall SetTxOffset(double f);
	void __fastcall SetRXFifo(int d);
	void __fastcall SetTXFifo(int d);
	void __fastcall UpdatePriority(int Priority);
	void __fastcall OnFontChange(BOOL fTX);
	void __fastcall DrawFreqScale(TCanvas *pCanvas, int XW, int YW,  int fftb, int fftw, int fh, BOOL fRadio);
	void __fastcall DrawErrorMsg(TCanvas *pCanvas, int XW, int YW, BOOL fReset);
	void __fastcall SetInfoMsg(LPCSTR pStr);
	void __fastcall SetInfoMsg(LPCSTR pStr, int pos);
	void __fastcall SetErrorMsg(LPCSTR pStr);
	int __fastcall GetOverlayTop(void);
	void __fastcall SetConvAlpha(BOOL f);

	void __fastcall SpeedChange(double b);
	void __fastcall UpdateSpeed(CRxSet *pRxSet, double b);
	void __fastcall UpdateMode(CRxSet *pRxSet, int offset);
	void __fastcall SetATCSpeed(int f);
	void __fastcall SetATCLimit(int f);
	BOOL __fastcall GetDataConds(LPCSTR p, int err, TSpeedButton *pButton);

	LPCSTR __fastcall GetMacroValue(LPCSTR pVal);
	LPCSTR __fastcall GetMacroStr(AnsiString &as, LPCSTR pVal);
	int __fastcall GetMacroInt(LPCSTR pVal);
	double __fastcall GetMacroDouble(LPCSTR pVal);
	int __fastcall ConvMacro(AnsiString &as, LPCSTR p, TSpeedButton *pButton);
	int __fastcall ConvMacro_(AnsiString &as, LPCSTR p, TSpeedButton *pButton);
	int __fastcall ConvMacro_(LPSTR t, LPCSTR p, TSpeedButton *pButton);

	int __fastcall GetSignalFreq(int freq, int fm, CRxSet *pRxSet);
	int __fastcall TMainVARI::GetSignalFreq(int fo, int fm, CRxSet *pRxSet, int th);
	void __fastcall OnChar(int Key);
    void __fastcall OpenWheelTimer(CDump *pDump);
	void __fastcall CloseWheelTimer(CDump *pDump);
	void __fastcall UpdateModGain(void);

	void __fastcall InitCollect(CRxSet *pRxSet, int n);
	inline void __fastcall ShowMacroSample(void){KHS->Click();};
	inline void __fastcall ShowManual(void){KHO->Click();};

	void __fastcall ExtFskIt(int r);
	void __fastcall SetRTTYFFT(int f);

	friend int __fastcall OnGetChar(void);

protected:
	void __fastcall OnWaveEvent(TMessage &Message);
	void __fastcall CMMML(TMessage &Message);
	void __fastcall CMMMR(TMessage &Message);
    void __fastcall OnActiveApp(TMessage &Message);
    void __fastcall OnMouseWheel(TMessage &Message);
BEGIN_MESSAGE_MAP
	MESSAGE_HANDLER(WM_WAVE, TMessage, OnWaveEvent)
	MESSAGE_HANDLER(CM_CMML, TMessage, CMMML)
	MESSAGE_HANDLER(CM_CMMR, TMessage, CMMMR)
	MESSAGE_HANDLER(WM_ACTIVATEAPP, TMessage, OnActiveApp)
	MESSAGE_HANDLER(WM_MOUSEWHEEL, TMessage, OnMouseWheel)
END_MESSAGE_MAP(TForm)
};
//---------------------------------------------------------------------------
extern PACKAGE TMainVARI *MainVARI;
//---------------------------------------------------------------------------
#endif
