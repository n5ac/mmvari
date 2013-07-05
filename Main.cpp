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

#include <SHELLAPI.H>
#include "clipbrd.hpp"
#include <math.h>
#include "Main.h"
#include "LogFile.h"
#include "MacEdit.h"
#include "FreqDisp.h"
#include "Option.h"
#include "VerDsp.h"
#include "InputWin.h"
#include "CodeVw.h"
#include "Country.h"
#include "Mmcg.h"
#include "LogLink.h"
#include "LogList.h"
#include "QSODlg.h"
#include "LogSet.h"
#include "RadioSet.h"
#include "TrackDlg.h"
#include "mml.h"
#if DEBUG
#include "Test.h"
#endif

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TMainVARI *MainVARI;
//ÉAÉvÉäÉPÅ[ÉVÉáÉìÉÅÉbÉZÅ[ÉWÇÃÉnÉìÉhÉâ---------------------------------------------
void __fastcall TMainVARI::OnAppMessage(tagMSG &Msg, bool &Handled)
{
/*
Msg.hwnd := Handle;
Msg.message := WM_KEYDOWN;
Msg.wParam := Message.WParam;
Msg.lParam := Message.LParam;
*/
    switch(Msg.message){
		case WM_KEYUP:
			if( !Active ) return;
			if( ActiveControl != PCTX ) return;
			if( Msg.wParam == VK_SHIFT ){
				m_fKeyShift = FALSE;
        	    m_Edit[m_CurrentEdit].CloseSelect();
	        }
            break;
        case WM_KEYDOWN:
			if( !Active ) return;
			if( ActiveControl != PCTX ) return;
			switch(Msg.wParam){
				case VK_SHIFT:
					m_fKeyShift = TRUE;
        			break;
				case VK_DOWN:
				case VK_UP:
				case VK_LEFT:
				case VK_RIGHT:
					if( m_fKeyShift ) m_Edit[m_CurrentEdit].OpenSelect();
					m_Edit[m_CurrentEdit].MoveCursor(Msg.wParam);
					Handled = TRUE;
					break;
				case VK_PRIOR:
					if( m_fKeyShift ){
						SBarRX->Position--;
                    }
                    else {
//						if( m_fKeyShift ) m_Edit[m_CurrentEdit].OpenSelect();
						for( int i = 0; i < m_Edit[m_CurrentEdit].GetScreenLine()/2; i++ ){
	    	    			m_Edit[m_CurrentEdit].MoveCursor(VK_UP);
						}
                    }
					Handled = TRUE;
					break;
				case VK_NEXT:
					if( m_fKeyShift ){
						SBarRX->Position++;
                    }
                    else {
//						if( m_fKeyShift ) m_Edit[m_CurrentEdit].OpenSelect();
						for( int i = 0; i < m_Edit[m_CurrentEdit].GetScreenLine()/2; i++ ){
	    	    			m_Edit[m_CurrentEdit].MoveCursor(VK_DOWN);
						}
                    }
					Handled = TRUE;
					break;
				case VK_TAB:
					m_Edit[m_CurrentEdit].PutKey('\t', 1);
					Handled = TRUE;
        			break;
			}
			break;
#if KEY_DIRECT
        case WM_CHAR:
			if( !Active ) return;
			if( ActiveControl != PCTX ) return;
//			if( !m_Edit[m_CurrentEdit].IsActive() ) return;
			OnChar(Msg.wParam & (sys.m_WinNT ? 0x0000ffff : 0x000000ff));
			Handled = TRUE;
			break;
#endif
    }
}
//ÉEÉCÉìÉhÉEÉÅÉbÉZÅ[ÉWÇÃÉnÉìÉhÉâ---------------------------------------------
void __fastcall TMainVARI::WndProc(TMessage &Message)
{
	switch(Message.Msg){
		case WM_COPYDATA:
			WndCopyData(Message);
			break;
		default:
			TForm::WndProc(Message);
			break;
	}
}
//---------------------------------------------------------------------------
// WM_COPYDATAÇÃèàóù
void __fastcall TMainVARI::WndCopyData(TMessage &Message)
{
	COPYDATASTRUCT *cp = (COPYDATASTRUCT *)Message.LParam;
	if( LogLink.IsCopyData() ){
       	Message.Result = LogLink.m_pLink->OnCopyData(HWND(Message.WParam), cp);
		return;
	}
	switch(cp->dwData){
		case 0:
		case 1:             // HamlogÇ©ÇÁÇÃï‘êM
			if( sys.m_LogLink != 1 ) return;
			switch(LogLink.AnaData(&Log.m_sd, cp)){
				case 115:
					UpdateTextData();
					break;
				case 106:
					LogFreq->Text = Log.GetFreqString(Log.m_sd.band, Log.m_sd.fq);
					OnLogFreq(FALSE);
					break;
			}
			Message.Result = TRUE;
			break;
		case 0x80001212:    // é¸îgêîÉfÅ[É^ÇÃéwíË
			if( cp->cbData && (cp->lpData != NULL) ){
				char bf[16];
				int len = cp->cbData;
				if( len > 15 ) len = 15;
				memcpy(bf, cp->lpData, len);
				bf[len] = 0;
				LogFreq->Text = bf;
				OnLogFreq(FALSE);
			}
			Message.Result = TRUE;
			break;
		case 0x80001217:    // ëóêM/éÛêMÇÃêÿÇËë÷Ç¶
			if( cp->cbData && (cp->lpData != NULL) ){
				if( *(const BYTE *)(cp->lpData) ){
					if( !m_TX ){
						ToTX();
					}
				}
				else if( m_TX ){
					ToRX();
				}
			}
			Message.Result = TRUE;
			break;
        case 0x8000121a:	// PTT
			if( cp->cbData && (cp->lpData != NULL) ){
				BOOL bPTT = *(const BYTE *)(cp->lpData);
				SetPTT(bPTT);
            }
        	break;
        case 0x8000121b:	// status
			{
				DWORD dw = m_TX ? 1 : 0;
				Message.Result = dw;
            }
        	break;
		default:
			Message.Result = FALSE;
			break;
	}
}
//---------------------------------------------------------------------------
int __fastcall OnGetChar(void)
{
	CDump *pEdit = &MainVARI->m_Edit[MainVARI->m_SendingEdit];
	if( MainVARI->IsRTTY() && MainVARI->m_fRttyWordOut ){
    	if( !MainVARI->m_fReqRX && !pEdit->IsWord() ){
			return 0;
        }
    }
	int c = pEdit->GetChar(MainVARI->m_SendingEdit==MainVARI->m_CurrentEdit);
	if( c < 0 ) c = pEdit->GetChar(MainVARI->m_SendingEdit==MainVARI->m_CurrentEdit);
    if( c == '\r' ){
		MainVARI->m_ModFSK.m_Encode.PutChar('\n');
    }
	if( c >= 0 ) MainVARI->m_fSendChar = TRUE;
	return c;
}
//---------------------------------------------------------------------------
LPCSTR __fastcall TMainVARI::GetHintStatus(LPCSTR pHint)
{
	if( m_WaveFile.m_mode ){
		m_fHintUpdate = TRUE;
        LPCSTR pFmt;
		if( sys.m_MsgEng ){
			pFmt = m_WaveFile.m_mode == 1 ? "Sound playing...  [%s] %u:%02u:%02u" : "Sound recording...  [%s] %u:%02u:%02u  (%.1lfMB)";
        }
        else {
			pFmt = m_WaveFile.m_mode == 1 ? "ÉTÉEÉìÉhÇçƒê∂íÜ...  [%s] %u:%02u:%02u" : "ÉTÉEÉìÉhÇò^âπíÜ...  [%s] %u:%02u:%02u  (%.1lfMB)";
        }
		int mb = m_WaveFile.GetPos();
		int h = int(mb * 0.5 / SAMPFREQ);
        int s = h % 60;
        h /= 60;
        int m = h % 60;
        h /= 60;
        sprintf(m_HintText, pFmt, m_WaveFile.m_Name.c_str(), h, m, s, double(mb)/(1024.0*1024.0));
        return m_HintText;
    }
	else if( !strcmp(pHint, "%SQ") ){
		m_fHintUpdate = TRUE;
		sprintf(m_HintText, sys.m_MsgEng ? "Squelch level (S/N) = %.1lfdB  (Right click for menu)":"ÉXÉPÉãÉ`ÉåÉxÉã(S/N) = %.1lfdB  (âEÉNÉäÉbÉNÇ≈ÉÅÉjÉÖÅ[)", double((m_RxSet[0].m_SQLevel) / 100.0));
		return m_HintText;
    }
    else if( !strcmp(pHint, "%SP") ){
		if( SBFFT->Down || SBWater->Down ){
			if( m_fSubWindow || m_Notches.m_Count ) m_fHintUpdate = TRUE;
			if( m_MouseNotch ){
				return sys.m_MsgEng ? "Drag for changing Notch FREQ":"ÉmÉbÉ`é¸îgêîÇïœçXÅiÉhÉâÉbÉOëÄçÏ)";
            }
			else if( m_fSubWindow && m_MouseSubChannel ){
				sprintf(m_HintText, sys.m_MsgEng ? "Drag for changing RX FREQ (CH%d)":"É`ÉÉÉìÉlÉã%uÇÃéÛêMÉLÉÉÉäÉAé¸îgêîÇïœçXÅiÉhÉâÉbÉOëÄçÏ)", m_MouseSubChannel);
				return m_HintText;
            }
            else {
				return sys.m_MsgEng ? "RX FREQ(Left click), open the popup menu(Right click)":"ç∂ÉNÉäÉbÉN=éÛêMÉLÉÉÉäÉAé¸îgêî,  âEÉNÉäÉbÉN=ÉTÉuÉÅÉjÉÖÅ[ÇäJÇ≠";
            }
        }
		else if( m_WaveType ){
			return sys.m_MsgEng ? "Amplitude ingredient" : "êUïùê¨ï™ï\é¶";
        }
		else if( m_RxSet[0].IsMFSK() || m_RxSet[0].IsQPSK() ){
			return sys.m_MsgEng ? "Timing" : "É^ÉCÉ~ÉìÉOÇï\é¶";
        }
	    else {
			return sys.m_MsgEng ? "Timing of the CODE" : "É^ÉCÉ~ÉìÉOÇïÑçÜíPà Ç≈ï\é¶";
        }
    }
    else if( !strcmp(pHint, "%SD") ){
		if( m_RxSet[0].IsMFSK() ){
			CDEMFSK *pDem = m_RxSet[0].m_pDem;
			char bf[64];
			sprintf(m_HintText, "mfsk%.0f   %s baud   %u tones  (R=1/2, K=7)  %.2lf bps", pDem->m_MFSK_SPEED, StrDbl(bf, pDem->m_MFSK_SPEED), pDem->m_MFSK_TONES, pDem->m_MFSK_SPEED * pDem->m_MFSK_BITS * 0.5);
        }
        else {
			sprintf(m_HintText, sys.m_MsgEng ? "Baudrate (Standard = %sbps)" : "ì`ëóë¨ìx(BPS) ïWèÄÇÕ%s", IsRTTY() ? "45.45":"31.25");
        }
		return m_HintText;
	}
    else if( !strcmp(pHint, "%TX") ){
		sprintf(m_HintText, sys.m_MsgEng ? "Switch TX/RX - %s (Right click for TONE)" : "ëóêM/éÛêMÇÃêÿÇËë÷Ç¶ - %s (âEÉNÉäÉbÉNÇ≈TONE)", GetKeyName(sys.m_DefKey[kkTX]));
		return m_HintText;
	}
    else if( !strcmp(pHint, "%TXOFF") ){
		sprintf(m_HintText, sys.m_MsgEng ? "Abort TX - %s":"ã≠êßìIÇ…éÛêMÇ…êÿÇËë÷Ç¶ - %s", GetKeyName(sys.m_DefKey[kkTXOFF]));
		return m_HintText;
    }
    else if( !strcmp(pHint, "%RX") ){
		if( IsRTTY() ){
			m_fHintUpdate = TRUE;
			int cf = UdRxCarrier->Position;
            int mf = m_RxSet[0].m_pDem->m_RTTYShift * 0.5;
			if( m_RxSet[0].m_Mode == MODE_U_RTTY ) mf = -mf;
            mf = cf - mf;
			sprintf(m_HintText, sys.m_MsgEng ? "RX Carrier FREQ=%uHz, Mark=%uHz":"éÛêMÉLÉÉÉäÉAé¸îgêî=%uHz, É}Å[ÉNé¸îgêî=%uHz", cf, mf);
			return m_HintText;
        }
        else {
			return sys.m_MsgEng ? "RX Carrier FREQ" : "éÛêMÉLÉÉÉäÉAé¸îgêî";
        }
	}
    else if( !strcmp(pHint, "%TXW") ){
		if( m_fpText ){
			return sys.m_MsgEng ? "Sending text file...":"ÉeÉLÉXÉgÉtÉ@ÉCÉãÇëóêMíÜ...";
        }
        else if( m_ReqMacroTimer || m_pMacroTimer ){
			sprintf(m_HintText, sys.m_MsgEng ? "Repeating...(Interval=%ums)":"åJÇËï‘ÇµíÜ...(éûä‘ä‘äu=%dms)", m_ReqMacroTimer);
			return m_HintText;
        }
        else {
			return sys.m_MsgEng ? "TX window" :"ëóêMâÊñ ";
        }
	}
    else if( !strcmp(pHint, "%ATC") ){
		m_fHintUpdate = TRUE;
		if( !SBATC->Enabled || (m_TX == txINTERNAL) ){
			return sys.m_MsgEng ? "Sync timing":"ìØä˙É^ÉCÉ~ÉìÉO";
        }
        else {
            double ppm = m_RxSet[0].m_pDem->m_Decode.m_dTmg2;
    	    double fq = SAMPFREQ * ppm * 1e-6;
			sprintf(m_HintText, sys.m_MsgEng ? "Sync timing [%.0fppm, RxOffset=%.2lfHz]":"ìØä˙É^ÉCÉ~ÉìÉO [%.0fppm, RxOffset=%.2lfHz]", ppm, fq);
	        return m_HintText;
        }
	}
    else if( !strncmp(pHint, "%PB", 3) ){
		int i;
        sscanf(pHint+3, "%d", &i);
		switch(i){
			case 0:
				i = m_PlayBackTime[0];
                break;
            case 1:
				i = m_PlayBackTime[1];
                break;
            default:
				i = m_PlayBackTime[2];
            	break;
        }
		sprintf(m_HintText, sys.m_MsgEng ? "Replay the latest %u(sec)":"%uïbëOÇ©ÇÁÉvÉåÅ[ÉoÉbÉN", i);
    	return m_HintText;
	}
	else if( !strncmp(pHint, "%M", 2) ){
        int i;
        sscanf(pHint+2, "%d", &i);
		int f =  i - UdMac->Position * (MACBUTTONXW*m_MacButtonVW);
    	m_HintText[0] = 0;
		if( i < MACBUTTONMAX ){
			if( (f >= 0) && (f <= 11) ){
				sprintf(m_HintText, "F%u", f + 1);
	        }
	        else if( (f >= 12) && (f <= 23) ){
				sprintf(m_HintText, "Shift+F%u", f + 1 - 12);
	        }
	        else if( (f >= 24) && (f <= 35) ){
				sprintf(m_HintText, "Alt+F%u", f + 1 - 24);
	        }
	        else if( (f >= 36) && (f <= 47) ){
				sprintf(m_HintText, "Ctrl+F%u", f + 1 - 36);
	        }
			WORD dkey = GetKeyCode(m_HintText);
	        if( (dkey == sys.m_DefKey[kkTX]) || (dkey == sys.m_DefKey[kkTXOFF]) ){
				m_HintText[0] = 0;
	        }
        }
		MACBUTTON *pList = &m_tMacButton[i];
		if( pList->Text.IsEmpty() ){
	        strcat(m_HintText, sys.m_MsgEng ? " -Right click for registration":" -âEÉNÉäÉbÉNÇ≈ìoò^");
        }
        else {
			LPSTR t = &m_HintText[strlen(m_HintText)];
			*t++ = ' ';
            *t++ = '[';
            LPCSTR p = pList->Text.c_str();
            int n = 128;
            for( ; *p && n; n--, p++ ){
				if( *p == '\t' ){
					*t++ = ' ';
                }
                else if( (BYTE(*p) >= 0x20) ){
                	*t++ = *p;
                }
            }
            *t++ = ']';
            *t = 0;
        }
        return m_HintText;
    }
    else {
		return pHint;
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::OnFontChange(BOOL fTX)
{
	if( fTX ){
		m_Edit[m_CurrentEdit].SetFont(PCTX->Font);
    }
    else {
		m_Dump.SetFont(PCRX->Font);
		UpdateCharset();
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::DisplayHint(TObject *Sender)
{
	m_HintKey = GetLongHint(Application->Hint);
	m_fHintUpdate = FALSE;
    DrawHint();
}
//---------------------------------------------------------------------------
// ÉTÉEÉìÉhÉJÅ[ÉhÇÃÉIÅ[ÉvÉìÇÃÉgÉâÉCÅiÉIÅ[ÉvÉìÇ≈Ç´Ç»Ç¢éûÇÃÉäÉJÉoÉäèàóùÅj
void __fastcall TMainVARI::SoundTimer(TObject *Sender)
{
	if( m_fSuspend ) return;

	BOOL r;
	if( m_RecoverSoundMode ){
		r = ReOutOpen();
    }
    else {
		r = OpenSound(FALSE);
    }
	if( r ){
		m_pSoundTimer->Enabled = FALSE;
		delete m_pSoundTimer;
        m_pSoundTimer = NULL;
    }
    else {
		m_SoundMsgTimer -= m_pSoundTimer->Interval;
        if( m_SoundMsgTimer < 0 ){
			m_fShowMsg = TRUE;
        }
		PBoxFFTPaint(NULL);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::LogLinkTimer(TObject *Sender)
{
	if( LogLink.IsEnabled() ){
		if( LogLink.TimerLogLink() ){
			UpdateLogLink();
		}
		if( LogLink.IsPolling() && LogLink.IsLink() && !m_TX ){
			LogLink.EventGetFreq();
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::RadioTimer(TObject *Sender)
{
	if( m_pRadio != NULL ){
		m_pRadio->Timer(m_TX, m_pRadioTimer->Interval);
		if( m_pRadio->IsFreqChange(AnsiString(LogFreq->Text).c_str()) ){	//JA7UDE 0428
			LogFreq->Text = m_pRadio->GetFreq();
			LogFreqChange(NULL);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::MacroTimer(TObject *Sender)
{
	if( m_pMacroTimer ){
		m_pMacroTimer->Enabled = FALSE;
		if( m_CurrentMacro >= 0 ){
			m_fMacroRepeat = TRUE;
    	   	SendButton(m_CurrentMacro);
	    }
        else if( (m_CurrentMacro == -1) && IsXMenu(m_pCurrentMacroMenu) ){
			m_fMacroRepeat = TRUE;
			OnXClick(m_pCurrentMacroMenu);
        }
        else {
			delete m_pMacroTimer;
            m_pMacroTimer = NULL;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::WheelTimer(TObject *Sender)
{
	if( m_pWheelTimer ){
		if( !m_pDump || !m_pDump->OnTimer() ){
			m_pWheelTimer->Enabled;
            delete m_pWheelTimer;
            m_pWheelTimer = NULL;
            m_pDump = NULL;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::OnActiveApp(TMessage &Message)
{
	if( LogLink.IsEnabled() ){
		if( LogLink.TimerLogLink() ){
			UpdateLogLink();
		}
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::OnActiveFormChange(TObject *Sender)
{
	if( m_pHelp ){
		if( !::IsWindowEnabled(m_pHelp->Handle) ) ::EnableWindow(m_pHelp->Handle, TRUE);
    }
	if( m_pEdit ){
		if( !::IsWindowEnabled(m_pEdit->Handle) ) ::EnableWindow(m_pEdit->Handle, TRUE);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::FormCreate(TObject *Sender)
{
	Application->OnMessage = OnAppMessage;
	Application->OnHint = DisplayHint;
    Screen->OnActiveFormChange = OnActiveFormChange;
#if !DEBUG || SHOWERRCOUNT
	Application->OnException = AppException;
#endif
}
//---------------------------------------------------------------------------
// ÉAÉvÉäÉPÅ[ÉVÉáÉìó·äO
void __fastcall TMainVARI::AppException(TObject *Sender, Exception *E)
{
	m_AppErr++;
#if SHOWERRCOUNT
	sprintf(m_TextBuff, "%u (%u): %s", m_AppErr, sys.m_ErrPhase, E->Message.c_str());
    Caption = m_TextBuff;
#endif
	if( m_AppErr < 128 ){
		if( m_AppErr == 3 ){
			ErrorMB(sys.m_MsgEng?
				"The indistinct error was detected.":
				"ó\ëzäOÇÃÉGÉâÅ[Ç™î≠ê∂ÇµÇ‹ÇµÇΩ. âÒïúÇééÇ›Ç‹Ç∑."
			);
		}
        if( m_FFT.m_FFTDIS ){
			m_FFT.InitFFT();
			m_FFT.m_FFTDIS = FALSE;
        }
#if 0
		if( !m_TX ){
			m_Wave.InClose();
        	OpenSound(FALSE);
	    }
#endif
    }
}
//---------------------------------------------------------------------------
__fastcall TMainVARI::TMainVARI(TComponent* Owner)
	: TForm(Owner)
{
	m_fInitFirst = TRUE;
	m_fDisEvent = TRUE;

   	sys.m_dwVersion = ::GetVersion();
	if( sys.m_dwVersion < 0x80000000 ){
		sys.m_WinNT = TRUE;			// NT,2000,XP,Vista

        OSVERSIONINFO osvi;

        ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
        osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

        GetVersionEx(&osvi);
        if (osvi.dwMajorVersion == 5) {
            sys.m_WinVista=FALSE;
        }
        else {
            sys.m_WinVista=TRUE;
        }
	}
	else {		// win95/98/ME
		sys.m_WinNT = FALSE;
        sys.m_WinVista=FALSE;
	}

	// åæåÍ
	sys.m_LCID = GetThreadLocale();
#if FORCELANG
	sys.m_wLang = FORCELANG;
#else
    sys.m_wLang = LANGIDFROMLCID(sys.m_LCID);
#endif
    if( sys.m_wLang == 0x0411 ){	// ì˙ñ{åÍ
		sys.m_FontName = "ÇlÇr ÇoÉSÉVÉbÉN";
		sys.m_FontCharset = SHIFTJIS_CHARSET;
		sys.m_LogLink = 1;
        sys.m_MsgEng = FALSE;
    }
    else {
		sys.m_FontName = "Arial";
		sys.m_FontCharset = ANSI_CHARSET;
		Log.m_LogSet.m_TimeZone = 'Z';
		sys.m_LogLink = 0;
        sys.m_MsgEng = TRUE;
    }
	sys.m_fShowLangMsg = TRUE;
	sys.m_fBaseMBCS = SetLangFont(NULL, sys.m_wLang);
    sys.m_DefaultMode = sys.m_fBaseMBCS ? MODE_GMSK : MODE_N_BPSK;

	sys.m_fRestoreSubChannel = FALSE;
	sys.m_fFixWindow = FALSE;
	sys.m_rcWindow.left = Left;
    sys.m_rcWindow.top = Top;
    sys.m_rcWindow.right = Width;
    sys.m_rcWindow.bottom = Height;

	sys.m_AutoTimeOffset = 0;
	sys.m_TimeOffset;
	sys.m_TimeOffsetMin;
	sys.m_LogName = "NOCALL.MDT";
	InitRADIOPara();
    InitDefKey();

	if( ParamCount() >= 0 ){
		SetDirName(sys.m_BgnDir, AnsiString(ParamStr(0)).c_str());	//JA7UDE 0428
	}
	else {
		SetCurDir(sys.m_BgnDir, sizeof(sys.m_BgnDir));
	}
	strcpy(sys.m_SoundDir, sys.m_BgnDir);
	strcpy(sys.m_LogDir, sys.m_BgnDir);
	strcpy(sys.m_ExtLogDir, sys.m_BgnDir);
	strcpy(sys.m_TextDir, sys.m_BgnDir);
	sprintf(sys.m_MacroDir, "%sMacroTxt\\", sys.m_BgnDir);

    g_VariCode.Init();		// ÉoÉäÉRÅ[ÉhÉeÅ[ÉuÉãÇèâä˙âª

	LogLink.SetHandle(Handle, CM_CMML);
	sys.m_CallSign = "NOCALL";
    sys.m_bFSKOUT = FALSE;
    sys.m_bINVFSK = FALSE;
    sys.m_PTTCOM = "NONE";
    sys.m_PTTLock = TRUE;
	sys.m_fSendSingleTone = TRUE;
    sys.m_fPlayBack = FALSE;
	sys.m_PlayBackSpeed = 5;
	sys.m_fShowCtrlCode = FALSE;
	sys.m_MFSK_Center = FALSE;
	sys.m_MFSK_SQ_Metric = FALSE;
    sys.m_MacroError = FALSE;
	sys.m_MaxCarrier = 2700;
    sys.m_DecCutOff = 2700;
    memset(&sys.m_PosMacEdit, 0, sizeof(sys.m_PosMacEdit));
	sys.m_MacBuffSize = 4096;
	sys.m_OnTimerInterval = 1000;

	m_QSOStart = 0;
	m_WaterNoiseL = 35;
	m_WaterNoiseH = 65;
	m_TX = txRX;
	m_AppErr = 0;
	m_pCom = NULL;
	m_pRadio = NULL;
    m_pHelp = NULL;
    m_pEdit = NULL;
    m_fReqRX = FALSE;
	m_pPlayBox = NULL;
    m_pSoundTimer = NULL;
    m_pLogLinkTimer = NULL;
    m_pRadioTimer = NULL;
    m_pMacroTimer = NULL;
    m_pWheelTimer = NULL;
    m_pMacroOnTimer = NULL;
    m_pDump = NULL;
	m_pClockView = NULL;
	sys.m_LoopBack = loopINTERNAL;
	m_fpText = NULL;
    m_fpTest = NULL;
	m_BufferSize = 2048;
	m_fHPF = FALSE;
    m_NotchFreq = 1750;
//    m_NotchTaps = 128;
//    m_NotchWidth = 1;
//    m_Notches.m_nBaseTaps = 128;
//    m_Notches.m_NotchWidth = 1;

	m_MouseNotch = 0;
    m_MouseSubChannel = FALSE;
	m_fSubWindow = FALSE;
	m_MouseDown = FALSE;
	m_CurrentMacro = 0;
    m_pCurrentMacroMenu = NULL;
	m_ReqMacroTimer = 0;
	m_fHintUpdate = FALSE;
	m_LostSoundRX = 0;
    m_LostSoundTX = 0;
	m_fShowMsg = FALSE;
	m_InfoMsgFlag = 0;
    m_cErrorMsg = FALSE;
	m_fKeyShift = FALSE;
	m_ReqAutoClear = FALSE;
	m_fSuspend = FALSE;
	m_fTone = FALSE;
	m_ReqAutoReturn = FALSE;
	m_ReqAutoNET = FALSE;
	m_pMacroPopup = NULL;
	m_MacroMenuNo = 0;
	m_fMacroRepeat = FALSE;

	m_ModGain = MODGAIN;
	m_FFTVType = 2;
	m_ScaleAsRigFreq = 0;
    m_ScaleDetails = TRUE;
	m_CurrentEdit = 0;
    m_SendingEdit = 0;

    m_WaveType = 0;
	m_fRttyWordOut = TRUE;
    m_ListBAUD = "15.625,20.0,31.25,62.5,93.75,125.0,250.0";

    m_AFCWidth = 50;
    m_AFCLevel = 12;	// SN=12dB
	m_ATCLevel = 15;	// SN=15dB
	m_ATCSpeed = 0;
    m_ATCLimit = 25000;
	m_AFCKeyTimer = 0;

	m_FFTSmooth = 2;
	m_FFTW = 1000;
	m_fMBCS = TRUE;
	m_fConvAlpha = TRUE;

    m_PlayBackTime[0]=60;
    m_PlayBackTime[1]=30;
    m_PlayBackTime[2]=15;

	m_CPUBENCHType = -1;
	m_ParentMenu = NULL;

	m_strLogMode = "";

	m_StatusUTC = FALSE;
    m_Priority = 1;
    m_RxSet[0].m_CarrierFreq = 1750;
	m_RxSet[0].Create(FALSE);

    m_RxSet[1].m_CarrierFreq = 1600;
    m_RxSet[2].m_CarrierFreq = 1900;
    m_RxSet[3].m_CarrierFreq = 1450;
    m_RxSet[4].m_CarrierFreq = 2050;
    m_RxSet[5].m_CarrierFreq = 2210;
    m_RxSet[6].m_CarrierFreq = 2210;
    m_RxSet[7].m_CarrierFreq = 2210;
    m_RxSet[8].m_CarrierFreq = 2210;
	for( int i = 1; i < RXMAX; i++ ){
		m_RxSet[i].m_rcView.left = 10 + (i * 16);
		m_RxSet[i].m_rcView.top = 10 + (i * 16);
	    m_RxSet[i].m_rcView.right = 500;
        m_RxSet[i].m_rcView.bottom = 124;
    }
#if DEBUG
	sys.m_test = FALSE;
	sys.m_testSN = 13;
	sys.m_testName = "eproject.txt";
	sys.m_testGain = 0;
    sys.m_testCarrier1 = 1750;
    sys.m_testCarrier2 = 0;
    sys.m_testDB2 = 0;
    sys.m_testQSBTime = 8000;
    sys.m_testQSBDB = 0;
    sys.m_test500 = FALSE;
    sys.m_testPhase = FALSE;
    sys.m_testClockErr = 0;
	m_pDebugButton = NULL;
#endif

	sys.m_fAutoTS = FALSE;
	sys.m_EnableMouseWheel = TRUE;

	sys.m_SoundIDRX = -1;
	sys.m_SoundIDTX = -1;

	sys.m_OptionPage = 1;
	sys.m_EventIndex = 0;
	sys.m_fFontFam = FALSE;
	memset(sys.m_tFontFam, 0, sizeof(sys.m_tFontFam));
	memset(m_fftbuf, 0, sizeof(m_fftbuf));

    SetLangFont(PCRX->Font, sys.m_wLang);
    PCTX->Font->Assign(PCRX->Font);

	m_tWaterLevel[0] = 28;
	m_tWaterLevel[1] = 100;
	m_tWaterLevel[2] = 168;
	m_tWaterLevel[3] = 192;
	m_tWaterLevel[4] = 220;
	m_tWaterLevel[5] = 240;

    m_tWaterColset[0].c = clBlack;		// back
    m_tWaterColset[1].d = RGB(0,255,255);	// low
    m_tWaterColset[2].c = clWhite;		// char
    m_tWaterColset[3].c = clRed;		// WAKU
	m_tWaterColset[4].d = RGB(0,255,255);		// RX
    m_tWaterColset[5].c = clBlue;		// TX
    m_tWaterColset[6].c = clYellow;		// mid-low
    m_tWaterColset[7].d = RGB(255,128,0);	// mid
    m_tWaterColset[8].d = RGB(255,0,128);	// mid-high
    m_tWaterColset[9].c = clRed;		// Peak
    m_tWaterColset[10].c = clRed;		// Peak
    m_tWaterColset[11].c = clRed;		// Peak

    m_tFFTColset[0].c = clBlack;		// back
    m_tFFTColset[1].d = RGB(0,255,0);	// high
    m_tFFTColset[2].c = clWhite;		// char
    m_tFFTColset[3].c = clGray;			// Gage
    m_tFFTColset[4].d = RGB(0,255,255);	// RX
    m_tFFTColset[5].c = clBlue;			// TX

	m_MacButtonVW = 3;
    int i;
    char bf[256];
	MACBUTTON *pList = m_tMacButton;
    for( i = 0; i < MACBUTTONALL; i++, pList++ ){
		pList->pButton = NULL;
		sprintf(bf, "M%u", i + 1);
        pList->Name = bf;
        pList->Color = clBlack;
        pList->Text = "";
        pList->Style = 0;
    }
	pList = m_tMacButton;
    pList->Name = "Clear";
    pList->Text = "<%ClearTXW>";
    pList++;
    pList->Name = "CQ";
//    pList->Text = "<%ClearTXW><%TX>CQ CQ CQ de <%MyCall> <%MyCall> <%MyCall> pse (<%VARITYPE>) K\r\n<%RX>";
    pList->Text = "<%ClearTXW><%TX>CQ CQ CQ de <%MyCall> <%MyCall> <%MyCall> pse ";
	if( sys.m_fBaseMBCS ) pList->Text += "(<%VARITYPE>) ";
    pList->Text += "K\r\n<%RX>";
	pList++;
    pList->Name = "CQ2";
    pList->Text = "<%ClearTXW><%TX>CQ CQ CQ de <%MyCall> <%MyCall> <%MyCall>\r\nCQ CQ CQ de <%MyCall> <%MyCall> <%MyCall> pse ";
	if( sys.m_fBaseMBCS ) pList->Text += "(<%VARITYPE>) ";
    pList->Text += "K\r\n<%RX>";
	pList++;
    pList->Name = "1x1";
    pList->Text = "<%NETON><%TX><%HisCall> de <%MyCall> pse K\r\n<%RX>";
	pList++;
    pList->Name = "2x2";
    pList->Text = "<%NETON><%TX><%HisCall> <%HisCall> de <%MyCall> <%MyCall> pse K\r\n<%RX>";
	pList++;
    pList->Name = "RRR";
    pList->Text = "\r\nRRR <%HisCall> de <%MyCall>\r\n";
	if( sys.m_fBaseMBCS ) pList->Text += "--- <%VARITYPE> ---\r\n";
	pList++;
    pList->Name = "BTU";
    pList->Text = "BTU <%HisCall> de <%MyCall> KN\r\n<%RX>";
	pList = &m_tMacButton[12];
    pList->Name = "CWID";
    pList->Text = "<%TX><%CWID=DE <%MyCall>><%RX><%EOF>";
    pList->Style = FSUNDERLINE;
    pList++;
    pList->Name = "TU SK";
    pList->Text = "<%TX><%CWID=TU:><%RX><%EOF>";
    pList->Style = FSUNDERLINE;
	pList++;
    pList++;
    pList->Name = "0x2 - 2x4";
	pList->Text = "<%DisableCR>\r\n#macro <%Menu= 0x2, 0x3, 0x4, -, 1x2, 1x3, 1x4, -, 2x3, 2x4>\r\n#if ValMenu\r\n#define\t_His\t<%Format=%c,<%Input$>>\r\n#define\t_My\t<%Format=%d,<%Find$=x,<%Input$>>>\r\n<%TX><%RX>\r\n#if !Is1stCR\r\n<%CR>\r\n#endif\r\n#if IsCall\r\n<%RepeatText=_His,<%HisCall> >\r\n#endif\r\nde\r\n<%RepeatText=_My, <%MyCall>>\r\n pse K<%CR>\r\n#endif\r\n<%EOF>";

    sys.m_AS = "<%DisableCR>\r\n#if !IsTX\r\n<%AutoNET><%AutoReturn>\r\n#if ValPage!=4\r\n<%Page=4>\r\n#else\r\n<%Page=3>\r\n#endif\r\n<%ClearTXW>\r\n#endif\r\n<%TX><%CWID=@><%RX><%EOF>\r\n";

	CBMode->Clear();
    CBMode->DropDownCount = MODE_END;
	for( i = 0; i < MODE_END; i++ ){
	    CBMode->Items->Add(g_tDispModeTable[i]);
    }
//	InitStgFFT(&m_StgFFT);
//    m_StgFFT.Timer = 0;

	m_fftMX = 0;
	m_fftSC = 109;

	m_nRadioMenu = 4;
	m_RadioMenu[0].strTTL = "7.030 LSB (FT847)";
    m_RadioMenu[0].strCMD = "\\$0070300001\\w10\\$0000000007\\w10";
	m_RadioMenu[1].strTTL = "14.073 LSB (FT847)";
    m_RadioMenu[1].strCMD = "\\$0140730001\\w10\\$0000000007\\w10";
	m_RadioMenu[2].strTTL = "21.073 LSB (FT847)";
    m_RadioMenu[2].strCMD = "\\$0210730001\\w10\\$0000000007\\w10";
	m_RadioMenu[3].strTTL = "28.073 LSB (FT847)";
    m_RadioMenu[3].strCMD = "\\$0280730001\\w10\\$0000000007\\w10";

	ReadRegister();
	m_Wave.SetSoundID();
	SampleFreq(SAMPFREQ);
	UpdateModGain();
	m_FFT.InitFFT();
	InitWater(iniwBOTH);

	SetSystemFont();
	UpdateLogHeight();
	InitWFX();
    m_Dec2.SetSampleFreq(m_fDec, SAMPFREQ);
 	m_ModFSK.m_Encode.m_pFunc = OnGetChar;
    m_ModFSK.SetMFSKType(m_RxSet[0].m_MFSK_TYPE);
    m_ModFSK.SetSampleFreq(SAMPFREQ+SAMPTXOFFSET);
    m_RxSet[0].SetSampleFreq(DEMSAMPFREQ);
	InitCollect(m_RxSet, IsRTTY() ? 7 : 14);
	m_HPF.Create(ffHPF, 300, SAMPFREQ, 2, 1, 0.3);
	m_ModFSK.SetSpeed(m_RxSet[0].m_Speed);
    m_ModFSK.SetType(m_RxSet[0].m_Mode);
	m_RxSet[0].m_pDem->m_fRTTYFFT = m_RxSet[0].m_RTTYFFT;
    m_RxSet[0].SetMFSKType(m_RxSet[0].m_MFSK_TYPE);
	m_RxSet[0].SetMode(m_RxSet[0].m_Mode);
    m_RxSet[0].m_pDem->SetSpeed(m_RxSet[0].m_Speed);
    m_RxSet[0].SetCarrierFreq(UdRxCarrier->Position);
    m_ModFSK.SetCarrierFreq(UdTxCarrier->Position);
    m_Notches.Create();
	SetATCSpeed(m_ATCSpeed);
	SetATCLimit(m_ATCLimit);
	if( sys.m_fPlayBack ) m_PlayBack.Init(m_BufferSize, SAMPBASE);

#if DEBUG
    m_ModTest.SetMFSKType(m_RxSet[0].m_MFSK_TYPE);
	m_ModTest.SetSampleFreq(SAMPFREQ+SAMPTXOFFSET);
    m_ModTest.SetSpeed(m_RxSet[0].m_Speed);
    m_ModTest.SetType(m_RxSet[0].m_Mode);
	if( sys.m_testCarrier1 ) m_ModTest.SetCarrierFreq(sys.m_testCarrier1);
	if( sys.m_testCarrier1 ) m_BPF500.Create(96, ffBPF, SAMPFREQ+SAMPTXOFFSET, sys.m_testCarrier1-250, sys.m_testCarrier1+250, 60.0, 1.0);

    m_VCOTest.SetSampleFreq(SAMPFREQ+SAMPTXOFFSET);
	if( sys.m_testCarrier2 ) m_VCOTest.SetFreeFreq(sys.m_testCarrier2);
#endif

	if( m_FFTW < 750 ){
		m_FFTW = 500;
		SBFFT500->Down = TRUE;
    }
    else if( m_FFTW < 1500 ){
		SBFFT1K->Down = TRUE;
    }
    else if( m_FFTW < 2500 ){
		SBFFT2K->Down = TRUE;
    }
    else {
		m_FFTW = 3000;
		SBFFT3K->Down = TRUE;
    }
    m_FFTWindow = m_FFTW * FFT_SIZE / m_FFTSampFreq;
    CalcFFTCenter(UdRxCarrier->Position);
	for( i = 0; i < RXMAX; i++ ){
		m_RxSet[i].m_AFCTimerW = 3 * SAMPFREQ / m_BufferSize;
		m_RxSet[i].m_AFCTimerN = 3 * SAMPFREQ / m_BufferSize;
    }
	m_pBitmapFFT = new Graphics::TBitmap;
    m_pBitmapFFT->Width = PBoxFFT->Width;
    m_pBitmapFFT->Height = PBoxFFT->Height;
    m_fftYW = m_pBitmapFFT->Height;
    m_fftXW = m_pBitmapFFT->Width;

	m_pBitmapLevel = new Graphics::TBitmap;
    m_pBitmapLevel->Width = PBoxLevel->Width;
    m_pBitmapLevel->Height = PBoxLevel->Height;
    m_levelYW = m_pBitmapLevel->Height;
    m_levelXW = m_pBitmapLevel->Width;

    m_pBitmapPF = new Graphics::TBitmap;
    m_pBitmapPF->Width = PBoxPF->Width;
    m_pBitmapPF->Height = PBoxPF->Height;
	m_pfXW = m_pBitmapPF->Width;
    m_pfYW = m_pBitmapPF->Height;
    m_pfXC = m_pfXW / 2;
	m_PFTimer = 0;

	CreateWaterColors();
    Draw(FALSE);
    DrawLevel(FALSE);
    DrawPF(FALSE);

	UpdateCharset();
    m_Dump.Create(PCRX->Handle, PCRX, PBoxRX, PCRX->Font, SBarRX, 1024);
#if DEBUG
    m_Edit[0].Create(PCTX->Handle, PCTX, PBoxTX, PCTX->Font, SBarTX, 256);
#else
    m_Edit[0].Create(PCTX->Handle, PCTX, PBoxTX, PCTX->Font, SBarTX, 512);
#endif
    m_Edit[0].SetCursorType(csCARET);
	m_Edit[0].SetMBCS(&m_RxSet[0].m_MBCS);
    m_Edit[0].m_fConvAlpha = m_fConvAlpha;
    m_Dump.SetCursorType(csSTATIC);
	m_Dump.SetRTTY(IsRTTY());
    m_Dump.ShowCtrl(sys.m_fShowCtrlCode);

	SetTXCaption();
	UpdateTitle();
	if( m_MacButtonVW != 3 ){
		UpdateMacButtonVW(m_MacButtonVW);
    }
    else {
		SetMacButtonMax();
    	CreateMacButton();
    }
	if( m_Priority ) UpdatePriority(-m_Priority);
    OpenCom();
    OpenRadio();
	UpdateSpeedList(m_RxSet[0].m_Mode);
	UpdateUI();
    UpdateWaveCaption();
    SampleStatus();
    PageStatus();
    UpdateMode(m_RxSet, 0);
	if( sys.m_fFixWindow ){
    	SetBounds(sys.m_rcWindow.left, sys.m_rcWindow.top, sys.m_rcWindow.right, sys.m_rcWindow.bottom);
		if( sys.m_WindowState == wsMaximized ) WindowState = wsMaximized;
    }
    if( KVMX->Checked ) CreateMacExButton();
    m_fDisEvent = FALSE;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SetSystemFont(void)
{
	Font->Name = sys.m_FontName;
    Font->Charset = sys.m_FontCharset;
	sys.m_MsgEng = (sys.m_FontCharset != SHIFTJIS_CHARSET);

	KECall->Caption = "<%HisCall> \t(&H)";
	KEName->Caption = "<%HisName>\t(&N)";
	KEDear->Caption = "<%DearName>\t(&D)";
	KERST->Caption = "<%HisRST> \t(&R)";
	KEMyCall->Caption = "<%MyCall> \t(&M)";
    KESeq->Caption = "<%HisCall> de <%MyCall> \t(&S)";
    KEFinal->Caption = "<%CR>CUL, TU SK...<%CR><%RX>\t(&F)";
	if( sys.m_MsgEng ){
		KF->Caption = "&File";
        KE->Caption = "&Edit";
        KV->Caption = "&View";
        KO->Caption = "&Options";
        KH->Caption = "&Help";

		KFL->Caption = "Save RX log file (Filename = &Date)";
        KFLR->Caption = "Show &RX log file...";
        KFS->Caption = "Send &Text file...";
        KFLO->Caption = "&Open QSO Log file...";
        KFLF->Caption = "Save QSO data no&w";
        KFMS->Caption = "&Save all Macros";
        KFML->Caption = "&Load all Macros";
        KFWST->Caption = "Sound recording (&Filename = Time stamp)";
        KFWS->Caption = "So&und recording...";
        KFRS->Caption = "Sound &Playing...";
		KFES->Caption = "&Close recording or playing";
        KFX->Caption = "E&xit MMVARI";

		KEX->Caption = "Cu&t and copy to the clipboard";
        KECP->Caption = "&Copy to the clipboard";
        KEP->Caption = "&Paste from the clipboard";
        KEC->Caption = "Character code &List...";

        KVF->Caption = "&FFT scale";
			KVF3->Caption = "&Square amplitude";
        KVFS->Caption = "FFT S&moothing";
        KVSF->Caption = "Frequency &Scale";
        	KVSFT->Caption = "&Tone(AF) frequencies";
            KVSFR->Caption = "&Rig(RF) frequencies";
            KVSD->Caption = "&Detailed scale";
        KVWA->Caption = "Waterfall &AGC";
        KVSP->Caption = "Show sound &Playing panel";
        KVS->Caption = "Sub &Channels";
        KVCR->Caption = "Clear &RX window";
        KVCT->Caption = "Clear &TX window";
		KVM->Caption = "Row of Macro &Buttons";
        KVMX->Caption = "S&how expansion macro buttons";
		KVLA->Caption = "Log panel size &Extension";
        KVL->Caption = "Show &Log list...";

		KOVO->Caption = "Soundcard output le&vel...";
		KOVI->Caption = "Soundcard &Input level...";
        KOAI->Caption = "Add CR/LF at the &Beginning";
        KOAO->Caption = "Add CR/LF at the &Ending";
		KORS->Caption = "&Way to send for RTTY";
        KORSC->Caption = "&Character out";
        KORSW->Caption = "&Word out";
        KOV->Caption = "Show V&ARICODE mapping...";
		KOA->Caption = "&Calibrating the SoundCard...";
        KOR->Caption = "Setup &Radio command...";
        KOL->Caption = "Setup &Logging...";
        KOO->Caption = "Setup &MMVARI...";

		KR->Caption = "&RadioCommand";
		KRM->Caption = "Edit menu";
		KRL->Caption = "&Load...";
		KRO->Caption = "&Setup...";
		KRADD->Caption = "&Add menu...";

        KHA->Caption = "About &Project...";
        KHO->Caption = "MMVARI &Operations...";
        KHS->Caption = "Macro &Samples...";
        KHH->Caption = "&History...";
		KHN->Caption = "JE3HHT WebSiteÅiJapanese)...";
		KHJ->Caption = "&JARTS Web site...";
		KHM->Caption = "MM Hamsoft Web Site...";
        KHM->Hint = "http://mmhamsoft.amateur-radio.ca/";
        KHV->Caption = "&About MMVARI...";

        KRXNAME->Caption = "Put into the Name";
        KRXQTH->Caption = "Put into the QTH";
        KRXNOTE->Caption = "Put into the Note";
        KRXCALL->Caption = "Put into the Callsign";
        KRXMY->Caption = "Put into the MyRST";
        KRXADDMY->Caption = "Add into the MyRST";
        KRXADDNAME->Caption = "Add into the Name";
        KRXADDNOTE->Caption = "Add into the Note";
        KRXADDQSL->Caption = "Add into the QSL";
        KRXCopy->Caption = "Copy to the clipboard";
        KRXINV->Caption = "Swap LTR/FIG (for RTTY)";

        KSAS->Caption = "Send &AS in CW on here";
        KSTX->Caption = "Set TX Carrier FREQ on here";
        KSRX->Caption = "Set RX Carrier FREQ on here";
		KSN->Caption = "Set Notch on here";
        KSNR->Caption = "Delete this notch";
        KSNRA->Caption = "Delete all notches";
		KSS->Caption = "Open sub channel on here";
        KSCan->Caption = "Cancel";

		KRWC->Caption = "&Clear window";
		KRWE->Caption = "Open &Editer...";
		KRWT->Caption = "Move to the &Top";
	    KRWB->Caption = "Move to the &Bottom";
		KRSC->Caption = "Show CTRL code";

		Splitter->Hint = "Drag & Drop for to change size of TX window";
		CBMode->Hint = "Choose mode";
		PLog->Hint = "Log panel (Right click for size extension/reduction)";
        SBFFT->Hint = "Spectrum scope";
        SBWater->Hint = "Waterfall scope";
        SBWave->Hint = "Timing/Amplitude scope (Right click for switch)";
		PF->Hint = "AFC level";
        SBFFT500->Hint = "500Hz width";
        SBFFT1K->Hint = "1KHz width";
        SBFFT2K->Hint = "2KHz width";
        SBFFT3K->Hint = "3KHz width";
        SBBPFW->Hint = "Broad (Right click for the characteristic display)";
        SBBPFM->Hint = "Narrow (Right click for the characteristic display)";
        SBBPFN->Hint = "Sharp (Right click for the characteristic display)";
        SBBPFS->Hint = "Very sharp (Right click for the characteristic display)";
        SBAFC->Hint = "Auto frequency control";
        SBNET->Hint = "TX Carrier FREQ = RX Carrier FREQ";
        CBTxCarrier->Hint = "TX Carrier FREQ";
        SBATC->Hint = "Auto timing control (Right click for clear)";
        SBInit->Hint = "Clear current QSO data";
		SBQSO->Hint = "Start/End logging";
        SBData->Hint = "Show current data";
        SBFind->Hint = "Find callsign";
        SBList->Hint = "Show log list";
        SBM->Hint = "Recent inquiry callsign";
        HisCall->Hint = "His/her callsign";
        HisName->Hint = "His/her name";
        HisRST->Hint = "His/her RST";
        MyRST->Hint = "My RST";
        HisQTH->Hint = "His/her QTH";
        EditNote->Hint = "Note (Remarks)";
        EditQSL->Hint = "QSL information or Note";
		LogFreq->Hint = "FREQ(MHz)";
        PBoxRX->Hint = "Left (or Right) click for capturing window text";
        UdMac->Hint = "Switch button page (Right click for seeking top)";
    }

	CreateSubMenu();
    HisRST->SelLength = 0;
    MyRST->SelLength = 0;
    LogFreq->SelLength = 0;

	for( int i = 0; i < KE->Count; i++ ){
		TMenuItem *pMenu;
		BOOL f = FALSE;
		if( PupTX->Items->Count <= i ){
        	pMenu = new TMenuItem(PupTX);
            f = TRUE;
        }
        else {
			pMenu = PupTX->Items->Items[i];
        }
		pMenu->Caption = KE->Items[i]->Caption;
		pMenu->OnClick = KE->Items[i]->OnClick;
        pMenu->ShortCut = KE->Items[i]->ShortCut;
		if( f ) PupTX->Items->Add(pMenu);
	}
#if DEBUG
	EATC->Color = clWhite;
#endif
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::UpdateCharset(void)
{
	SetMBCP(PCRX->Font->Charset);
	m_RxSet[0].m_MBCS.Create();
    m_RxSet[0].m_MBCS.SetCharset(PCRX->Font->Charset);
    m_RxSet[0].m_fJA = (PCRX->Font->Charset == SHIFTJIS_CHARSET);
    m_ModFSK.m_Encode.m_fJA = m_RxSet[0].m_fJA;

	LPCSTR pKey;
    switch(PCRX->Font->Charset){
		case SHIFTJIS_CHARSET:
			pKey = "JA";
            m_fMBCS = TRUE;
//            KJA->Checked = TRUE;
			break;
        case JOHAB_CHARSET:
		case HANGEUL_CHARSET:
			pKey = "HL";
            m_fMBCS = TRUE;
//            KHL->Checked = TRUE;
			break;
		case CHINESEBIG5_CHARSET:   //
			pKey = "BV";
            m_fMBCS = TRUE;
//            KBV->Checked = TRUE;
			break;
		case 134:       // ä»ó™
			pKey = "BY";
            m_fMBCS = TRUE;
//            KBY->Checked = TRUE;
			break;
        case ANSI_CHARSET:
			pKey = "ANSI";
            m_fMBCS = FALSE;
//            KANSI->Checked = TRUE;
			break;
        case SYMBOL_CHARSET:
			pKey = "SYM";
            m_fMBCS = FALSE;
//            KMISC->Checked = TRUE;
            break;
		default:
			pKey = "Other";
            m_fMBCS = FALSE;
//            KMISC->Checked = TRUE;
			break;
	}
	DrawStatus(statusVARI, pKey);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::FormDestroy(TObject *Sender)
{
	if( m_fpText ) fclose(m_fpText);
	if( m_fpTest ) fclose(m_fpTest);
	m_fpText = m_fpTest = NULL;

	if( m_pHelp ) delete m_pHelp;
	if( m_pEdit ) delete m_pEdit;
	if( m_pPlayBox ) delete m_pPlayBox;
    if( m_pClockView ) delete m_pClockView;
	if( m_pMacroPopup ) delete m_pMacroPopup;
    delete m_pBitmapFFT;
    delete m_pBitmapLevel;
    m_pBitmapFFT = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::FormCloseQuery(TObject *Sender, bool &CanClose)
{
#if !DEBUG || SHOWERRCOUNT
	Application->OnException = NULL;
#endif
    Screen->OnActiveFormChange = NULL;
	DoEvent(macOnExit);
	SBTXOFFClick(NULL);
	sys.m_rcWindow.left = Left;
    sys.m_rcWindow.top = Top;
    sys.m_rcWindow.right = Width;
    sys.m_rcWindow.bottom = Height;

    m_Dump.CloseLogFile();
	for( int i = 0; i < RXMAX; i++ ){
		m_RxSet[i].Delete();
    }
    WriteRegister();

	sys.m_EnableMouseWheel = FALSE;
    if( m_pSoundTimer ) delete m_pSoundTimer;
    if( m_pLogLinkTimer ) delete m_pLogLinkTimer;
    if( m_pRadioTimer ) delete m_pRadioTimer;
    if( m_pMacroTimer ) delete m_pMacroTimer;
    if( m_pWheelTimer ) delete m_pWheelTimer;
    if( m_pMacroOnTimer ) delete m_pMacroOnTimer;
	m_Wave.OutAbort();
    m_Wave.InClose();
	if( m_pCom ) delete m_pCom;
	if( m_pRadio ) delete m_pRadio;
	UpdatePriority(0);
	Log.Close();
	Log.DoBackup();
	CanClose = TRUE;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::UpdateTitle(void)
{
	if( !Log.IsOpen() || !strcmp(sys.m_CallSign.c_str(), "NOCALL") ){
		strcpy(m_TextBuff, VERTTL);
    }
    else {
    	sprintf(m_TextBuff, "%s (%s) - "VERTTL2, sys.m_CallSign.c_str(), Log.GetName());
    }

	if( LogLink.IsLink() ){
		char bf[256];
		sprintf(bf, " [Link to %s]", LogLink.GetSessionName());
		strcat(m_TextBuff, bf);
	}
#if DEBUG
	strcat(m_TextBuff, " (Running on debug mode)");
    sprintf(&m_TextBuff[strlen(m_TextBuff)], " %08X:%d", m_RxSet[0].m_pDem, m_RxSet[0].m_pDem->m_DEBUG);
#endif
#if SHOWERRCOUNT
	strcat(m_TextBuff, " (Enable ERRMSG)");
#endif
    Caption = m_TextBuff;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::UpdatePriority(int Priority)
{
	if( Priority == m_Priority ) return;

	m_Priority = ABS(Priority);
    if( m_TX ) ToRX();
	BOOL f = m_Wave.IsInOpen();
    if( f ) m_Wave.InClose();
	HANDLE hThread = GetCurrentThread();
	switch(m_Priority){
		case 1:
			::SetThreadPriority(hThread, THREAD_PRIORITY_ABOVE_NORMAL);
			break;
		case 2:
			::SetThreadPriority(hThread, THREAD_PRIORITY_HIGHEST);
			break;
		default:
			::SetThreadPriority(hThread, THREAD_PRIORITY_NORMAL);
			break;
	}
	if( f ) OpenSound(FALSE);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::ReadRegister(void)
{
	char bf[256];
	sprintf(bf, "%sMmvari.ini", sys.m_BgnDir);
	TMemIniFile	*pIniFile = new TMemIniFile(bf);

	int i;

    // ÉTÉEÉìÉhÉJÅ[Éh
	AnsiString as;
    sprintf(bf, "%.2lf", SAMPFREQ);
    as = pIniFile->ReadString("SoundCard", "Clock", bf);
	double fq;
    sscanf(as.c_str(), "%lf", &fq);
	LimitDbl(&fq, MIN_SAMP, MAX_SAMP);
    SampleFreq(fq);
	sprintf(bf, "%.2lf", SAMPTXOFFSET);
    as = pIniFile->ReadString("SoundCard", "TxOffset", bf);
    sscanf(as.c_str(), "%lf", &fq);
	if( fabs(fq) <= 500.0 ){
	    SAMPTXOFFSET = fq;
    }
	m_Wave.m_InFifoSize = pIniFile->ReadInteger("SoundCard", "RxFIFO", m_Wave.m_InFifoSize);
	m_Wave.m_OutFifoSize = pIniFile->ReadInteger("SoundCard", "TxFIFO", m_Wave.m_OutFifoSize);
	m_Wave.m_SoundStereo = pIniFile->ReadInteger("SoundCard", "Source", m_Wave.m_SoundStereo);
	sys.m_SoundIDRX = pIniFile->ReadString("SoundCard", "ID", sys.m_SoundIDRX);
	sys.m_SoundIDTX = pIniFile->ReadString("SoundCard", "IDTX", sys.m_SoundIDTX);
//	sys.m_SoundMMW = pIniFile->ReadString("SoundCard", "MMW", sys.m_SoundMMW);

    LimitInt(&m_Wave.m_InFifoSize, 4, 32);
    LimitInt(&m_Wave.m_OutFifoSize, 4, 32);

    /* SYSTEM */
	int IniSchema = pIniFile->ReadInteger("System", "Schema", 0);
    m_Priority = pIniFile->ReadInteger("System", "Priority", m_Priority);
	sys.m_FontName = pIniFile->ReadString("System", "FontName", sys.m_FontName);
	sys.m_FontCharset = (BYTE)pIniFile->ReadInteger("System", "FontCharset", sys.m_FontCharset);

    /* Window */
	sys.m_fFixWindow = pIniFile->ReadInteger("Window", "Fixed", sys.m_fFixWindow);
    sys.m_rcWindow.left = pIniFile->ReadInteger("Window", "Left", sys.m_rcWindow.left);
    sys.m_rcWindow.top = pIniFile->ReadInteger("Window", "Top", sys.m_rcWindow.top);
    sys.m_rcWindow.right = pIniFile->ReadInteger("Window", "Width", sys.m_rcWindow.right);
    sys.m_rcWindow.bottom = pIniFile->ReadInteger("Window", "Height", sys.m_rcWindow.bottom);
	sys.m_WindowState = pIniFile->ReadInteger("Window", "WindowState", wsNormal);
	KVLA->Checked = pIniFile->ReadInteger("Window", "LogExtension", KVLA->Checked);
	sys.m_fRestoreSubChannel = pIniFile->ReadInteger("Window", "RestoreSubChannels", sys.m_fRestoreSubChannel);

	/* Folder */
    as = pIniFile->ReadString("Folder", "Sound", sys.m_SoundDir);
	strcpy(sys.m_SoundDir, as.c_str());
    as = pIniFile->ReadString("Folder", "Log", sys.m_LogDir);
	strcpy(sys.m_LogDir, as.c_str());
    as = pIniFile->ReadString("Folder", "ExtLog", sys.m_ExtLogDir);

	strcpy(sys.m_ExtLogDir, as.c_str());
    as = pIniFile->ReadString("Folder", "Text", sys.m_TextDir);
	strcpy(sys.m_TextDir, as.c_str());

	/* PTT */
	sys.m_PTTCOM = pIniFile->ReadString("PTT", "Name", sys.m_PTTCOM);
    sys.m_PTTLock = pIniFile->ReadInteger("PTT", "Lock", sys.m_PTTLock);
	sys.m_bFSKOUT = pIniFile->ReadInteger("PTT", "OUTFSK", sys.m_bFSKOUT);
	sys.m_bINVFSK = pIniFile->ReadInteger("PTT", "INVFSK", sys.m_bINVFSK);

    /* TX */
    sys.m_LoopBack = pIniFile->ReadInteger("TX", "LoopBack", sys.m_LoopBack);
	int f = pIniFile->ReadInteger("TX", "AutoCR", KOAI->Checked | (KOAO->Checked << 1));
	KOAI->Checked = (f & 1);
    KOAO->Checked = (f & 2);
	UdTxCarrier->Position = (short)pIniFile->ReadInteger("TX", "Carrier", UdTxCarrier->Position);
	SBNET->Down = pIniFile->ReadInteger("TX", "NET", SBNET->Down);
	m_ModGain = pIniFile->ReadInteger("TX", "ModGain", m_ModGain);
	m_fRttyWordOut = pIniFile->ReadInteger("TX", "RttyWordOut", m_fRttyWordOut);

    as = pIniFile->ReadString("TX", "Mode", g_tDispModeTable[sys.m_DefaultMode]);
	m_RxSet[0].m_Mode = GetModeIndex(as.c_str());
    CBMode->ItemIndex = m_RxSet[0].m_Mode;
	as = pIniFile->ReadString("TX", "Speed", "31.25");
    sscanf(as.c_str(), "%lf", &m_RxSet[0].m_Speed);
	LimitDbl(&m_RxSet[0].m_Speed, MIN_SPEED, MAX_SPEED);
	m_fConvAlpha = pIniFile->ReadInteger("TX", "ConvAlpha", m_fConvAlpha);
	m_RxSet[0].m_MFSK_TYPE = pIniFile->ReadInteger("TX", "MFSK_TYPE", m_RxSet[0].m_MFSK_TYPE);
	SetCBSpeed();
    sys.m_MFSK_Center = pIniFile->ReadInteger("TX", "MFSK_CENTER", sys.m_MFSK_Center);
	sys.m_fSendSingleTone = pIniFile->ReadInteger("TX", "SendSingleTone", sys.m_fSendSingleTone);

    /* RX */
	UdRxCarrier->Position = (short)pIniFile->ReadInteger("RX", "Carrier", UdRxCarrier->Position);
	SetBPFType(pIniFile->ReadInteger("RX", "BPF", GetBPFType()));
	SBAFC->Down = pIniFile->ReadInteger("RX", "AFC", SBAFC->Down);
	m_RxSet[0].m_pDem->m_fAFC = m_RxSet[0].m_fAFC = SBAFC->Down;
	SBATC->Down = pIniFile->ReadInteger("RX", "ATC", SBATC->Down);
	m_RxSet[0].m_pDem->m_Decode.m_fATC = SBATC->Down;
	m_RxSet[0].m_SQLevel = pIniFile->ReadInteger("RX", "SQLevel", m_RxSet[0].m_SQLevel);
	m_AFCWidth = pIniFile->ReadInteger("RX", "AFCWidth", m_AFCWidth);
	m_AFCLevel = pIniFile->ReadInteger("RX", "AFCLevel", m_AFCLevel);
	if( IniSchema < 3 ){	// For old version
		if( m_AFCWidth > 50 ) m_AFCWidth = 50;
        if( m_AFCLevel < 12 ) m_AFCLevel = 12;
    }
	m_fHPF = pIniFile->ReadInteger("RX", "HPF", m_fHPF);
	m_ATCLevel = pIniFile->ReadInteger("RX", "ATCLevel", m_ATCLevel);
	m_ATCSpeed = pIniFile->ReadInteger("RX", "ATCSpeed", m_ATCSpeed);
	m_ATCLimit = pIniFile->ReadInteger("RX", "ATCLimit", m_ATCLimit);
	sys.m_fAutoTS = pIniFile->ReadInteger("RX", "SWL_TS", sys.m_fAutoTS);
	m_NotchFreq = pIniFile->ReadInteger("RX", "NotchFreq", m_NotchFreq);
	sys.m_fPlayBack = pIniFile->ReadInteger("RX", "PlayBack", sys.m_fPlayBack);
	sys.m_PlayBackSpeed = pIniFile->ReadInteger("RX", "PlayBackSpeed", sys.m_PlayBackSpeed);
    sys.m_MFSK_SQ_Metric = pIniFile->ReadInteger("RX", "MFSK_SQ_METRIC", sys.m_MFSK_SQ_Metric);
	m_RxSet[0].m_RTTYFFT = pIniFile->ReadInteger("RX", "RTTY_FFT", m_RxSet[0].m_RTTYFFT);

    /* View */
	m_FFTW = pIniFile->ReadInteger("FFT", "Width", m_FFTW);
	m_FFTSmooth = pIniFile->ReadInteger("FFT", "Smooth", m_FFTSmooth);
	m_FFTVType = pIniFile->ReadInteger("FFT", "Type", m_FFTVType);
	if( m_FFTSmooth < 2 ) m_FFTSmooth = 2;
	m_FFT.m_FFTGain = (m_FFTVType < 2) ? 0 : 1;
	for( i = 0; i < 12; i++ ){
		sprintf(bf, "Col%u", i + 1);
		if( i < 6 ){
	        m_tFFTColset[i].d = pIniFile->ReadInteger("Spec", bf, m_tFFTColset[i].d);
		}
        m_tWaterColset[i].d = pIniFile->ReadInteger("WF", bf, m_tWaterColset[i].d);
		if( i < 6 ){
			sprintf(bf, "Lvl%u", i + 1);
			m_tWaterLevel[i] = pIniFile->ReadInteger("WF", bf, m_tWaterLevel[i]);
        }
    }
	SetDrawType(pIniFile->ReadInteger("View", "DrawType", GetDrawType()));
	m_WaveType = pIniFile->ReadInteger("View", "WaveForm", m_WaveType);
    m_ScaleAsRigFreq = pIniFile->ReadInteger("View", "RigFreqScale", m_ScaleAsRigFreq);
    m_ScaleDetails = pIniFile->ReadInteger("View", "ScaleDetails", m_ScaleDetails);
	if( IniSchema >= 2 ){
	    KVWA->Checked = pIniFile->ReadInteger("View", "WaterAGC", FALSE);
    }

    /* RXW */
    for( i = 0; i < 5; i++ ){
		sprintf(bf, "Color%d", i + 1);
		m_Dump.m_Color[i].d = pIniFile->ReadInteger("RXW", bf, m_Dump.m_Color[i].d);
    }
    LoadFontFromInifile(PCRX->Font, "RXW", pIniFile);
	m_StatusUTC = pIniFile->ReadInteger("RXW", "StatusUTC", m_StatusUTC);
	sys.m_EnableMouseWheel = pIniFile->ReadInteger("RXW", "MouseWheel", sys.m_EnableMouseWheel);
	sys.m_fShowCtrlCode = pIniFile->ReadInteger("RXW", "ShowCtrl", sys.m_fShowCtrlCode);

    /* TXW */
    for( i = 0; i < 4; i++ ){
		sprintf(bf, "Color%d", i + 1);
		m_Edit[0].m_Color[i].d = pIniFile->ReadInteger("TXW", bf, m_Edit[0].m_Color[i].d);
		for( int j = 1; j < 4; j++ ){
			m_Edit[j].m_Color[i] = m_Edit[0].m_Color[i];
        }
    }
    for( i = 0; i < 4; i++ ){
		m_Edit[i].m_Color[4] = m_Edit[i].m_Color[0];
    }
    LoadFontFromInifile(PCTX->Font, "TXW", pIniFile);
	PCRX->Color = m_Dump.m_Color[0].c;
    PCTX->Color = m_Edit[0].m_Color[0].c;

    /* Macro */
    sys.m_CallSign = pIniFile->ReadString("Macro", "CallSign", sys.m_CallSign);
	m_MacButtonVW = pIniFile->ReadInteger("Macro", "ButtonRow", m_MacButtonVW);
	UdMac->Position = (short)pIniFile->ReadInteger("Macro", "ButtonPos", UdMac->Position);
	LoadMacro(pIniFile);
	AnsiString ws;
	CrLf2Yen(as, sys.m_AS);
	ws = pIniFile->ReadString("Macro", "CWAS", as);
	Yen2CrLf(sys.m_AS, ws);
	for( i = 0; i < macOnEnd; i++ ){
		CrLf2Yen(as, sys.m_MacEvent[i]);
		ws = pIniFile->ReadString("Macro", g_tMacEvent[i], as);
		Yen2CrLf(sys.m_MacEvent[i], ws);
    }
	KVMX->Checked = pIniFile->ReadInteger("Macro", "Expand", FALSE);

    /* Log */
   	Log.m_FileName = pIniFile->ReadString("Log", "Name", "");
    if( Log.m_FileName.IsEmpty() ){
		as = sys.m_CallSign;
		Log.MakePathName(ClipCall(as.c_str()));
    }
    else {
		Log.MakeName(Log.m_FileName.c_str());
    }

    /* LogSet */
	Log.ReadIniFile("LogSet", pIniFile);
	sys.m_LogLink = pIniFile->ReadInteger("LogSet", "LogLink", sys.m_LogLink);
	LogLink.LoadMMLink(pIniFile);
	LogLink.SetPolling(pIniFile->ReadInteger("LogSet", "LinkPoll", 0));
	LogLink.SetPTTEnabled(pIniFile->ReadInteger("LogSet", "LinkPTT", FALSE));

	LoadRADIOSetup(pIniFile);

    for( i = 0; i < kkEOF; i++ ){
		sprintf(bf, "K%u", i+1);
        sys.m_DefKey[i] = (WORD)pIniFile->ReadInteger("DefKey", bf, sys.m_DefKey[i]);
    }
	if( !IniSchema && (sys.m_DefKey[1] == VK_ESCAPE) ) sys.m_DefKey[1] = VK_PAUSE;

    /* Sub channel */
	CRxSet *pRxSet = &m_RxSet[1];
    for( i = 1; i < RXMAX; i++, pRxSet++ ){
		sprintf(bf, "Channel%u", i);
		pRxSet->m_fShowed = pIniFile->ReadInteger(bf, "Show", FALSE);
	    as = pIniFile->ReadString(bf, "Mode", g_tDispModeTable[sys.m_DefaultMode]);
		pRxSet->m_Mode = GetModeIndex(as.c_str());
		pRxSet->m_CarrierFreq = pIniFile->ReadInteger(bf, "Carrier", pRxSet->m_CarrierFreq);
		LimitInt(&pRxSet->m_CarrierFreq, MIN_CARRIER, sys.m_MaxCarrier);
        pRxSet->m_WaterW = pIniFile->ReadInteger(bf, "WaterWidth", pRxSet->m_WaterW);
		LimitInt(&pRxSet->m_WaterW, 200, 800);
        pRxSet->m_rcView.left = pIniFile->ReadInteger(bf, "Left", pRxSet->m_rcView.left);
        pRxSet->m_rcView.top = pIniFile->ReadInteger(bf, "Top", pRxSet->m_rcView.top);
        pRxSet->m_rcView.right = pIniFile->ReadInteger(bf, "Width", pRxSet->m_rcView.right);
        pRxSet->m_rcView.bottom = pIniFile->ReadInteger(bf, "Height", pRxSet->m_rcView.bottom);
		LoadFontFromInifile(&pRxSet->m_FontData, bf, pIniFile);
        pRxSet->m_RTTYFFT = pIniFile->ReadInteger(bf, "RTTYFFT", pRxSet->m_RTTYFFT);
		pRxSet->m_MFSK_TYPE = pIniFile->ReadInteger(bf, "MFSK_TYPE", pRxSet->m_MFSK_TYPE);
		as = pIniFile->ReadString(bf, "Speed", "31.25");
	    sscanf(as.c_str(), "%lf", &pRxSet->m_Speed);
		LimitDbl(&pRxSet->m_Speed, MIN_SPEED, MAX_SPEED);
		pRxSet->m_fAFC = pIniFile->ReadInteger(bf, "AFC", TRUE);
    }
	//RadioMenu
    m_nRadioMenu = pIniFile->ReadInteger("RadioMenu", "Menus", m_nRadioMenu);
	for( i = 0; i < m_nRadioMenu; i++ ){
		sprintf(bf, "Cap%d", i+1);
        m_RadioMenu[i].strTTL = pIniFile->ReadString("RadioMenu", bf, m_RadioMenu[i].strTTL);
		sprintf(bf, "Cmd%d", i+1);
        m_RadioMenu[i].strCMD = pIniFile->ReadString("RadioMenu", bf, m_RadioMenu[i].strCMD);
    }
    /* Callsign */
	for( i = 0; i < STGCALLMAX; i++ ){
		sprintf(bf, "C%d", i+1);
		as = pIniFile->ReadString("Calls", bf, "");
		if( as.IsEmpty() ) break;
		LPSTR p;
		for( p = as.c_str(); *p; p++ ){
			if( *p == '_' ) *p = '\t';
		}
		TMenuItem *pm = new TMenuItem(PupCalls);
		pm->Caption = as.c_str();
		pm->RadioItem = FALSE;
		pm->OnClick = KCallClick;
		pm->Checked = FALSE;
		pm->Enabled = TRUE;
		PupCalls->Items->Add(pm);
	}

	m_MacroVal.ReadInifile(pIniFile, "MacroVal");

	sys.m_fShowLangMsg = pIniFile->ReadInteger("Misc", "ShowLangMsg", sys.m_fShowLangMsg);

#if DEBUG
//debug
	sys.m_testSN = pIniFile->ReadInteger("DEBUG", "testSN", sys.m_testSN);
	sys.m_testName = pIniFile->ReadString("DEBUG", "testName", sys.m_testName);
	sys.m_testGain = pIniFile->ReadInteger("DEBUG", "testGain", sys.m_testGain);
    sys.m_testCarrier1 = pIniFile->ReadInteger("DEBUG", "testCarrier1", sys.m_testCarrier1);
    sys.m_testCarrier2 = pIniFile->ReadInteger("DEBUG", "testCarrier2", sys.m_testCarrier2);
    sys.m_testDB2 = pIniFile->ReadInteger("DEBUG", "testDB2", sys.m_testDB2);
    sys.m_testQSBTime = pIniFile->ReadInteger("DEBUG", "testQSBTime", sys.m_testQSBTime);
    sys.m_testQSBDB = pIniFile->ReadInteger("DEBUG", "testQSBDB", sys.m_testQSBDB);
    sys.m_test500 = pIniFile->ReadInteger("DEBUG", "test500", sys.m_test500);
    sys.m_testPhase = pIniFile->ReadInteger("DEBUG", "testPhase", sys.m_testPhase);
    sys.m_testClockErr = pIniFile->ReadInteger("DEBUG", "testClockErr", sys.m_testClockErr);
#endif

    delete pIniFile;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::WriteRegister(void)
{
	int i;
	char bf[256];
	sprintf(bf, "%sMmvari.ini", sys.m_BgnDir);
	TMemIniFile	*pIniFile = new TMemIniFile(bf);

    // ÉTÉEÉìÉhÉJÅ[Éh
    sprintf(bf, "%.2lf", SAMPFREQ);
    pIniFile->WriteString("SoundCard", "Clock", bf);
	sprintf(bf, "%.2lf", SAMPTXOFFSET);
    pIniFile->WriteString("SoundCard", "TxOffset", bf);
	pIniFile->WriteInteger("SoundCard", "RxFIFO", m_Wave.m_InFifoSize);
	pIniFile->WriteInteger("SoundCard", "TxFIFO", m_Wave.m_OutFifoSize);
	pIniFile->WriteInteger("SoundCard", "Source", m_Wave.m_SoundStereo);
	pIniFile->WriteString("SoundCard", "ID", sys.m_SoundIDRX);
	pIniFile->WriteString("SoundCard", "IDTX", sys.m_SoundIDTX);
//	pIniFile->WriteString("SoundCard", "MMW", sys.m_SoundMMW);

    /* SYSTEM */
    pIniFile->WriteInteger("System", "Schema", INI_SCHEMA);
    pIniFile->WriteInteger("System", "Priority", m_Priority);
	pIniFile->WriteString("System", "FontName", sys.m_FontName);
	pIniFile->WriteInteger("System", "FontCharset", sys.m_FontCharset);

    /* Window */
	if( WindowState == wsNormal ){
		pIniFile->WriteInteger("Window", "Fixed", sys.m_fFixWindow);
    	pIniFile->WriteInteger("Window", "Left", sys.m_rcWindow.left);
    	pIniFile->WriteInteger("Window", "Top", sys.m_rcWindow.top);
    	pIniFile->WriteInteger("Window", "Width", sys.m_rcWindow.right);
    	pIniFile->WriteInteger("Window", "Height", sys.m_rcWindow.bottom);
    }
	pIniFile->WriteInteger("Window", "WindowState", WindowState);
	pIniFile->WriteInteger("Window", "LogExtension", KVLA->Checked);
	pIniFile->WriteInteger("Window", "RestoreSubChannels", sys.m_fRestoreSubChannel);

	/* Folder */
    pIniFile->WriteString("Folder", "Sound", sys.m_SoundDir);
	pIniFile->WriteString("Folder", "Log", sys.m_LogDir);
    pIniFile->WriteString("Folder", "ExtLog", sys.m_ExtLogDir);
    pIniFile->WriteString("Folder", "Text", sys.m_TextDir);

	/* PTT */
	pIniFile->WriteString("PTT", "Name", sys.m_PTTCOM);
    pIniFile->WriteInteger("PTT", "Lock", sys.m_PTTLock);
	pIniFile->WriteInteger("PTT", "OUTFSK", sys.m_bFSKOUT);
	pIniFile->WriteInteger("PTT", "INVFSK", sys.m_bINVFSK);

    /* TX */
    pIniFile->WriteInteger("TX", "LoopBack", sys.m_LoopBack);
	pIniFile->WriteInteger("TX", "AutoCR", KOAI->Checked | (KOAO->Checked << 1));
	pIniFile->WriteInteger("TX", "Carrier", UdTxCarrier->Position);
	pIniFile->WriteInteger("TX", "NET", SBNET->Down);
	pIniFile->WriteString("TX", "Mode", g_tDispModeTable[m_RxSet[0].m_Mode]);
	pIniFile->WriteString("TX", "Speed", StrDbl(bf, m_RxSet[0].m_Speed));
	pIniFile->WriteInteger("TX", "ModGain", m_ModGain);
	pIniFile->WriteInteger("TX", "ConvAlpha", m_fConvAlpha);
	pIniFile->WriteInteger("TX", "RttyWordOut", m_fRttyWordOut);
	pIniFile->WriteInteger("TX", "MFSK_TYPE", m_RxSet[0].m_MFSK_TYPE);
    pIniFile->WriteInteger("TX", "MFSK_CENTER", sys.m_MFSK_Center);
	pIniFile->WriteInteger("TX", "SendSingleTone", sys.m_fSendSingleTone);

    /* View */
	pIniFile->WriteInteger("FFT", "Width", m_FFTW);
	pIniFile->WriteInteger("FFT", "Smooth", m_FFTSmooth);
	pIniFile->WriteInteger("FFT", "Type", m_FFTVType);
	for( i = 0; i < 12; i++ ){
		sprintf(bf, "Col%u", i + 1);
		if( i < 6 ){
	        pIniFile->WriteInteger("Spec", bf, m_tFFTColset[i].d);
        }
        pIniFile->WriteInteger("WF", bf, m_tWaterColset[i].d);
		if( i < 6 ){
			sprintf(bf, "Lvl%u", i + 1);
			pIniFile->WriteInteger("WF", bf, m_tWaterLevel[i]);
        }
    }
	pIniFile->WriteInteger("View", "DrawType", GetDrawType());
	pIniFile->WriteInteger("View", "WaveForm", m_WaveType);
    pIniFile->WriteInteger("View", "RigFreqScale", m_ScaleAsRigFreq);
	pIniFile->WriteInteger("View", "ScaleDetails", m_ScaleDetails);
    pIniFile->WriteInteger("View", "WaterAGC", KVWA->Checked);

    /* RXW */
    for( i = 0; i < 5; i++ ){
		sprintf(bf, "Color%d", i + 1);
		pIniFile->WriteInteger("RXW", bf, m_Dump.m_Color[i].d);
    }
	SaveFontToInifile(PCRX->Font, "RXW", pIniFile);
	pIniFile->WriteInteger("RXW", "StatusUTC", m_StatusUTC);
	pIniFile->WriteInteger("RXW", "MouseWheel", sys.m_EnableMouseWheel);
	pIniFile->WriteInteger("RXW", "ShowCtrl", sys.m_fShowCtrlCode);

    /* TXW */
    for( i = 0; i < 4; i++ ){
		sprintf(bf, "Color%d", i + 1);
		pIniFile->WriteInteger("TXW", bf, m_Edit[0].m_Color[i].d);
    }
	SaveFontToInifile(PCTX->Font, "TXW", pIniFile);

    /* RX */
	pIniFile->WriteInteger("RX", "Carrier", UdRxCarrier->Position);
	pIniFile->WriteInteger("RX", "BPF", GetBPFType());
	pIniFile->WriteInteger("RX", "AFC", SBAFC->Down);
	pIniFile->WriteInteger("RX", "ATC", SBATC->Down);
	pIniFile->WriteInteger("RX", "SQLevel", m_RxSet[0].m_SQLevel);
	pIniFile->WriteInteger("RX", "AFCWidth", m_AFCWidth);
	pIniFile->WriteInteger("RX", "AFCLevel", m_AFCLevel);
	pIniFile->WriteInteger("RX", "HPF", m_fHPF);
	pIniFile->WriteInteger("RX", "ATCLevel", m_ATCLevel);
	pIniFile->WriteInteger("RX", "ATCSpeed", m_ATCSpeed);
	pIniFile->WriteInteger("RX", "ATCLimit", m_ATCLimit);
	pIniFile->WriteInteger("RX", "SWL_TS", sys.m_fAutoTS);
	pIniFile->WriteInteger("RX", "NotchFreq", m_NotchFreq);
	pIniFile->WriteInteger("RX", "PlayBack", sys.m_fPlayBack);
	pIniFile->WriteInteger("RX", "PlayBackSpeed", sys.m_PlayBackSpeed);
    pIniFile->WriteInteger("RX", "MFSK_SQ_METRIC", sys.m_MFSK_SQ_Metric);
	pIniFile->WriteInteger("RX", "RTTY_FFT", m_RxSet[0].m_RTTYFFT);

    /* Log */
	pIniFile->WriteString("Log", "Name", Log.m_FileName);

    /* LogSet */
	Log.WriteIniFile("LogSet", pIniFile);
	pIniFile->WriteInteger("LogSet", "LogLink", sys.m_LogLink);
	pIniFile->WriteInteger("LogSet", "LinkPoll", LogLink.IsPolling());
	pIniFile->WriteInteger("LogSet", "LinkPTT", LogLink.GetPTTEnabled());
	LogLink.SaveMMLink(pIniFile);

	/* Radio */
	SaveRADIOSetup(pIniFile);

    /* Macro */
    pIniFile->WriteString("Macro", "CallSign", sys.m_CallSign);
	pIniFile->WriteInteger("Macro", "ButtonRow", m_MacButtonVW);
	pIniFile->WriteInteger("Macro", "ButtonPos", UdMac->Position);
	SaveMacro(pIniFile);
	AnsiString as;
	CrLf2Yen(as, sys.m_AS);
	pIniFile->WriteString("Macro", "CWAS", as);
	for( i = 0; i < macOnEnd; i++ ){
		CrLf2Yen(as, sys.m_MacEvent[i]);
		pIniFile->WriteString("Macro", g_tMacEvent[i], as);
    }
	pIniFile->WriteInteger("Macro", "Expand", PBoxFFT->Align != alClient);

    /* DefKey */
    for( i = 0; i < kkEOF; i++ ){
		sprintf(bf, "K%u", i+1);
        pIniFile->WriteInteger("DefKey", bf, sys.m_DefKey[i]);
    }

    /* Sub channel */
	CRxSet *pRxSet = &m_RxSet[1];
    for( i = 1; i < RXMAX; i++, pRxSet++ ){
		sprintf(bf, "Channel%u", i);
		pIniFile->WriteInteger(bf, "Show", pRxSet->m_fShowed);
		pIniFile->WriteString(bf, "Mode", g_tDispModeTable[pRxSet->m_Mode]);
        pIniFile->WriteInteger(bf, "Carrier", pRxSet->m_CarrierFreq);
        pIniFile->WriteInteger(bf, "WaterWidth", pRxSet->m_WaterW);
        pIniFile->WriteInteger(bf, "Left", pRxSet->m_rcView.left);
        pIniFile->WriteInteger(bf, "Top", pRxSet->m_rcView.top);
        pIniFile->WriteInteger(bf, "Width", pRxSet->m_rcView.right);
        pIniFile->WriteInteger(bf, "Height", pRxSet->m_rcView.bottom);
		SaveFontToInifile(&m_RxSet[i].m_FontData, bf, pIniFile);
		pIniFile->WriteInteger(bf, "RTTYFFT", pRxSet->m_RTTYFFT);
		char bbf[64];
		pIniFile->WriteString(bf, "Speed", StrDbl(bbf, pRxSet->m_Speed));
		pIniFile->WriteInteger(bf, "MFSK_TYPE", pRxSet->m_MFSK_TYPE);
		pIniFile->WriteInteger(bf, "AFC", pRxSet->m_fAFC);
    }

//RadioMenu
	try{
		pIniFile->EraseSection("RadioMenu");
    }
    catch(...){
    }
    pIniFile->WriteInteger("RadioMenu", "Menus", m_nRadioMenu);
	for( i = 0; i < m_nRadioMenu; i++ ){
		sprintf(bf, "Cap%d", i+1);
        pIniFile->WriteString("RadioMenu", bf, m_RadioMenu[i].strTTL);
		sprintf(bf, "Cmd%d", i+1);
        pIniFile->WriteString("RadioMenu", bf, m_RadioMenu[i].strCMD);
    }

	for( i = 0; i < PupCalls->Items->Count; i++ ){
		sprintf(bf, "C%d", i+1);
		char bbf[64];
		StrCopy(bbf, AnsiString(PupCalls->Items->Items[i]->Caption).c_str(), 63);	//JA7UDE 0428
		LPSTR p;
		for( p = bbf; *p; p++ ){
			if( *p == '\t' ) *p = '_';
		}
		pIniFile->WriteString("Calls", bf, bbf);
	}

	m_MacroVal.WriteInifile(pIniFile, "MacroVal");

	pIniFile->WriteInteger("Misc", "ShowLangMsg", sys.m_fShowLangMsg);

#if DEBUG
//debug
	pIniFile->WriteInteger("DEBUG", "testSN", sys.m_testSN);
	pIniFile->WriteString("DEBUG", "testName", sys.m_testName);
	pIniFile->WriteInteger("DEBUG", "testGain", sys.m_testGain);
	pIniFile->WriteInteger("DEBUG", "testCarrier1", sys.m_testCarrier1);
	pIniFile->WriteInteger("DEBUG", "testCarrier2", sys.m_testCarrier2);
	pIniFile->WriteInteger("DEBUG", "testDB2", sys.m_testDB2);
	pIniFile->WriteInteger("DEBUG", "testQSBTime", sys.m_testQSBTime);
	pIniFile->WriteInteger("DEBUG", "testQSBDB", sys.m_testQSBDB);
	pIniFile->WriteInteger("DEBUG", "test500", sys.m_test500);
	pIniFile->WriteInteger("DEBUG", "testPhase", sys.m_testPhase);
	pIniFile->WriteInteger("DEBUG", "testClockErr", sys.m_testClockErr);
#endif

	pIniFile->UpdateFile();
    delete pIniFile;
}
//---------------------------------------------------------------------------
/*
	Clock	ïúí≤äÌ	FFT
	6000Hz	6000Hz	6000Hz	1024
	8000Hz	8000Hz	8000Hz	2048
	11025Hz	5513Hz	11025Hz	2048
	12000Hz	6000Hz	6000Hz	1024
	16000Hz	5333Hz	10667Hz	2048
	18000Hz	6000Hz	6000Hz	1024
	22050Hz	5513Hz	11025Hz	2048
	24000Hz	6000Hz	6000Hz	1024
	44100Hz	6300Hz	6300Hz	1024
	48000Hz	6000Hz	6000Hz	1024
	50000Hz	6250Hz	6250Hz	1024
*/
BOOL __fastcall TMainVARI::SampleFreq(double f)
{
	int fftsize = FFT_SIZE;
	if( f >= 49000.0 ){							// 50000Hz
	    SAMPFREQ = f;
		SAMPBASE = 50000;
        m_fDec = 7;
        m_DecFactor = 0.125;
        SAMPTYPE = 10;
        m_BufferSize = 8192;
		FFT_SIZE = 1024;
        m_FFTFactor = 0.125;
		sys.m_DecCutOff = 2900;
		sys.m_MaxCarrier = 3000;
	}
	else if( f >= 46000.0 ){					// 48000Hz
	    SAMPFREQ = f;
		SAMPBASE = 48000;
        m_fDec = 7;
        m_DecFactor = 0.125;
        SAMPTYPE = 9;
        m_BufferSize = 8192;
		FFT_SIZE = 1024;
        m_FFTFactor = 0.125;
		sys.m_DecCutOff = 2800;
		sys.m_MaxCarrier = 2950;
    }
	else if( f >= 43000.0 ){					// 44100Hz
	    SAMPFREQ = f;
		SAMPBASE = 44100;
        m_fDec = 6;
        m_DecFactor = 1.0/7.0;
        SAMPTYPE = 8;
        m_BufferSize = 8192;
		FFT_SIZE = 1024;
        m_FFTFactor = 1.0/7.0;
		sys.m_DecCutOff = 2900;
		sys.m_MaxCarrier = 3000;
    }
	else if( f >= 23000.0 ){					// 24000Hz
	    SAMPFREQ = f;
		SAMPBASE = 24000;
        m_fDec = 3;
        m_DecFactor = 0.25;
        SAMPTYPE = 7;
        m_BufferSize = 4096;
		FFT_SIZE = 1024;
        m_FFTFactor = 0.25;
		sys.m_DecCutOff = 2800;
		sys.m_MaxCarrier = 2950;
    }
	else if( f >= 20000.0 ){					// 22050Hz
	    SAMPFREQ = f;
		SAMPBASE = 22050;
        m_fDec = 2;
        m_DecFactor = 1.0/3.0;
        SAMPTYPE = 6;
        m_BufferSize = 6144;
		FFT_SIZE = 2048;
        m_FFTFactor = 1.0/3.0;
		sys.m_DecCutOff = 2700;
		sys.m_MaxCarrier = 2700;
    }
	else if( f >= 17000.0 ){					// 18000Hz
	    SAMPFREQ = f;
		SAMPBASE = 18000;
        m_fDec = 2;
        m_DecFactor = 1.0/3.0;
        SAMPTYPE = 5;
        m_BufferSize = 3072;
		FFT_SIZE = 1024;
        m_FFTFactor = 1.0/3.0;
		sys.m_DecCutOff = 2800;
		sys.m_MaxCarrier = 2950;
    }
	else if( f >= 15000.0 ){					// 16000Hz
	    SAMPFREQ = f;
		SAMPBASE = 16000;
        m_fDec = 2;
        m_DecFactor = 1.0/3.0;
        SAMPTYPE = 4;
        m_BufferSize = 6144;
		FFT_SIZE = 2048;
        m_FFTFactor = 1.0/3.0;
		sys.m_DecCutOff = 2600;
		sys.m_MaxCarrier = 2600;
    }
	else if( f >= 11600.0 ){					// 12000Hzån
	    SAMPFREQ = f;
		SAMPBASE = 12000;
        m_fDec = 1;
        m_DecFactor = 0.5;
        SAMPTYPE = 3;
        m_BufferSize = 2048;
		FFT_SIZE = 1024;
        m_FFTFactor = 0.5;
		sys.m_DecCutOff = 2900;
		sys.m_MaxCarrier = 2950;
	}
    else if( f > 10000.0 ){						// 11025Hzån
	    SAMPFREQ = f;
		SAMPBASE = 11025;
        m_fDec = 1;
        m_DecFactor = 0.5;
        SAMPTYPE = 0;
        m_BufferSize = 2048;
		FFT_SIZE = 2048;
        m_FFTFactor = 1.0;
		sys.m_DecCutOff = 2700;
		sys.m_MaxCarrier = 2700;
    }
    else if( f > 7000.0 ){						// 8000Hzån
	    SAMPFREQ = f;
		SAMPBASE = 8000;
        m_fDec = 0;
        m_DecFactor = 1.0;
        SAMPTYPE = 1;
        m_BufferSize = 2048;
		FFT_SIZE = 2048;
        m_FFTFactor = 1.0;
		sys.m_DecCutOff = 3000;
		sys.m_MaxCarrier = 3000;
    }
	else if( f > 5000 ){						// 6000Hzån
	    SAMPFREQ = f;
		SAMPBASE = 6000;
        m_fDec = 0;
        m_DecFactor = 1.0;
        SAMPTYPE = 2;
        m_BufferSize = 1024;
		FFT_SIZE = 1024;
        m_FFTFactor = 1.0;
		sys.m_DecCutOff = 2950;
		sys.m_MaxCarrier = 2950;
    }
    else {
		return FALSE;
    }
    DEMSAMPFREQ = SAMPFREQ * m_DecFactor;
	m_FFTSampFreq = SAMPFREQ * m_FFTFactor;
    if( fftsize != FFT_SIZE ) m_FFT.InitFFT();
	UdRxCarrier->Max = short(sys.m_MaxCarrier);
	UdTxCarrier->Max = short(sys.m_MaxCarrier);

	m_Notches.m_SampleFreq = m_FFTSampFreq;
    return TRUE;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::FFTSampleFreq(double fq)
{
    m_FFTSampFreq = fq * m_FFTFactor;
    m_FFTWindow = m_FFTW * FFT_SIZE / m_FFTSampFreq;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SampleStatus(void)
{
	if( SAMPTXOFFSET ){
		sprintf(m_TextBuff, "%.2lf/%.2lf", SAMPFREQ, SAMPTXOFFSET);
    }
    else {
		sprintf(m_TextBuff, "%.2lfHz", SAMPFREQ);
    }
	DrawStatus(statusSAMP, m_TextBuff);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::PageStatus(void)
{
	sprintf(m_TextBuff, "Page%d", m_CurrentEdit + 1);
    DrawStatus(statusPAGE, m_TextBuff);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SetSampleFreq(double f, BOOL fForce)
{
	if( !fForce && (SAMPFREQ == f) ) return;

	if( !SampleFreq(f) ) return;
	BOOL fi = m_Wave.IsInOpen();
    BOOL fo = m_Wave.IsOutOpen();
    m_Wave.InClose(); m_Wave.OutAbort();
	InitWFX();
    m_Dec2.SetSampleFreq(m_fDec, SAMPFREQ);
	CRxSet *pRxSet = m_RxSet;
	for( int i = 0; i < RXMAX; i++, pRxSet++ ){
	    pRxSet->SetSampleFreq(DEMSAMPFREQ);
    }
    m_FFTWindow = m_FFTW * FFT_SIZE / m_FFTSampFreq;
	InitCollect();
	m_HPF.Create(ffHPF, 300, SAMPFREQ, 2, 1, 0.3);
    m_ModFSK.SetSampleFreq(SAMPFREQ+SAMPTXOFFSET);
    m_Notches.Create();
#if DEBUG
    m_ModTest.SetSampleFreq(SAMPFREQ+SAMPTXOFFSET);
    m_VCOTest.SetSampleFreq(SAMPFREQ+SAMPTXOFFSET);
	m_TestHPF.Create(ffHPF, 700, SAMPFREQ+SAMPTXOFFSET, 3, 0, 0);
	if( sys.m_testCarrier1 ) m_BPF500.Create(96, ffBPF, SAMPFREQ+SAMPTXOFFSET, sys.m_testCarrier1-250, sys.m_testCarrier1+250, 60.0, 1.0);
#endif
	if( sys.m_fPlayBack ) m_PlayBack.Init(m_BufferSize, SAMPBASE);

    SampleStatus();
    if( fi ) OpenSound(FALSE);
	if( fo ) ReOutOpen();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SetTxOffset(double f)
{
	if( SAMPTXOFFSET == f ) return;

	SAMPTXOFFSET = f;
    m_ModFSK.SetSampleFreq(SAMPFREQ+SAMPTXOFFSET);
#if DEBUG
    m_ModTest.SetSampleFreq(SAMPFREQ+SAMPTXOFFSET);
    m_VCOTest.SetSampleFreq(SAMPFREQ+SAMPTXOFFSET);
	if( sys.m_testCarrier1 ) m_BPF500.Create(96, ffBPF, SAMPFREQ+SAMPTXOFFSET, sys.m_testCarrier1-250, sys.m_testCarrier1+250, 60.0, 1.0);
#endif
    SampleStatus();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SetRXFifo(int d)
{
	if( d == m_Wave.m_InFifoSize ) return;

    BOOL f = m_Wave.IsInOpen();
    m_Wave.InClose();
    m_Wave.m_InFifoSize = d;
    if( f ) OpenSound(FALSE);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SetTXFifo(int d)
{
	if( d == m_Wave.m_OutFifoSize ) return;

    BOOL f = m_Wave.IsOutOpen();
    m_Wave.OutAbort();
    m_Wave.m_OutFifoSize = d;
    if( f ) ReOutOpen();
}
//---------------------------------------------------------------------------
BOOL __fastcall TMainVARI::ReOutOpen(void)
{
	if( !OpenSound(TRUE) ) return FALSE;
    memset(m_wBuffer, 0, sizeof(m_wBuffer));
#if 1
	while(!m_Wave.IsOutBufFull()) m_Wave.OutWrite(m_wBuffer);
#else
    for( int i = 0; i < m_Wave.m_OutFifoSize; i++ ) m_Wave.OutWrite(m_wBuffer);
#endif
    return TRUE;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SetSoundCard(int ch, LPCSTR pID, LPCSTR pIDTX)
{
	int id = m_Wave.m_SoundID;
    int idtx = m_Wave.m_SoundTxID;
	BOOL f = FALSE;

	if( pID ){
		if( !strcmpi(pID, "default") ) pID = "-1";
		if( !strcmpi(pIDTX, "default") ) pIDTX = "-1";

		AnsiString as = sys.m_SoundMMW;
		sys.m_SoundIDRX = pID;
	    sys.m_SoundIDTX = pIDTX;
		m_Wave.SetSoundID(id, idtx);
		if( id == -2 ) f = strcmp(as.c_str(), pID);
    }

	if( (m_Wave.m_SoundStereo != ch) || (id != m_Wave.m_SoundID) || (idtx != m_Wave.m_SoundTxID) || f ){
		if( m_TX ) ToRX();
		BOOL f = m_Wave.IsInOpen();
		m_Wave.InClose();
        m_Wave.m_SoundStereo = ch;
        m_Wave.m_SoundID = id;
        m_Wave.m_SoundTxID = idtx;
        InitWFX();
	    if( f ) OpenSound(FALSE);
    }
}
//---------------------------------------------------------------------------
BOOL __fastcall TMainVARI::OpenSound(BOOL fTX)
{
	m_Wave.m_hWnd = Handle;
	m_Wave.UpdateDevice(m_Wave.m_SoundID);
	BOOL r;
	if( fTX ){
		r = m_Wave.OutOpen(&m_WFX, m_Wave.m_SoundTxID, m_BufferSize);
    }
    else {
		r = m_Wave.InOpen(&m_WFX, m_Wave.m_SoundID, m_BufferSize);
    }
    if( r || m_pSoundTimer || m_fSuspend ) return r;

	// ÉTÉEÉìÉhÉJÅ[ÉhÉäÉJÉoÉäÉ^ÉCÉ}Å[ÇãNìÆ
//    m_fShowMsg = TRUE;
	m_SoundMsgTimer = m_Wave.GetTimeout();
	m_RecoverSoundMode = fTX;
	m_pSoundTimer = new TTimer(this);
    m_pSoundTimer->OnTimer = SoundTimer;
    m_pSoundTimer->Interval = 1000;
    return r;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SetCBSpeed(void)
{
	m_fDisEvent++;
	char bf[256];
	CBSpeed->Text = StrDbl(bf, m_RxSet[0].GetSpeed());
    m_fDisEvent--;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::UpdateUI(void)
{
	BOOL f = m_TX != txINTERNAL;
//	SBTX->Enabled = !m_RxSet[0].m_fJA || CBMode->ItemIndex != MODE_BPSK;
    CBRXCarrier->Enabled = f;
    UdRxCarrier->Enabled = f;
	SBATC->Enabled = (!IsRTTY() && !m_RxSet[0].IsMFSK());
//    CBSpeed->Enabled = !m_RxSet[0].IsMFSK();
	SetCBSpeed();
    f = f && SBATC->Enabled;
    f = f && !SBATC->Down;
    EATC->Enabled = f;
    UdATC->Enabled = f;
    f = m_TX || !SBNET->Down;
    CBTxCarrier->Enabled = f;
    UdTxCarrier->Enabled = f;
    SBM->Enabled = (PupCalls->Items->Count != 0);
	if( m_PlayBack.IsActive() ){
	    SBP60->Visible = TRUE;
    	SBP30->Visible = TRUE;
	    SBP15->Visible = TRUE;
	    f = !m_TX;
	    SBP60->Enabled = f;
    	SBP30->Enabled = f;
	    SBP15->Enabled = f;
    }
    else {
	    SBP60->Visible = FALSE;
    	SBP30->Visible = FALSE;
	    SBP15->Visible = FALSE;
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::UpdateLogPanel(void)
{
	if( Log.IsOpen() ){
		if( Log.m_CurChg ){
			Log.SetLastPos();
			UpdateTextData();
			Log.m_CurChg = 0;
			SBQSO->Down = Log.m_sd.btime ? 1 : 0;
		}

		SBQSO->Enabled = !HisCall->Text.IsEmpty();
		SBData->Enabled = TRUE;
		SBFind->Enabled = SBQSO->Enabled;
		SBList->Enabled = TRUE;
	}
	else {
		SBQSO->Enabled = FALSE;
		SBData->Enabled = FALSE;
		SBFind->Enabled = FALSE;
		SBList->Enabled = FALSE;
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::OpenCom(void)
{
	if( m_pCom ){
    	delete m_pCom;
        m_pCom = NULL;
    }
	if( strcmpi(sys.m_PTTCOM.c_str(), "NONE") ){
	    m_pCom = new CComm();
		m_pCom->m_Baud = IsRTTY() ? int(m_RxSet[0].m_Speed) : 45;
    	if( m_pCom->Open(sys.m_PTTCOM.c_str()) ){
			m_pCom->SetFSK(sys.m_bFSKOUT, sys.m_bINVFSK);
			m_pCom->SetPTT(m_TX);
    	}
    	else {
			delete m_pCom;
    	    m_pCom = NULL;
    	}
    }
	DrawStatus(statusCOM);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::OpenRadio(void)
{
	if( m_pRadio ){
    	delete m_pRadio;
        m_pRadio = NULL;
    }
    if( m_pRadioTimer ){
		delete m_pRadioTimer;
        m_pRadioTimer = NULL;
    }
	if( strcmp(RADIO.StrPort, "NONE" ) ){
		m_pRadio = new CCradio(TRUE);
		m_pRadio->Open(&RADIO, Handle, CM_CMMR, CM_CRADIO);
		if( m_pRadio->m_CreateON != TRUE ){
			delete m_pRadio;
			m_pRadio = NULL;
			ErrorMB( sys.m_MsgEng ? "Could not open '%s' for Radio commands":"RadioÉRÉ}ÉìÉhÉ|Å[Ég'%s'Ç™ÉIÅ[ÉvÉìÇ≈Ç´Ç‹ÇπÇÒ.", RADIO.StrPort);
		}
		else {
			m_pRadio->SendCommand(RADIO.CmdInit.c_str());
			if( RADIO.PollScan ){
				m_fDisEvent++;
				LogFreq->Text = "???";
				m_fDisEvent--;
				RADIO.Cmdxx = 0x00;
				m_pRadio->m_ScanAddr = 1;
			}
			if( RADIO.PollType ){
    	        m_pRadioTimer = new TTimer(this);
	            m_pRadioTimer->OnTimer = RadioTimer;
				int intval = ((RADIO.PollInterval + 2) * 100)/2;
        	    m_pRadioTimer->Interval = intval;
            }
		}
	}
	RADIO.change = 0;
}
//---------------------------------------------------------------------------
// ÉTÉEÉìÉhÉJÅ[Éhèâä˙âªÉpÉâÉÅÅ[É^ÇÃê∂ê¨
void __fastcall TMainVARI::InitWFX(void)
{
	m_WFX.wFormatTag = WAVE_FORMAT_PCM;
	m_WFX.nChannels = WORD(m_Wave.m_SoundStereo ? 2 : 1);
	m_WFX.wBitsPerSample = 16;
	m_WFX.nSamplesPerSec = SAMPBASE;
	m_WFX.nBlockAlign = WORD(m_WFX.nChannels *(m_WFX.wBitsPerSample/8));
	m_WFX.nAvgBytesPerSec = m_WFX.nBlockAlign * m_WFX.nSamplesPerSec;
	m_WFX.cbSize = 0;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::PBoxFFTPaint(TObject *Sender)
{
	PBoxFFT->Canvas->Draw(0, 0, m_pBitmapFFT);
    if( !SBWave->Down && (m_fSubWindow || m_Notches.m_Count) ){
		DrawSubChannel(PBoxFFT);
    }
	DrawErrorMsg(PBoxFFT->Canvas, m_fftXW, m_fftYW, TRUE);
//	if( SBWave->Down && (m_fftMX > 0) ){
	if( m_fftMX > 0 ){
		TCanvas *pCanvas = PBoxFFT->Canvas;
		pCanvas->Pen->Style = psDot;
		pCanvas->Pen->Color = clYellow;
        pCanvas->Brush->Color = clBlack;
		pCanvas->MoveTo(m_fftMX, 0);
		pCanvas->LineTo(m_fftMX, m_fftYW);
		pCanvas->Pen->Style = psSolid;
        if( SBWave->Down ){
			sprintf(m_TextBuff, "%.1lfms", 1000.0 * m_fftMX * m_Collect1.GetMax() / (m_fftXW * DEMSAMPFREQ) );
        }
        else {
			sprintf(m_TextBuff, "%uHz", m_RightFreq);
        }
		DrawMessage(pCanvas, PBoxFFT->Width, PBoxFFT->Height, m_TextBuff, m_fftMX);
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::PBoxLevelPaint(TObject *Sender)
{
	PBoxLevel->Canvas->Draw(0, 0, m_pBitmapLevel);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::Draw(BOOL fPaint)
{
	if( SBWave->Down ){
		DrawWave(fPaint);
    }
	else if( SBFFT->Down ){
		DrawFFT(fPaint);
    }
    else {
		DrawWater(fPaint, TRUE);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::CalcFFTCenter(int fo)
{
	if( m_FFTW == 3000 ){
		m_FFTB = 0;
        m_FFTU = m_FFTB + m_FFTW;
    }
    int fb = fo - m_FFTW/2;
    if( fb < 0 ) fb = 0;
	fb /= 100;
    fb *= 100;
	if( m_FFTW == 3000 ){
		fb = 0;
    }
    else if( (fb + m_FFTW) >= 3000 ){
		fb = 3000 - m_FFTW;
    }
    m_FFTB = fb;
    m_FFTU = m_FFTB + m_FFTW;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::CalcFFTFreq(void)
{
	int fo = m_RxSet[0].m_pDem->m_CarrierFreq;
	int edge = 100;
    if( m_RxSet[0].IsMFSK() ){
        edge = 200;
		if( !sys.m_MFSK_Center ){
			int off = m_RxSet[0].m_pDem->m_MFSK_BW/2;
			if( m_RxSet[0].m_Mode == MODE_mfsk_L ){
        	   	fo -= off;
	        }
    	    else {
				fo += off;
	        }
        }
    }
    else if( m_RxSet[0].IsRTTY() ){
		edge = 200;
    }
	if( m_FFTW == 3000 ){
		m_FFTB = 0;
        m_FFTU = m_FFTB + m_FFTW;
    }
	if( (m_FFTB >= 0) && ((m_FFTB+m_FFTW) <= 3000 ) ){
	    if( (fo > m_FFTB+edge) && (fo < (m_FFTB+m_FFTW)-edge) ){
			return;
    	}
    }
	CalcFFTCenter(fo);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::DrawSubChannel(TPaintBox *pBox)
{
	TCanvas *pCanvas = pBox->Canvas;
    int fw = pCanvas->TextWidth("2");
	int fh = pCanvas->TextHeight("2");
	int ffw = fw - 2;
    int ffh = fh - 3;
    int i;
	POINT pt[6];
	char bf[256];
	CRxSet *pRxSet = &m_RxSet[1];
	for( i = 1; i < RXMAX; i++, pRxSet++ ){
		if( pRxSet->IsActive() ){
			int fo = pRxSet->m_pDem->m_CarrierFreq + 0.5;
			int xx = 0.5 + (fo - m_FFTB) * m_fftXW / m_FFTW;
			pt[0].x = xx - ffw; pt[0].y = 0;
            pt[1].x = xx + ffw; pt[1].y = 0;
            pt[2].x = xx + ffw; pt[2].y = ffh;
            pt[3].x = xx; pt[3].y = ffh + 3;
            pt[4].x = xx - ffw; pt[4].y = ffh;
            pt[5].x = xx - ffw; pt[5].y = 0;
            pCanvas->Brush->Color = clWhite;
            pCanvas->Pen->Color = clRed;
           	pCanvas->Polygon(pt, 5);
            pRxSet->m_X = xx;
            pRxSet->m_Y = fh/2;
			sprintf(bf, "%u", i);
            fw = pCanvas->TextWidth(bf);
			xx -= fw/2;
			::SetBkMode(pCanvas->Handle, TRANSPARENT);
			pCanvas->TextOut(xx, 0, bf);
        }
    }
    if( m_Notches.m_Count ){
		for( i = 0; i < m_Notches.m_Count; i++ ){
			int fo = m_Notches.m_pBase[i].Freq;
			int xx = 0.5 + (fo - m_FFTB) * m_fftXW / m_FFTW;
			pt[0].x = xx - ffw; pt[0].y = 0;
    	    pt[1].x = xx + ffw; pt[1].y = 0;
	        pt[2].x = xx + ffw; pt[2].y = ffh;
    	    pt[3].x = xx; pt[3].y = ffh + 3;
	        pt[4].x = xx - ffw; pt[4].y = ffh;
    	    pt[5].x = xx - ffw; pt[5].y = 0;
	        pCanvas->Brush->Color = clYellow;
    	    pCanvas->Pen->Color = clRed;
      		pCanvas->Polygon(pt, 5);
            m_Notches.m_pBase[i].m_MX = xx;
            m_Notches.m_pBase[i].m_MY = fh/2;
	        strcpy(bf, "N");
    	    fw = pCanvas->TextWidth(bf);
			xx -= fw/2;
			::SetBkMode(pCanvas->Handle, TRANSPARENT);
			pCanvas->TextOut(xx, 0, bf);
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::DrawFreqScale(TCanvas *pCanvas, int XW, int YW,  int fftb, int fftw, int fh, BOOL fRadio)
{
    int fw;
	char bf[128];
	int A, B, L;
    switch(fftw){
		case 500:
        	B = 50; A = 100; L = 100;
            break;
		case 1000:
			B = 50; A = 100; L = 200;
        	break;
        case 2000:
			B = 100; A = 500; L = 500;
        	break;
        case 3000:
			B = 100; A = 500; L = 500;
        	break;
    }
	BOOL fReal = fRadio && m_ScaleAsRigFreq && m_pRadio && m_pRadio->m_FreqHz;
    BOOL fLSB = (fReal && m_pRadio->IsRigLSB());
    UINT f, fb, fu;
    if( fReal ){
    	int rfo = m_pRadio->m_CarrierFreq;
		if( fLSB ){
		    fb = m_pRadio->m_FreqHz + rfo - m_FFTB;
    		fu = fb - fftw;
        }
        else {
		    fb = m_pRadio->m_FreqHz - rfo + m_FFTB;
    		fu = fb + fftw;
        }
    }
    else {
		fb = fftb;
    	fu = fftb+fftw;
    }
	int xx;
	f = fb - (fb % A);
  	if( !fReal && (int(f) < 0) ) f = 0;
#if 0	// For Debug
	sprintf(m_TextBuff, "%u,%u,%u,%u", m_pRadio ? m_pRadio->m_FreqHz:0, fb, fu, f);
    Caption = m_TextBuff;
#endif
	int _yt;
	TColor LCol, FCol;
	if( m_ScaleDetails ){
		LCol = pCanvas->Pen->Color;
    	FCol = pCanvas->Font->Color;
        _yt = 2;
    }
    else {
    	B = A;
        _yt = 0;
    }
	int vy = pCanvas->TextHeight("0") + _yt;
	if( fLSB ){
		for( ; f > fu; f -= B ){
		    xx = int(fb - f) * XW / fftw;
    	    if( xx >= XW ) break;
			if( !(f % 1000) || !(f % L) ){
				sprintf(bf, "%.1lf", double(f) * 0.001);
	            fw = pCanvas->TextWidth(bf)/2;
    	        if( (xx - fw) > 0 ){
                	pCanvas->TextOut(xx - fw, _yt, bf);
	    			if( fh ){
                    	pCanvas->MoveTo(xx, fh); pCanvas->LineTo(xx, YW);
                    }
                    else {
                    	pCanvas->MoveTo(xx, vy); pCanvas->LineTo(xx, vy+3);
                    }
                }
	        }
            else if( fh && !(f % A) ){
    			pCanvas->MoveTo(xx, _yt); pCanvas->LineTo(xx, YW);
            }
            if( m_ScaleDetails && !(f % B) ){
				pCanvas->Pen->Color = FCol;
				pCanvas->MoveTo(xx, 0); pCanvas->LineTo(xx, 2);
				pCanvas->Pen->Color = LCol;
            }
    	}
	}
    else {
		for( ; f < fu; f += B ){
		    xx = int(f - fb) * XW / fftw;
        	if( xx >= XW ) break;
			if( !(f % 1000) || !(f % L) ){
				if( fReal ){
					sprintf(bf, "%.1lf", double(f) * 0.001);
	            }
    	        else {
					sprintf(bf, "%u", f);
	            }
    	        fw = pCanvas->TextWidth(bf)/2;
        	    if( (xx - fw) > 0 ){
                	pCanvas->TextOut(xx - fw, _yt, bf);
	    			if( fh ){
                    	pCanvas->MoveTo(xx, fh); pCanvas->LineTo(xx, YW);
                    }
                    else {
                    	pCanvas->MoveTo(xx, vy); pCanvas->LineTo(xx, vy+3);
                    }
                }
    	    }
            else if( fh && !(f % A) ){
    			pCanvas->MoveTo(xx, _yt); pCanvas->LineTo(xx, YW);
            }
            if( m_ScaleDetails && !(f % B) ){
				pCanvas->Pen->Color = FCol;
				pCanvas->MoveTo(xx, 0); pCanvas->LineTo(xx, 2);
				pCanvas->Pen->Color = LCol;
            }
	    }
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SetInfoMsg(LPCSTR pStr)
{
	SetInfoMsg(pStr, 1);
}
//---------------------------------------------------------------------------
int __fastcall TMainVARI::GetMsgCount(void)
{
	if( SBWave->Down ){
		int n = DEMSAMPFREQ * 3 / m_Collect1.GetMax();
        if( !n ) n++;
        return n;
    }
    else {
		return 15;
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SetInfoMsg(LPCSTR pStr, int pos)
{
	if( (pos >= 0) && (pos < 5) ){
		m_InfoMsg[pos] = pStr;
		m_InfoMsgFlag |= _tBitData[pos];
		m_cInfoMsg[pos] = GetMsgCount();
    	m_fShowMsg = TRUE;
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SetErrorMsg(LPCSTR pStr)
{
	m_ErrorMsg = pStr;
	m_cErrorMsg = GetMsgCount();
    m_fShowMsg = TRUE;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::DrawErrorMsg(TCanvas *pCanvas, int XW, int YW, BOOL fReset)
{
	LPCSTR p = NULL;
	if( m_fShowMsg ){
		if( m_fSuspend ){
			p = sys.m_MsgEng ? "Suspend..." : "ÉTÉXÉyÉìÉhíÜ...";
        }
		else if( m_pSoundTimer ){
	        p = sys.m_MsgEng ? "SoundCard open error. Now trying to open every 1sec." : "ÉTÉEÉìÉhÉJÅ[ÉhÉIÅ[ÉvÉìÉGÉâÅ[, çƒééçsíÜ(1ïbñà)...";
	    }
		else if( m_LostSoundRX ){
			if( fReset ) m_LostSoundRX--;
    	    p = sys.m_MsgEng ? "Lost RX Sound":"éÛêMÉTÉEÉìÉhåáóé";
	    }
		else if( m_LostSoundTX ){
			if( fReset ) m_LostSoundTX--;
    	    p = sys.m_MsgEng ? "Lost TX Sound":"ëóêMÉTÉEÉìÉhåáóé";
	    }
		else if( m_cErrorMsg ){
        	m_cErrorMsg--;
            p = m_ErrorMsg.c_str();
		}
		else if( m_InfoMsgFlag ){
			for( int i = 0; i < 5; i++ ){
				if( m_InfoMsgFlag & _tBitData[i] ){
			        DrawMessage(pCanvas, XW, YW, m_InfoMsg[i].c_str(), i);
		        	m_cInfoMsg[i]--;
                    if( !m_cInfoMsg[i] ) m_InfoMsgFlag &= ~_tBitData[i];
                }
            }
        }
        else {
			m_fShowMsg = FALSE;
        }
	}
    else if( m_ScaleAsRigFreq && m_pRadio ){
		if( fReset ) m_RadioScaleCounter++;
        if( m_RadioScaleCounter & 4 ){
			if( m_pRadio->m_FreqHz ){
				if( (RADIO.PollOffset == 3) && !m_pRadio->m_LSB ){
					p = sys.m_MsgEng ? "Invalid RIG MODE":"ñ¢ëŒâûÇÃRIGÉÇÅ[Éh";
                }
    	    }
        	else {
				p = sys.m_MsgEng ? "NO RIG DATA" : "RIGÉfÅ[É^ñ¢éÛêM";
	        }
        }
    }
#if 0
    if( !p && !SBAFC->Down && m_RxSet[0].m_SQ && m_RxSet[0].m_pDem->IsFreqErr() ){
		DrawMessage(pCanvas, XW, YW, sys.m_MsgEng ? "OFF FREQ":"FÉYÉå", 3);
    }
#endif
	if( p ) DrawMessage(pCanvas, XW, YW, p, 3);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::DrawFFT(BOOL fPaint)
{
    TCanvas *pCanvas = m_pBitmapFFT->Canvas;
    TRect rc;
    rc.Left = 0; rc.Top = 0;
    rc.Right = m_fftXW; rc.Bottom = m_fftYW;
    pCanvas->Brush->Color = m_tFFTColset[0].c;
    pCanvas->Pen->Color = m_tFFTColset[0].c;
    pCanvas->FillRect(rc);

	if( !m_MouseDown ) CalcFFTFreq();
	pCanvas->Pen->Style = psDot;

    int xx, y;
    int fh = pCanvas->TextHeight("A");

	switch(m_FFTVType){
		case 0:
			pCanvas->Pen->Color = m_tFFTColset[3].c;
			for( y = 6; y <= 86; y += 20 ){
				xx = y * m_fftYW / 100;
    			pCanvas->MoveTo(0, xx); pCanvas->LineTo(m_fftXW, xx);
    		}
        	break;
        case 1:
			pCanvas->Pen->Color = m_tFFTColset[3].c;
			for( y = 6; y <= 66; y += 20 ){
				xx = y * m_fftYW / 60;
    			pCanvas->MoveTo(0, xx); pCanvas->LineTo(m_fftXW, xx);
    		}
        	break;
    }
	pCanvas->Font->Height = -12;
	pCanvas->Font->Color = m_tFFTColset[2].c;
	pCanvas->Pen->Color = m_tFFTColset[3].c;
	int rfo = UdRxCarrier->Position;
	DrawFreqScale(pCanvas, m_fftXW, m_fftYW, m_FFTB, m_FFTW, fh, TRUE);

    if( m_RxSet[0].m_pDem->GetSyncState() && m_RxSet[0].m_SQ && m_RxSet[0].m_pDem->m_Lock ){
//		DrawMessage(pCanvas, m_fftXW, m_fftYW, "SYNC", 1);
		pCanvas->TextOut(1, m_ScaleDetails ? 3 : 1, "SYNC");
    }
//	pCanvas->Pen->Color = clYellow;
//	xx = m_Hilbert.m_CarrierFreq * m_fftXW / m_FFTW;
//   	pCanvas->MoveTo(xx, 0); pCanvas->LineTo(xx, m_fftYW);
	pCanvas->Pen->Color = clGreen;
	xx = (DEMSAMPFREQ*0.5 - m_FFTB) * m_fftXW / m_FFTW;
   	pCanvas->MoveTo(xx, fh); pCanvas->LineTo(xx, m_fftYW);
    int tfo = UdTxCarrier->Position;
	POINT pt[4];
	if( (tfo != rfo) && !SBNET->Down ){
		pCanvas->Pen->Color = m_tFFTColset[5].c;
		xx = 0.5 + (tfo - m_FFTB) * m_fftXW / m_FFTW;
		pCanvas->MoveTo(xx, fh); pCanvas->LineTo(xx, m_fftYW);
		pt[0].x = xx; pt[0].y = fh;
    	pt[1].x = xx - 3; pt[1].y = fh+3;
    	pt[2].x = xx; pt[2].y = fh + 6;
    	pt[3].x = xx + 3; pt[3].y = fh+3;
		pCanvas->Brush->Color = m_tFFTColset[5].c;
		pCanvas->Polygon(pt, 3);
		if( Is170() ){
			double hw = m_RxSet[0].m_pDem->m_RTTYShift * 0.5;
			xx = 0.5 + (tfo - m_FFTB - hw) * m_fftXW / m_FFTW;
	        pCanvas->MoveTo(xx, fh+5);
    	    pCanvas->LineTo(xx, fh+3);
			xx = 0.5 + (tfo - m_FFTB + hw) * m_fftXW / m_FFTW;
	        pCanvas->LineTo(xx, fh+3);
    	    pCanvas->LineTo(xx, fh+6);
	    }
	    else if( m_RxSet[0].IsMFSK() ){
			xx = 0.5 + (tfo - m_FFTB) * m_fftXW / m_FFTW;
        	pCanvas->MoveTo(xx, fh+5);
        	pCanvas->LineTo(xx, fh+3);
			double bw = m_RxSet[0].m_pDem->m_MFSK_BW;
			if( sys.m_MFSK_Center ){
				int x = 0.5 + (tfo - m_FFTB - bw*0.5) * m_fftXW / m_FFTW;
    	    	pCanvas->LineTo(x, fh+3);
        		pCanvas->LineTo(x, fh+6);
				x = 0.5 + (tfo - m_FFTB + bw*0.5) * m_fftXW / m_FFTW;
	        	pCanvas->MoveTo(xx, fh+3);
    	    	pCanvas->LineTo(x, fh+3);
        		pCanvas->LineTo(x, fh+6);
            }
            else {
				xx = 0.5 + (tfo - m_FFTB + (m_RxSet[0].m_Mode == MODE_mfsk_U ? bw : -bw)) * m_fftXW / m_FFTW;
    	    	pCanvas->LineTo(xx, fh+3);
        		pCanvas->LineTo(xx, fh+6);
            }
    	}
    }
	pCanvas->Pen->Color = m_tFFTColset[4].c;
	xx = 0.5 + (rfo - m_FFTB) * m_fftXW / m_FFTW;
   	pCanvas->MoveTo(xx, fh); pCanvas->LineTo(xx, m_fftYW);
	pt[0].x = xx; pt[0].y = fh;
    pt[1].x = xx - 3; pt[1].y = fh+3;
    pt[2].x = xx; pt[2].y = fh + 6;
    pt[3].x = xx + 3; pt[3].y = fh+3;
	pCanvas->Brush->Color = m_tFFTColset[4].c;
	pCanvas->Polygon(pt, 3);
	if( Is170() ){
		double hw = m_RxSet[0].m_pDem->m_RTTYShift * 0.5;
		xx = 0.5 + (rfo - m_FFTB - hw) * m_fftXW / m_FFTW;
        pCanvas->MoveTo(xx, fh+5);
        pCanvas->LineTo(xx, fh+3);
		xx = 0.5 + (rfo - m_FFTB + hw) * m_fftXW / m_FFTW;
        pCanvas->LineTo(xx, fh+3);
        pCanvas->LineTo(xx, fh+6);
    }
    else if( m_RxSet[0].IsMFSK() ){
		xx = 0.5 + (rfo - m_FFTB) * m_fftXW / m_FFTW;
        pCanvas->MoveTo(xx, fh+5);
        pCanvas->LineTo(xx, fh+3);
		double bw = m_RxSet[0].m_pDem->m_MFSK_BW;
		if( sys.m_MFSK_Center ){
			int x = 0.5 + (rfo - m_FFTB - bw*0.5) * m_fftXW / m_FFTW;
   	    	pCanvas->LineTo(x, fh+3);
       		pCanvas->LineTo(x, fh+6);
			x = 0.5 + (rfo - m_FFTB + bw*0.5) * m_fftXW / m_FFTW;
        	pCanvas->MoveTo(xx, fh+3);
   	    	pCanvas->LineTo(x, fh+3);
       		pCanvas->LineTo(x, fh+6);
        }
		else {
			xx = 0.5 + (rfo - m_FFTB + (m_RxSet[0].m_Mode == MODE_mfsk_U ? bw : -bw)) * m_fftXW / m_FFTW;
    	    pCanvas->LineTo(xx, fh+3);
        	pCanvas->LineTo(xx, fh+6);
        }
    }
    if( (m_Wave.IsInOpen() || m_Wave.IsOutOpen()) && SBFFT->Down ){
		pCanvas->Pen->Style = psSolid;
		pCanvas->Pen->Color = m_tFFTColset[1].c;

    	int x, y;
#if 1
		int xo = (m_FFTB*FFT_SIZE/m_FFTSampFreq) + 0.5;
		int xe = (m_FFTW*FFT_SIZE/m_FFTSampFreq) + 0.5;
		double kx = double(m_fftXW) / double(xe);
        xe += xo;
#else
		int xo = ((m_FFTB+(m_FFTSampFreq/FFT_SIZE))*FFT_SIZE/m_FFTSampFreq) + 0.5;
#endif
		switch(m_FFTVType){
			case 0:{			// 100dB
				double k = m_fftYW*0.01/m_fftSC;
		    	for( x = xo; x <= xe; x++ ){
        		    y = m_fftout[x];
					y = m_fftYW - y * k;
					xx = int((x - xo) * kx);
					if( x > xo ){
		    		    pCanvas->LineTo(xx, y);
        	    	}
            		else {
						pCanvas->MoveTo(xx, y);
	            	}
    			}
        	    break;}
			case 1:{			// 60dB
				double k = double(m_fftYW)/double(m_fftSC);
				int yo = m_fftYW * 40 / k;
                k = k / 60.0;
		    	for( x = xo; x <= xe; x++ ){
        		    y = m_fftout[x];
					y = m_fftYW - ((y-yo)*k);
					xx = int((x - xo) * kx);
					if( x > xo ){
		    		    pCanvas->LineTo(xx, y);
        	    	}
            		else {
						pCanvas->MoveTo(xx, y);
	            	}
    			}
        	    break;}
            default:{			// SQR
				for( x = xo; x <= xe; x++ ){
        			y = m_fftout[x];
					y = m_fftYW - (y/100);
					xx = int((x - xo) * kx);
					if( x > xo ){
		    		    pCanvas->LineTo(xx, y);
					}
                    else {
						pCanvas->MoveTo(xx, y);
                    }
                }
            	break;}
        }
    }
    if( fPaint ) PBoxFFTPaint(NULL);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::InitWater(int sw)
{
	if( m_WaterNoiseL > m_WaterNoiseH ){
		int f = m_WaterNoiseL;
        m_WaterNoiseL = m_WaterNoiseH;
        m_WaterNoiseH = f;
	}
	if( m_FFT.m_FFTGain ){
		if( sw & iniwMETRIC ){
	        m_StgWater.Sum = 1000;
   		    m_StgWater.Max = 5120;
       		m_StgWater.VW = 5120;
        }
		if( sw & iniwLIMIT ){
	        m_StgWater.LimitL = DBToSqrt(m_WaterNoiseL*100-500);
    	    m_StgWater.LimitH = DBToSqrt(m_WaterNoiseH*100-500);
        }
    }
    else {
		if( sw & iniwMETRIC ){
	        m_StgWater.Sum = 5000;
   		    m_StgWater.Max = 8000;
       		m_StgWater.VW = 6000;
        }
		if( sw & iniwLIMIT ){
	        m_StgWater.LimitL = m_WaterNoiseL * 100 - 500;
    	    m_StgWater.LimitH = m_WaterNoiseH * 100 - 500;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::DrawWater(BOOL fPaint, BOOL fClear)
{
    TCanvas *pCanvas = m_pBitmapFFT->Canvas;
    int fh = pCanvas->TextHeight("A");
    int ft = fh + 8;

    int b = m_FFTB;
    int fo = UdRxCarrier->Position;
	if( !m_MouseDown ) CalcFFTFreq();

    TRect rc;
   	rc.Left = 0; rc.Top = 0; rc.Right = m_fftXW;
	if( fClear || (b != m_FFTB)){
        rc.Bottom = m_fftYW;
        InitWater(iniwMETRIC);
    }
    else {
		rc.Bottom = ft;
    }
   	pCanvas->Brush->Color = m_tWaterColset[0].c;
   	pCanvas->Pen->Color = m_tWaterColset[0].c;
	pCanvas->Pen->Style = psSolid;
   	pCanvas->FillRect(rc);
    rc.Bottom = m_fftYW;

	pCanvas->Font->Height = -12;
	pCanvas->Font->Color = m_tWaterColset[2].c;
    pCanvas->Pen->Color = m_tWaterColset[2].c;
	DrawFreqScale(pCanvas, m_fftXW, m_fftYW, m_FFTB, m_FFTW, 0, TRUE);

    if( m_RxSet[0].m_pDem->GetSyncState() && m_RxSet[0].m_SQ && m_RxSet[0].m_pDem->m_Lock ){
		pCanvas->TextOut(1, m_ScaleDetails ? 3 : 1, "SYNC");
    }

    int xx;
	POINT pt[3];
	int tfo = UdTxCarrier->Position;
	if( tfo != fo && !SBNET->Down ){
		if( Is170() ){
			double hw = m_RxSet[0].m_pDem->m_RTTYShift * 0.5;
			pCanvas->Pen->Color = m_tWaterColset[5].c;
			xx = 0.5 + (tfo - m_FFTB - hw) * m_fftXW / m_FFTW;
	        pCanvas->MoveTo(xx, fh+5);
    	    pCanvas->LineTo(xx, fh+3);
			xx = 0.5 + (tfo - m_FFTB + hw) * m_fftXW / m_FFTW;
    	    pCanvas->LineTo(xx, fh+3);
        	pCanvas->LineTo(xx, fh+6);
	    }
	    else if( m_RxSet[0].IsMFSK() ){
			pCanvas->Pen->Color = m_tWaterColset[5].c;
			xx = 0.5 + (tfo - m_FFTB) * m_fftXW / m_FFTW;
	        pCanvas->MoveTo(xx, fh+5);
    	    pCanvas->LineTo(xx, fh+3);
			double bw = m_RxSet[0].m_pDem->m_MFSK_BW;
			if( sys.m_MFSK_Center ){
				int x = 0.5 + (tfo - m_FFTB - bw*0.5) * m_fftXW / m_FFTW;
   	    		pCanvas->LineTo(x, fh+3);
       			pCanvas->LineTo(x, fh+6);
				x = 0.5 + (tfo - m_FFTB + bw*0.5) * m_fftXW / m_FFTW;
	        	pCanvas->MoveTo(xx, fh+3);
   	    		pCanvas->LineTo(x, fh+3);
       			pCanvas->LineTo(x, fh+6);
	        }
			else {
				xx = 0.5 + (tfo - m_FFTB + (m_RxSet[0].m_Mode == MODE_mfsk_U ? bw : -bw)) * m_fftXW / m_FFTW;
		        pCanvas->LineTo(xx, fh+3);
    		    pCanvas->LineTo(xx, fh+6);
            }
	    }
		xx = 0.5 + (tfo - m_FFTB) * m_fftXW / m_FFTW;
	    pt[0].x = xx - 4; pt[0].y = fh;
	    pt[1].x = xx + 4; pt[1].y = fh;
	    pt[2].x = xx; pt[2].y = fh + 6;
		pCanvas->Pen->Color = m_tWaterColset[3].c;
		pCanvas->Brush->Color = m_tWaterColset[5].c;
		pCanvas->Polygon(pt, 2);
    }
	pCanvas->Pen->Color = m_tWaterColset[3].c;
	if( Is170() ){
		double hw = m_RxSet[0].m_pDem->m_RTTYShift * 0.5;
		pCanvas->Pen->Color = m_tWaterColset[4].c;
		xx = 0.5 + (fo - m_FFTB - hw) * m_fftXW / m_FFTW;
        pCanvas->MoveTo(xx, fh+5);
        pCanvas->LineTo(xx, fh+3);
		xx = 0.5 + (fo - m_FFTB + hw) * m_fftXW / m_FFTW;
        pCanvas->LineTo(xx, fh+3);
        pCanvas->LineTo(xx, fh+6);
    }
    else if( m_RxSet[0].IsMFSK() ){
		pCanvas->Pen->Color = m_tWaterColset[4].c;
		xx = 0.5 + (fo - m_FFTB) * m_fftXW / m_FFTW;
        pCanvas->MoveTo(xx, fh+5);
        pCanvas->LineTo(xx, fh+3);
		double bw = m_RxSet[0].m_pDem->m_MFSK_BW;
		if( sys.m_MFSK_Center ){
			int x = 0.5 + (fo - m_FFTB - bw*0.5) * m_fftXW / m_FFTW;
    		pCanvas->LineTo(x, fh+3);
   			pCanvas->LineTo(x, fh+6);
			x = 0.5 + (fo - m_FFTB + bw*0.5) * m_fftXW / m_FFTW;
        	pCanvas->MoveTo(xx, fh+3);
    		pCanvas->LineTo(x, fh+3);
   			pCanvas->LineTo(x, fh+6);
        }
		else {
			xx = 0.5 + (fo - m_FFTB + (m_RxSet[0].m_Mode == MODE_mfsk_U ? bw : -bw)) * m_fftXW / m_FFTW;
    	    pCanvas->LineTo(xx, fh+3);
        	pCanvas->LineTo(xx, fh+6);
        }
    }
	xx = 0.5 + (fo - m_FFTB) * m_fftXW / m_FFTW;
    pt[0].x = xx - 4; pt[0].y = fh;
    pt[1].x = xx + 4; pt[1].y = fh;
    pt[2].x = xx; pt[2].y = fh + 6;
	pCanvas->Pen->Color = m_tWaterColset[3].c;
	pCanvas->Brush->Color = m_tWaterColset[4].c;
	pCanvas->Polygon(pt, 2);

    if( (m_Wave.IsInOpen() || m_Wave.IsOutOpen()) && SBWater->Down ){
		TRect trc = rc;
		rc.Top += ft;
        rc.Bottom--;
		trc.Top += ft + 1;
		pCanvas->CopyRect(trc, pCanvas, rc);
		int xo = ((m_FFTB+(m_FFTSampFreq/FFT_SIZE))*FFT_SIZE/m_FFTSampFreq) + 0.5;
    	int x, y;
		int n = 0;
		int sum = m_StgWater.LimitL;
        int max = 0;
        int wmax = 0;
		int xl = 0.5 + (fo - m_RxSet[0].m_Speed - m_FFTB) * m_fftXW / m_FFTW;
		int xh = 0.5 + (fo + m_RxSet[0].m_Speed - m_FFTB) * m_fftXW / m_FFTW;
        double k = 256.0 / m_StgWater.VW;
    	for( x = 0; x < m_fftXW; x++ ){
			xx = xo + (x * m_FFTWindow / m_fftXW);
            y = m_fftout[xx];
            if( max < y ) max = y;
			if( (x >= xl) && (x <= xh) ){
	            if( wmax < y ) wmax = y;
            }
			if( (y > m_StgWater.LimitL) && (y < m_StgWater.LimitH) ){
	            sum += y;
   	            n++;
            }
            y = (y - m_StgWater.Sum) * k;
			if( y < 0 ) y = 0;
            if( y >= 256 ) y = 255;
            pCanvas->Pixels[x][ft] = m_tWaterColors[y];
    	}
		if( m_TX == txINTERNAL ){
			sum = m_StgWater.LimitL;
            if( m_RxSet[0].IsMFSK() || m_RxSet[0].Is170() ){
				sum = (sum + m_StgWater.LimitH)/2;
            }
        }
        else if( n < 16 ){
			sum = m_StgWater.LimitL;
        }
        else {
        	sum /= n;
        }
		m_StgWater.Sum = (m_StgWater.Sum + sum) / 2;
        if( m_StgWater.Sum >= m_StgWater.LimitH ) m_StgWater.Sum = m_StgWater.LimitH;
		if( (wmax-sum) >= 320 ){
        	max = wmax;
        }
        m_StgWater.Max = (m_StgWater.Max + max) / 2;
        m_StgWater.VW = m_StgWater.Max - m_StgWater.Sum;
#if DEBUG
//		sprintf(m_TextBuff, "Max:%d, Sum:%d, DB:%d, VW:%d", max, sum, int(DBToSqrt(4000)), m_StgWater.VW);
//        Caption = m_TextBuff;
#endif
		int low, high;
		if( KVWA->Checked ){		// AGC
			if( m_FFT.m_FFTGain ){
				low = 3000; high = 7000;
            }
            else {
				low = 4000; high = 10000;
            }
        }
        else {
			low = high = m_FFT.m_FFTGain ? 5120 : 5000;
        }
        if( m_TX == txINTERNAL ) high = 100000;
        if( m_StgWater.VW < low ) m_StgWater.VW = low;
        if( m_StgWater.VW > high ) m_StgWater.VW = high;
    }
    if( fPaint ) PBoxFFTPaint(NULL);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::DrawWave(BOOL fPaint)
{
    TCanvas *pCanvas = m_pBitmapFFT->Canvas;
    TRect rc;
    rc.Left = 0; rc.Top = 0;
    rc.Right = m_fftXW; rc.Bottom = m_fftYW;
    pCanvas->Brush->Color = clBlack;
    pCanvas->Pen->Color = clBlack;
    pCanvas->FillRect(rc);

	pCanvas->Pen->Color = clGray;
	pCanvas->Pen->Style = psDot;
	int O1 = m_fftYW / 4;
   	pCanvas->MoveTo(0, O1); pCanvas->LineTo(m_fftXW, O1);
	int O2 = m_fftYW * 3 / 4;
   	pCanvas->MoveTo(0, O2); pCanvas->LineTo(m_fftXW, O2);

    if( (m_Wave.IsInOpen() || m_Wave.IsOutOpen()) && SBWave->Down ){
		double k = m_fftYW * 0.3 / 32768.0;
		pCanvas->Pen->Style = psSolid;
		pCanvas->Pen->Color = TColor(RGB(0,255,0));
    	int x, y, xx;
		int W = m_Collect1.GetMax();
		int M = m_Collect1.GetCount();
        double *t1 = m_Collect1.GetZP();
        double *t2 = m_Collect2.GetZP();
    	for( x = 0; x < m_fftXW; x++ ){
			xx = x * W / m_fftXW;
            if( xx >= W ) xx = W - 1;
			if( xx >= M ){
				break;
            }
			y = O1 - (t1[xx] * k);
			if( x ){ pCanvas->LineTo(x, y); } else { pCanvas->MoveTo(x, y); }
    	}
		if( m_WaveType ){
			double d;
			double max = 128;
			for( x = 0; x < M; x++ ){
				d = fabs(t2[x]);
                if( max < d ) max = d;
            }
            k = m_fftYW * 0.2 / max;
			for( x = 0; x < M; x++ ){
				xx = x * m_fftXW / W;
                y = O2 - (t2[x] * k);
				if( x ){ pCanvas->LineTo(xx, y); } else { pCanvas->MoveTo(xx, y); }
            }
        }
        else {
	    	for( x = 0; x < m_fftXW; x++ ){
				xx = x * W / m_fftXW;
        	    if( xx >= W ) xx = W - 1;
				if( xx >= M ){
					break;
    	        }
				y = O2 - (t2[xx] * k);
				if( x ){ pCanvas->LineTo(x, y); } else { pCanvas->MoveTo(x, y); }
	    	}
		}
	    if( m_RxSet[0].m_pDem->GetSyncState() && m_RxSet[0].m_SQ ){
			pCanvas->TextOut(1, 1, "SYNC");
    	}
    }
    if( fPaint ) PBoxFFTPaint(NULL);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::DrawLevel(BOOL fPaint)
{
    TCanvas *pCanvas = m_pBitmapLevel->Canvas;
    TRect rc;
    rc.Left = 0; rc.Top = 0;
    rc.Right = m_levelXW; rc.Bottom = m_levelYW;
    pCanvas->Brush->Color = clBlack;
    pCanvas->Pen->Color = clBlack;
    pCanvas->FillRect(rc);

//	pCanvas->Pen->Color = clYellow;
	int d = m_RxSet[0].m_StgFFT.Sig - 500;
	if( (m_TX != txINTERNAL) && m_RxSet[0].IsMFSK() && sys.m_MFSK_SQ_Metric ){
		d = m_RxSet[0].m_pDem->GetMFSKMetric(0);
	}
   	if( d > LEVELMAX ) d = LEVELMAX;
    if( m_Wave.IsInOpen() || m_Wave.IsOutOpen() ){
		if( m_RxSet[0].m_pDem->m_AGC.GetOver() && !m_TX ){
			pCanvas->Brush->Color = clRed;
        }
		else if( !m_RxSet[0].m_pDem->m_Lock ){
			pCanvas->Brush->Color = m_RxSet[0].m_SQ ? clBlue : clGray;
        }
        else {
			TColor col = (m_TX == txINTERNAL) ? clYellow : TColor(RGB(0,255,0));
			pCanvas->Brush->Color = m_RxSet[0].m_SQ ? col : clGray;
        }
    	rc.Top = m_levelYW - (d * m_levelYW / LEVELMAX);
    	pCanvas->FillRect(rc);
    }
    else {
		m_RxSet[0].m_SQ = FALSE;
    }
    pCanvas->Pen->Color = m_RxSet[0].m_SQ ? clBlack : clWhite;
	d = m_levelYW - (m_RxSet[0].m_SQLevel * m_levelYW / LEVELMAX);
	pCanvas->MoveTo(0, d); pCanvas->LineTo(m_levelXW, d);

    if( fPaint ) PBoxLevelPaint(NULL);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::RemoveUselessMessage(UINT wParam)
{
	MSG msg;
	while( ::PeekMessage(&msg, Handle, WM_WAVE, WM_WAVE, PM_NOREMOVE) ){
		if( msg.wParam == wParam ){
			::PeekMessage(&msg, Handle, WM_WAVE, WM_WAVE, PM_REMOVE);
#if DEBUG
			RxStatus(m_RxSet, "Remove");
#endif
		}
		else {
			break;
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::OnWave(void)
{
	OnWaveIn();
    OnWaveOut();
}
//---------------------------------------------------------------------------
// ÉTÉEÉìÉhÉJÅ[ÉhÇ©ÇÁÇÃÉCÉxÉìÉg
void __fastcall TMainVARI::OnWaveEvent(TMessage &Message)
{
	switch(Message.WParam){
		case waveIN:			// ì¸óÕÉCÉxÉìÉg
			OnWaveIn();
        	break;
        case waveOUT:			// èoóÕÉCÉxÉìÉg
			OnWaveOut();
        	break;
		case waveCloseFileEdit:	// OnCloseFileEdit
			if( Message.LParam == int(m_pHelp) ){
				if( m_pHelp ){
					delete m_pHelp;
        	        m_pHelp = NULL;
            	}
            }
			else if( Message.LParam == int(m_pEdit) ){
				if( m_pEdit ){
					delete m_pEdit;
        	        m_pEdit = NULL;
            	}
            }
        	break;
        case waveCodeView:		// CodeView
			if( Message.LParam & 0x10000000 ){
				PCTX->Font->Name = PCRX->Font->Name;
                PCTX->Font->Charset = PCRX->Font->Charset;
				OnFontChange(FALSE); OnFontChange(TRUE);
            }
            else {
				int c = Message.LParam & 0x0000ffff;
                if( c == '\b' ){
					OnChar(c);
                }
                else {
					m_Edit[m_CurrentEdit].PutChar(c, 1);
                }
            }
        	break;
        case wavePlayDlg:		// PlayDlg
			if( Message.LParam == int(m_pPlayBox) ){
				KFESClick(NULL);
            }
        	break;
        case waveCloseRxView:	// RxView
        case waveSwapRxView:
        	for( int i = 1; i < RXMAX; i++ ){
				if( Message.LParam == int(m_RxSet[i].m_pView) ){
					CRxSet *pRxSet = &m_RxSet[i];
					switch(Message.WParam){
						case waveCloseRxView:
							pRxSet->Delete();
                            break;
                        case waveSwapRxView:		// ì¸ÇÍë÷Ç¶
                        	{
								int fq = pRxSet->m_pDem->m_CarrierFreq;
								pRxSet->m_pDem->m_Decode.Reset();
                                pRxSet->m_pDem->ResetMeasMFSK();
                                pRxSet->SetCarrierFreq(UdRxCarrier->Position);
//                                pRxSet->m_pDem->SetCarrierFreq(UdRxCarrier->Position);
                                SetRxFreq(fq);
                                SetTxFreq(fq);
                                int sf = pRxSet->m_pDem->m_RTTYShift;
								pRxSet->m_pDem->SetRTTYShift(m_RxSet[0].m_pDem->m_RTTYShift);
                                m_RxSet[0].m_pDem->SetRTTYShift(sf);

								int ModeSub = pRxSet->m_Mode;
                                int ModeMain = CBMode->ItemIndex;
								double SpeedSub = pRxSet->GetSpeed();
                                double SpeedMain = m_RxSet[0].GetSpeed();
                                int MfskSub = pRxSet->m_MFSK_TYPE;
                                int MfskMain = m_RxSet[0].m_MFSK_TYPE;

                                pRxSet->m_pView->SetMode(ModeMain);
							    SetMode(ModeSub);
                                m_RxSet[0].m_pDem->m_Decode.Reset();
                                m_RxSet[0].m_pDem->ResetMeasMFSK();

								pRxSet->SetSpeed(SpeedMain);
                                m_RxSet[0].SetSpeed(SpeedSub);

                                pRxSet->SetMFSKType(MfskMain);
                                m_RxSet[0].SetMFSKType(MfskSub);
								m_ModFSK.SetMFSKType(m_RxSet[0].m_MFSK_TYPE);
                                SetCBSpeed();
                                SetSpeedInfo(m_RxSet[0].m_Speed);
                            }
                        	break;
					}
                    break;
                }
            }
            UpdateSubWindow();
            break;
        case waveClockAdj:		// Clock ADJ window
			if( Message.LParam == int(m_pClockView) ){
				delete m_pClockView;
                m_pClockView = NULL;
            }
        	break;
        case waveSeekMacro:		// Seek macro
			if( Message.LParam ){
				int d = int(UdMac->Position) + Message.LParam;
                if( d < 0 ) d += UdMac->Max+1;
                if( d > UdMac->Max ) d -= UdMac->Max+1;
                UdMac->Position = short(d);
                CreateMacButton();

            }
			break;
        case waveDoMacro:		// Do Macro
			SendButton(Message.LParam);
        	break;
        case waveLoadMacro:		// Load Macro
			if( Message.LParam ){
				LPCSTR p = (LPCSTR)Message.LParam;
                LoadMacro(p);
                delete p;
            }
        	break;
#if DEBUG
        case waveRepeatMacro:
			{
				TSpeedButton *pButton = (TSpeedButton *)Message.LParam;
				pButton->Down = TRUE;
                pButton->Click();
            }
            break;
#endif
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::OnWaveIn(void)
{
	while(m_Wave.IsInOpen() && !m_Wave.IsInBufNull()){
		if( !m_Wave.InRead(m_wBuffer) ){
			m_Wave.InClose();
        	OpenSound(FALSE);
            m_LostSoundRX = LOSTMSGTIME * int(SAMPFREQ) / m_BufferSize;
            m_fShowMsg = TRUE;
        }
		m_WaveFile.ReadWrite(m_wBuffer, m_BufferSize);
		if( m_PlayBack.IsActive() && !m_TX ){
			if( m_PlayBack.IsPlaying() ){
				for( int i = 0; i < sys.m_PlayBackSpeed; i++ ){
		        	if( m_PlayBack.Read(m_wBuffer) ){
						DoDem();
                    }
                    else {
						break;
                    }
/*
					MSG msg;
					if( ::PeekMessage(&msg, Handle, WM_LBUTTONDOWN, WM_LBUTTONDOWN, PM_NOREMOVE) ){
						break;
                    }
*/
                }
                if( !m_PlayBack.IsPlaying() ){
					StopPlayBack();
                }
            }
            else {
	        	m_PlayBack.Write(m_wBuffer);
		        DoDem();
            }
        }
        else {
	        DoDem();
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::OnWaveOut(void)
{
#if DEBUG
	if( sys.m_test ){
		while(m_Wave.IsOutOpen() && !m_Wave.IsOutBufFull()){
           	m_WaveFile.ReadWrite(m_wBuffer, m_BufferSize);
			DoDem();
			if( !m_Wave.OutWrite(m_wBuffer) ){
				m_Wave.OutAbort();
				OpenSound(TRUE);
	            m_LostSoundTX = LOSTMSGTIME * int(SAMPFREQ) / m_BufferSize;
	            m_fShowMsg = TRUE;
			}
		}
		if( m_fpTest ){
			while(!m_ModTest.m_Encode.IsBuffFull()){
				int c = fgetc(m_fpTest);
    			if( m_RxSet[0].m_MBCS.IsLead(BYTE(c)) ){
					c = c << 8;
					c += fgetc(m_fpTest);
				}
				m_ModTest.m_Encode.PutChar(c);
				if( feof(m_fpTest) ){
					rewind(m_fpTest);
				}
            }
		}
	}
	else {
#endif
		while(m_Wave.IsOutOpen() && !m_Wave.IsOutBufFull()){
			if( (m_WaveFile.m_mode == 1) && (!m_TX) ){	// Read func
            	m_WaveFile.ReadWrite(m_wBuffer, m_BufferSize);
                DoDem();
            }
            else {
				DoMod();
				if( (sys.m_LoopBack == loopINTERNAL) ) DoDem();
            }
			if( !m_Wave.OutWrite(m_wBuffer) ){
				m_Wave.OutAbort();
				OpenSound(TRUE);
	            m_LostSoundTX = LOSTMSGTIME * int(SAMPFREQ) / m_BufferSize;
	            m_fShowMsg = TRUE;
			}
			if( m_WaveFile.m_mode == 2 ){	// Write func
            	m_WaveFile.ReadWrite(m_wBuffer, m_BufferSize);
            }
		}
#if DEBUG
	}
#endif
	if( m_fReqRX ){
		if( m_fTone ){
			if( m_fReqRX == 1 ){
                m_ModFSK.m_OutVol = 0;
				m_Wave.SetOutBCC(m_Wave.GetOutBC());
				m_fReqRX++;
				SetTXCaption();
            }
			else if( m_Wave.GetOutBCC() < 0 ){
				ToRX();
			}
        }
   		else if( m_ModFSK.m_Encode.m_Idle && !m_Edit[m_SendingEdit].GetCharCount(TRUE) && m_Edit[m_SendingEdit].IsCursorLast() ){
			if( m_fReqRX == 1 ){
				m_fReqRX++;
				SetTXCaption();
				if( KOAO->Checked && m_fSendChar ){
    				m_ModFSK.m_Encode.PutChar('\r');
    				m_ModFSK.m_Encode.PutChar('\n');
				}
				else {
					if( m_pCom ) m_pCom->SetMark();
					m_ModFSK.m_Encode.SetMark();
					m_Wave.SetOutBCC(m_Wave.GetOutBC());
					m_fReqRX++;
				}
			}
			else if( m_fReqRX == 2 ){
				if( m_pCom ) m_pCom->SetMark();
				m_ModFSK.m_Encode.SetMark();
				m_Wave.SetOutBCC(m_Wave.GetOutBC());
				m_fReqRX++;
			}
			else if( m_Wave.GetOutBCC() < 0 ){
                if( !sys.m_bFSKOUT || !IsRTTY() || !m_pCom || m_pCom->m_QueueExtfsk.IsEmpty() ){
					ToRX();
                }
			}
            else {
                m_ModFSK.m_OutVol = 0;
            }
		}
	}
    if( m_pPlayBox ) m_pPlayBox->UpdateItem();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::DoDem(double d)
{
	if( m_SkipTmg > 0 ){
		m_SkipTmg--;
        return;
    }
	CRxSet *pRxSet = &m_RxSet[0];
	CDEMFSK *pDem = pRxSet->m_pDem;
    int c;
	if( pDem->Do(d, pRxSet->m_SQ, pRxSet->m_fATC ) ){
		while((c = pDem->GetData())!=-1){
			PutDumpChar(c, pRxSet, &m_Dump);
        }
	}
	if( SBWave->Down ){
		BOOL fMFSK = pRxSet->IsMFSK();
        BOOL fQPSK = pRxSet->IsQPSK();
		if( m_WaveType ){
			m_Collect1.Do(pDem->GetS());
			m_Collect2.Do(pDem->m_d);
			if( m_Collect1.IsFull() ){
    		    DrawWave(TRUE);
		    	m_Collect1.Clear(); m_Collect2.Clear();
                m_Lock = FALSE;
            }
        }
		else if(fMFSK || fQPSK){
			m_Collect1.Do(pDem->GetTmg()*16384);
			if( fQPSK ){
				m_Collect2.Do(pDem->m_DecPSK.GetD());
            }
            else {
				m_Collect2.Do(pDem->GetS());
            }
			if( m_Collect1.IsFull() ){
    		    DrawWave(TRUE);
		    	m_Collect1.Clear(); m_Collect2.Clear();
                m_Lock = FALSE;
            }
        }
        else {
			if( pDem->GetTmgLock() && pDem->m_Lock ){
				m_Collect1.Do(pDem->GetTmg()*16384);
   				m_Collect2.Do(pDem->GetS());
    	        m_Lock = TRUE;
	        }
    	    else if( m_Lock ){
    		    DrawWave(TRUE);
		    	m_Collect1.Clear(); m_Collect2.Clear();
				m_Lock = FALSE;
        	}
		}
    }
    if( m_fSubWindow ){
		pRxSet = &m_RxSet[1];
	    for( int i = 1; i < RXMAX; i++, pRxSet++ ){
	        pDem = pRxSet->m_pDem;
			if( pRxSet->IsActive() ){
				if( pDem->Do(d, pRxSet->m_SQ, pRxSet->m_fATC ) ){
					while((c = pDem->GetData())!=-1){
						PutDumpChar(c, pRxSet, &pRxSet->m_pView->m_Dump);
                    }
	            }
	        }
	    }
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::DoDem(void)
{
	int i;
    SHORT *wp = m_wBuffer;

#if DEBUG
	if( sys.m_test ){
		if( m_TestTimer ) m_TestTimer--;
	    for( i = 0; i < m_BufferSize; i++, wp++ ){
			if( !m_TestTimer ){
				if( sys.m_testCarrier1 ) *wp = SHORT(m_QSB.Do(m_ModTest.Do()));
				if( sys.m_testCarrier2 ) *wp += SHORT(m_VCOTest.Do(0)*sys.m_testGain2);
            }
            else {
				*wp = 0;
            }
    	}
		if( SAMPFREQ < 7000.0 ){
			AddGaussian(m_wBuffer, m_BufferSize, sys.m_testNoiseGain*0.5);
		}
        else {
			AddGaussian(m_wBuffer, m_BufferSize, sys.m_testNoiseGain);
        }
	    wp = m_wBuffer;
		for( i = 0; i < m_BufferSize; i++, wp++ ){
			*wp = m_TestHPF.Do(*wp);
			if( sys.m_test500 ) *wp = m_BPF500.Do(*wp);
        }
	    wp = m_wBuffer;
    }
	if( m_pDebugButton ){
		::PostMessage(Handle, WM_WAVE, waveRepeatMacro, DWORD(m_pDebugButton));
        m_pDebugButton = NULL;
    }
#endif
    double *dp = m_fftbuf;
    BOOL fNOTCH = m_Notches.m_Count && (m_TX != txINTERNAL);
	switch(SAMPTYPE){
        case 0:		// 11025Hz
		    for( i = 0; i < m_BufferSize; i++, wp++ ){
				if( m_fHPF ) *wp = m_HPF.Do(*wp);
				if( fNOTCH ) *wp = m_Notches.m_FIR.Do(*wp);
				if( m_Dec2.Do(*wp) ){
					DoDem(m_Dec2.GetOut());
        		}
				*dp++ = *wp;
        	}
        	break;
        case 1:		// 8000Hz
		case 2:		// 6000Hz
		    for( i = 0; i < m_BufferSize; i++, wp++ ){
				if( m_fHPF ) *wp = m_HPF.Do(*wp);
				if( fNOTCH ) *wp = m_Notches.m_FIR.Do(*wp);
				DoDem(*wp);
				*dp++ = *wp;
        	}
        	break;
        default:
//					// 16000Hz
//					// 22050Hz
//					12000Hz
//					18000Hz
//					24000Hz
//					44100Hz
//					48000Hz
//					50000Hz
		    for( i = 0; i < m_BufferSize; i++, wp++ ){
				if( m_fHPF ) *wp = m_HPF.Do(*wp);
				if( m_Dec2.Do(*wp) ){
					if( fNOTCH ) m_Dec2.SetOut(m_Notches.m_FIR.Do(m_Dec2.GetOut()));
					DoDem(m_Dec2.GetOut());
					*dp++ = m_Dec2.GetOut();
        		}
        	}
        	break;
    }
	try {
	    m_FFT.Calc(m_fftbuf, 3001 * FFT_SIZE / m_FFTSampFreq, m_fftSC * 10.0, m_FFTSmooth, m_fftout);
    }
    catch(...){
		m_FFT.InitFFT();
        m_FFT.m_FFTDIS = 0;
    }
	for( i = 0; i < RXMAX; i++ ){
		CalcStgFFT(&m_RxSet[i]);
    }
	if( SBFFT->Down ){
    	DrawFFT(TRUE);
    }
    else if( SBWater->Down ){
		DrawWater(TRUE, FALSE);
    }
	DrawLevel(TRUE);
    DrawPF(TRUE);

#if MEASIMD
	m_RxSet[0].m_pDem->CalcIMD();
#endif

    m_fDisEvent++;
	if( m_TX != txINTERNAL ){
		if( m_AFCKeyTimer ){
        	m_AFCKeyTimer--;
        }
		else if( SBAFC->Down ){
			UdRxCarrier->Position = short(m_RxSet[0].m_pDem->m_CarrierFreq+0.5);
	        if( m_pRadio ) m_pRadio->SetCarrierFreq(UdRxCarrier->Position);
#if 0
            if( !m_TX && SBNET->Down ){
				UdTxCarrier->Position = UdRxCarrier->Position;
            }
#endif
			m_RxSet[0].m_pDem->UpdateBPF();
        }
		if( IsRTTY() ){
			if( m_RxSet[0].m_pDem->m_Decode.IsRTTYTmg() ){
				sprintf(m_TextBuff, "%.2lf", m_RxSet[0].m_pDem->m_Decode.GetRTTYTmg());
				EATC->Text = m_TextBuff;
            }
            else {
           		EATC->Text = "***";
            }
        }
		else if( m_RxSet[0].IsMFSK() ){
			sprintf(m_TextBuff, "%d", m_RxSet[0].m_pDem->GetClockError());
			EATC->Text = m_TextBuff;
        }
		else if( SBATC->Down ){
			int d = m_RxSet[0].m_pDem->GetClockError();
			EATC->Text = d;
        }
    }
	m_fDisEvent--;

//    StatusBar->Panels->Items[statusSYNC]->Text = (m_RxSet[0].m_SQ && m_RxSet[0].m_pDem->m_Decode.GetSyncState()) ? "SYNC" : "";

    if( m_fSubWindow ){
		CRxSet *pRxSet = &m_RxSet[1];
   		for( i = 1; i < RXMAX; i++, pRxSet++ ){
			if( pRxSet->IsActive() ){
				pRxSet->m_pDem->UpdateBPF();
           		pRxSet->m_pView->UpdateStatus();
            }
        }
    }
    if( m_fHintUpdate ){
		DrawHint();
    }
    if( m_pClockView ){
		wp = m_wBuffer;
		for( i = 0; i < m_BufferSize; i++ ){
			m_pClockView->Do(*wp++);
        }
    	m_pClockView->UpdateFFT();
    }
#if DEBUG && 0

	sprintf(m_TextBuff, "%9.1lf, %9.4lf, %4u",
    	m_RxSet[0].m_pDem->m_RxFreq,
    	m_RxSet[0].m_pDem->m_out,
    	m_RxSet[0].m_AFCFQ,
    );
    Caption = m_TextBuff;
#endif
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::DoMod(void)
{
	if( m_fpText ){
		while(m_Edit[m_CurrentEdit].GetCharCount(TRUE) < int(16 * m_RxSet[0].GetSpeed() / 31.25) ){
#if 1
			int c = 0;
			int len = fread(&c, 1, 1, m_fpText);
			if( len <= 0 ){
				fclose(m_fpText); m_fpText = NULL;
                break;
            }
    	    if( m_RxSet[0].m_MBCS.IsLead(BYTE(c)) ){
				c = c << 8;
				int len = fread(&c, 1, 1, m_fpText);
				if( len <= 0 ){
					fclose(m_fpText); m_fpText = NULL;
            	    break;
	            }
        	}
            m_Edit[m_CurrentEdit].PutChar(c, 1);
#else
	    	int c = fgetc(m_fpText);
    	    if( m_RxSet[0].m_MBCS.IsLead(BYTE(c)) ){
				c = c << 8;
            	c += fgetc(m_fpText);
        	}
            m_Edit[m_CurrentEdit].PutChar(c, 1);
            if( feof(m_fpText) ){
				fclose(m_fpText);
                m_fpText = NULL;
            }
#endif
        }
    }

	int i;
    SHORT *wp = m_wBuffer;
	if( m_fTone ){
	    for( i = 0; i < m_BufferSize; i++, wp++ ){
			*wp = SHORT(m_ModFSK.DoCarrier()*m_ModGainR);
	  	}
    }
    else {
	    for( i = 0; i < m_BufferSize; i++, wp++ ){
			*wp = SHORT(m_ModFSK.Do()*m_ModGainR);
	  	}
    }
}
//---------------------------------------------------------------------------
//Added by JA7UDE on April 5, 2010
int MungeBits( int r )
{
  int oc = 0;
  if( r & 0x00000010 )
    oc += 0x01;
  if( r & 0x00000008 )
    oc += 0x02;
  if( r & 0x00000004 )
    oc += 0x04;
  if( r & 0x00000002 )
    oc += 0x08;
  if( r & 0x00000001 )
    oc += 0x10;
  return (int)oc;
}
void __fastcall TMainVARI::ExtFskIt( int r )
{
	if( m_pCom && m_pCom->m_bFSKOUT && IsRTTY() ) m_pCom->m_QueueExtfsk.Push( (BYTE)MungeBits(r) );
}
//Till here
//---------------------------------------------------------------------------
void __fastcall TMainVARI::PutDumpChar(int d, CRxSet *pRxSet, CDump *pDump)
{
	if( pRxSet->IsRTTY() ){
		if( d ) pDump->PutChar(d, m_TX ? 2 : 1);
    }
    else {
		int m;
		if( pRxSet->IsMFSK() ){
        	m = d;
        }
        else {
	        m = g_VariCode.Index2Mbcs(d, (pRxSet->m_fJA && !pRxSet->m_fTWO));
        }
		if( pRxSet->m_fTWO ){
			if( (m >= 0) && (m < 256) ){
				if( pRxSet->m_fMBCS ){
					m |= pRxSet->m_fMBCS;
           	        pRxSet->m_fMBCS = 0;
                }
				else if( pRxSet->m_MBCS.IsLead(BYTE(m)) ){
					pRxSet->m_fMBCS = (m << 8);
           	    }
				if( !pRxSet->m_fMBCS ) pDump->PutChar(m, m_TX ? 2 : 1);
   	        }
       	}
        else {
			pDump->PutChar(m, m_TX ? 2 : 1);
       	}
   }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::InitStgFFT(FFTSTG *pStg)
{
	if( m_FFT.m_FFTGain ){
        pStg->Sum = 1024;
	    pStg->Max = 1024;
    	pStg->WMax = 1024;
    }
    else {
        pStg->Sum = 5000;
   	    pStg->Max = 5000;
   	    pStg->WMax = 5000;
    }
    pStg->Sig = 0;
}
//---------------------------------------------------------------------------
double __fastcall TMainVARI::SqrtToDB(double d)
{
	d /= (0.00345 * 10.0 * m_fftSC);
    d = d * d;
	d = d * d;
    return m_fftSC * 10.0 * (log10(d+2.81458e4) - 4.4494132);
}
//---------------------------------------------------------------------------
double __fastcall TMainVARI::DBToSqrt(double d)
{
	d /= m_fftSC * 10.0;
    d += 4.4494132;
    d = pow(10, d) - 2.81458e4;
	d = pow(d, 0.25);
    return d * (0.00345 * 10.0 * m_fftSC);
}
//---------------------------------------------------------------------------
double __fastcall TMainVARI::AdjDB(double d)
{
    if( m_FFT.m_FFTGain ){
		return SqrtToDB(d);
    }
    else {
		return d * 100.0 / m_fftSC;
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::CalcStgFFT(CRxSet *pRxSet)
{
	if( !pRxSet->IsActive() ) return;

	CDEMFSK *pDem = pRxSet->m_pDem;
	int fo = pDem->m_CarrierFreq;

	int fm, xl, xh, xnl, xnh, xol, xoh;
    switch(pRxSet->m_Mode){
		case MODE_GMSK:
		    fm = pRxSet->m_Speed * 2 + 100;
			xl = (fo - fm) * FFT_SIZE / m_FFTSampFreq;
		    xh = (fo + fm) * FFT_SIZE / m_FFTSampFreq;
			xnl = (fo - pRxSet->m_Speed*1.6) * FFT_SIZE / m_FFTSampFreq;
		    xnh = (fo + pRxSet->m_Speed*1.6) * FFT_SIZE / m_FFTSampFreq;
		    xol = (fo - pRxSet->m_Speed/4) * FFT_SIZE / m_FFTSampFreq;
		    xoh = (fo + pRxSet->m_Speed/4) * FFT_SIZE / m_FFTSampFreq;
			break;
        case MODE_FSK:
		    fm = pRxSet->m_Speed * 4 + 100;
			xl = (fo - fm) * FFT_SIZE / m_FFTSampFreq;
		    xh = (fo + fm) * FFT_SIZE / m_FFTSampFreq;
			xnl = (fo - pRxSet->m_Speed*3.2) * FFT_SIZE / m_FFTSampFreq;
		    xnh = (fo + pRxSet->m_Speed*3.2) * FFT_SIZE / m_FFTSampFreq;
		    xol = (fo - pRxSet->m_Speed/2) * FFT_SIZE / m_FFTSampFreq;
		    xoh = (fo + pRxSet->m_Speed/2) * FFT_SIZE / m_FFTSampFreq;
        	break;
		case MODE_N_BPSK:
		case MODE_BPSK:
        case MODE_qpsk_L:
        case MODE_qpsk_U:
		    fm = pRxSet->m_Speed * 2 + 100;
			xl = (fo - fm) * FFT_SIZE / m_FFTSampFreq;
		    xh = (fo + fm) * FFT_SIZE / m_FFTSampFreq;
			xnl = (fo - pRxSet->m_Speed*1.6) * FFT_SIZE / m_FFTSampFreq;
		    xnh = (fo + pRxSet->m_Speed*1.6) * FFT_SIZE / m_FFTSampFreq;
		    xol = (fo - pRxSet->m_Speed*0.6) * FFT_SIZE / m_FFTSampFreq;
		    xoh = (fo + pRxSet->m_Speed*0.6) * FFT_SIZE / m_FFTSampFreq;
			break;
		case MODE_FSKW:
		case MODE_RTTY:
        case MODE_U_RTTY:
#if 1
			fm = (pRxSet->m_pDem->m_RTTYShift * 0.5) + 150;
            if( fm > 1500 ) fm = 1500;
			xl = (fo - fm) * FFT_SIZE / m_FFTSampFreq;
		    xh = (fo + fm) * FFT_SIZE / m_FFTSampFreq;
			xnl = xl;
		    xnh = xh;
			fm = (pRxSet->m_pDem->m_RTTYShift * 0.5) + 15;
		    xol = (fo - fm) * FFT_SIZE / m_FFTSampFreq;
		    xoh = (fo + fm) * FFT_SIZE / m_FFTSampFreq;
#else
		    fm = (pRxSet->m_pDem->m_RTTYShift * 500)/170;
            if( fm > 1500 ) fm = 1500;
			xl = (fo - fm) * FFT_SIZE / m_FFTSampFreq;
		    xh = (fo + fm) * FFT_SIZE / m_FFTSampFreq;
			xnl = (fo - (fm/2)) * FFT_SIZE / m_FFTSampFreq;
		    xnh = (fo + (fm/2)) * FFT_SIZE / m_FFTSampFreq;
			fm = (pRxSet->m_pDem->m_RTTYShift * 0.5) + 15;
		    xol = (fo - fm) * FFT_SIZE / m_FFTSampFreq;
		    xoh = (fo + fm) * FFT_SIZE / m_FFTSampFreq;
#endif
        	break;
		case MODE_mfsk_L:
			fm = pRxSet->m_pDem->m_MFSK_BW;
			if( sys.m_MFSK_Center ){
				xl = (fo - fm/2 - 150) * FFT_SIZE / m_FFTSampFreq;
			    xh = (fo + fm/2 + 150) * FFT_SIZE / m_FFTSampFreq;
				xnl = xl;
		    	xnh = xh;
			    xol = (fo - fm/2 - 15) * FFT_SIZE / m_FFTSampFreq;
			    xoh = (fo + fm/2 + 15) * FFT_SIZE / m_FFTSampFreq;
            }
            else {
				xl = (fo - fm - 150) * FFT_SIZE / m_FFTSampFreq;
			    xh = (fo + 150) * FFT_SIZE / m_FFTSampFreq;
				xnl = xl;
		    	xnh = xh;
			    xol = (fo - fm - 15) * FFT_SIZE / m_FFTSampFreq;
			    xoh = (fo + 15) * FFT_SIZE / m_FFTSampFreq;
            }
            break;
		case MODE_mfsk_U:
			fm = pRxSet->m_pDem->m_MFSK_BW;
			if( sys.m_MFSK_Center ){
				xl = (fo - fm/2 - 150) * FFT_SIZE / m_FFTSampFreq;
			    xh = (fo + fm/2 + 150) * FFT_SIZE / m_FFTSampFreq;
				xnl = xl;
		    	xnh = xh;
			    xol = (fo - fm/2 - 15) * FFT_SIZE / m_FFTSampFreq;
			    xoh = (fo + fm/2 + 15) * FFT_SIZE / m_FFTSampFreq;
            }
			else {
				xl = (fo - 150) * FFT_SIZE / m_FFTSampFreq;
			    xh = (fo + fm + 150) * FFT_SIZE / m_FFTSampFreq;
				xnl = xl;
			    xnh = xh;
			    xol = (fo - 15) * FFT_SIZE / m_FFTSampFreq;
			    xoh = (fo + fm + 15) * FFT_SIZE / m_FFTSampFreq;
            }
            break;
    }
	fm = 200 * FFT_SIZE / m_FFTSampFreq;
	if( xl < fm ) xl = fm;
	if( xol == xoh ){xol--; xoh++;}
    if( xol < fm ) xol = fm;

	int i, y;
	int sum = 0;
    int sumn = 0;
    int max = 0;
    int wmax = 0;
	int m = 0;
    int n = 0;
	for( i = xl; i < xh; i++ ){
		y = m_fftout[i];
        if( max < y ) max = y;
		if( (i >= xol) && (i <= xoh) ){
			if( wmax < y ) wmax = y;
        }
		else if( (i >= xnl) && (i <= xnh) ){
			sumn += y;
            n++;
        }
        else {
	        sum += y;
            m++;
        }
    }
	if( m ) sum /= m;
    if( n ) sumn /= n;
	if( !m || (sumn < sum) ) sum = sumn;
	pRxSet->m_StgFFT.Sum = (pRxSet->m_StgFFT.Sum + sum) / 2;
    pRxSet->m_StgFFT.Max = (pRxSet->m_StgFFT.Max + max) / 2;
	pRxSet->m_StgFFT.WMax = (pRxSet->m_StgFFT.WMax + wmax) / 2;
    if( m_FFT.m_FFTGain ){
		pRxSet->m_StgFFT.dBSum = SqrtToDB(pRxSet->m_StgFFT.Sum);
		pRxSet->m_StgFFT.dBMax = SqrtToDB(pRxSet->m_StgFFT.Max);
		pRxSet->m_StgFFT.dBWMax = SqrtToDB(pRxSet->m_StgFFT.WMax);
		if( m_TX == txINTERNAL ) pRxSet->m_StgFFT.dBSum = 0;
    }
    else {
		double k = 100.0 / m_fftSC;
		pRxSet->m_StgFFT.dBSum = pRxSet->m_StgFFT.Sum * k;
		pRxSet->m_StgFFT.dBMax = pRxSet->m_StgFFT.Max * k;
		pRxSet->m_StgFFT.dBWMax = pRxSet->m_StgFFT.WMax * k;
		if( m_TX == txINTERNAL ) pRxSet->m_StgFFT.dBSum = -200;
    }

	BOOL fMFSKMet = FALSE;
	int d = pRxSet->m_StgFFT.Sig - 500;
    if( d > LEVELMAX ) d = LEVELMAX;
	int sq = pRxSet->m_SQ;
	if( (m_TX != txINTERNAL) && pRxSet->IsMFSK() && sys.m_MFSK_SQ_Metric ){
		pRxSet->m_SQ = (pRxSet->m_pDem->GetMFSKMetric(0)) >= pRxSet->m_SQLevel;
    }
    else {
		if( pRxSet->m_pDem->IsMFSKSQ() && (d < pRxSet->m_SQLevel) ){
			fMFSKMet = TRUE;
        	pRxSet->m_SQ = TRUE;
	    }
    	else {
	    	pRxSet->m_SQ = (d >= pRxSet->m_SQLevel);
	    }
    	if( pRxSet->m_SQLevel < 50 ) pRxSet->m_SQ = TRUE;
	}
    if( pRxSet->m_SQ ){
		pRxSet->m_SQTimer++;
		if( !sq ){
			pRxSet->m_pDem->m_AFCCount = 0;
			pRxSet->m_pDem->m_Decode.m_BAUDOT.ClearRX();
            pRxSet->m_fMBCS = 0;
			if( !m_TX ){
				if( sys.m_fAutoTS && (!pRxSet->m_cAutoTS1 || !pRxSet->m_cAutoTS2) ){
					RxStatus(pRxSet, "SON");
        	    }
	           	pRxSet->m_PeakSig = 0;
   		        pRxSet->m_AvgSig.Create(32);
            }
        }
		if( m_TX ){
			pRxSet->m_cAutoTS1 = -5 * SAMPFREQ/m_BufferSize;
        }
		else if( pRxSet->m_cAutoTS1 >= 0 ){
			pRxSet->m_cAutoTS1 = 10 * SAMPFREQ/m_BufferSize;
        }
    }
    else {
		pRxSet->m_SQTimer = 0;
		if( pRxSet->m_cAutoTS1 ){
			if( pRxSet->m_cAutoTS1 > 0 ){
				pRxSet->m_cAutoTS1--;
				if( sys.m_fAutoTS && !pRxSet->m_cAutoTS1 ){
					RxStatus(pRxSet, "SOFF");
            	}
            }
            else {
				pRxSet->m_cAutoTS1++;
            }
        }
    }
    if( pRxSet->m_cAutoTS2 ) pRxSet->m_cAutoTS2--;

	if( pRxSet->m_AFCTimerN ){
		pRxSet->m_AFCTimerN--;
        pRxSet->m_pDem->m_fEnableAFC = FALSE;
    }
	else if( pRxSet->Is170() ){
		pRxSet->m_pDem->m_fEnableAFC = pRxSet->m_SQ && (pRxSet->m_SQTimer > 4) && (pRxSet->m_StgFFT.Sig >= (600+500));
    }
    else {
		pRxSet->m_pDem->m_fEnableAFC = pRxSet->m_SQ;
    }

	if( pRxSet->m_StgFFT.Timer ){
		pRxSet->m_StgFFT.Timer--;
        return;
	}
    else {
	    pRxSet->m_StgFFT.Sig = (pRxSet->m_StgFFT.dBWMax - pRxSet->m_StgFFT.dBSum);
		pRxSet->m_fATC = (pRxSet->m_StgFFT.Sig >= ((m_ATCLevel*100)+300));
		m = pRxSet->Is170() ? (1700+300) : (2000+300);
        pRxSet->m_pDem->m_Decode.SetMeasClock((pRxSet->m_StgFFT.Sig >= m));
		pRxSet->m_pDem->m_DecPSK.SetSN(pRxSet->m_StgFFT.Sig);
    }

	// çLàÊAFCÇÃìÆçÏ
	BOOL fAFC = pRxSet->m_pDem->m_fAFC && m_AFCWidth && (m_TX != txINTERNAL);
	if( fAFC ){
		switch(pRxSet->m_Mode){
			case MODE_N_BPSK:
			case MODE_BPSK:
	        case MODE_qpsk_L:
	        case MODE_qpsk_U:
			case MODE_GMSK:
        		sq = (pRxSet->m_SQTimer >= 8) ? pRxSet->m_AFCSQ : (d >= pRxSet->m_StgFFT.dBMax/3);
        		break;
			case MODE_FSKW:
			case MODE_FSK:
        		sq = (pRxSet->m_SQTimer >= 32) ? pRxSet->m_AFCSQ : (d >= pRxSet->m_StgFFT.dBMax/2);
        		break;
			case MODE_RTTY:
			case MODE_U_RTTY:
        		sq = (pRxSet->m_SQTimer >= 32) ? pRxSet->m_AFCSQ : (d >= pRxSet->m_StgFFT.dBMax/2);
				break;
			case MODE_mfsk_L:
			case MODE_mfsk_U:
        		sq = (pRxSet->m_SQTimer >= 8) ? pRxSet->m_AFCSQ : (d >= pRxSet->m_StgFFT.dBMax/2);
				break;
		}
		if( !sq ){
			if( (m_AFCWidth > 50) && !pRxSet->IsMFSK() ){
				xl = (fo - m_AFCWidth) * FFT_SIZE / m_FFTSampFreq;
    			xh = (fo + m_AFCWidth) * FFT_SIZE / m_FFTSampFreq;
				int o = (xh + xl)/2;
				int m = (xh - xl)/2;
				int rmax = 0;
				int lmax = 0;
				int rc = 0;
				int lc = 0;
				int lm = 200 * FFT_SIZE / m_FFTSampFreq;
				for( i = 1; i < m; i++ ){
					if( (o - i) > lm ){
						y = m_fftout[o-i];
    					if( y > lmax ){
							lc = i;
            				lmax = y;
            			}
					}
					y = m_fftout[o+i];
					if( y > rmax ){
						rc = i;
						rmax = y;
					}
				}
				int th = ((m_AFCLevel*100) + 450);
                if( pRxSet->IsMFSK() && (th < 1650) ) th = 1650;
				lmax = AdjDB(lmax) - pRxSet->m_StgFFT.dBSum;
				rmax = AdjDB(rmax) - pRxSet->m_StgFFT.dBSum;
				if( pRxSet->IsRTTY() ){
					if( (lmax > th) && (rmax > th) ){
						o += (rc - lc);
					}
					else if( !sq ){
						if( lmax > th ){
							o -= lc;
						}
						else if( rmax > th ){
							o += rc;
						}
						else {
							o = 0;
						}
					}
					else {
						o = 0;
					}
				}
				else if( (lmax > th) && (rmax > th) ){
					if( (rc + lc) < 10 ){
						if( rmax > lmax ){
							o += rc;
       					}
       					else {
							o -= lc;
       					}
					}
					else {
						if( rc < lc ){
							o += rc;
       					}
       					else {
							o -= lc;
       					}
					}
				}
				else if( lmax > th ){
					o -= lc;
				}
				else if( rmax > th ){
					o += rc;
				}
				else {
					o = 0;
				}
				if( o ){
					int fq = o * m_FFTSampFreq / FFT_SIZE;
					o = fq - fo;
					m = ABS(o);
					if( m < m_AFCWidth ){
						int diff;
						BOOL bPSK = FALSE;
						switch(pRxSet->m_Mode){
							case MODE_N_BPSK:
							case MODE_BPSK:
					        case MODE_qpsk_L:
					        case MODE_qpsk_U:
                    			bPSK = TRUE;
        	        			diff = pRxSet->m_Speed*1.2;
								break;
							case MODE_GMSK:
        	        			diff = pRxSet->m_Speed;
								break;
                			case MODE_FSK:
        	        			diff = pRxSet->m_Speed*2;
                				break;
							case MODE_FSKW:
                			case MODE_RTTY:
                			case MODE_U_RTTY:
        	        			diff = pRxSet->m_pDem->m_RTTYShift;
                				break;
                            case MODE_mfsk_L:
								if( !sys.m_MFSK_Center ) fq += pRxSet->m_pDem->m_MFSK_BW/2;
                            	diff = pRxSet->m_pDem->m_MFSK_BW*1.5;
                                break;
                            case MODE_mfsk_U:
								if( !sys.m_MFSK_Center ) fq -= pRxSet->m_pDem->m_MFSK_BW/2;
                            	diff = pRxSet->m_pDem->m_MFSK_BW*1.5;
                            	break;
            			}
						if( !pRxSet->m_AFCTimerW && (pRxSet->m_AFCTimerW2 > 8) &&
                        	(ABS(pRxSet->m_AFCFQ - fq) < diff)
                        ){
							if( (m < 30) || (bPSK && (m < 50)) || (pRxSet->IsRTTY() && (m < 220)) ){
								fo = GetSignalFreq(fo+o, 50, pRxSet);
   								o = 0;
       						}
           	    			else if( m < 80 ){
								o /= 3;
							}
							else if( m < 160 ){
								o /= 8;
           					}
               				else {
								o /= 10;
							}
							pDem->SetCarrierFreq(fo + o);
       					}
						pRxSet->m_AFCTimerW2++;
//						pRxSet->m_AFCFQ = (pRxSet->m_AFCFQ + (fq * 2)) / 3;
						pRxSet->m_AFCFQ = pRxSet->m_AvgAFC.DoZ(fq);
					}
				}
				else {
					pRxSet->m_AFCTimerW2 -= pRxSet->IsRTTY() ? 3 : 2;
					if( pRxSet->m_AFCTimerW2 < 0 ) pRxSet->m_AFCTimerW2 = 0;
				}
			}
            else {
				pRxSet->m_AFCFQ = fo;
            }
		}
		else if( pRxSet->IsMFSK() ){
	        if( !fMFSKMet ){
				DoAFCMFSK(pRxSet, fo, TRUE);
        	}
        }
		else {
			double bw = pRxSet->GetBandWidth();
			if( bw > 25.0 ){
				fm = GetSignalFreq(fo, bw*0.75, pRxSet, pRxSet->m_SQ ? 600 : (m_AFCLevel*100)+450);
				pRxSet->m_AFCFQ = pRxSet->m_AvgAFC.DoZ(fm);
				if( pRxSet->m_SQ ){
					m = ABS(fo - pRxSet->m_AFCFQ);
			        fm = bw * 0.5;
	   			    if( fm < m_AFCWidth ) fm = m_AFCWidth;
					if( (m >= int(bw*0.2)) && (m < fm) ){
						pRxSet->m_AFCTimerPSK++;
						if( pRxSet->m_AFCTimerPSK >= UINT(IsBPSK() ? 6 : 8) ){
							fm = (pRxSet->m_AFCFQ - fo)*0.75;
							pDem->SetCarrierFreq(fo+fm);
						}
					}
					else {
						pRxSet->m_AFCTimerPSK = 0;
					}
	            }
			}
		}
	}
	else if( pRxSet->IsMFSK() ){
		DoAFCMFSK(pRxSet, fo, FALSE);
    }
    else {
		double bw = pRxSet->GetBandWidth();
		fm = GetSignalFreq(fo, bw*0.75, pRxSet, pRxSet->m_SQ ? 600 : (m_AFCLevel*100)+450);
		pRxSet->m_AFCFQ = pRxSet->m_AvgAFC.DoZ(fm);
    }
	pRxSet->m_AFCSQ = pRxSet->m_SQ;
    if( pRxSet->m_SQ ){
        pRxSet->m_AFCTimerW = 3 * SAMPFREQ / m_BufferSize;
    }
	else if( pRxSet->m_AFCTimerW ){
    	pRxSet->m_AFCTimerW--;
        pRxSet->m_AFCTimerW2 = 0;
    }
    sum = int((pRxSet->m_StgFFT.Sig-500)*0.01);
	if( m_TX != txINTERNAL ){
		if( m_FFT.m_FFTGain ){
			if( sum >= 38 ){
				sum += (sum - 38) * 1.75;
            }
        }
        else {
			if( sum >= 32 ){
				sum += (sum - 32) * 0.357;
            }
        }
    }
    if( sum < 0 ) sum = 0;
    if( sum > 96 ) sum = 96;
    pRxSet->m_StgFFT.DispSig = sum;
	if( pRxSet == &m_RxSet[0] ){
	    sprintf(m_TextBuff, "S/N=%ddB", sum);
        DrawStatus(statusSN, m_TextBuff);
    }
	if( !m_TX ){
       	if( pRxSet->m_PeakSig < sum ) pRxSet->m_PeakSig = sum;
		if( pRxSet->m_SQ ) pRxSet->m_AvgSig.Do(sum);
    }
}
//---------------------------------------------------------------------------
#define	MFSKAFC_1stATACK	(8*11025/2048)		// ç≈èâÇÃ8ïbÇÕîÕàÕçLÇ¢
#define	MFSKAFC_2ndATACK	(30*11025/2048)		// ÇRÇOïbåoâﬂå„ÇÕîÕàÕã∑Ç¢
#define	MFSKAFC_MAX			(60*11025/2048)
void __fastcall TMainVARI::DoAFCMFSK(CRxSet *pRxSet, int fo, BOOL fUpdate)
{
	if( !m_AFCWidth ) return;

	int bw;
	int xl, xh, x, xx;
    CDEMFSK *pDem = pRxSet->m_pDem;

	if( sys.m_MFSK_Center ){
		bw = (pDem->m_MFSK_BW*0.5) + 0.5;
    	xl = fo - bw;
    	xh = fo + bw;
    }
    else {
		bw = pDem->m_MFSK_BW + 0.5;
		if( pRxSet->m_Mode == MODE_mfsk_U ){
			xl = fo; xh = fo + bw;
        }
        else {
			xl = fo - bw; xh = fo;
        }
    }
	xl = int((xl * FFT_SIZE / m_FFTSampFreq));
	xh = int((xh * FFT_SIZE / m_FFTSampFreq) + 0.5);
	int f;
    if( pRxSet->m_AFCTimerMFSK < MFSKAFC_2ndATACK ){
		if( pRxSet->m_AFCTimerMFSK < MFSKAFC_1stATACK ){
			f = m_AFCWidth;
   			if( f >= 32 ) f = 32;
	    }
    	else {
			f = pDem->m_MFSK_SPEED;
	    }
	    f = f * FFT_SIZE / m_FFTSampFreq;
		xl -= f; xh += f;
    }
    else {
		if( pDem->m_MFSK_TONES == 8 ){
			xl-=2; xh+=2;
	    }
    	else {
			xl--; xh++;
	    }
    }

	if( pRxSet->m_AFCTimerMFSK < MFSKAFC_MAX ){
		pRxSet->m_AFCTimerMFSK++;
        if( pDem->GetMFSKMetric(0) >= 800 ){
			pRxSet->m_AFCTimerMFSK+=8;
        }
    }

	int d;
    int avg = 0;
    int max = 0;
    xx = xl;
	for( x = xl; x <= xh; x++ ){
    	d = m_fftout[x];
        avg += d;
        if( max < d ){
        	max = d;
            xx = x;
        }
    }
    avg /= (xh - xl);
	int maxdb, avgdb;
    if( m_FFT.m_FFTGain ){
		avgdb = SqrtToDB(avg);
		maxdb = SqrtToDB(max);
    }
    else {
		double k = 100.0 / m_fftSC;
        avgdb = avg * k;
        maxdb = max * k;
	}
	if( (maxdb - avgdb) < 750 ) return;

   	xl = m_fftout[xx] - m_fftout[xx-1];
   	xh = m_fftout[xx] - m_fftout[xx+1];
	if( (xl > 0) && (xh > 0) ){	// FFTä‘äuï‚ê≥
        double off = double(xl - xh) * 0.5 / double(xl + xh);
		x = ((xx + off) * m_FFTSampFreq / FFT_SIZE) + 0.5;
//		sprintf(m_TextBuff, "%u", x);
//        Caption = m_TextBuff;
   	}
    else {
		x = (xx * m_FFTSampFreq / FFT_SIZE) + 0.5;
    }
	f = 0;
	if( sys.m_MFSK_Center ){
		if( x < (fo - bw) ){
			f = x + bw;
        }
        else if( x > (fo + bw) ){
			f = x - bw;
        }
    }
    else if( pRxSet->m_Mode == MODE_mfsk_U ){
		if( x < fo ){
			f = x;
        }
        else if( x > (fo + bw) ){
			f = x - bw;
        }
    }
    else {
		if( x < (fo - bw) ){
			f = x + bw;
        }
        else if( x > fo ){
			f = x;
        }
    }
	double fq;
    if( pDem->m_pMFSK->GetAFCShift(fq) ){
#if 0
		static int Cnt = 0;
		char bf[256];
		sprintf(bf, "%.1lf (%d)", fq, Cnt++);
		Application->MainForm->Caption = bf;
#endif
		fq += pDem->m_CarrierFreq;
		pRxSet->m_AvgAFC.Do(fq);
		pRxSet->m_AFCFQ = pRxSet->m_AvgAFC.Do(fq) + 0.5;
		pDem->m_RxFreq = pRxSet->m_AFCFQ;
		if( fUpdate ){
           	if( pRxSet->m_AFCFQ != int(pDem->m_CarrierFreq) ){
       			pDem->SetCarrierFreq(pRxSet->m_AFCFQ);
            }
        }
    }
    else if( f ){
		pRxSet->m_AFCFQ = pRxSet->m_AvgAFC.Do(f)+0.5;
		pDem->m_RxFreq = pRxSet->m_AFCFQ;
		if( fUpdate ){
           	if( pRxSet->m_AFCFQ != int(pDem->m_CarrierFreq) ){
	        	pDem->SetCarrierFreq(pRxSet->m_AFCFQ);
            }
        }
        else {
			pRxSet->m_AFCTimerMFSK = 0;
        }
    }
    else if( !fUpdate ){
		DoAvg(pDem->m_RxFreq, fo, (pRxSet->m_AFCTimerMFSK >= (15*4)) ? 0.1 : 0.005);
		pRxSet->m_AvgAFC.Do(pDem->m_RxFreq);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::PBoxRXPaint(TObject *Sender)
{
	m_Dump.Paint();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::PBoxTXPaint(TObject *Sender)
{
	m_Edit[m_CurrentEdit].Paint();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::AttachFocus(void)
{
	SetTXFocus();
	m_Edit[m_CurrentEdit].CreateCaret();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::DettachFocus(void)
{
	m_Edit[m_CurrentEdit].DestroyCaret();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::FormActivate(TObject *Sender)
{
	AttachFocus();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::FormDeactivate(TObject *Sender)
{
	DettachFocus();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::PCTXEnter(TObject *Sender)
{
	AttachFocus();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::PCTXExit(TObject *Sender)
{
	DettachFocus();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::PBoxTXClick(TObject *Sender)
{
	DeleteMacroTimer();
	AttachFocus();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::OnChar(int Key)
{
	CDump *pEdit = &m_Edit[m_CurrentEdit];
	if( m_TX ){
		if( Key == '\b' ){
			if( !pEdit->CanEdit() ) return;
			if( !pEdit->GetCharCount(TRUE) && pEdit->GetCharCount(FALSE) ){
				if( IsRTTY() ){
					Key = 'X';
                }
                else {
					m_ModFSK.m_Encode.PutChar(Key);
                }
   			}
   	    }
    }
	if( Key >= 0x8140 ){
		pEdit->PutChar(Key, 1);
    }
    else {
		if( IsRTTY() ) Key = toupper(Key);
		pEdit->PutKey(char(Key), 1);
    }
    if( m_TX && (m_fReqRX == 1) ){
    	m_fReqRX = 0;
		m_ModFSK.m_Encode.m_fReqRX = FALSE;
        SetTXCaption();
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::FormKeyPress(TObject *Sender, char &Key)
{
#if !KEY_DIRECT
	if( ActiveControl == PCTX ){
		OnChar(Key & 0x00ff);
    }
#endif
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::UpdateWaveCaption(void)
{
	SBWave->Caption = m_WaveType ? "Wav.":"Sync";
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SetTXCaption(void)
{
	if( m_fReqRX ){
		SBTX->Caption = m_fReqRX > 1 ? "Wait" : "ReqRX";
    }
	else if( m_fTone ){
		SBTX->Caption = "TONE";
    }
	else {
		AnsiString as = m_TX ? "RX" : "TX";
		LPCSTR pKey = GetKeyName(sys.m_DefKey[kkTX]);
        if( *pKey && (strlen(pKey) < 5) ){
			sprintf(m_TextBuff, "(%s)", pKey);
            as += m_TextBuff;
        }
	    SBTX->Caption = as;
    }

	AnsiString as = "TXOFF";
	LPCSTR pKey = GetKeyName(sys.m_DefKey[kkTXOFF]);
    if( *pKey && (strlen(pKey) < 3) ){
		sprintf(m_TextBuff, "(%s)", pKey);
        as += m_TextBuff;
	}
	SBTXOFF->Caption = as;

}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SetPTT(BOOL fTX)
{
	if( m_pCom ) m_pCom->SetPTT(fTX);
    if( m_pRadio ){
		if( fTX ) WaitICOM();
    	m_pRadio->SetPTT(fTX);
    }
    m_Wave.SetPTT(fTX);
	LogLink.SetPTT(fTX);
    DoEvent(macOnPTT);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::RxStatus(CRxSet *pRxSet, LPCSTR p)
{
	CDump *pDump;
	if( pRxSet == &m_RxSet[0] ){
		pDump = &m_Dump;
    }
    else if( pRxSet->m_pView ){
		pDump = &pRxSet->m_pView->m_Dump;
    }
    else {
		return;
    }
	if( m_StatusUTC ){
		::GetUTC(&m_LocalTime);
    }
    else {
		::GetLocal(&m_LocalTime);
    }
	pDump->PutStatus(3, "[%s %u/%u/%u %02u:%02u:%02u]\r",
		p,
    	m_LocalTime.wYear,
        m_LocalTime.wMonth,
        m_LocalTime.wDay,
        m_LocalTime.wHour,
        m_LocalTime.wMinute,
        m_LocalTime.wSecond
    );
	pRxSet->m_cAutoTS2 = 600 * SAMPFREQ/m_BufferSize;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SetTXInternal(void)
{
	if( sys.m_LoopBack == loopINTERNAL ){
		m_RxSet[0].SetCarrierFreq(UdTxCarrier->Position * SAMPFREQ/(SAMPFREQ+SAMPTXOFFSET));
   		CRxSet *pRxSet = m_RxSet;
		for( int i = 0; i < RXMAX; i++, pRxSet++ ){
			if( pRxSet->IsActive() ){
				if( i ){
       	        	pRxSet->SetSampleFreq((SAMPFREQ+SAMPTXOFFSET)*m_DecFactor);
					if( pRxSet->m_fAFC ){
						int f = pRxSet->m_pDem->m_CarrierFreq - UdTxCarrier->Position;
    	                f = ABS(f);
   	    	            if( f < (pRxSet->m_Speed * 0.5) ) pRxSet->SetCarrierFreq(UdTxCarrier->Position);
                    }
				}
		        pRxSet->m_pDem->MakeBPF(8);		// CPUïââ◊í·å∏ÇÃÇΩÇﬂ
				pRxSet->m_pDem->m_Decode.m_fATC = FALSE;
                pRxSet->m_pDem->m_DecPSK.m_bATC = FALSE;
				pRxSet->m_pDem->m_fAFC = FALSE;
				pRxSet->m_pDem->m_Decode.Create();
//				pRxSet->m_pDem->m_Decode.SetTmg(i ? 0 : 1e6*((SAMPFREQ+SAMPTXOFFSET)/SAMPFREQ - 1.0));
				pRxSet->m_pDem->SetTmg(i ? 0 : 1e6*((SAMPFREQ+SAMPTXOFFSET)/SAMPFREQ - 1.0));
		    	pRxSet->m_pDem->m_AGC.Reset();
//                pRxSet->m_PeakSig = 0;
//                pRxSet->m_AvgSig.Create(32);
				for( int j = 0; j < m_BufferSize; j++ ){
       				pRxSet->m_pDem->Do(0);
		        }
				pRxSet->InitStgFFT();
            }
		}
		FFTSampleFreq(SAMPFREQ + SAMPTXOFFSET);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::ToTone(void)
{
	if( m_fSuspend ) return;
	if( !SBTX->Enabled ) return;
	m_fTone = TRUE;
    SBTX->Down = TRUE;
	m_TX = (sys.m_LoopBack == loopINTERNAL) ? txINTERNAL : txEXTERNAL;
	if( sys.m_LoopBack == loopINTERNAL ){
    	m_Wave.InClose();
		DeleteSoundTimer();
    }
   	if( SBNET->Down ){
       	m_fDisEvent++;
        UdTxCarrier->Position = UdRxCarrier->Position;
        m_fDisEvent--;
    }
	m_ModFSK.Reset();	// For Signal/CW gain
	m_ModFSK.SetCarrierFreq(UdTxCarrier->Position);
	SetTXInternal();
	m_fSendChar = FALSE;
	m_Wave.m_hWnd = Handle;
	InitWater(iniwMETRIC);
	if( !m_Wave.IsOutOpen() ) OpenSound(TRUE);
	memset(m_wBuffer, 0, sizeof(m_wBuffer));
	if( m_pCom ){
    	m_pCom->SetSendChar(FALSE);
    }
	SetPTT(TRUE);
    SetTXCaption();
	RxStatus(&m_RxSet[0], "TX");
	if( m_Wave.IsOutFirst() ){
		while(!m_Wave.IsOutBufFull()){
			DoMod();
			if( sys.m_LoopBack == loopINTERNAL ) DoDem();
			m_Wave.OutWrite(m_wBuffer);
   		}
    }
    UpdateUI();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::ToTX(void)
{
#if DEBUG
	if( m_fpTest ){
		fclose(m_fpTest);
        m_fpTest = NULL;
    }
    if( sys.m_test ){
        m_Wave.OutAbort();
        sys.m_test = FALSE;
    }
#endif
	if( m_fSuspend ) return;
	if( !SBTX->Enabled ) return;
	StopPlayBack();
	m_SendingEdit = m_CurrentEdit;
	BOOL fATone = m_fTone;
	m_fTone = FALSE;
	SBTX->Down = TRUE;
	m_TX = (sys.m_LoopBack == loopINTERNAL) ? txINTERNAL : txEXTERNAL;
	if( sys.m_LoopBack == loopINTERNAL ){
    	m_Wave.InClose();
		DeleteSoundTimer();
    }
   	if( SBNET->Down ){
       	m_fDisEvent++;
        UdTxCarrier->Position = UdRxCarrier->Position;
        m_fDisEvent--;
    }
	CDump *pEdit = &m_Edit[m_CurrentEdit];
	BOOL fCW = (pEdit->GetCharNB() & 0xff00) == 0x100;
	if( !fATone ) m_ModFSK.Reset();	// For Signal/CW gain
	m_ModFSK.m_Encode.Reset(fCW);
	while(pEdit->GetCharNB() == 0x200){
		pEdit->GetChar(TRUE);
	   	m_ModFSK.m_Encode.PutChar(0x200);
    }
	if( KOAI->Checked && !fCW ){
	   	m_ModFSK.m_Encode.PutChar('\r');
	   	m_ModFSK.m_Encode.PutChar('\n');
    }
	if( !fATone ) m_ModFSK.SetCarrierFreq(SBNET->Down ? m_RxSet[0].m_pDem->m_CarrierFreq : UdTxCarrier->Position);
	m_RxSet[0].m_pDem->ResetMFSK();
	SetTXInternal();
	m_fSendChar = FALSE;
	m_Wave.m_hWnd = Handle;
	InitWater(iniwMETRIC);
	if( !m_Wave.IsOutOpen() ) OpenSound(TRUE);
	if( m_pCom ){
		m_pCom->SetDiddle(m_ModFSK.m_Encode.GetDiddle());
    	m_pCom->SetSendChar(IsRTTY());
    }
	if( !fATone ){
		SetPTT(TRUE);
		memset(m_wBuffer, 0, sizeof(m_wBuffer));
    }
	if( sys.m_LoopBack == loopINTERNAL ){
		DoDem();
		if( !m_RxSet[0].IsMFSK() ){
			for( int i = 0; i < 8192; i++ ){
				m_RxSet[0].m_pDem->m_Decode.Do(1, TRUE, FALSE);
        	}
			if( m_RxSet[0].IsQPSK() ) m_RxSet[0].m_pDem->m_DecPSK.ResetRX();
        }
    }
	if( !fATone ) RxStatus(&m_RxSet[0], "TX");
	if( m_Wave.IsOutFirst() ){
		while(!m_Wave.IsOutBufFull()){
			DoMod();
			if( sys.m_LoopBack == loopINTERNAL ) DoDem();
			m_Wave.OutWrite(m_wBuffer);
   		}
    }
    UpdateUI();
    if( m_fReqRX ){
		m_fReqRX = 1;
		m_ModFSK.m_Encode.m_fReqRX = TRUE;
	    SBTX->Caption = "ReqRX";
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::ToRX(void)
{
	m_fTone = FALSE;
	RxStatus(&m_RxSet[0], "RX");

	if( m_fpText ){
		fclose(m_fpText);
        m_fpText = NULL;
    }
	SBTX->Down = FALSE;
	m_TX = txRX;
	m_fReqRX = FALSE;
	m_ModFSK.m_Encode.m_fReqRX = FALSE;
    SetTXCaption();
	if( m_WaveFile.m_mode != 1 ){
    	m_Wave.OutAbort();
		DeleteSoundTimer();
    }
	SetPTT(FALSE);
	InitWater(iniwMETRIC);
	if( !m_Wave.IsInOpen() ){		// Loopback == loopINTERNAL
		SetBPFType(GetBPFType());
	    m_RxSet[0].SetCarrierFreq( (UdRxCarrier->Position != int(m_ModFSK.m_CarrierFreq+0.5)) ? UdRxCarrier->Position : m_ModFSK.m_CarrierFreq);
        CRxSet *pRxSet = m_RxSet;
        int atctmg;
		if( sscanf(AnsiString(EATC->Text).c_str(), "%d", &atctmg) != 1 ) atctmg = 0;	//JA7UDE 0428
		for(int i = 0; i < RXMAX; i++, pRxSet++ ){
			if( pRxSet->IsActive() ){
				if( i ){
					pRxSet->SetSampleFreq(DEMSAMPFREQ);
                	pRxSet->m_pDem->MakeBPF(m_RxSet[0].m_pDem->m_PreBPFTaps);
                }
				pRxSet->m_pDem->m_AFCCount = 0;
				pRxSet->m_pDem->m_fAFC = i ? pRxSet->m_fAFC : SBAFC->Down;
				pRxSet->m_pDem->m_Decode.m_fATC = i ? TRUE : SBATC->Down;
                pRxSet->m_pDem->m_DecPSK.m_bATC = pRxSet->m_pDem->m_Decode.m_fATC;
//				if( !pRxSet->IsRTTY() ) pRxSet->m_pDem->m_Decode.SetTmg(i ? 0 : atctmg);
				pRxSet->m_pDem->SetTmg((i || pRxSet->IsMFSK()||pRxSet->IsQPSK()) ? 0 : atctmg);
                pRxSet->m_pDem->m_Decode.ResetMeasRTTY();
                pRxSet->m_pDem->ResetMFSK();
				for( int j = 0; j < m_BufferSize; j++ ){
		        	pRxSet->m_pDem->Do(0);
		            pRxSet->m_pDem->m_Decode.Do(0, 0, FALSE);
        		}
				pRxSet->m_pDem->m_DecPSK.ResetRX();
                pRxSet->m_pDem->m_AGC.Reset();
				pRxSet->InitStgFFT();
		        pRxSet->m_SQ = FALSE;
        		pRxSet->m_AFCTimerW = 5 * SAMPFREQ / m_BufferSize;
		        pRxSet->m_AFCTimerN = SAMPFREQ / m_BufferSize;
                pRxSet->m_SQTimer = 0;
				pRxSet->m_AFCTimerPSK = 0;
				pRxSet->m_AFCTimerMFSK = MFSKAFC_MAX;
                pRxSet->m_PeakSig = 0;
                pRxSet->m_AvgSig.Create(32);
            }
        }
		if( m_WaveFile.m_mode != 1 ) OpenSound(FALSE);
		FFTSampleFreq(SAMPFREQ);
    	m_FFT.ClearStg();
    }
    else {								// loopEXTERNAL
        m_RxSet[0].m_AFCTimerN = 3 * SAMPFREQ / m_BufferSize;
    }
	m_RxSet[0].m_PeakSig = 0;
	m_RxSet[0].m_AFCTimerPSK = 0;
	m_RxSet[0].m_AFCTimerMFSK = MFSKAFC_MAX;

	if( m_ReqMacroTimer ) CreateMacroTimer(m_ReqMacroTimer);
    UpdateUI();
    if( m_ReqAutoClear ) KVCTClick(NULL);
	if( m_ReqAutoReturn ){
		m_ReqAutoReturn = FALSE;
    	SetEditPage(m_SaveEditPage);
    }
    if( m_ReqAutoNET ){
		m_ReqAutoNET = FALSE;
		if( !SBNET->Down ){
        	SBNET->Down = TRUE;
            SBNETClick(NULL);
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::DeleteSoundTimer(void)
{
	if( m_pSoundTimer ){
		m_pSoundTimer->Enabled = FALSE;
		delete m_pSoundTimer;
        m_pSoundTimer = NULL;
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SBTXMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if( Button == mbRight ){
		if( m_TX ){
			if( m_fTone && (m_fReqRX != 1) ){
				m_fReqRX = 1;
				m_ModFSK.m_Encode.m_fReqRX = TRUE;
            }
        }
        else {
			ToTone();
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SBTXClick(TObject *Sender)
{
	DeleteMacroTimerS();
	if( SBTX->Down ){
		ToTX();
    }
    else if( m_fReqRX ){
		if( m_fReqRX == 1 ){
			m_fReqRX = FALSE;
			m_ModFSK.m_Encode.m_fReqRX = FALSE;
        }
       	SBTX->Down = SBTX->Enabled ? TRUE : FALSE;
    	if( !SBTX->Down ) m_TX = txRX;
    }
    else {
		m_fReqRX = TRUE;
		m_ModFSK.m_Encode.m_fReqRX = TRUE;
       	SBTX->Down = SBTX->Enabled ? TRUE : FALSE;
    	if( !SBTX->Down ) m_TX = txRX;
        m_Edit[m_CurrentEdit].MoveCursor(dmpMoveLAST);
    }
    SetTXCaption();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SBTXOFFClick(TObject *Sender)
{
	m_ReqAutoClear = FALSE;
	DeleteMacroTimer();
	if( m_TX ){
    	ToRX();
    }
    else if( m_fReqRX ){
    	m_fReqRX = FALSE;
		SetTXCaption();
    }
    m_Edit[m_CurrentEdit].MoveCursor(dmpMoveLAST);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::PCRXResize(TObject *Sender)
{
	m_Dump.Resize();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::PCTXResize(TObject *Sender)
{
	m_Edit[m_CurrentEdit].Resize();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SBFFTClick(TObject *Sender)
{
    m_fftMX = 0;
	Draw(TRUE);
}
//---------------------------------------------------------------------------
TSpeedButton *__fastcall TMainVARI::GetDraw(int n)
{
	switch(n){
		case 1:
        	return SBWater;
        case 2:
        	return SBWave;
        default:
        	return SBFFT;
    }
}
//---------------------------------------------------------------------------
int __fastcall TMainVARI::GetDrawType(void)
{
	for( int i = 0; i < 3; i++ ){
		if( GetDraw(i)->Down ) return i;
    }
    return 0;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SetDrawType(int n)
{
	TSpeedButton *pButton = GetDraw(n);
    pButton->Down = TRUE;
}
//---------------------------------------------------------------------------
TSpeedButton *__fastcall TMainVARI::GetBPF(int n)
{
	switch(n){
		case 1:
        	return SBBPFM;
        case 2:
        	return SBBPFN;
        case 3:
        	return SBBPFS;
        default:
        	return SBBPFW;
    }
}
//---------------------------------------------------------------------------
int __fastcall TMainVARI::GetBPFType(void)
{
	for( int i = 0; i < 4; i++ ){
		if( GetBPF(i)->Down ) return i;
    }
    return 0;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SetBPFType(int n)
{
	TSpeedButton *pButton = GetBPF(n);
    pButton->Down = TRUE;
    m_RxSet[0].m_pDem->MakeBPF(GetBPFTaps(pButton));
}
//---------------------------------------------------------------------------
int __fastcall TMainVARI::GetBPFTaps(TObject *Sender)
{
	for( int i = 0; i < 4; i++ ){
		if( Sender == (TObject *)GetBPF(i) ){
			return g_tBpfTaps[i];
        }
    }
    return 64;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SBBPFWClick(TObject *Sender)
{
	if( m_fDisEvent ) return;

	int taps = GetBPFTaps(Sender);
	CDEMFSK *pDem = m_RxSet[0].m_pDem;
    int delay = (pDem->m_inBPF.GetTap() - taps)/2;
    pDem->MakeBPF(taps);
    CRxSet *pRxSet = &m_RxSet[1];
	for( int i = 1; i < RXMAX; i++, pRxSet++ ){
		if( pRxSet->IsActive() ){
			pRxSet->m_pDem->MakeBPF(pDem->m_PreBPFTaps);
    	    pRxSet->m_pDem->m_pBPF = &pDem->m_inBPF;
        }
    }
	if( !m_Wave.IsInOpen() ) return;

    if( delay < 0 ){
		while( delay < 0 ){
			DoDem(0);
       	    delay++;
       	}
    }
   	else {
		m_SkipTmg = delay;
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SetTxFreq(int fq)
{
    if( (fq >= MIN_CARRIER) && (fq <= sys.m_MaxCarrier) ){
		m_ModFSK.SetCarrierFreq(fq);
		m_fDisEvent++;
		UdTxCarrier->Position = short(fq);
		if( m_TX && SBNET->Down ){
			UdRxCarrier->Position = short(fq);
        }
        if( UdTxCarrier->Position != UdRxCarrier->Position ) SBNET->Down = FALSE;
        m_fDisEvent--;
		if( (m_TX == txINTERNAL) || SBNET->Down ){
        	m_RxSet[0].SetCarrierFreq(fq);
        }
        UpdateUI();
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SetRxFreq(int fq)
{
    if( (fq >= MIN_CARRIER) && (fq <= sys.m_MaxCarrier) ){
		if( m_TX != txINTERNAL ){
			m_RxSet[0].SetCarrierFreq(fq);
        }
		m_fDisEvent++;
		UdRxCarrier->Position = short(fq);
        if( m_pRadio ) m_pRadio->SetCarrierFreq(fq);
		if( !m_TX && SBNET->Down ){
			UdTxCarrier->Position = short(fq);
        }
        m_fDisEvent--;
        m_RxSet[0].m_AFCTimerW = 1;
        m_RxSet[0].m_SQTimer = 0;
        if( !m_RxSet[0].IsBPSK() ) m_PFTimer = 4;
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::UdTxCarrierClick(TObject *Sender,
      TUDBtnType Button)
{
	if( m_fDisEvent ) return;

	SetTxFreq(UdTxCarrier->Position);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::CBTxCarrierChange(TObject *Sender)
{
	if( m_fDisEvent ) return;

	int fq;
	sscanf(AnsiString(CBTxCarrier->Text).c_str(), "%u", &fq);	//JA7UDE 0428
	SetTxFreq(fq);
    if( m_fDrop ){
		m_fDrop = FALSE;
		SetTXFocus();
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::UdRxCarrierClick(TObject *Sender,
      TUDBtnType Button)
{
	if( m_fDisEvent ) return;

	SetRxFreq(UdRxCarrier->Position);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::CBRXCarrierChange(TObject *Sender)
{
	if( m_fDisEvent ) return;

	int fq;
	sscanf(AnsiString(CBRXCarrier->Text).c_str(), "%u", &fq);
	SetRxFreq(fq);
    if( m_fDrop ){
		m_fDrop = FALSE;
		SetTXFocus();
    }
    else {
		m_AFCKeyTimer = 3 * SAMPBASE / m_BufferSize;
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SetSpeedInfo(double b)
{
	if( m_RxSet[0].IsMFSK() ){
		switch(m_RxSet[0].m_MFSK_TYPE){
			case typMFSK16:
            	strcpy(m_TextBuff, "mfsk16 (15.625 baud, 16 tones)");
            	break;
            case typMFSK8:
            	strcpy(m_TextBuff, "mfsk8 (7.8125 baud, 32 tones)");
            	break;
            case typMFSK31:
            	strcpy(m_TextBuff, "mfsk31 (31.25 baud, 8 tones)");
            	break;
            case typMFSK11:
            	strcpy(m_TextBuff, "mfsk11 (10.767 baud, 16 tones)");
            	break;
            case typMFSK22:
            	strcpy(m_TextBuff, "mfsk22 (21.533 baud, 16 tones)");
            	break;
            case typMFSK32:
            	strcpy(m_TextBuff, "mfsk32 (31.25 baud, 16 tones)");
            	break;
            case typMFSK64:
            	strcpy(m_TextBuff, "mfsk64 (62.5 baud, 16 tones)");
            	break;
            case typMFSK4:
            	strcpy(m_TextBuff, "mfsk4 (3.90625 baud, 32 tones)");
            	break;
		}
    }
    else {
		StrDbl(m_TextBuff, b);
    	strcat(m_TextBuff, "Bps");
    }
	switch( m_RxSet[0].m_Mode ){
		case MODE_BPSK:
        case MODE_N_BPSK:
			if( b == 31.25 ){
            	strcat(m_TextBuff, " (PSK31)");
            }
            else if( b == 62.5 ){
            	strcat(m_TextBuff, " (PSK63)");
            }
            else if( b == 125 ){
            	strcat(m_TextBuff, " (PSK125)");
            }
            else if( b == 250 ){
            	strcat(m_TextBuff, " (PSK250)");
            }
        	break;
		case MODE_qpsk_L:
        case MODE_qpsk_U:
			if( b == 31.25 ){
            	strcat(m_TextBuff, " (QPSK31)");
            }
            else if( b == 62.5 ){
            	strcat(m_TextBuff, " (QPSK63)");
            }
            else if( b == 125 ){
            	strcat(m_TextBuff, " (QPSK125)");
            }
            else if( b == 250 ){
            	strcat(m_TextBuff, " (QPSK250)");
            }
        	break;
        case MODE_RTTY:
        case MODE_U_RTTY:
			if( b == 45.45 ){
				strcat(m_TextBuff, sys.m_MsgEng ? "(Standard)":"(ïWèÄ)");
            }
        	break;
    }
	if( IsRTTY() ){
		if( b != 45.45 ){
			strcat(m_TextBuff, sys.m_MsgEng ? " : 45.45Bps is standard" : " : ïWèÄÇÕ45.45Bps");
	    }
	}
    else if( !m_RxSet[0].IsMFSK() ){
		if( b != 31.25 ){
			strcat(m_TextBuff, sys.m_MsgEng ? " : 31.25Bps is standard" : " : ïWèÄÇÕ31.25Bps");
	    }
    }
    SetInfoMsg(m_TextBuff);
    DoEvent(macOnSpeed);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::CBSpeedChange(TObject *Sender)
{
	if( m_fDisEvent ) return;

	double b;
	sscanf(AnsiString(CBSpeed->Text).c_str(), "%lf", &b);	//JA7UDE 0428
    SpeedChange(b);
    if( m_fDrop ){
		m_fDrop = FALSE;
		SetTXFocus();
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SpeedChange(double b)
{
	if( m_RxSet[0].IsMFSK() ){
        m_RxSet[0].SetMFSKType(MFSK_Speed2Type(b));
#if DEBUG
		m_ModTest.SetMFSKType(m_RxSet[0].m_MFSK_TYPE);
#endif
		m_ModFSK.SetMFSKType(m_RxSet[0].m_MFSK_TYPE);
		InitCollect();
		SetSpeedInfo(b);
    }
    else if( (b >= MIN_SPEED) && (b <= MAX_SPEED) && (b != m_RxSet[0].m_Speed) ){
        m_RxSet[0].SetSpeed(b);
#if DEBUG
		m_ModTest.SetSpeed(b);
#endif
		m_ModFSK.SetSpeed(b);
		InitCollect();
		SetSpeedInfo(b);
		if( IsRTTY() && sys.m_bFSKOUT && m_pCom && m_pCom->IsBaudChange(b) ){
			OpenCom();
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::UpdateSpeed(CRxSet *pRxSet, double b)
{
	if( pRxSet == &m_RxSet[0] ){
		if( pRxSet->IsMFSK() ){
			SpeedChange(b);
	        UpdateUI();
        }
        else {
			m_fDisEvent++;
			char bf[256];
			CBSpeed->Text = StrDbl(bf, b);
	    	m_fDisEvent--;
		    CBSpeedChange(NULL);
        }
    }
	else if( pRxSet->IsMFSK() ){
		pRxSet->SetMFSKType(MFSK_Speed2Type(b));
    }
    else {
		pRxSet->SetSpeed(b);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::UpdateMode(CRxSet *pRxSet, int offset)
{
	if( pRxSet == &m_RxSet[0] ){
    	if( IsRTTY() ){
			GBTiming->Caption = "Timing(ms)";
        }
        else {
			GBTiming->Caption = "Timing(ppm)";
        }
        if( offset ){
        	SetRxFreq(UdRxCarrier->Position + offset);
			if( m_TX || !SBNET->Down ){
	        	SetTxFreq(UdTxCarrier->Position + offset);
            }
        }
    }
    else if( offset ){
		pRxSet->SetCarrierFreq(pRxSet->m_CarrierFreq + offset);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KFSClick(TObject *Sender)
{
	if( m_fpText ){
		fclose(m_fpText);
        m_fpText = NULL;
    }
	else {
		TOpenDialog *pBox = new TOpenDialog(this);
		pBox->Options >> ofCreatePrompt;
		pBox->Options << ofFileMustExist;
		if( sys.m_MsgEng ){
			pBox->Title = "Send text file";
			pBox->Filter = "Text Files(*.txt)|*.txt|";
		}
		else {
			pBox->Title = "ÉeÉLÉXÉgÉtÉ@ÉCÉãÇÃëóêM";
			pBox->Filter = "ÉeÉLÉXÉgÉtÉ@ÉCÉã(*.txt)|*.txt|";
		}
		pBox->FileName = "";
		pBox->DefaultExt = "txt";
		pBox->InitialDir = sys.m_TextDir;
		DettachFocus();
		OnWave();
		if( pBox->Execute() == TRUE ){
			SetDirName(sys.m_TextDir, AnsiString(pBox->FileName).c_str());	//JA7UDE 0428
			OnWave();
			m_fpText = fopen(AnsiString(pBox->FileName).c_str(), "rb");	//JA7UDE 0428
		    if( !m_TX ) ToTX();
		}
		delete pBox;
		AttachFocus();
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SetFFTWidth(int fw)
{
//	int fo = m_FFTB + m_FFTW/2;
	if( SBWater->Down ) InitWater(iniwMETRIC);
	int fo = UdRxCarrier->Position;
	switch(fw){
		case 500:
			SBFFT500->Down = TRUE;
            break;
        case 1000:
			SBFFT1K->Down = TRUE;
            break;
        case 2000:
			SBFFT2K->Down = TRUE;
            break;
        default:
			SBFFT3K->Down = TRUE;
			fw = 3000;
            break;
    }
    m_FFTW = fw;
    m_FFTWindow = m_FFTW * FFT_SIZE / m_FFTSampFreq;
	CalcFFTCenter(fo);
    Draw(TRUE);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SBFFT500Click(TObject *Sender)
{
	int fw;
	if( SBFFT3K->Down ){
		fw = 3000;
    }
    else if( SBFFT2K->Down ){
		fw = 2000;
    }
    else if( SBFFT500->Down ){
		fw = 500;
    }
	else {
		fw = 1000;
    }
    SetFFTWidth(fw);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::UdATCClick(TObject *Sender,
      TUDBtnType Button)
{
	if( m_fDisEvent ) return;

	int d;
	sscanf(AnsiString(EATC->Text).c_str(), "%d", &d);	//JA7UDE 0428
	d = d - (d % 200);
	if( Button == Comctrls::btNext ){
		d += 200;
	}
	else {
		d -= 200;
	}
	if( d < -m_ATCLimit ) d = -m_ATCLimit;
    if( d > m_ATCLimit ) d = m_ATCLimit;
	m_RxSet[0].m_pDem->m_Decode.SetTmg(d);
    m_RxSet[0].m_pDem->m_DecPSK.SetTmg(d);
    EATC->Text = d;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SBAFCClick(TObject *Sender)
{
	if( m_TX != txINTERNAL ){
		CDEMFSK *pDem = m_RxSet[0].m_pDem;
		m_RxSet[0].m_fAFC = pDem->m_fAFC = SBAFC->Down;
		if( pDem->m_fAFC && m_RxSet[0].IsMFSK() ){
			m_RxSet[0].m_AFCFQ = pDem->m_CarrierFreq;
			m_RxSet[0].m_AvgAFC.Reset(pDem->m_CarrierFreq);
			pDem->m_RxFreq = pDem->m_CarrierFreq;
			if( m_RxSet[0].m_AFCTimerMFSK < MFSKAFC_1stATACK ){
				m_RxSet[0].m_AFCTimerMFSK = MFSKAFC_1stATACK;
			}
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SBATCClick(TObject *Sender)
{
	UpdateUI();
	if( m_TX == txINTERNAL ) return;

	m_RxSet[0].m_pDem->m_Decode.m_fATC = SBATC->Down;
	m_RxSet[0].m_pDem->m_DecPSK.m_bATC = SBATC->Down;
	if( !SBATC->Down ){
		int atctmg;
		if( sscanf(AnsiString(EATC->Text).c_str(), "%d", &atctmg) == 1 ){	//JA7UDE 0428
			m_RxSet[0].m_pDem->m_Decode.SetTmg(atctmg);
			m_RxSet[0].m_pDem->m_DecPSK.SetTmg(atctmg);
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::PBoxFFTMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	DeleteMacroTimer();
	m_MouseDown = FALSE;
	if( SBFFT->Down || SBWater->Down ){
		double fq = m_FFTB + X * m_FFTW / m_fftXW;
		if( fq < MIN_CARRIER ) fq = MIN_CARRIER;
        if( fq > sys.m_MaxCarrier ) fq = sys.m_MaxCarrier;
		if( Button == mbRight ){
			m_RightX = X;
			m_RightFreq = fq + 0.5;
			RECT rc;
			::GetWindowRect(PFFT->Handle, &rc);
		    rc.left += X;
			if( !m_MouseNotch && (!m_fSubWindow || !m_MouseSubChannel) ){
				m_fftMX = ((fq - m_FFTB) * m_fftXW / m_FFTW) + 0.5;
				PBoxFFTPaint(NULL);
		    }
			PupSpec->Popup(rc.left, rc.bottom);
		    m_fftMX = 0;
			PBoxFFTPaint(NULL);
        }
        else if( Button == mbLeft ){
			if( m_MouseSubChannel || m_MouseNotch ){
				m_MouseDown = TRUE;
            }
			else if( m_TX != txINTERNAL ){
				m_RxSet[0].m_pDem->m_Decode.Reset();
                m_RxSet[0].m_pDem->ResetMeasMFSK();
                m_RxSet[0].m_PeakSig = 0;
                m_RxSet[0].m_AFCTimerMFSK = (SBFFT500->Down || SBFFT1K->Down) ? MFSKAFC_MAX : 0;
				if( !SBFFT500->Down ){
					fq = GetSignalFreq(fq, SBFFT3K->Down ? 50 : 32, &m_RxSet[0]);
                }
				SetRxFreq(fq);
				m_MouseDown = TRUE;
            }
        }
	}
    else {
		if( Button == mbRight ){
			m_fftMX = 0;
        }
        else {
			m_fftMX = X;
        }
		PBoxFFTPaint(NULL);
    }
}
//---------------------------------------------------------------------------

void __fastcall TMainVARI::PBoxLevelMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if( Button == mbLeft ){
		int sq = (m_levelYW - Y) * LEVELMAX / m_levelYW;
		m_RxSet[0].m_SQLevel = ((sq + 5)/ 10) * 10;
        DrawLevel(TRUE);
        m_MouseDown = TRUE;
    }
    else if( Button == mbRight ){
		RECT rc;
		::GetWindowRect(PCLevel->Handle, &rc);
		PupSQ->PopupComponent = this;
		PupSQ->Popup(rc.left + X, rc.top + Y);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::PBoxLevelMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
	if( m_MouseDown ){
		X = (m_levelYW - Y) * LEVELMAX / m_levelYW;
        if( (X >= 0) && (X <= LEVELMAX) ){
			m_RxSet[0].m_SQLevel = X;
	        DrawLevel(TRUE);
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::PBoxLevelMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	m_MouseDown = FALSE;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::FormResize(TObject *Sender)
{
	if( !m_pBitmapFFT ) return;

	if( (ClientWidth < 600) || (ClientHeight < 400) ){
		if( ClientWidth < 600 ) ClientWidth = 600;
        if( ClientHeight < 400 ) ClientHeight = 400;
        return;
    }
    PFFT->Font->Charset = sys.m_FontCharset;
    PFFT->Font->Height = -12;
	int x = SBFFT->Left + SBFFT->Width + PCLevel->Width + 2;
	PFFT->Width = ClientWidth - x;
	if( PBoxFFT->Align != alClient ) CreateMacExButton();
    m_fftXW = PBoxFFT->Width;
    if( m_fftXW != m_pBitmapFFT->Width ){
		m_pBitmapFFT->Width = m_fftXW;
        DrawWater(FALSE, TRUE);
   	}
	UdMac->Left = PCMac->Width - UdMac->Width;
	CreateMacButton();
//	StatusBar->SizeGrip = (WindowState == wsNormal);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KFClick(TObject *Sender)
{
	KFS->Checked = m_fpText != NULL;
    KFES->Enabled = m_WaveFile.m_mode;
    KFRS->Enabled = m_WaveFile.m_mode != 2;
    KFWS->Enabled = m_WaveFile.m_mode != 1;
    KFWST->Enabled = m_WaveFile.m_mode != 1;
    if( m_WaveFile.m_mode != 2 ){
		KFWS->Checked = FALSE;
        KFWST->Checked = FALSE;
    }
    KFLF->Enabled = Log.IsEdit();
    KFL->Checked = m_Dump.IsLogging();
    OnMenuProc(KF, "&File");
}
//---------------------------------------------------------------------------

void __fastcall TMainVARI::CBRXCarrierDropDown(TObject *Sender)
{
	m_fDrop = TRUE;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::CBRXCarrierKeyPress(TObject *Sender, char &Key)
{
	m_fDrop = FALSE;
    if( Key == VK_RETURN ){
		Key = 0;
        if( Sender == HisCall ){
			FindCall();
            UpdateUI();
        }
        else if( Sender == CBSpeed ){
			double speed;
			if( m_RxSet[0].IsMFSK() ){
            	speed = m_RxSet[0].m_pDem->m_MFSK_SPEED;
            }
            else {
				speed = m_RxSet[0].m_Speed;
            }
			SetCBSpeed();
   	        SetSpeedInfo(speed);
        }
		SetTXFocus();
    }
    else if( Sender == HisCall ){
		Key = char(toupper(Key));
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SetMacButtonMax(void)
{
    int m = MACBUTTONXW * m_MacButtonVW;
    UdMac->Max = short((MACBUTTONMAX-1) / m);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::CreateMacButton(void)
{
	int xw = PCMac->Width - UdMac->Width - 6;
    xw /= MACBUTTONXW;
    int xa = xw * MACBUTTONXW;
    xa = ((PCMac->Width - UdMac->Width) - xa) / 2;
	int yw = PCMac->Height / m_MacButtonVW;

	int low = UdMac->Position * (MACBUTTONXW*m_MacButtonVW);
    int high = low + (MACBUTTONXW*m_MacButtonVW);
	MACBUTTON *pList = m_tMacButton;
    for( int i = 0; i < MACBUTTONMAX; i++, pList++ ){
    	if( (i < low) || (i >= high) ){
			if( pList->pButton ){
				PCMac->RemoveControl(pList->pButton);
                delete pList->pButton;
                pList->pButton = NULL;
            }
        }
		else {
			int n = i - low;
			int y = (n / MACBUTTONXW) * yw;
            int xm = n % MACBUTTONXW;
    	    int x = xm * xw;
            if( xm >= 4 ) x+=xa;
            if( xm >= 8 ) x+=xa;
			TSpeedButton *pButton = pList->pButton;
        	if( !pButton ){
				pButton = new TSpeedButton(PCMac);
				PCMac->InsertControl(pButton);
            	pButton->Parent = PCMac;
				pButton->OnClick = OnMacButtonClick;
                pButton->OnMouseDown = OnMacButtonDown;
                pButton->GroupIndex = 1;
                pButton->AllowAllUp = TRUE;
				char bf[16];
                sprintf(bf, "%%M%u", i);
                pButton->Hint = bf;
                pButton->Down = (i == m_CurrentMacro) && (m_ReqMacroTimer || m_pMacroTimer);
                pList->pButton = pButton;
        	}
            else {
				pButton->Font->Name = sys.m_FontName;
                pButton->Font->Charset = sys.m_FontCharset;
            }
           	pButton->Caption = ConvAndChar(m_TextBuff, pList->Name.c_str());
            pButton->SetBounds(x, y, xw, yw);
			pButton->Font->Height = -(yw-4);
            pButton->Font->Color = pList->Text.IsEmpty() ? clGrayText : pList->Color;
			TFontStyles fs = Code2FontStyle(pList->Style);
            pButton->Font->Style = fs;
    	}
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::UdMacClick(TObject *Sender, TUDBtnType Button)
{
	CreateMacButton();
}
//---------------------------------------------------------------------------
int __fastcall TMainVARI::GetMacButtonNo(TSpeedButton *pButton)
{
	MACBUTTON *pList = m_tMacButton;
    int i;
    for( i = 0; i < MACBUTTONALL; i++, pList++ ){
		if( pButton == pList->pButton ) return i;
    }
    return -1;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::OnMacButtonClick(TObject *Sender)
{
	TSpeedButton *pButton = (TSpeedButton *)Sender;
	MACBUTTON *pList = m_tMacButton;
    int i;
    for( i = 0; i < MACBUTTONALL; i++, pList++ ){
		if( pButton == pList->pButton ){
			if( pButton->Down ){
				m_fMacroRepeat = FALSE;
   	           	SendButton(i);
            }
            else {
				DeleteMacroTimer();
            }
           	break;
    	}
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::OnMacButtonDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	TSpeedButton *pButton = (TSpeedButton *)Sender;

	MACBUTTON *pList = m_tMacButton;
    int i;
    for( i = 0; i < MACBUTTONALL; i++, pList++ ){
		if( pButton == pList->pButton ){
			if( Button == mbRight ){
				TMacEditDlg *pBox = new TMacEditDlg(this);
				DettachFocus();
				if( pBox->Execute(pList, i) ){
		           	pButton->Caption = ConvAndChar(m_TextBuff, pList->Name.c_str());
                    pButton->Font->Color = pList->Text.IsEmpty() ? clGrayText : pList->Color;
					TFontStyles fs = Code2FontStyle(pList->Style);
        		    pButton->Font->Style = fs;
                }
                delete pBox;
                AttachFocus();
            }
            return;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::PupRXPopup(TObject *Sender)
{
	int n = GetPopupIndex(PupRX->PopupComponent);
	if( n ){
		KRXINV->Visible = m_RxSet[n].IsRTTY();
    }
    else {
		KRXINV->Visible = IsRTTY();
    }
    KRXN->Visible = KRXINV->Visible;
	OnMenuProc(PupRX->Items, "PopRX");
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KRXINVClick(TObject *Sender)
{
	int n = GetPopupIndex(PupRX->PopupComponent);
	CDump *pDump;
    if( n ){
		pDump = &m_RxSet[n].m_pView->m_Dump;
    }
    else {
		pDump = &m_Dump;
    }
	pDump->SwapLTR();
}
//---------------------------------------------------------------------------
static void __fastcall AdjustCallsign(AnsiString &as)
{
	AnsiString cs;

    LPCSTR p;
	int nl = 0;
    int nu = 0;
    for(p = as.c_str(); *p; p++ ){
		if( isupper(*p) ) nu++;
   	    if( islower(*p) ) nl++;
    }
	for( p = as.c_str(); *p; p++ ){
		if( isdigit(*p) ) break;
		if( nl > nu ){
			if( islower(*p) ) break;
        }
        else {
			if( isupper(*p) ) break;
        }
	}
    for( ; *p; p++ ){
		if( *p != '/' ){
			if( nl > nu ){
				if( isupper(*p) ) break;
       	    }
           	else {
				if( islower(*p) ) break;
            }
        }
       	cs += *p;
    }
	if( IsCall(cs.c_str()) ){
	    as = cs;
    }
   	jstrupr(as.c_str());
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KRXCALLClick(TObject *Sender)
{
	if( Sender == NULL ){
		AnsiString as = m_StrSel;
		AdjustCallsign(as);
		HisCall->Text = as;		//    m_UStrSel;
    }
    else {
		HisCall->Text = m_UStrSel;
    }
	FindCall();
	UpdateUI();
    SetTXFocus();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KRXMYClick(TObject *Sender)
{
	LPCSTR p = m_UStrSel.c_str();
	if( PCRX->Font->Charset == SHIFTJIS_CHARSET ){
		LPSTR t = m_TextBuff;
		for( ; *p; p++ ){
			if( m_RxSet[0].m_MBCS.IsLead((const unsigned char *)p) ){
				int c = *p & 0x000000ff;
                c = c << 8; p++;
                c |= (*p & 0x00ff);
                if( (c >= 0x824f) && (c <= 0x8258) ){
                    *t++ = BYTE(c + 0x30 - 0x824f);
                }
                else {
					*t++ = BYTE(c >> 8);
                    *t++ = *p;
                }
            }
            else {
				*t++ = *p;
            }
        }
        *t = 0;
        p = m_TextBuff;
    }
	MyRST->Text = p;
    SetTXFocus();
}
//---------------------------------------------------------------------------

void __fastcall TMainVARI::KRXQTHClick(TObject *Sender)
{
	HisQTH->Text = m_StrSel;
    SetTXFocus();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KRXNAMEClick(TObject *Sender)
{
	HisName->Text = m_StrSel;
    SetTXFocus();
}
//---------------------------------------------------------------------------
static BOOL FindDlmStr(LPCSTR v, LPCSTR s)
{
	LPSTR pBF = StrDupe(v);
	BOOL f = FALSE;
	LPSTR p = pBF;
    LPSTR t;
    while(*p){
		p = StrDlm(t, p, -1);
        if( *t && !strcmpi(t, s) ){
			f = TRUE;
            break;
        }
    }
    delete pBF;
    return f;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KRXADDMYClick(TObject *Sender)
{
	AnsiString as = MyRST->Text;
    as += m_StrSel;
    MyRST->Text = as;
    SetTXFocus();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KRXADDNAMEClick(TObject *Sender)
{
	if( !FindDlmStr(AnsiString(HisName->Text).c_str(), m_StrSel.c_str()) ){	//JA7UDE 0428
		AnsiString as = HisName->Text;
		if( !as.IsEmpty() ) as += char(',');
		as += m_StrSel;
		HisName->Text = as;
	}
	SetTXFocus();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KRXADDNOTEClick(TObject *Sender)
{
	if( !FindDlmStr(AnsiString(EditNote->Text).c_str(), m_StrSel.c_str()) ){	//JA7UDE 0428
		AnsiString as = EditNote->Text;
		if( !as.IsEmpty() ) as += char(',');
        as += m_StrSel;
        EditNote->Text = as;
    }
    SetTXFocus();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KRXADDQSLClick(TObject *Sender)
{
	if( !FindDlmStr(AnsiString(EditQSL->Text).c_str(), m_StrSel.c_str()) ){	//JA7UDE 0428
		AnsiString as = EditQSL->Text;
		if( !as.IsEmpty() ) as += char(',');
        as += m_StrSel;
        EditQSL->Text = as;
    }
    SetTXFocus();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KRXNOTEClick(TObject *Sender)
{
	EditNote->Text = m_StrSel;
    SetTXFocus();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KRXCopyClick(TObject *Sender)
{
	Clipboard()->AsText = m_StrSel;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::PBoxRXMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	DeleteMacroTimer();
	BOOL f = m_Dump.ClearAllSelect(FALSE);
	int r = m_Dump.GetWindowText(m_StrSel, X, Y);
	if( f || r ) m_Dump.Paint();
    if( r ){
		m_UStrSel = m_StrSel.c_str();
		jstrupr(m_UStrSel.c_str());
		RECT rc;
		::GetWindowRect(PCRX->Handle, &rc);
        rc.left += X; rc.top += Y+8;
		if( Button == mbLeft ){
			if( r == 1 ){		// ASCII
				if( IsCall(m_UStrSel.c_str()) ){
					KRXCALLClick(NULL);
	    	    }
    	    	else if( IsRST(m_UStrSel.c_str()) ){
					KRXMYClick(NULL);
		        }
    	        else {
					PupRX->PopupComponent = this;
					PupRX->Popup(rc.left, rc.top);
            	}
	        }
    	    else {				// Shift-JIS
				PupRX->PopupComponent = this;
				PupRX->Popup(rc.left, rc.top);
	        }
			DoEvent(macOnClick);
        }
        else if( Button == mbRight ){
			PupRX->PopupComponent = this;
			PupRX->Popup(rc.left, rc.top);
        }
    }
	else if( Button == mbRight ){
		RECT rc;
		::GetWindowRect(PCRX->Handle, &rc);
        rc.left += X; rc.top += Y+8;
		PupRXW->PopupComponent = StatusBar;
		PupRXW->Popup(rc.left, rc.top);
    }
    else {
		SetTXFocus();
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
	TShiftState sc1, sc2, sa1, sa2, ss1, ss2;

	WORD nKey = WORD(Key & 0x00ff);
	sc1 << ssCtrl;
	sc2 << ssCtrl;
	sc1 *= Shift;

	sa1 << ssAlt;
	sa2 << ssAlt;
	sa1 *= Shift;

	ss1 << ssShift;
	ss2 << ssShift;
	ss1 *= Shift;

	if( sc1 == sc2  ){		// Ctrl+Any
		nKey |= 0x0100;
	}
	else if( sa1 == sa2 ){	// Alt + Any
		nKey |= 0x0200;
	}
	else if( ss1 == ss2 ){	// Shift + Any
		nKey |= 0x0400;
	}

	if( nKey == sys.m_DefKey[kkTX] ){
		if( SBTX->Enabled ){
			SBTX->Down = SBTX->Down ? FALSE : TRUE;
			SBTXClick(NULL);
        }
		SetTXFocus();
		Key = 0;
    }
    else if( nKey == sys.m_DefKey[kkTXOFF] ){
		SBTXOFFClick(NULL);
		SetTXFocus();
		Key = 0;
    }
	else {
		CDump *pEdit = &m_Edit[m_CurrentEdit];
		switch(nKey){
			case VK_DELETE:
            	if( ActiveControl == PCTX ){
					pEdit->DeleteChar();
                }
                break;
			case VK_HOME:
			case VK_END:
            case VK_END + 0x0100:
            case VK_HOME + 0x0100:
            	if( ActiveControl == PCTX ){
					pEdit->MoveCursor(nKey);
                }
                break;
	        default:
    	    	if( (Key >= VK_F1) && (Key <= VK_F12) ){
					int m = Key - VK_F1;
					Key = 0;
            	    if( nKey & 0x0400 ){		// Shift
						m += 12;
	                }
    	            else if( nKey & 0x0200 ){	// Alt
						if( m_MacButtonVW < 3 ) break;
                        m += 24;
            	    }
    	            else if( nKey & 0x0100 ){	// Ctrl
						if( m_MacButtonVW < 4 ) break;
						m += 36;
					}
					m_fMacroRepeat = FALSE;
   	            	SendButton(m + UdMac->Position * (MACBUTTONXW*m_MacButtonVW));
	            }
				break;
	    }
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SBQSOClick(TObject *Sender)
{
	UpdateLogData();
    UpdateLogMode();
	SYSTEMTIME	now;
	GetUTC(&now);
	int Year = now.wYear % 100;
	int Month = now.wMonth;
	int Day = now.wDay;
	int Hour = now.wHour;
	UTCtoJST(Year, Month, Day, Hour);

//	Log.m_sd.cq = m_Running ? 'A' : 'C';
	if( SBQSO->Down ){		// Start QSO
		m_QSOStart = SystemTime2GPS(&now);
		AddCall(AnsiString(HisCall->Text).c_str());	//JA7UDE 0428
		LogLink.SetTime(&now, 0);
		Log.m_sd.year = char(Year % 100);
		Log.m_sd.date = WORD(Month * 100 + Day);
		Log.m_sd.btime = WORD((Hour * 60 + now.wMinute) * 30 + now.wSecond/2);
		if( !Log.m_sd.btime ) Log.m_sd.btime++;
		CWaitCursor w;
		if( Log.FindSet(&Log.m_Find, Log.m_sd.call) ){
			OnWave();
			SDMMLOG	sd;
			Log.GetData(&sd, Log.m_Find.pFindTbl[0]);	// àÍî‘ç≈êVÇÃÉfÅ[É^
			if( !Log.m_sd.name[0] && Log.m_LogSet.m_CopyName ) strcpy(Log.m_sd.name, sd.name);
			if( !Log.m_sd.qth[0] && Log.m_LogSet.m_CopyQTH ) strcpy(Log.m_sd.qth, sd.qth);
			if( !Log.m_sd.rem[0] && Log.m_LogSet.m_CopyREM ) strcpy(Log.m_sd.rem, sd.rem);
			if( !Log.m_sd.qsl[0] && Log.m_LogSet.m_CopyQSL ) strcpy(Log.m_sd.qsl, sd.qsl);
		}
		Log.CopyAF();
		if( Log.m_sd.call[0] ){
			LPCSTR pCC = ClipCC(Log.m_sd.call);
			Log.SetOptStr(0, &Log.m_sd, Cty.GetCountry(pCC));
			Log.SetOptStr(1, &Log.m_sd, Cty.GetCont(pCC));
		}
		UpdateTextData();
		if( Log.PutData(&Log.m_sd, Log.m_CurNo) == FALSE ){
			SBQSO->Down = FALSE;
		}

		if( Log.m_Find.m_FindCmp1Max ){
			switch(Log.m_LogSet.m_CheckBand){
				case 1:
					if( Log.FindSameBand(FALSE) ) m_Dupe = 1;
					break;
				case 2:
					if( Log.FindSameBand(TRUE) ) m_Dupe = 1;
					break;
				default:
					m_Dupe = 1;
					break;
			}
		}
        else {
			m_Dupe = 0;
        }
		Log.m_Find.Ins(Log.m_CurNo);
		Log.m_CurChg = 0;
		DoEvent(macOnQSO);
		LogLink.Write(&Log.m_sd, 1);
	}
	else {					// Finish QSO
		LogLink.SetTime(&now, 1);
		Log.m_sd.etime = WORD((Hour * 60 + now.wMinute) * 30 + now.wSecond/2);
		if( !Log.m_sd.etime ) Log.m_sd.etime++;
		if( !Log.m_sd.ur[0] ){
			strcpy(Log.m_sd.ur, "599");
		}
		if( !Log.m_sd.my[0] ){
			strcpy(Log.m_sd.my, "599");
		}
		Log.PutData(&Log.m_sd, Log.m_CurNo);
		LogLink.Write(&Log.m_sd, 2);
		DoEvent(macOnQSO);

		memcpy(&Log.m_asd, &Log.m_sd, sizeof(Log.m_asd));
		Log.m_CurNo++;
		Log.m_CurChg = 0;
		Log.m_Find.Clear();
		Log.InitCur();
		UpdateTextData();
		AutoLogSave();
	}
    UpdateCallsign();
	UpdateUI();
	SetTXFocus();
}
//---------------------------------------------------------------------------

void __fastcall TMainVARI::KVClick(TObject *Sender)
{
	switch(m_FFTVType){
		case 0:
			KVF1->Checked = TRUE;
        	break;
        case 1:
			KVF2->Checked = TRUE;
        	break;
        default:
			KVF3->Checked = TRUE;
        	break;
    }
    switch(m_FFTSmooth){
		case 3:
			KVFS3->Checked = TRUE;
            break;
        case 4:
			KVFS4->Checked = TRUE;
            break;
        default:
			KVFS2->Checked = TRUE;
            break;
    }
    switch(m_MacButtonVW){
		case 2:
			KVM2->Checked = TRUE;
        	break;
        case 4:
			KVM4->Checked = TRUE;
        	break;
        default:
			KVM3->Checked = TRUE;
        	break;
    }
    KVSP->Enabled = m_pPlayBox != NULL;
    KVSP->Checked = (m_pPlayBox != NULL) && m_pPlayBox->Active;
    for( int i = 1; i < RXMAX; i++ ){
		TMenuItem *pm = KVS->Items[i-1];
        pm->Checked = m_RxSet[i].IsActive();
    }
    switch(m_ScaleAsRigFreq){
		case 1:
			KVSFR->Checked = TRUE;
			break;
        default:
			KVSFT->Checked = TRUE;
        	break;
    }
    KVSD->Checked = m_ScaleDetails;
	KVMX->Checked = PBoxFFT->Align != alClient;
    OnMenuProc(KV, "&View");
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KVF1Click(TObject *Sender)
{
	int vtype = m_FFTVType;
	if( Sender == KVF1 ){
		m_FFTVType = 0;
		m_FFT.m_FFTGain = 0;
    }
    else if( Sender == KVF2 ){
		m_FFTVType = 1;
		m_FFT.m_FFTGain = 0;
    }
    else {
		m_FFTVType = 2;
		m_FFT.m_FFTGain = 1;
    }
	if( vtype == m_FFTVType ) return;

    InitWater(iniwBOTH);
	for( int i = 0; i < RXMAX; i++ ){
	    m_RxSet[i].m_StgFFT.Timer = SAMPFREQ/m_BufferSize;
	    m_RxSet[i].m_AFCTimerW = SAMPFREQ / m_BufferSize;
    }
    DoEvent(macOnFFTScale);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SBNETClick(TObject *Sender)
{
	UpdateUI();
    if( SBNET->Down && !m_TX ){
		SetTxFreq(UdRxCarrier->Position);
    }
    m_ReqAutoNET = FALSE;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KVFS2Click(TObject *Sender)
{
	m_FFTSmooth = 2;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KVFS3Click(TObject *Sender)
{
	m_FFTSmooth = 3;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KVFS4Click(TObject *Sender)
{
	m_FFTSmooth = 4;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KFXClick(TObject *Sender)
{
	Close();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KEClick(TObject *Sender)
{
	CDump *pEdit = &m_Edit[m_CurrentEdit];
	BOOL f = pEdit->CanEdit();
    KEX->Enabled = pEdit->IsSelText() && f;
	KECP->Enabled = pEdit->IsSelText();
	KEP->Enabled = ::IsClipboardFormatAvailable(CF_TEXT) && f;
    OnMenuProc(KE, "&Edit");
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::PupTXPopup(TObject *Sender)
{
	if( KE->Count != PupTX->Items->Count ) return;
	KEClick(NULL);
    for( int i = 0; i < KE->Count; i++ ){
		PupTX->Items->Items[i]->Enabled = KE->Items[i]->Enabled;
        for( int j = 0; j < KE->Items[i]->Count; j++ ){
			PupTX->Items->Items[i]->Items[j]->Checked = KE->Items[i]->Items[j]->Checked;
        }
    }
	OnMenuProc(PupTX->Items, "PopTX");
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KEPClick(TObject *Sender)
{
	AnsiString as = Clipboard()->AsText.c_str();
	LPCSTR p = as.c_str();
	for( ; *p; p++ ){
		m_Edit[m_CurrentEdit].PutKey(*p, 1);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SBarRXChange(TObject *Sender)
{
	m_Dump.OnScrollBarChange();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SBarTXChange(TObject *Sender)
{
	m_Edit[m_CurrentEdit].OnScrollBarChange();
}
//---------------------------------------------------------------------------

void __fastcall TMainVARI::SBBPFWMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if( Button == mbRight ){
		CFIR2	fir;
		int fo = UdRxCarrier->Position;
		switch(CBMode->ItemIndex){
			case MODE_N_BPSK:
			case MODE_BPSK:
			case MODE_qpsk_L:
			case MODE_qpsk_U:
        	case MODE_GMSK:
				fir.Create(GetBPFTaps(Sender), ffBPF, DEMSAMPFREQ, fo-m_RxSet[0].m_Speed, fo+m_RxSet[0].m_Speed, 60, 1.0);
                break;
        	case MODE_FSK:
				fir.Create(GetBPFTaps(Sender), ffBPF, DEMSAMPFREQ, fo-m_RxSet[0].m_Speed*2, fo+m_RxSet[0].m_Speed*2, 60, 1.0);
                break;
            case MODE_FSKW:
            case MODE_RTTY:
            case MODE_U_RTTY:
				{
					int taps = GetBPFTaps(Sender);
					double hw = m_RxSet[0].m_pDem->m_RTTYShift*0.5;
                    double bw = GetRTTYBW(taps);
					fir.Create(taps, ffBPF, DEMSAMPFREQ, fo-hw-bw, fo+hw+bw, 60, 1.0);
                }
            	break;
            case MODE_mfsk_L:
				{
					int taps = GetBPFTaps(Sender);
                    double bw = GetMFSKBW(taps);
					if( sys.m_MFSK_Center ){
						fir.Create(taps, ffBPF, DEMSAMPFREQ, fo-m_RxSet[0].m_pDem->m_MFSK_BW/2-bw, fo+m_RxSet[0].m_pDem->m_MFSK_BW/2+bw, 60, 1.0);
                    }
                    else {
						fir.Create(taps, ffBPF, DEMSAMPFREQ, fo-m_RxSet[0].m_pDem->m_MFSK_BW-bw, fo+bw, 60, 1.0);
                    }
                }
            	break;
            case MODE_mfsk_U:
				{
					int taps = GetBPFTaps(Sender);
                    double bw = GetMFSKBW(taps);
					if( sys.m_MFSK_Center ){
						fir.Create(taps, ffBPF, DEMSAMPFREQ, fo-m_RxSet[0].m_pDem->m_MFSK_BW/2-bw, fo+m_RxSet[0].m_pDem->m_MFSK_BW/2+bw, 60, 1.0);
                    }
                    else {
						fir.Create(taps, ffBPF, DEMSAMPFREQ, fo-bw, fo+m_RxSet[0].m_pDem->m_MFSK_BW+bw, 60, 1.0);
                    }
                }
            	break;
        }
		DettachFocus();
		TFreqDispDlg *pBox = new TFreqDispDlg(this);
		pBox->Execute(&fir, 3000, DEMSAMPFREQ);
        delete pBox;
        AttachFocus();
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KOVOClick(TObject *Sender)
{
	HWND hWnd = ::FindWindow("Volume Control", NULL);
	if( hWnd != NULL ){
		::PostMessage(hWnd, WM_CLOSE, 0, 0);
		::Sleep(200);
        OnWave();
	}

	char cmd[128];
    if( sys.m_WinVista ){
		if( Sender != KOVO ){
			strcpy(cmd, "control.exe mmsys.cpl,,1");
		}
        else {
			strcpy(cmd, "sndvol.exe");
		}
    }
    else {
		strcpy(cmd, "SNDVOL32.EXE");
		if( sys.m_WinNT && (Sender != KOVO) ) strcat(cmd, " /R");
    }

	WinExec(cmd, SW_SHOW);
	if( !sys.m_WinNT && (Sender != KOVO) ){
		CWaitCursor w;
		int i;
		for( i = 0; i < 30; i++ ){
			OnWave();
			::Sleep(100);
			hWnd = ::FindWindow("Volume Control", NULL);
			if( hWnd != NULL ) break;
		}
		if( i < 30 ){
			::SetForegroundWindow(hWnd);
			::Sleep(100);
            OnWave();
			const short _tt[]={
				VK_MENU, 'P', 'P'|0x8000, VK_MENU|0x8000,
				'R', 'R'|0x8000, VK_TAB, VK_TAB|0x8000,
				VK_DOWN, VK_DOWN|0x8000, VK_RETURN, VK_RETURN|0x8000,
				0
			};
			KeyEvent(_tt);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KOOClick(TObject *Sender)
{
	TOptDlgBox *pBox = new TOptDlgBox(this);
    DettachFocus();
    if( pBox->Execute(DWORD(Sender)) ){
		if( pBox->m_fComChange ) OpenCom();
		if( RADIO.change ) OpenRadio();
		if( pBox->m_fLangChange ){
        	SetSystemFont();
	        CreateMacButton();
			if( PBoxFFT->Align != alClient ) CreateMacExButton();
        }
		if( sys.m_fPlayBack ){
			if( !m_PlayBack.IsActive() ) m_PlayBack.Init(m_BufferSize, SAMPBASE);
        }
        else {
			m_PlayBack.Delete();
        }
		OnWave();
		SetTXCaption();
        UpdateUI();
        OnWave();
		UpdateShowCtrl();
		PCRX->Color = m_Dump.m_Color[0].c;
		PCTX->Color = m_Edit[0].m_Color[0].c;
        m_Dump.Paint();
        m_Edit[m_CurrentEdit].Paint();
		CreateWaterColors();
		InitWater(iniwLIMIT);
		UpdateMacroOnTimer();
    }
	delete pBox;
	OnWave();
	UpdateCharset();
    AttachFocus();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::InitCheckValKey(LPCSTR pKey, AnsiString *pAS)
{
	m_pCheckKey = pKey;
    m_pCheckBuff = pAS;
}
//---------------------------------------------------------------------------
BOOL __fastcall TMainVARI::CheckKey(LPCSTR pTemplate)
{
	if( *m_pCheckKey != *pTemplate ) return FALSE;
	return !strcmp(m_pCheckKey, pTemplate);
}
//---------------------------------------------------------------------------
LPCSTR __fastcall TMainVARI::CheckValKey(LPCSTR pTemplate)
{
	return CheckValKey(m_pCheckKey, pTemplate);
}
//---------------------------------------------------------------------------
LPCSTR __fastcall TMainVARI::CheckValKey(LPCSTR pKey, LPCSTR pTemplate)
{
	if( *pKey != *pTemplate ) return NULL;

	BOOL	f = TRUE;
	LPCSTR pBase = pKey;
    LPCSTR pEnd = NULL;
    int seq = 0;
	for( ; *pKey && *pTemplate; pKey++, pTemplate++ ){
		switch(*pTemplate){
			case '*':
				pBase = pKey;
				for( ; *pKey; pKey++ ){
					if( (*pKey == '<') && (*(pKey+1) == '%') ){
						seq++;
                    }
					else if( *pKey == '>' ){
						if( seq ){
							seq--;
                        }
                        else {
							pEnd = pKey;
                        	break;
                        }
                    }
                }
                pTemplate++;
            	break;
            default:
//				if( *pKey != *pTemplate ) f = FALSE;
				if( *pKey != *pTemplate ) return NULL;
            	break;
        }
    }
	if( *pKey != *pTemplate ) f = FALSE;
	if( f && pEnd ){
    	int len = pEnd - pBase;
        if( len < 0 ) len = 0;
        if( len >= 4095 ) len = 4095;
		LPSTR pBF = new char[len+1];
		if( len > 0 ){
			memcpy(pBF, pBase, len); pBF[len] = 0;
        }
        else {
			pBF[0] = 0;
        }
        *m_pCheckBuff = pBF;
        pBase = m_pCheckBuff->c_str();
        delete pBF;
    }
    return f ? pBase : NULL;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::MacroDate(LPSTR t, SYSTEMTIME &now)
{
	switch(Log.m_LogSet.m_DateType){
		case 2:
		case 3:
			sprintf(t, "%02u-%s-%04u", now.wDay, MONT1[now.wMonth], now.wYear);
			break;
		case 4:
			case 5:
			sprintf(t, "%s-%02u-%04u", MONT1[now.wMonth], now.wDay, now.wYear);
			break;
		default:
			sprintf(t, "%04u-%s-%02u", now.wYear, MONT1[now.wMonth], now.wDay);
			break;
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::MacroHisName(LPSTR t)
{
	strcpy(m_TextBuff, AnsiString(HisName->Text).c_str());	//JA7UDE 0428
    LPSTR pp;
    LPSTR p = m_TextBuff;
	while(*p){
	    p = StrDlm(pp, p, -1);
        BOOL f = m_RxSet[0].m_MBCS.IsLead((const unsigned char *)pp);
        if( IsRTTY() ) f = f ? FALSE : TRUE;
        if( *pp && f ){
			strcpy(t, pp);
            return;
        }
    }
	strcpy(m_TextBuff, AnsiString(HisName->Text).c_str());	//JA7UDE 0428
    p = m_TextBuff;
	while(*p){
	    p = StrDlm(pp, p, -1);
        if( *pp ){
			strcpy(t, pp);
            return;
        }
    }
	*t = 0;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::MacroGreeting(LPSTR t, LPCSTR pCall, int type)
{
	LPCSTR _tt[][3]={
		{"Hi", "Hello", "Ç‹Ç¢Ç«Ç≈Ç∑",},
		{"GM", "Good morning", "Ç®ÇÕÇÊÇ§Ç≤Ç¥Ç¢Ç‹Ç∑",},
		{"GA", "Good afternoon", "Ç±ÇÒÇ…ÇøÇÕ",},
		{"GE", "Good evening", "Ç±ÇÒÇŒÇÒÇÕ",},
    };
    strcpy(t, _tt[0][type]);
	if( !Cty.IsData() ) return;
	LPCSTR	p = ClipCC(pCall);
	if( *p ){
		int		n;
		if( (n = Cty.GetNoP(p))!=0 ){
			CTL *cp = Cty.GetCTL(n-1);
			if( cp->TD != NULL ){
				SYSTEMTIME	now;
				GetUTC(&now);

				WORD tim = WORD((now.wHour * 60 + now.wMinute) * 30 + now.wSecond/2);
				tim = AdjustRolTimeUTC(tim, *cp->TD);
				if( tim ){
					tim /= WORD(30);
					if( tim < 12*60 ){
                        n = 1;
					}
					else if( tim < 18*60 ){
						n = 2;
					}
					else {
						n = 3;
					}
                    strcpy(t, _tt[n][type]);
				}
			}
		}

	}
}
//---------------------------------------------------------------------------
static BOOL IsMMVARI(BYTE charset, int mode)
{
	BOOL r = FALSE;

	switch(mode){
		case MODE_GMSK:
        case MODE_FSK:
        case MODE_FSKW:
        case MODE_BPSK:
			switch(charset){
				case SHIFTJIS_CHARSET:
				case HANGEUL_CHARSET:
				case CHINESEBIG5_CHARSET:   //
				case 134:       // ä»ó™
                	r = TRUE;
                    break;
				default:
   			        break;
		    }
        	break;
        default:
			break;
    }
    return r;
}
//---------------------------------------------------------------------------
static LPCSTR GetVariType(BYTE charset, int mode)
{
	if( (mode == MODE_RTTY) || (mode == MODE_U_RTTY) ){
		return "BAUDOT";
    }
	else if( IsMFSK(mode) ){
    	LPCSTR p;
		switch(charset){
			case SHIFTJIS_CHARSET:
				p = "VariMFSK/JA";
				break;
			case HANGEUL_CHARSET:
		        case JOHAB_CHARSET:
       		   	p = "VariMFSK/HL";
	           	break;
			case CHINESEBIG5_CHARSET:   //
   	    	   	p = "VariMFSK/BV";
       	    	break;
			case 134:       // ä»ó™
				p = "VariMFSK/BY";
				break;
			default:
            	p = "VariMFSK";
   		        break;
	    }
        return p;
    }
	BOOL f = (mode == ((MODE_N_BPSK)) || IsQPSK(mode));
	LPCSTR p = "VariSTD";
	switch(charset){
		case SHIFTJIS_CHARSET:
			p = f ? "VariSTD/JA":"VariJA";
			break;
		case HANGEUL_CHARSET:
	        case JOHAB_CHARSET:
       	   	p = f ? "VariSTD/HL":"VariHL";
           	break;
		case CHINESEBIG5_CHARSET:   //
   	       	p = f ? "VariSTD/BV":"VariBV";
       	    break;
		case 134:       // ä»ó™
			p = f ? "VariSTD/BY":"VariBY";
			break;
		default:
   	        break;
    }
	return p;
}
//---------------------------------------------------------------------------
static int GetOnOff(LPCSTR pKey)
{
	return FindStringTable(g_tOnOff, pKey, 3);
}
//---------------------------------------------------------------------------
static int GetMacroOnOff(LPCSTR pKey)
{
	AnsiString as;
    int f = FindStringTable(g_tOnOff, MainVARI->GetMacroStr(as, pKey), 3);
	if( f < 0 ){
		f = MainVARI->GetMacroInt(pKey) != 0;
    }
    return f;
}
//---------------------------------------------------------------------------
static BOOL __fastcall StrCond(LPCSTR p, LPCSTR v, int op)
{
	int f = strcmp(p, v);
	switch(op){
		case 0:
        	return *p ? TRUE : FALSE;
		case 1:
			return !f;
        case 2:
        	return f>=0;
        case 3:
        	return f<=0;
        case 4:
        	return f>0;
        case 5:
        	return f<0;
        case 6:
        	return f;
		case 7:
        	return strstr(p, v)!=NULL;
        default:
        	return FALSE;
    }
}
//---------------------------------------------------------------------------
BOOL __fastcall ValCond(double dd, double dv, int op)
{
	switch(op){
		case 0:
        	return dd ? TRUE : FALSE;
        case 1:
        	return dd == dv;
        case 2:
        	return dd >= dv;
        case 3:
        	return dd <= dv;
        case 4:
        	return dd > dv;
        case 5:
        	return dd < dv;
        case 6:
        	return dd != dv;
        default:
        	return FALSE;
    }
}
//---------------------------------------------------------------------------
BOOL __fastcall CheckCond(LPCSTR p, LPCSTR v, int type, int &op, double &d, AnsiString &as, AnsiString &vs)
{
	int l;

    if( v ){
		if( *p != *v ) return FALSE;
		l = strlen(v);
		if( strncmp(p, v, l) ){
			for( ; *v; v++, p++ ){
				if( *v == '*' ){
					vs = "";
					int f = 1;
					for( ; *p; p++ ){
						if( *p == '(' ){
    	                	f++;
	                    }
    	                else if( *p == ')' ){
							f--;
    	                    if( !f ) break;
	                    }
    	            	vs += char(*p);
	                }
					if( *p != ')' ) return FALSE;
	                p--;
    	        }
	            else if( *p != *v ){
					return FALSE;
	            }
	        }
    	    l = 0;
	    }
    }
    else {
		int f = 0;
        l = 0;
		for( ; *p; p++ ){
			if( (*p == '<') && (*(p+1)=='%') ){
				f++;
				vs += char(*p);
            }
            else if( *p == '>' ){
				if( f ){
					f--;
					vs += char(*p);
                }
                else {
                    l = -1;
                    break;
                }
            }
            else if( !f ){
				if( (*p == '=')||(*p == '<')||(*p == '!') ){
					l = -1;
                    break;
                }
                else {
					vs += char(*p);
                }
            }
            else {
				vs += char(*p);
            }
        }
    }
	DelLastSpace(vs.c_str());

    p += l;
	p = SkipSpace(p);
	if( !strncmp(p, "==", 2) ){
		op = 1;
        p += 2;
    }
    else if( !strncmp(p, "!=", 2) ){
		op = 6;
        p += 2;
	}
    else if( !strncmp(p, ">=", 2) ){
		op = 2;
        p += 2;
    }
    else if( !strncmp(p, "<=", 2) ){
		op = 3;
        p += 2;
    }
    else if( !strncmp(p, ">>", 2) ){
		op = 7;
        p += 2;
	}
    else if( !strncmp(p, "=", 1) ){
		op = 1;
        p++;
	}
    else if( !strncmp(p, ">", 1) ){
		op = 4;
        p++;
	}
    else if( !strncmp(p, "<", 1) ){
		op = 5;
        p++;
	}
    else if( !*p ){
		op = 0;
    }
	char bf[256];
    StrCopy(bf, SkipSpace(p), sizeof(bf)-1);
    if( type ){
		if( bf[0] ){
			d = MainVARI->GetMacroDouble(bf);
        }
        else {
			d = 0;
        }
    }
    else {
		if( (bf[0]=='<') && (bf[1] == '%') ){
            MainVARI->ConvMacro_(as, bf, NULL);
		}
        else {
			as = bf;
        }
    }
    return TRUE;
}
//---------------------------------------------------------------------------
BOOL __fastcall TMainVARI::IsMBCSStr(LPCSTR p)
{
	while(*p){
		if( m_RxSet[0].m_MBCS.IsLead(*p) ) return TRUE;
        p++;
    }
    return FALSE;
}
//---------------------------------------------------------------------------
BOOL __fastcall TMainVARI::GetDataCond(LPCSTR p, int err, TSpeedButton *pButton)
{
	AnsiString vs = p;
    p = DelLastSpace(vs.c_str());

	int op;
    AnsiString as, ms;
	double d;

	BOOL r = FALSE;
    BOOL fInv = FALSE;
	p = SkipSpace(p);
    if( *p == '!' ){
		p++;
        fInv = TRUE;
    }
    if( !strcmp(p, "IsCall") ){
		r = !HisCall->Text.IsEmpty();
    }
    else if( !strcmp(p, "IsName") ){
		r = !HisName->Text.IsEmpty();
    }
    else if( !strcmp(p, "IsHisRST") ){
		r = !HisRST->Text.IsEmpty();
    }
    else if( !strcmp(p, "IsMyRST") ){
		r = !MyRST->Text.IsEmpty();
    }
    else if( !strcmp(p, "IsQTH") ){
		r = !HisQTH->Text.IsEmpty();
    }
    else if( !strcmp(p, "IsNote") ){
		r = !EditNote->Text.IsEmpty();
    }
    else if( !strcmp(p, "IsLocal") ){
		if( !HisCall->Text.IsEmpty() ){
        	char bf[32];
			LPCSTR pCC = ClipCC(AnsiString(HisCall->Text).c_str());	//JA7UDE 0428
			strcpy(bf, Cty.GetCountry(pCC));
			pCC = ClipCC(sys.m_CallSign.c_str());
			r = !strcmp(bf, Cty.GetCountry(pCC));
        }
    }
    else if( !strcmp(p, "IsAFC") ){
		r = SBAFC->Down;
	}
    else if( !strcmp(p, "IsNET") ){
		r = SBNET->Down;
	}
    else if( !strcmp(p, "IsQSO") ){
		r = SBQSO->Enabled && SBQSO->Down;
	}
    else if( !strcmp(p, "IsTX") ){
		r = SBTX->Enabled && m_TX;
	}
    else if( !strcmp(p, "IsPTT") ){
		r = m_Wave.IsPTT();
	}
    else if( !strcmp(p, "IsDupe") ){
		r = m_Dupe;
	}
    else if( !strcmp(p, "IsTone") ){
		r = SBTX->Enabled && m_TX && m_fTone;
	}
    else if( !strcmp(p, "IsTXEmpty") ){
		r = !m_Edit[m_CurrentEdit].GetCharCount(TRUE);
	}
    else if( !strcmp(p, "IsRXScroll") ){
		r = !m_Dump.IsCursorLast();
	}
    else if( !strcmp(p, "IsSQ") ){
		r = m_RxSet[0].m_SQ;
	}
    else if( !strcmp(p, "IsIME") ){
		r = FALSE;
		HWND hWnd = ActiveControl->Handle;
		if( hWnd ){
			HIMC hIMC;
			if( (hIMC = ::ImmGetContext(hWnd))!=NULL ){
				r = ::ImmGetOpenStatus(hIMC);
				::ImmReleaseContext(hWnd, hIMC);
   	        }
        }
	}
	else if( !strcmp(p, "IsUOS") ){
		r = m_RxSet[0].m_pDem->m_Decode.GetUOS();
	}
	else if( !strcmp(p, "IsRadioLSB") ){
		r = m_pRadio && m_pRadio->IsRigLSB();
	}
	else if( CheckCond(p, "IsCaptureText(*)", 1, op, d, as, ms) ){
		r = m_Dump.IsWindowText(GetMacroStr(as, ms.c_str()));
	}
    else if( !strcmp(p, "IsRepeat") ){
		r = m_fMacroRepeat;
	}
    else if( !strcmp(p, "IsOnTimer") ){
		r = m_pMacroOnTimer != NULL;
	}
	else if( !strcmp(p, "IsIdle") ){
		r = m_TX && m_ModFSK.m_Encode.m_Idle && !m_Edit[m_SendingEdit].GetCharCount(TRUE) && m_Edit[m_SendingEdit].IsCursorLast();
    }
    else if( !strcmp(p, "IsFileSending") ){
		r = m_TX && (m_fpText != NULL);
	}
    else if( !strcmp(p, "IsPlaying") ){
		r = (m_WaveFile.m_mode == 1);
	}
    else if( !strcmp(p, "IsRecording") ){
		r = (m_WaveFile.m_mode == 2);
	}
    else if( !strcmp(p, "IsEnglish") ){
		r = sys.m_MsgEng;
	}
    else if( !strcmp(p, "IsMfskCenter") ){
		r = sys.m_MFSK_Center;
	}
    else if( !strcmp(p, "IsMfskMetricSq") ){
		r = sys.m_MFSK_SQ_Metric;
	}
    else if( !strcmp(p, "Is1stCR") ){
		r = KOAI->Checked;
	}
    else if( !strcmp(p, "IsPlayBack") ){
		r = m_PlayBack.IsActive();
    }
    else if( !strcmp(p, "IsCodeMM") ){
		r = IsMMVARI(PCRX->Font->Charset, m_RxSet[0].m_Mode);
    }
	else if( CheckCond(p, "ValFreq", 1, op, d, as, ms) ){
		double dd;
		sscanf(AnsiString(LogFreq->Text).c_str(), "%lf", &dd);	//JA7UDE 0428
        r = ValCond(dd, d, op);
    }
	else if( CheckCond(p, "ValBaud", 1, op, d, as, ms) ){
        r = ValCond(m_RxSet[0].m_Speed, d, op);
    }
	else if( CheckCond(p, "ValCarrierRX", 1, op, d, as, ms) ){
        r = ValCond(UdRxCarrier->Position, d, op);
    }
	else if( CheckCond(p, "ValCarrierTX", 1, op, d, as, ms) ){
        r = ValCond(UdTxCarrier->Position, d, op);
    }
	else if( CheckCond(p, "ValPage", 1, op, d, as, ms) ){
        r = ValCond(m_CurrentEdit+1, d, op);
    }
	else if( CheckCond(p, "ValTimeLocal", 1, op, d, as, ms) ){
		::GetLocal(&m_LocalTime);
        r = ValCond(m_LocalTime.wHour*100 + m_LocalTime.wMinute, d, op);
	}
	else if( CheckCond(p, "ValTimeUTC", 1, op, d, as, ms) ){
		SYSTEMTIME	now;
		GetUTC(&now);
        r = ValCond(now.wHour*100 + now.wMinute, d, op);
	}
	else if( CheckCond(p, "ValDateLocal", 1, op, d, as, ms) ){
		::GetLocal(&m_LocalTime);
        r = ValCond(m_LocalTime.wMonth*100 + m_LocalTime.wDay, d, op);
	}
	else if( CheckCond(p, "ValDateUTC", 1, op, d, as, ms) ){
		SYSTEMTIME	now;
		GetUTC(&now);
        r = ValCond(now.wMonth*100 + now.wDay, d, op);
	}
	else if( CheckCond(p, "ValMenu", 1, op, d, as, ms) ){
        r = ValCond(m_MacroMenuNo, d, op);
	}
	else if( CheckCond(p, "ValScope", 1, op, d, as, ms) ){
		if( SBFFT->Down ){
			r = 0;
        }
        else if( SBWater->Down ){
			r = 1;
        }
        else {
			r = 2;
        }
        r = ValCond(r, d, op);
	}
	else if( CheckCond(p, "StrCall", 0, op, d, as, ms) ){
		r = StrCond(AnsiString(HisCall->Text).c_str(), as.c_str(), op);
	}
	else if( CheckCond(p, "StrMode", 0, op, d, as, ms) ){
		r = StrCond(AnsiString(CBMode->Text).c_str(), as.c_str(), op);
	}
	else if( CheckCond(p, "StrBand", 0, op, d, as, ms) ){
		r = StrCond(_BandText[Log.m_sd.band], as.c_str(), op);
	}
	else if( CheckCond(p, "StrMyRST", 0, op, d, as, ms) ){
		r = StrCond(AnsiString(MyRST->Text).c_str(), as.c_str(), op);
	}
	else if( CheckCond(p, "StrHisRST", 0, op, d, as, ms) ){
		r = StrCond(AnsiString(HisRST->Text).c_str(), as.c_str(), op);
	}
	else if( CheckCond(p, "StrNote", 0, op, d, as, ms) ){
		r = StrCond(AnsiString(EditNote->Text).c_str(), as.c_str(), op);
	}
	else if( CheckCond(p, "StrEntity", 0, op, d, as, ms) ){
		LPCSTR pCC = ClipCC(AnsiString(HisCall->Text).c_str());
		r = StrCond(Cty.GetCountry(pCC), as.c_str(), op);
	}
	else if( CheckCond(p, "StrContinent", 0, op, d, as, ms) ){
		LPCSTR pCC = ClipCC(AnsiString(HisCall->Text).c_str());
		r = StrCond(Cty.GetCont(pCC), as.c_str(), op);
    }
	else if( CheckCond(p, "StrVARITYPE", 0, op, d, as, ms) ){
		if( IsRTTY() ){
			r = StrCond("BAUDOT", as.c_str(), op);
        }
        else {
			r = StrCond(GetVariType(PCRX->Font->Charset, m_RxSet[0].m_Mode)+4, as.c_str(), op);
        }
    }
	else if( CheckCond(p, "StrPLATFORM", 0, op, d, as, ms) ){
		char bf[128];
        StrWindowsVer(bf);
		r = StrCond(bf, as.c_str(), op);
	}
	else if( CheckCond(p, "StrMacro(*)", 0, op, d, as, ms) ){
		AnsiString ts;
		ConvMacro_(ts, ms.c_str(), pButton);
		r = StrCond(ts.c_str(), as.c_str(), op);
	}
	else if( CheckCond(p, "ValMacro(*)", 1, op, d, as, ms) ){
		AnsiString ts;
		ConvMacro_(ts, ms.c_str(), pButton);
        double dd;
        if( sscanf(ts.c_str(), "%lf", &dd) != 1 ) dd = 0;
		r = ValCond(dd, d, op);
	}
	else if( CheckCond(p, "IsFile(*)", 0, op, d, as, ms) ){
		if( !op && !ms.IsEmpty() ){
			r = IsFile(GetMacroStr(as, ms.c_str()));
        }
        else {
			r = FALSE;
        }
	}
	else if( CheckCond(p, "IsDefined(*)", 1, op, d, as, ms) ){
		if( !op && !ms.IsEmpty() ){
			r = (m_MacroVal.FindName(ms.c_str(), _VAL_NORMAL|_VAL_INIFILE|_VAL_PROC) >= 0);
        }
        else {
			r = FALSE;
        }
	}
	else if( CheckCond(p, "IsMBCS(*)", 1, op, d, as, ms) ){
		GetMacroStr(as, ms.c_str());
		r = !as.IsEmpty() && IsMBCSStr(as.c_str());
	}
	else if( CheckCond(p, "IsAlpha(*)", 1, op, d, as, ms) ){
		GetMacroStr(as, ms.c_str());
		r = !as.IsEmpty() && IsAlphaAll(as.c_str());
	}
	else if( CheckCond(p, "IsNumber(*)", 1, op, d, as, ms) ){
		GetMacroStr(as, ms.c_str());
		r = !as.IsEmpty() && IsNumbAll(as.c_str());
	}
	else if( CheckCond(p, "IsCall(*)", 1, op, d, as, ms) ){
		GetMacroStr(as, ms.c_str());
    	r = !as.IsEmpty() && IsCall(as.c_str());
	}
	else if( CheckCond(p, "IsRST(*)", 1, op, d, as, ms) ){
		GetMacroStr(as, ms.c_str());
    	r = !as.IsEmpty() && IsRST(as.c_str());
	}
	else if( CheckCond(p, "IsMenu(*)", 1, op, d, as, ms) ){
		GetMacroStr(as, ms.c_str());
		TMenuItem *pMenu = GetMenuArg(ms, as.c_str(), FALSE);
		r = pMenu != NULL;
    }
	else if( CheckCond(p, "IsMenuEnabled(*)", 1, op, d, as, ms) ){
		GetMacroStr(as, ms.c_str());
		TMenuItem *pMenu = GetMenuArg(ms, as.c_str(), FALSE);
        DoParentClick(pMenu);
		r = pMenu && pMenu->Enabled;
	}
	else if( CheckCond(p, "IsMenuChecked(*)", 1, op, d, as, ms) ){
		GetMacroStr(as, ms.c_str());
		TMenuItem *pMenu = GetMenuArg(ms, as.c_str(), FALSE);
        DoParentClick(pMenu);
		r = pMenu && pMenu->Checked;
	}
	else if( CheckCond(p, "IsMenuVisible(*)", 1, op, d, as, ms) ){
		GetMacroStr(as, ms.c_str());
		TMenuItem *pMenu = GetMenuArg(ms, as.c_str(), FALSE);
        DoParentClick(pMenu);
		r = pMenu && pMenu->Visible;
	}
	else if( CheckCond(p, "IsButton(*)", 1, op, d, as, ms) ){
		GetMacroStr(as, ms.c_str());
		r = (FindButton(this, as.c_str(), NULL, FALSE) != NULL);
	}
	else if( CheckCond(p, "IsButtonEnabled(*)", 1, op, d, as, ms) ){
		GetMacroStr(as, ms.c_str());
		TSpeedButton *pButton = FindButton(this, as.c_str(), NULL, TRUE);
        r = pButton && pButton->Enabled;
	}
	else if( CheckCond(p, "IsButtonDown(*)", 1, op, d, as, ms) ){
		GetMacroStr(as, ms.c_str());
		TSpeedButton *pButton = FindButton(this, as.c_str(), NULL, TRUE);
        r = pButton && pButton->Enabled && pButton->Down;
	}
	else if( CheckCond(p, NULL, 1, op, d, as, ms) ){
		r = ValCond(GetMacroDouble(ms.c_str()), d, op);
    }
    else {
		r = GetMacroInt(p);
    }
	r = r ? TRUE : FALSE;
	r = fInv ? !r : r;
    return r;
}
//---------------------------------------------------------------------------
BOOL __fastcall TMainVARI::GetDataConds(LPCSTR p, int err, TSpeedButton *pButton)
{
	BOOL r = TRUE;
    int op = '&';
	LPSTR pBF = StrDupe(p);
	BOOL f;
    LPSTR s, t;
    t = s = pBF;
    for( ; *s; s++ ){
		if( ((*s=='&')&&(*(s+1)=='&')) || ((*s=='|')&&(*(s+1)=='|')) ){
			int c = *s;
			*s = 0;
			f = GetDataCond(t, err, pButton);
            if( op == '|' ){ r = r || f; }else{ r = r && f; };
            op = c;
            s++;
            t = s + 1;
        }
    }
	f = GetDataCond(t, err, pButton);
    if( op == '|' ){ r = r || f; }else{ r = r && f; };
    delete pBF;
    return r;
}
//---------------------------------------------------------------------------
LPCSTR __fastcall TMainVARI::Cond(LPCSTR p, TSpeedButton *pButton)
{
	sys.m_MacroError = FALSE;
	BOOL f = FALSE;
	LPCSTR sp = p;
    for( ; *p; p++ ){
		if( *p == '#' ){
			if( !strncmp(p, "#if", 3) ||
            	!strncmp(p, "#else", 5) ||
                !strncmp(p, "#endif", 6) ||
                !strncmp(p, "#define", 7) ||
                !strncmp(p, "#DEFINE", 7) ||
                !strncmp(p, "#DELETE", 7) ||
                !strncmp(p, "#macro", 6) ||
                !strncmp(p, "#exit", 5) ||
                !strncmp(p, "#repeat", 7) ||
            	!strncmp(p, "#proc", 5) ||
            	!strncmp(p, "#endp", 5) ||
                !strncmp(p, "#comment", 8) ){
				f = TRUE;
				break;
            }
        }
    }
	if( !f ) return NULL;

	AnsiString asBuff;
	CCond *pCond = new CCond(64);
    TStringList *pString = new TStringList;
	pString->Text = sp;
	int count = pString->Count;
    int cProc = 0;
    AnsiString	asProc, asName, asPara;
    for( int i = 0; i < count; i++ ){
		LPCSTR pLine = AnsiString(pString->Strings[i]).c_str();	//JA7UDE 0428
		if( cProc ){		// procÇÃìoò^
			LPCSTR p = SkipSpace(pLine);
			if( !strncmp(p, "#endp", 5) ){
                if( cProc ) cProc--;
				if( !cProc ){
					if( asName.IsEmpty() ){	// #repeatÇÃé¿çs
						int n;
						CalcI(n, asPara.c_str());
						if( (n > 0) && !asProc.IsEmpty() ){
							LPSTR pBF = new char[sys.m_MacBuffSize];
    	                    for( int i = 1; (i <= n) && !sys.m_MacroError; i++ ){
            	                sprintf(pBF, "%u,%u", n, i);
								ConvDummy(asPara, asProc.c_str(), "$repeat,$counter", pBF);
								DoMacroReturn(ConvMacro(pBF, asPara.c_str(), pButton));
			            	    asBuff += pBF;
	                        }
		                	delete pBF;
                        }
                    }
                    else {							// #procÇÃìoò^
	            	    m_MacroVal.RegisterString(asName.c_str(), asProc.c_str(), asPara.c_str(), _VAL_PROC);
                    }
                }
                else {
					asProc += pLine;
   	                asProc += "\r\n";
                }
            }
            else {
				if( !strncmp(p, "#proc", 5) || !strncmp(p, "#repeat", 7) ) cProc++;
				asProc += pLine;
                asProc += "\r\n";
            }
  	    }
		else if( pCond->CondJob(pLine, pButton) ){
			LPCSTR p = SkipSpace(pLine);
			if( !strncmp(p, "#proc", 5) ){
				pCond->SetCond();
                LPSTR pBF = StrDupe(SkipSpace(p+5));
				LPSTR pp = pBF;
				for(; *pp; pp++ ){
					if( (*pp == ' ') || (*pp == '\t') ){
						*pp = 0;
						pp++;
						break;
					}
				}
				if( pBF[0] && !isdigit(pBF[0]) ){
					asName = pBF;
                    asPara = SkipSpace(pp);
					asProc = "";
                    cProc++;
				}
                delete pBF;
			}
			else if( !strncmp(p, "#repeat", 7) ){
				pCond->SetCond();
                asName = "";
    			asPara = SkipSpace(p+7);
                asProc = "";
                cProc++;
			}
			else if( !strncmp(p, "#macro", 6) ){
				pCond->SetCond();
				p = SkipSpace(p+6);
				LPSTR pBF = new char[sys.m_MacBuffSize];
				DoMacroReturn(ConvMacro_(pBF, p, pButton));
                asBuff += pBF;
                delete pBF;
   			}
			else if( !strncmp(p, "#define", 7) || !strncmp(p, "#DEFINE", 7) ){
				BYTE mtype = BYTE(*(p+1)=='D' ? _VAL_INIFILE : _VAL_NORMAL);
				pCond->SetCond();
				LPSTR pBF = StrDupe(SkipSpace(p+7));
				LPSTR pp = pBF;
				for(; *pp; pp++ ){
					if( (*pp == ' ') || (*pp == '\t') ){
						*pp = 0;
						pp++;
						break;
					}
				}
				if( pBF[0] && !isdigit(pBF[0]) ){
					pp = SkipSpace(pp);
            		m_MacroVal.RegisterString(pBF, GetMacroStr(asProc, pp), NULL, mtype);
				}
                delete pBF;
			}
			else if( !strncmp(p, "#DELETEALL", 10) ){
				pCond->SetCond();
				m_MacroVal.Delete(_VAL_NORMAL|_VAL_INIFILE);
			}
			else if( !strncmp(p, "#DELETE", 7) ){
				pCond->SetCond();
				m_MacroVal.Delete(SkipSpace(p+7), _VAL_NORMAL|_VAL_INIFILE|_VAL_PROC);
			}
			else if( !strncmp(p, "#exit", 5) ){
				pCond->SetCond();
				pCond->ClearLevel();
				break;
			}
			else if( !strncmp(p, "#comment", 8) ){
				pCond->SetCond();
			}
			else {
				asBuff += pString->Strings[i];
				asBuff += '\r';
			}
        }
    }
    delete pString;
	f = pCond->IsCond();
    BOOL fLevel = pCond->GetLevel();
    delete pCond;
    if( !f ){
        return NULL;
    }
	else if( !sys.m_MacroError ){
    	if( fLevel ){
			sys.m_MacroError = TRUE;
			InfoMB(sys.m_MsgEng ? "#if-#endif block is not closed..." : "#ifÅ`#endifÉuÉçÉbÉNÇèIóπÇµÇƒÇ¢Ç‹ÇπÇÒ.");
	    }
    	else if( cProc ){
			sys.m_MacroError = TRUE;
			InfoMB(sys.m_MsgEng ? "Procedure block is not closed... (#endp is necessary)":"ÉvÉçÉVÉWÉÉÅ[ÉuÉçÉbÉNÇèIóπÇµÇƒÇ¢Ç‹ÇπÇÒ (#endpÇ™ïKóvÇ≈Ç∑).");
    	}
    }
    return StrDupe(asBuff.c_str());
}
//---------------------------------------------------------------------------
int __fastcall TMainVARI::ConvMacro(LPSTR t, LPCSTR p, TSpeedButton *pButton)
{
	LPCSTR bp = Cond(p, pButton);
	if( bp ) p = bp;
	int r = ConvMacro_(t, p, pButton);
	if( bp ) delete bp;
    return r;
}
//---------------------------------------------------------------------------
int __fastcall TMainVARI::ConvMacro(AnsiString &as, LPCSTR p, TSpeedButton *pButton)
{
	LPCSTR bp = Cond(p, pButton);
	if( bp ) p = bp;
	int r = ConvMacro_(as, p, pButton);
	if( bp ) delete bp;
    return r;
}
//---------------------------------------------------------------------------
int __fastcall TMainVARI::ConvMacro_(AnsiString &as, LPCSTR p, TSpeedButton *pButton)
{
	LPSTR bp = new char[sys.m_MacBuffSize];
	int r = ConvMacro_(bp, p, pButton);
	as = bp;
    delete bp;
    return r;
}
//---------------------------------------------------------------------------
LPCSTR __fastcall TMainVARI::GetMacroValue(LPCSTR pVal)
{
	pVal = SkipSpace(pVal);
	int r = m_MacroVal.FindName(pVal, _VAL_NORMAL|_VAL_INIFILE);
    if( r >= 0 ){
		pVal = m_MacroVal.GetString(r);
    }
    return pVal;
}
//---------------------------------------------------------------------------
LPCSTR __fastcall TMainVARI::GetMacroStr(AnsiString &as, LPCSTR pVal)
{
	pVal = GetMacroValue(pVal);
	if( strstr(pVal, "<%") ){
        ConvMacro_(as, pVal, NULL);
    }
    else {
	    as = pVal;
    }
   	return as.c_str();
}
//---------------------------------------------------------------------------
int __fastcall TMainVARI::GetMacroInt(LPCSTR pVal)
{
	if( !*pVal ) return 0;

	AnsiString as;
    GetMacroStr(as, pVal);
	int d;
	if( sscanf(SkipToValue(as.c_str()), "%d", &d) != 1 ) d = 0;
    return d;
}
//---------------------------------------------------------------------------
double __fastcall TMainVARI::GetMacroDouble(LPCSTR pVal)
{
	if( !*pVal ) return 0;

	AnsiString as;
    GetMacroStr(as, pVal);
	double d;
	if( sscanf(SkipToValue(as.c_str()), "%lf", &d) != 1 ) d = 0;
    return d;
}
//---------------------------------------------------------------------------
static LPCSTR __fastcall ClipRST(LPSTR bf, LPCSTR p)
{
	if( strlen(p) <= 3 ) return p;
	StrCopy(bf, p, 3); bf[3] = 0;
    return bf;
}
//---------------------------------------------------------------------------
static LPCSTR __fastcall ClipNR(LPCSTR p)
{
	if( strlen(p) <= 3 ) return "";
	return p + 3;
}
//---------------------------------------------------------------------------
static LPSTR __fastcall strchrl(LPSTR p, LPSTR v)
{
	LPSTR t;
	while(*v){
		if( (t = strchr(p, *v++))!= NULL ) return t;
    }
	return NULL;
}
//---------------------------------------------------------------------------
static void __fastcall EncodeColors(LPSTR t, UCOL *pColors, int max)
{
	int i;
	for( i = 0; i < max; i++ ){
		if( i ) *t++ = ',';
		sprintf(t, "%06X", pColors[i].d); t += strlen(t);
	}
}
//---------------------------------------------------------------------------
static BOOL __fastcall DecodeColors(LPCSTR p, UCOL *pColors, int max)
{
    AnsiString as;
	LPSTR pBF = StrDupe(MainVARI->GetMacroStr(as, p));
	LPSTR pp, tt;
    pp = SkipSpace(pBF);
	int n = 0;
	while(*pp && (n < max)){
		pp = SkipSpace(StrDlm(tt, pp));
		pColors[n].d = htoin(MainVARI->GetMacroStr(as, tt), -1);
		n++;
	}
    delete pBF;
    return n != 0;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::OnMacroMenuClick(TObject *Sender)
{
	TMenuItem *pMenu = (TMenuItem *)Sender;
	m_MacroInput = pMenu->Caption;

	if( !m_pMacroPopup ) return;
	int i, n;
    for( i = n = 0; i < m_pMacroPopup->Items->Count; i++ ){
		TMenuItem *pRef = m_pMacroPopup->Items->Items[i];
        if( pRef->Caption != "-" ){
			if( pRef == pMenu ){
				m_MacroMenuNo = n + 1;
    	        break;
            }
            n++;
        }
    }
}
//---------------------------------------------------------------------------
int __fastcall TMainVARI::ConvMacro_(LPSTR t, LPCSTR p, TSpeedButton *pButton)
{
	int r = 0;
    int f;
	AnsiString	as, vs;
    SYSTEMTIME now;
	BOOL fKanji = FALSE;
    LPCSTR pVal;
	UINT cDisCR = 0;
    BOOL fDisTAB = FALSE;
    BOOL fDisSP = FALSE;
	while(*p){
		if( fKanji ){
            fKanji = FALSE;
			*t++ = *p++;
        }
		else if( m_RxSet[0].m_MBCS.IsLead(*p) ){
			fKanji = TRUE;
            *t++ = *p++;
        }
        else if( (*p == '<') && (*(p+1) == '%') ){
			*t = 0;
			AnsiString key;
			int seq = 0;
			for( p+=2; *p ; p++ ){
				key += char(*p);
				if( (*p == '<') && (*(p+1) == '%') ){
					seq++;
                }
                else if( *p == '>' ){
					if( seq ){
						seq--;
                    }
                    else {
						p++;
    	            	break;
                    }
                }
            }
            LPCSTR pKey = key.c_str();
			InitCheckValKey(pKey, &vs);
            if( CheckKey("TX>") ){
				r |= macTX;
            }
            else if( CheckKey("RX>") ){
				r |= macRX;
            }
            else if( CheckKey("TXRX>") ){
				r |= macTXRX;
            }
            else if( CheckKey("TXOFF>") ){
				r |= macTXOFF;
            }
            else if( CheckKey("TONE>") ){
				r |= macTONE;
			}
            else if( CheckKey("MyCall>") ){
				strcpy(t, sys.m_CallSign.c_str());
            }
            else if( CheckKey("HisCall>") ){
				strcpy(t, AnsiString(HisCall->Text).c_str());	//JA7UDE 0428
            }
            else if( CheckKey("HisName>") ){
            	MacroHisName(t);
            }
            else if( CheckKey("DearName>") ){
				char bf[256];
            	MacroHisName(bf);
		        if( bf[0] ){
                	if( (PCRX->Font->Charset == SHIFTJIS_CHARSET) && m_RxSet[0].m_MBCS.IsLead(bf[0]) ){
						sprintf(t, "%sÇ≥ÇÒ", bf);
                    }
                    else {
						sprintf(t, "Dear %s", bf);
                    }
                }
            }
            else if( CheckKey("HisRST>") ){
				char bf[MLRST+1];
				strcpy(t, ClipRST(bf, AnsiString(HisRST->Text).c_str()));	//JA7UDE 0428
			}
			else if( CheckKey("MyRST>") ){
				char bf[MLRST+1];
				strcpy(t, ClipRST(bf, AnsiString(MyRST->Text).c_str()));	//JA7UDE 0428
			}
			else if( CheckKey("HisNR>") ){
				strcpy(t, ClipNR(AnsiString(HisRST->Text).c_str()));	//JA7UDE 0428
			}
			else if( CheckKey("MyNR>") ){
				strcpy(t, ClipNR(AnsiString(MyRST->Text).c_str()));	//JA7UDE 0428
			}
			else if( CheckKey("HisQTH>") ){
				strcpy(t, AnsiString(HisQTH->Text).c_str());	//JA7UDE 0428
			}
			else if( CheckKey("Note>") ){
				strcpy(t, AnsiString(EditNote->Text).c_str());	//JA7UDE 0428
			}
			else if( CheckKey("AutoClear>") ){
				r |= macAUTOCLEAR;
			}
			else if( CheckKey("AutoReturn>") ){
				m_ReqAutoReturn = TRUE;
				m_SaveEditPage = m_CurrentEdit;
			}
			else if( CheckKey("AutoNET>") ){
				m_ReqAutoNET = TRUE;
			}
			else if( CheckKey("ClearTXW>") ){
				KVCTClick(NULL);
			}
			else if( CheckKey("ClearRXW>") ){
				KVCRClick(NULL);
			}
			else if( CheckKey("MoveTop>") ){
				*t++ = button1ST; *t++ = button2ND; *t++ = buttonMOVETOP; *t = 0;
			}
			else if( CheckKey("MoveEnd>") ){
				*t++ = button1ST; *t++ = button2ND; *t++ = buttonMOVEEND; *t = 0;
			}
			else if( CheckKey("ResetScroll>") ){
				if( SBarRX->Enabled ){
					SBarRX->Position = SBarRX->Max;
					SetTXFocus();
				}
			}
			else if( CheckKey("DupeText>") ){
				m_Edit[m_CurrentEdit].DupeText(1);
			}
			else if( CheckKey("PopupTXW>") ){
				if( pButton && pButton->Down ) pButton->Down = FALSE;
				RECT rc;
				::GetWindowRect(PCTX->Handle, &rc);
                int X, Y;
                m_Edit[m_CurrentEdit].GetCursorPos(X, Y);
		        rc.left += X; rc.top += Y+8;
				PupTX->Popup(rc.left, rc.top);
            }
            else if( CheckKey("NETON>") ){
				if( SBNET->Enabled ){
					SBNET->Down = TRUE;
    	            SBNETClick(NULL);
                }
            }
            else if( CheckKey("NETOFF>") ){
				if( SBNET->Enabled ){
					SBNET->Down = FALSE;
	                SBNETClick(NULL);
                }
            }
            else if( CheckKey("AFCON>") ){
				if( SBAFC->Enabled ){
					SBAFC->Down = TRUE;
    	            SBAFCClick(NULL);
                }
            }
            else if( CheckKey("AFCOFF>") ){
				if( SBAFC->Enabled ){
					SBAFC->Down = FALSE;
    	            SBAFCClick(NULL);
                }
            }
            else if( CheckKey("ATCON>") ){
				if( SBATC->Enabled ){
                	SBATC->Down = TRUE;
	                SBATCClick(NULL);
                }
            }
            else if( CheckKey("ATCOFF>") ){
				if( SBATC->Enabled ){
                	SBATC->Down = FALSE;
	                SBATCClick(NULL);
                }
            }
			else if( CheckKey("FREQ>") ){
				strcpy(t, AnsiString(LogFreq->Text).c_str());	//JA7UDE 0428
            }
			else if( CheckKey("BAND>") ){
				strcpy(t, _BandText[Log.m_sd.band]);
			}
			else if( CheckKey("UDATE>") ){
				GetUTC(&now);
				MacroDate(t, now);
			}
			else if( CheckKey("LDATE>") ){
				GetLocal(&now);
				MacroDate(t, now);
			}
			else if( CheckKey("UTIME>") ){
				GetUTC(&now);
				sprintf(t, "%02u:%02u", now.wHour, now.wMinute);
			}
			else if( CheckKey("LTIME>") ){
				GetLocal(&now);
				sprintf(t, "%02u:%02u", now.wHour, now.wMinute);
			}
			else if( CheckKey("UTIMES>") ){
				GetUTC(&now);
				sprintf(t, "%02u:%02u:%02u", now.wHour, now.wMinute, now.wSecond);
			}
			else if( CheckKey("LTIMES>") ){
				GetLocal(&now);
				sprintf(t, "%02u:%02u:%02u", now.wHour, now.wMinute, now.wSecond);
			}
			else if( CheckKey("QSOON>") ){
				if( SBQSO->Enabled && !SBQSO->Down ){
					SBQSO->Down = TRUE;
                    SBQSOClick(NULL);
                }
			}
			else if( CheckKey("PTIME>") ){
            	GetUTC(&now);
				sprintf(t, "%u", SystemTime2GPS(&now));
			}
			else if( CheckKey("LPTIME>") ){
            	GetLocal(&now);
				sprintf(t, "%u", SystemTime2GPS(&now));
			}
			else if( CheckKey("QPTIME>") ){
				sprintf(t, "%u", m_QSOStart);
			}
			else if( CheckKey("ATCPPM>") ){
				sprintf(t, "%d", m_RxSet[0].m_pDem->GetClockError());
            }
			else if( CheckKey("CPUBENCHMARK>") ){
				DoBench(t, 0);
            }
			else if( CheckKey("BENCHDECM>") ){
				DoBench(t, 1);
            }
			else if( CheckKey("RANDOM>") ){
				sprintf(t, "%d", rand());
			}
			else if( CheckKey("QSOOFF>") ){
				if( SBQSO->Enabled && SBQSO->Down ){
					SBQSO->Down = FALSE;
                    SBQSOClick(NULL);
                }
			}
			else if( CheckKey("MODE>") ){
				strcpy(t, g_tDispModeTable[m_RxSet[0].m_Mode]);
			}
			else if( CheckKey("VARITYPE>") ){
				strcpy(t, GetVariType(PCRX->Font->Charset, m_RxSet[0].m_Mode));
			}
			else if( CheckKey("BAUD>") ){
				char bf[256];
				strcpy(t, StrDbl(bf, m_RxSet[0].IsMFSK() ? m_RxSet[0].m_pDem->m_MFSK_SPEED : m_RxSet[0].m_Speed));
			}
			else if( CheckKey("SkipCR>") ){
				cDisCR++;
			}
			else if( CheckKey("DisableCR>") ){
				cDisCR = 0x7fffffff;
			}
			else if( CheckKey("EnableCR>") ){
				cDisCR = 0;
			}
			else if( CheckKey("DisableTAB>") ){
				fDisTAB = TRUE;
			}
			else if( CheckKey("EnableTAB>") ){
				fDisTAB = 0;
			}
			else if( CheckKey("DisableSP>") ){
				fDisSP = TRUE;
			}
			else if( CheckKey("EnableSP>") ){
				fDisSP = 0;
			}
			else if( CheckKey("SP>") ){
            	*t++ = ' '; *t = 0;
			}
			else if( CheckKey("CR>") ){
            	*t++ = '\r'; *t = 0;
			}
			else if( CheckKey("BS>") ){
            	*t++ = '\b'; *t = 0;
			}
			else if( CheckKey("TAB>") ){
            	*t++ = '\t'; *t = 0;
			}
			else if( CheckKey("IDLE>") ){
				*t++ = button1ST; *t++ = button2ND; *t++ = buttonIDLE;
				*t = 0;
			}
			else if( CheckKey("SeekTop>") ){
				if( pButton && pButton->Down ) pButton->Down = FALSE;
				f = UdMac->Max - UdMac->Position + 1;
                if( f > UdMac->Max ) f = 0;
                if( f ){
					::PostMessage(Handle, WM_WAVE, waveSeekMacro, f);
    	            r |= macSEEK; pButton = NULL;
                }
			}
			else if( CheckKey("SeekNext>") ){
				if( pButton && pButton->Down ) pButton->Down = FALSE;
				::PostMessage(Handle, WM_WAVE, waveSeekMacro, 1);
                r |= macSEEK; pButton = NULL;
			}
			else if( CheckKey("SeekPrev>") ){
				if( pButton && pButton->Down ) pButton->Down = FALSE;
				::PostMessage(Handle, WM_WAVE, waveSeekMacro, -1);
                r |= macSEEK; pButton = NULL;
			}
			else if( CheckKey("Exit>") ){
				Close();
			}
			else if( CheckKey("ShutDown>") ){
				ShutDown();
			}
			else if( CheckKey("Suspend>") ){
				DoSuspend(TRUE);
			}
			else if( CheckKey("Resume>") ){
				DoResume();
			}
			else if( CheckKey("PTTON>") ){
				SetPTT(TRUE);
			}
			else if( CheckKey("PTTOFF>") ){
				SetPTT(FALSE);
			}
			else if( CheckKey("VER>") ){
            	strcpy(t, VERNO);
			}
			else if( CheckKey("VERMINOR>") ){
            	strcpy(t, VERBETA);
			}
			else if( CheckKey("RxCarrier>") ){
				sprintf(t, "%u", UdRxCarrier->Position);
            }
			else if( CheckKey("TxCarrier>") ){
				sprintf(t, "%u", UdTxCarrier->Position);
            }
			else if( CheckKey("AFCFrequency>") ){
				double fq;
                if( m_TX == txINTERNAL ){
					fq = m_ModFSK.m_CarrierFreq;
                }
                else {
					fq = m_RxSet[0].m_pDem->m_RxFreq;
                }
				sprintf(t, (m_RxSet[0].Is170() || m_RxSet[0].IsMFSK()) ? "%.0lf":"%.1lf", fq);
            }
			else if( CheckKey("MouseFrequency>") ){
				sprintf(t, "%d", m_RightFreq);
			}
			else if( CheckKey("Input$>") ){
				strcpy(t, m_MacroInput.c_str());
			}
			else if( CheckKey("Click$>") ){
				strcpy(t, m_StrSel.c_str());
			}
			else if( CheckKey("CLICK$>") ){
				strcpy(t, m_UStrSel.c_str());
			}
			else if( CheckKey("vvv>") ){
				strcpy(t, "the quick brown fox jumps over a lazy dog");
			}
			else if( CheckKey("VVV>") ){
				strcpy(t, "ìVãCê∞òNÇ»ÇÍÇ«ëŒînÇÃâ´Ç…îgçÇÇµ");
			}
			else if( CheckKey("PLATFORM>") ){
				strcpy(t, "Windows "); t += strlen(t);
                StrWindowsVer(t);
			}
			else if( CheckKey("OutputVolume>") ){
				KOVOClick(KOVO);
			}
			else if( CheckKey("InputVolume>") ){
				KOVOClick(KOVI);
			}
			else if( CheckKey("Setup>") ){
				if( pButton && pButton->Down ) pButton->Down = FALSE;
				KOOClick(KOO);
			}
			else if( CheckKey("SetupLog>") ){
				if( pButton && pButton->Down ) pButton->Down = FALSE;
				KOLClick(NULL);
			}
			else if( CheckKey("SetupRadio>") ){
				if( pButton && pButton->Down ) pButton->Down = FALSE;
				KORClick(NULL);
			}
			else if( CheckKey("Calibration>") ){
//				if( pButton && pButton->Down ) pButton->Down = FALSE;
				KOAClick(NULL);
			}
			else if( CheckKey("PlaySound>") ){
				if( pButton && pButton->Down ) pButton->Down = FALSE;
				KFRSClick(NULL);
            }
			else if( CheckKey("RecordSound>") ){
				if( pButton && pButton->Down ) pButton->Down = FALSE;
				KFWSClick(NULL);
            }
			else if( CheckKey("QuickRecordSound>") ){
				KFWSTClick(NULL);
            }
			else if( CheckKey("StopPlayRecord>") ){
				KFESClick(NULL);
            }
			else if( CheckKey("SoundTime>") ){
				sprintf(t, "%u", UINT(m_WaveFile.GetPos()*0.5/SAMPFREQ));
            }
			else if( CheckKey("Level>") ){
				sprintf(t, "%d", m_RxSet[0].m_StgFFT.DispSig);
            }
			else if( CheckKey("PeakLevel>") ){
				sprintf(t, "%d", m_RxSet[0].m_PeakSig);
			}
			else if( CheckKey("AverageLevel>") ){
				sprintf(t, "%d", int(m_RxSet[0].m_AvgSig.GetAvg()+0.5));
			}
			else if( CheckKey("CWID>") ){
				*t++ = button1ST; *t++ = button2ND; *t++ = buttonCWON;
                strcpy(t, sys.m_CallSign.c_str()); t += strlen(t);
				*t++ = button1ST; *t++ = button2ND; *t++ = buttonCWOFF;
                *t = 0;
			}
			else if( (pVal = CheckValKey("CWID=*>"))!=NULL ){
				pVal = GetMacroStr(as, pVal);
				*t++ = button1ST; *t++ = button2ND; *t++ = buttonCWON;
                for( ; *pVal; pVal++ ) *t++ = *pVal;
                *t++ = button1ST; *t++ = button2ND; *t++ = buttonCWOFF;
                *t = 0;
			}
			else if( CheckKey("DigitalLevel>") ){
				sprintf(t, "%u", m_ModGain);
			}
			else if( (pVal = CheckValKey("DigitalLevel=*>"))!=NULL ){
				f = GetMacroInt(pVal);
				if( f >= 1024 ){
	                if( f > 32768 ) f = 32768;
    	            m_ModGain = f;
        	        UpdateModGain();
                }
			}
			else if( (pVal = CheckValKey("ToLower=*>"))!=NULL ){
				LPSTR pBF = StrDupe(GetMacroStr(as, pVal));
				strcpy(t, (LPSTR)jstrlwr(LPUSTR(pBF)));
                delete pBF;
			}
			else if( (pVal = CheckValKey("ToUpper=*>"))!=NULL ){
				LPSTR pBF = StrDupe(GetMacroStr(as, pVal));
				strcpy(t, (LPSTR)jstrupr(LPUSTR(pBF)));
                delete pBF;
			}
			else if( (pVal = CheckValKey("Page=*>"))!=NULL ){
				f = GetMacroInt(pVal);
				if( (f >= 1) && (f <= 4) ){
					f--;
					SetEditPage(f);
                }
			}
			else if( CheckKey("LogMODE>") ){
				strcpy(t, Log.GetModeString(Log.m_sd.mode));
			}
			else if( (pVal = CheckValKey("LogMODE=*>"))!=NULL ){
				pVal = GetMacroStr(as, pVal);
				m_strLogMode = pVal;
                UpdateLogMode();
			}
			else if( (pVal = CheckValKey("BAND=*>"))!=NULL ){
				pVal = GetMacroStr(as, pVal);
				if( *pVal ){
					for(f = 0; _BandText[f]; f++ ){
						if( !strcmpi(pVal, _BandText[f]) ){
                	        LogFreq->Text = Log.GetFreqString(BYTE(f), 0);
							OnLogFreq(FALSE);
                            break;
                    	}
	                }
                }
			}
			else if( (pVal = CheckValKey("FREQ=*>"))!=NULL ){
				pVal = GetMacroStr(as, pVal);
				if( *pVal ){
           	        LogFreq->Text = pVal;
					OnLogFreq(FALSE);
                }
			}
			else if( (pVal = CheckValKey("HisCall=*>"))!=NULL ){
				pVal = GetMacroStr(as, pVal);
				if( *pVal ){
					HisCall->Text = pVal;
					FindCall();
					UpdateUI();
		    	    SetTXFocus();
                }
			}
			else if( (pVal = CheckValKey("HisRST=*>"))!=NULL ){
				HisRST->Text = GetMacroStr(as, pVal);
			}
			else if( (pVal = CheckValKey("MyRST=*>"))!=NULL ){
				MyRST->Text = GetMacroStr(as, pVal);
			}
			else if( (pVal = CheckValKey("Note=*>"))!=NULL ){
                EditNote->Text = GetMacroStr(as, pVal);
			}
			else if( (pVal = CheckValKey("IME=*>"))!=NULL ){
				f = GetMacroOnOff(pVal);
                if( f >= 0 ){
					HWND hWnd = ActiveControl->Handle;
					if( hWnd ){
						HIMC hIMC;
						if( (hIMC = ::ImmGetContext(hWnd))!=NULL ){
							if( f == 2 ){
								f = !::ImmGetOpenStatus(hIMC);
                	        }
							::ImmSetOpenStatus(hIMC, f);
							::ImmReleaseContext(hWnd, hIMC);
						}
                    }
                }
			}
			else if( (pVal = CheckValKey("HisGreetings=*>"))!=NULL ){
				f = GetMacroInt(pVal);
                if( (f >= 1) && (f <= 3) ){
					MacroGreeting(t, AnsiString(HisCall->Text).c_str(), f-1);	//JA7UDE 0428
                }
			}
			else if( (pVal = CheckValKey("Entity=*>"))!=NULL ){
                strcpy(t, Cty.GetCountry(ClipCC(GetMacroStr(as, pVal))));
			}
			else if( (pVal = CheckValKey("Continent=*>"))!=NULL ){
                strcpy(t, Cty.GetCont(ClipCC(GetMacroStr(as, pVal))));
			}
			else if( (pVal = CheckValKey("EntityName=*>"))!=NULL ){
				if( (f = Cty.GetNoP(ClipCC(GetMacroStr(as, pVal))))!=0 ){
					CTL *cp = Cty.GetCTL(f-1);
	                strcpy(t, (cp->QTH!=NULL) ? cp->QTH : "");
				}
			}
			else if( (pVal = CheckValKey("RANDOM=*>"))!=NULL ){
				srand(GetMacroInt(pVal));
			}
			else if( (pVal = CheckValKey("Wait=*>"))!=NULL ){
				f = GetMacroInt(pVal);
				f /= 10;
				CWaitCursor w;
                for( ; f; f-- ){
					OnWave();
					::Sleep(10);
                }
			}
			else if( (pVal = CheckValKey("MacroText=*>"))!=NULL ){
				pVal = GetMacroStr(as, pVal);
            	if( *pVal ){
                	AnsiString ts;
					GetFullPathName(ts, pVal, sys.m_MacroDir);
					FILE *fp = fopen(ts.c_str(), "rb");
                    int len = sys.m_MacBuffSize;
					if( fp ){
                        LPSTR pBF = new char[len];
    	    	        int rlen = fread(pBF, 1, len-1, fp);
    	    	        if( rlen > 0 ){
							pBF[rlen] = 0;
            	    	    r |= ConvMacro(as, pBF, pButton);
                            StrCopy(t, as.c_str(), len/2);
                		}
                        delete pBF;
						fclose(fp);
                    }
                }
			}
			else if( (pVal = CheckValKey("SendFile=*>"))!=NULL ){
				pVal = GetMacroStr(as, pVal);
            	if( *pVal ){
					if( m_fpText ) fclose(m_fpText);
					::SetCurrentDirectory(sys.m_BgnDir);
		    		if( (m_fpText = fopen(pVal, "rb")) != NULL ){
						r |= macTX;
                    }
                }
			}
			else if( (pVal = CheckValKey("RefRXW>"))!=NULL ){
				AnsiString	rs;
			    m_Dump.GetWindowText(rs);
			    if( !m_pEdit ){
					m_pEdit = new TFileEdit(this);
			        m_pEdit->SetEvent(Handle, WM_WAVE, 2);
			    }
			    m_pEdit->Execute(rs, "Received text", "Received.txt");
            }
			else if( (pVal = CheckValKey("EditFile=*>"))!=NULL ){
				LPSTR pBF = StrDupe(GetMacroValue(pVal));
				LPSTR pp, tt;
                pp = StrDlm(tt, pBF);
				pVal = GetMacroStr(as, tt);
            	if( *pVal ){
				    if( !m_pEdit ){
						m_pEdit = new TFileEdit(this);
				        m_pEdit->SetEvent(Handle, WM_WAVE, 2);
				    }
					f = GetMacroInt(pp);
					m_pEdit->SetWordWrap(f & 2 ? FALSE : TRUE);
				    m_pEdit->Execute(pVal, f & 1);
                }
                delete pBF;
			}
			else if( (pVal = CheckValKey("SaveMacro=*>"))!=NULL ){
				pVal = GetMacroStr(as, pVal);
            	if( *pVal ){
					SaveMacro(pVal);
                }
			}
			else if( (pVal = CheckValKey("LoadMacro=*>"))!=NULL ){
				pVal = GetMacroStr(as, pVal);
            	if( *pVal ){
                    ::PostMessage(Handle, WM_WAVE, waveLoadMacro, DWORD(StrDupe(pVal)));
                }
			}
			else if( (pVal = CheckValKey("PlaySound=*>"))!=NULL ){
				pVal = GetMacroStr(as, pVal);
            	if( *pVal ){
					AnsiString as;
                    GetFullPathName(as, pVal);
					m_Wave.InClose();
					m_WaveFile.Play(as.c_str());
			        ReOutOpen();
					if( !m_pPlayBox ){
						m_pPlayBox = new TPlayDlgBox(this);
			        }
		    	    m_pPlayBox->Execute(&m_WaveFile);
					m_pPlayBox->Visible = TRUE;
			        m_pPlayBox->SetFocus();
                }
            }
			else if( (pVal = CheckValKey("RecordSound=*>"))!=NULL ){
				pVal = GetMacroStr(as, pVal);
            	if( *pVal ){
					AnsiString as;
                    GetFullPathName(as, pVal);
					m_WaveFile.Rec(as.c_str());
				    KFWST->Checked = FALSE; KFWS->Checked = TRUE;
                }
			}
			else if( (pVal = CheckValKey("Slider=*>"))!=NULL ){
				CWaitCursor w;
				if( pButton && pButton->Down ) pButton->Down = FALSE;
				LPSTR pBF = StrDupe(GetMacroValue(pVal));
                LPSTR pp, tt;
                pp = StrDlm(tt, pBF);
				pVal = GetMacroStr(as, tt);
                pp = StrDlm(tt, pp);
                double d = GetMacroDouble(tt);
                pp = StrDlm(tt, pp);
                double dmin = GetMacroDouble(tt);
                pp = StrDlm(tt, pp);
                double dmax = GetMacroDouble(tt);
                pp = StrDlm(tt, pp);
                double per = GetMacroDouble(tt);
                StrDlm(tt, pp);
                int sc = GetMacroInt(tt);
                m_MacroInput = "";
				DettachFocus();
				OnWave();
                TTrackDlgBox *pBox = new TTrackDlgBox(this);
				f = pBox->Execute(pVal, d, dmin, dmax, sc, per);
                delete pBox;
				AttachFocus();
                if( f ){
            		char bf[256];
					m_MacroInput = StrDbl(bf, d);
                }
                delete pBF;
			}
			else if( (pVal = CheckValKey("FileDialog=*>"))!=NULL ){
				CWaitCursor w;
				if( pButton && pButton->Down ) pButton->Down = FALSE;
				LPSTR pBF = StrDupe(GetMacroValue(pVal));
                LPSTR pp, tt;
                pp = StrDlm(tt, pBF);
				TOpenDialog *pBox = new TOpenDialog(this);
				pBox->Options >> ofCreatePrompt;
				pBox->Options << ofFileMustExist;
				pVal = GetMacroStr(as, tt);
                if( *pVal ) pBox->Title = pVal;
                pp = StrDlm(tt, pp);
				pBox->Filter = GetMacroStr(as, tt);
                pp = StrDlm(tt, pp);
				pBox->FileName = GetMacroStr(as, tt);
                pp = StrDlm(tt, pp);
				pBox->DefaultExt = GetMacroStr(as, tt);
                StrDlm(tt, pp);
                pVal = GetMacroStr(as, tt);
				pBox->InitialDir = *pVal ? pVal : sys.m_BgnDir;
				DettachFocus();
				OnWave();
				m_MacroInput = "";
				if( pBox->Execute() ){
					m_MacroInput = pBox->FileName.c_str();
           		}
                delete pBox;
                delete pBF;
				AttachFocus();
			}
			else if( CheckKey("ListSpeed>") ){
				GetComboBox(as, CBSpeed); strcpy(t, as.c_str());
			}
			else if( (pVal = CheckValKey("ListSpeed=*>"))!=NULL ){
				pVal = GetMacroStr(as, pVal);
                if( *pVal ){
					m_ListBAUD = pVal;
					if( !m_RxSet[0].IsMFSK() && !m_RxSet[0].IsRTTY() ){
						SetComboBox(CBSpeed, pVal);
                    }
				}
            }
			else if( CheckKey("ListCarrier>") ){
				GetComboBox(as, CBRXCarrier); strcpy(t, as.c_str());
			}
			else if( (pVal = CheckValKey("ListCarrier=*>"))!=NULL ){
				pVal = GetMacroStr(as, pVal);
                if( *pVal ){
					SetComboBox(CBRXCarrier, pVal);
    	            SetComboBox(CBTxCarrier, pVal);
                }
			}
			else if( CheckKey("ListRST>") ){
				GetComboBox(as, HisRST); strcpy(t, as.c_str());
			}
			else if( (pVal = CheckValKey("ListRST=*>"))!=NULL ){
				pVal = GetMacroStr(as, pVal);
                if( *pVal ){
					SetComboBox(HisRST, pVal);
    	            SetComboBox(MyRST, pVal);
                }
			}
			else if( CheckKey("ListLogFreq>") ){
				GetComboBox(as, LogFreq); strcpy(t, as.c_str());
			}
			else if( (pVal = CheckValKey("ListLogFreq=*>"))!=NULL ){
				pVal = GetMacroStr(as, pVal);
                if( *pVal ){
					SetComboBox(LogFreq, pVal);
                }
			}
			else if( CheckKey("CWSpeed>") ){
				sprintf(t, "%d", m_ModFSK.GetCWSpeed());
            }
			else if( (pVal = CheckValKey("CWSpeed=*>"))!=NULL ){
				f = GetMacroInt(pVal);
                if( (f >= 10) && (f <= 60) ){
					m_ModFSK.SetCWSpeed(f);
                }
			}
			else if( (pVal = CheckValKey("RxCarrier=*>"))!=NULL ){
				f = GetMacroInt(pVal);
				if( f > 0 ){
	                SetRxFreq(f);
                }
            }
			else if( (pVal = CheckValKey("TxCarrier=*>"))!=NULL ){
				f = GetMacroInt(pVal);
				if( f > 0 ){
	                SetTxFreq(f);
                }
            }
			else if( CheckKey("Clock>") ){
				sprintf(t, "%.2lf", SAMPFREQ);
			}
			else if( CheckKey("BaseClock>") ){
				sprintf(t, "%u", SAMPBASE);
			}
			else if( CheckKey("TxOffset>") ){
				sprintf(t, "%.2lf", SAMPTXOFFSET);
			}
			else if( CheckKey("DemodulatorClock>") ){
				sprintf(t, "%.2lf", DEMSAMPFREQ);
			}
			else if( CheckKey("FFTClock>") ){
				sprintf(t, "%.2lf", m_FFTSampFreq);
			}
			else if( CheckKey("FFTSize>") ){
				sprintf(t, "%u", FFT_SIZE);
			}
			else if( CheckKey("SoundName>") ){
                m_Wave.GetInDevName(as);
				strcpy(t, as.c_str());
			}
			else if( CheckKey("SoundOutName>") ){
                m_Wave.GetOutDevName(as);
				strcpy(t, as.c_str());
			}
			else if( CheckKey("SoundDevice>") ){
				LPCSTR pVal = g_tSoundCH[m_Wave.m_SoundStereo];
				if( m_Wave.m_SoundID == -2 ){
					sprintf(t, "%s,%s", pVal, sys.m_SoundMMW.c_str());
                }
                else {
					sprintf(t, "%s,%d", pVal, m_Wave.m_SoundID);
					if( m_Wave.m_SoundID != m_Wave.m_SoundTxID ){
                    	t += strlen(t);
                        sprintf(t, ",%d", m_Wave.m_SoundTxID);
                    }
                }
			}
			else if( (pVal = CheckValKey("SoundDevice=*>"))!=NULL ){
				LPSTR pBF = StrDupe(GetMacroStr(as, pVal));
                LPSTR tt, pp;
                pp = StrDlm(tt, pBF);
                pVal = GetMacroStr(as, tt);
                f = FindStringTable(g_tSoundCH, pVal, 3);
                if( f < 0 ) f = m_Wave.m_SoundStereo;
                pp = StrDlm(tt, pp);
                pVal = GetMacroStr(as, tt);
				{
					AnsiString ts;
                    LPCSTR pVal2 = GetMacroStr(ts, pp);
					SetSoundCard(f, *pVal ? pVal : NULL, *pVal2 ? pVal2 : pVal);
                }
                delete pBF;
            }
			else if( (pVal = CheckValKey("Clock=*>"))!=NULL ){
				double d = GetMacroDouble(pVal);
				if( (d >= MIN_SAMP) && (d <= MAX_SAMP) ){
					SetSampleFreq(d, FALSE);
                }
            }
			else if( (pVal = CheckValKey("TxOffset=*>"))!=NULL ){
				double d = GetMacroDouble(pVal);
				if( fabs(d) <= 3000.0 ){
					SetTxOffset(d);
                }
            }
			else if( CheckKey("FFTScale>") ){
				sprintf(t, "%d", m_FFTVType);
			}
			else if( (pVal = CheckValKey("FFTScale=*>"))!=NULL ){
				f = GetMacroInt(pVal);
                switch(f){
					case 0:
						KVF1Click(KVF1);
                        break;
                    case 1:
						KVF1Click(KVF2);
                        break;
                    default:
						KVF1Click(KVF3);
                        break;
                }
			}
			else if( CheckKey("BPFTaps>") ){
				sprintf(t, "%d,%d,%d,%d", g_tBpfTaps[0], g_tBpfTaps[1], g_tBpfTaps[2], g_tBpfTaps[3]);
			}
			else if( (pVal = CheckValKey("BPFTaps=*>"))!=NULL ){
				LPSTR pBF = StrDupe(GetMacroValue(pVal));
				LPSTR pp, tt;
                pp = SkipSpace(pBF);
                for( int n = 0; *pp && (n < 4); n++ ){
	                pp = SkipSpace(StrDlm(tt, pp));
                    if( *tt ){
						f = GetMacroInt(tt) & 0x0000fffe;
                        if( (f >= 8) && (f <= 512) ){
							g_tBpfTaps[n] = f;
                        }
                    }
                }
                delete pBF;
				SBBPFWClick(GetBPF(GetBPFType()));
			}
			else if( (pVal = CheckValKey("BPF=*>"))!=NULL ){
				f = GetMacroInt(pVal);
                if( (f >= 0) && (f < 4) ){
					TSpeedButton *pButton = GetBPF(f);
                    pButton->Down = TRUE;
					SBBPFWClick(pButton);
                }
            }
			else if( CheckKey("NotchTaps>") ){
				sprintf(t, "%d", m_Notches.m_nBaseTaps);
			}
			else if( (pVal = CheckValKey("NotchTaps=*>"))!=NULL ){
				f = GetMacroInt(pVal);
                if( (f >= 16) && (f <= TAPMAX) ){
					m_Notches.m_nBaseTaps = f & 0x0ffe;
                    m_Notches.Create();
                }
			}
			else if( CheckKey("NotchWidth>") ){
				sprintf(t, "%d", m_Notches.m_NotchWidth);
			}
			else if( (pVal = CheckValKey("NotchWidth=*>"))!=NULL ){
				f = GetMacroInt(pVal);
                if( (f >= 1) && (f <= 1000) ){
					m_Notches.m_NotchWidth = f;
                    m_Notches.Create();
                }
			}
			else if( CheckKey("Notch>") ){
				sprintf(t, "%d", m_Notches.m_Count ? m_Notches.m_pBase[0].Freq : 0);
			}
			else if( (pVal = CheckValKey("Notch=*>"))!=NULL ){
				pVal = GetMacroStr(as, pVal);
				switch(GetOnOff(pVal)){
					case 0:
						m_Notches.Delete();
                        break;
                    case 1:
						if( !m_Notches.m_Count ){
							m_Notches.Add(m_NotchFreq);
                        }
                        break;
                    case 2:
						if( m_Notches.m_Count ){
							m_Notches.Delete();
                        }
                        else {
							m_Notches.Add(m_NotchFreq);
                        }
                        break;
                    default:
						f = GetMacroInt(pVal);
		                if( f ){
							m_Notches.Add(f);
                        	m_NotchFreq = f;
                        }
                        else {
							m_Notches.Delete();
                        }
                        break;
                }
            }
			else if( (pVal = CheckValKey("BAUD=*>"))!=NULL ){
				double b = GetMacroDouble(pVal);
				if( b > 0 ){
					if( m_RxSet[0].IsMFSK() || ((b >= MIN_SPEED) && (b <= MAX_SPEED)) ){
						UpdateSpeed(&m_RxSet[0], b);
        	        }
                }
            }
			else if( CheckKey("SquelchLevel>") ){
				sprintf(t, "%.1lf", double(m_RxSet[0].m_SQLevel * 0.01));
			}
			else if( (pVal = CheckValKey("SquelchLevel=*>"))!=NULL ){
				if( *pVal ){
					f = GetMacroInt(pVal) * 100.0;
					if( f < 0 ) f = 0;
        	        if( f > 2048 ) f = 2048;
					m_RxSet[0].m_SQLevel = f;
                }
			}
			else if( CheckKey("AFCWidth>") ){
				sprintf(t, "%d", m_AFCWidth);
			}
			else if( (pVal = CheckValKey("AFCWidth=*>"))!=NULL ){
				if( *pVal ){
					f = GetMacroInt(pVal);
    	            if( (f >= 0) && (f <= 2000) ){
						m_AFCWidth = f;
            	    }
                }
            }
			else if( CheckKey("AFCLevel>") ){
				sprintf(t, "%d", m_AFCLevel);
			}
			else if( (pVal = CheckValKey("AFCLevel=*>"))!=NULL ){
				f = GetMacroInt(pVal);
                if( (f >= 3) && (f < 20) ){
					m_AFCLevel = f;
                }
            }
			else if( CheckKey("PTT>") ){
				strcpy(t, sys.m_PTTCOM.c_str());
			}
			else if( CheckKey("Radio>") ){
				strcpy(t, RADIO.StrPort);
			}
			else if( (pVal = CheckValKey("PTT=*>"))!=NULL ){
				pVal = GetMacroStr(as, pVal);
                if( *pVal ){
					if( strcmpi(sys.m_PTTCOM.c_str(), pVal) ){
						sys.m_PTTCOM = pVal;
    	                OpenCom();
                    }
                }
            }
			else if( (pVal = CheckValKey("Radio=*>"))!=NULL ){
				pVal = GetMacroStr(as, pVal);
                if( *pVal ){
					if( strcmpi(RADIO.StrPort, pVal) ){
						strcpy(RADIO.StrPort, pVal);
    	                OpenRadio();
                    }
                }
            }
			else if( (pVal = CheckValKey("RadioOut=*>"))!=NULL ){
				pVal = GetMacroStr(as, pVal);
				if( m_pRadio && *pVal ){
					WaitICOM();
                	m_pRadio->SendCommand(pVal);
				}
            }
			else if( CheckKey("RadioCarrierKHz>") ){
				if( m_pRadio ){
					sprintf(t, "%.3lf", double(m_pRadio->m_FreqHz)*0.001);
                }
            }
			else if( CheckKey("RadioKHz>") ){
				if( m_pRadio ){
					sprintf(t, "%.3lf", double(m_pRadio->m_RigHz)*0.001);
                }
            }
			else if( (pVal = CheckValKey("RadioKHz=*>"))!=NULL ){
				LPSTR pBF = StrDupe(GetMacroValue(pVal));
                LPSTR tt;
				pVal = StrDlm(tt, pBF);
				LPCSTR pKey = GetMacroStr(as, tt);
                double d;
                Calc(d, pVal);
                d *= 1000.0;		// KHz -> Hz
                if( m_pRadio && (d >= 50000.0) && (d <= 6.0e9) ){
					m_pRadio->CalcRigFreq(d*1.0e-6);
	                if( !strcmp(pKey, "YAESU-VU") ){
						OutYaesuVU(d);
        	        }
            	    else if( !strcmp(pKey, "YAESU-HF") ){
						OutYaesuHF(d);
	                }
    	            else if( !strcmp(pKey, "CI-V") ){
						OutCIV(d);
            	    }
    	            else if( !strcmp(pKey, "CI-V4") ){
						OutCIV4(d);
            	    }
                	else if( !strcmp(pKey, "KENWOOD") ){
						OutKENWOOD(d);
    	            }
                	else if( !strcmp(pKey, "YAESU-NEW") ){
						OutKENWOOD(d);
    	            }
        	        else if( !strcmp(pKey, "JST245") ){
						OutJST245(d);
                	}
	                else {
						strcpy(t, "Rig type = YAESU-HF/YAESU-VU/YAESU-NEW/CI-V/CI-V4/KENWOOD/JST245\r\n");
        	        }
                }
                delete pBF;
			}
			else if( CheckKey("RadioMode>") ){
				if( m_pRadio ){
					strcpy(t, g_tRadioMode[m_pRadio->m_RigMode]);
                }
            }
			else if( (pVal = CheckValKey("RadioMode=*>"))!=NULL ){
				LPSTR pBF = StrDupe(GetMacroValue(pVal));
                LPSTR tt;
				pVal = StrDlm(tt, pBF);
				LPCSTR pKey = GetMacroStr(as, tt);
                AnsiString ts;
                LPCSTR pMode = GetMacroStr(ts, pVal);
                if( m_pRadio ){
					m_pRadio->SetInternalRigMode(pMode);
	                if( !strcmp(pKey, "YAESU-VU") ){
						OutModeYaesuVU(pMode);
        	        }
            	    else if( !strcmp(pKey, "YAESU-HF") ){
						OutModeYaesuHF(pMode);
	                }
    	            else if( !strcmp(pKey, "CI-V") || !strcmp(pKey, "CI-V4") ){
						OutModeCIV(pMode);
            	    }
                	else if( !strcmp(pKey, "KENWOOD") ){
						OutModeKENWOOD(pMode);
    	            }
                	else if( !strcmp(pKey, "YAESU-NEW") ){
						OutModeKENWOOD(pMode);
    	            }
        	        else if( !strcmp(pKey, "JST245") ){
						OutModeJST245(pMode);
                	}
	                else {
						strcpy(t, "Rig type = YAESU-HF/YAESU-VU/YAESU-NEW/CI-V/CI-V4/KENWOOD/JST245\r\n");
        	        }
                }
                delete pBF;
			}
			else if( (pVal = CheckValKey("PlayBack=*>"))!=NULL ){
				f = GetOnOff(GetMacroStr(as, pVal));
				switch(f){
                	case 0:			// OFF
						sys.m_fPlayBack = FALSE;
						break;
                    case 1:			// ON
						sys.m_fPlayBack = TRUE;
						break;
                    case 2:			// ON-OFF
						sys.m_fPlayBack = !sys.m_fPlayBack;
						break;
                    default:
						f = GetMacroInt(pVal);
		                LimitInt(&f, 0, 60);
		           	    DoPlayBack(f);
                        f = -1;
                        break;
                }
                if( f >= 0 ){
					if( sys.m_fPlayBack ){
						if( !m_PlayBack.IsActive() ) m_PlayBack.Init(m_BufferSize, SAMPBASE);
			        }
		    	    else {
						m_PlayBack.Delete();
			        }
    	            UpdateUI();
                }
			}
			else if( CheckKey("PlayBackSpeed>") ){
				sprintf(t, "%d", sys.m_PlayBackSpeed);
			}
			else if( (pVal = CheckValKey("PlayBackSpeed=*>"))!=NULL ){
				f = GetMacroInt(pVal);
                if( f > 0 ){
	                LimitInt(&f, 1, 20);
    	            sys.m_PlayBackSpeed = f;
                }
			}
			else if( CheckKey("PlayBackButtons>") ){
				sprintf(t, "%d,%d,%d", m_PlayBackTime[0], m_PlayBackTime[1], m_PlayBackTime[2]);
			}
			else if( (pVal = CheckValKey("PlayBackButtons=*>"))!=NULL ){
				LPSTR pBF = StrDupe(GetMacroStr(as, pVal));
                LPSTR pp, tt;
				pp = StrDlm(tt, pBF); f = GetMacroInt(tt);
                if( f ){ LimitInt(&f, 1, 60); m_PlayBackTime[0]=BYTE(f); }
				pp = StrDlm(tt, pp); f = GetMacroInt(tt);
                if( f ){ LimitInt(&f, 1, 60); m_PlayBackTime[1]=BYTE(f); }
				StrDlm(tt, pp); f = GetMacroInt(tt);
                if( f ){ LimitInt(&f, 1, 60); m_PlayBackTime[2]=BYTE(f); }
                UpdatePlayBack();
                delete pBF;
			}
			else if( (pVal = CheckValKey("CODE=*>"))!=NULL ){
				pVal = GetMacroStr(as, pVal);
                if( sscanf(pVal, "%X", &f) == 1 ){
	                char c = char(f >> 8);
					if( (f < 0x0100) || m_RxSet[0].m_MBCS.IsLead(c) ){
						if( c ) *t++ = c;
           	            *t++ = char(f);
               	        *t = 0;
                    }
                }
            }
			else if( (pVal = CheckValKey("FFTWidth=*>"))!=NULL ){
				f = GetMacroInt(pVal);
				SetFFTWidth(f);
			}
			else if( (pVal = CheckValKey("SyncWidth=*>"))!=NULL ){
                InitCollect(m_RxSet, GetMacroInt(pVal));
			}
			else if( (pVal = CheckValKey("ShowCH=*>"))!=NULL ){
				LPSTR pBF = StrDupe(GetMacroStr(as, pVal));
                LPSTR tt, pp;
				pp = StrDlm(tt, pBF);
				f = GetMacroInt(tt);
				if( (f >= 1) && (f <= RXMAX) ){
					int sw = GetMacroOnOff(pp);
					if( sw >= 0 ) ShowSubChannel(f, sw);
                }
                delete pBF;
			}
			else if( (pVal = CheckValKey("SetCHSpeed=*>"))!=NULL ){
				LPSTR pBF = StrDupe(GetMacroStr(as, pVal));
                LPSTR tt, pp;
				pp = StrDlm(tt, pBF);
				f = GetMacroInt(tt);
				if( (f >= 1) && (f <= RXMAX) ){
					CRxSet *pRxSet = &m_RxSet[f];
					double b = GetMacroDouble(pp);
					if( pRxSet->IsMFSK() ){
						pRxSet->SetMFSKType(MFSK_Speed2Type(b));
                    }
                    else {
						pRxSet->SetSpeed(b);
                    }
				}
                delete pBF;
			}
			else if( (pVal = CheckValKey("SetCHMode=*>"))!=NULL ){
				LPSTR pBF = StrDupe(GetMacroStr(as, pVal));
                LPSTR tt, pp;
				pp = StrDlm(tt, pBF);
				f = GetMacroInt(tt);
				if( (f >= 1) && (f <= RXMAX) ){
					pVal = GetMacroStr(as, pp);
					CRxSet *pRxSet = &m_RxSet[f];
                    f = GetModeIndex(pVal);
                    if( pRxSet->IsActive() && pRxSet->m_pView ){
						pRxSet->m_pView->SetMode(f);
                    }
                    else {
						pRxSet->SetMode(f);
                    }
				}
                delete pBF;
			}
			else if( (pVal = CheckValKey("ClearCHW=*>"))!=NULL ){
				f = GetMacroInt(pVal);
				if( (f >= 1) && (f <= RXMAX) ){
					m_RxSet[f].ClearWindow();
                }
			}
			else if( (pVal = CheckValKey("RepeatTX=*>"))!=NULL ){
				f = GetMacroInt(pVal);
				if( f > 0 ){
					if( f < 100 ) f = 100;
					m_ReqMacroTimer = f;
					r |= macRX | macTX;
					DrawHint();
                }
			}
			else if( (pVal = CheckValKey("Repeat=*>"))!=NULL ){
				f = GetMacroInt(pVal);
				if( f > 0 ){
					if( f < 100 ) f = 100;
					m_ReqMacroTimer = f;
					CreateMacroTimer(f);
                    DrawHint();
                }
            }
			else if( (pVal = CheckValKey("RepeatText=*>"))!=NULL ){
				LPSTR pBF = StrDupe(GetMacroValue(pVal));
                LPSTR tt, pp;
				pp = StrDlm(tt, pBF);
				f = GetMacroInt(tt);
				if( f > 0 ){
					if( f > 100 ) f = 100;
					for( ; f; f-- ){
						ConvMacro_(t, pp, pButton); t += strlen(t);
                    }
                }
                delete pBF;
            }
			else if( (pVal = CheckValKey("MODE=*>"))!=NULL ){
				pVal = GetMacroStr(as, pVal);
				SetMode(GetModeIndex(pVal));
			}
			else if( (pVal = CheckValKey("Execute=*>"))!=NULL ){
				CWaitCursor w;
				pVal = GetMacroStr(as, pVal);
            	if( *pVal ){
					if( (pVal[1] == ':')||(pVal[0]=='\\') ){
						char bf[256];
    	                SetDirName(bf, pVal);
        	            if( bf[0] ){
							::SetCurrentDirectory(bf);
                	    }
                    }
                    else {
						::SetCurrentDirectory(sys.m_BgnDir);
                    }
                    ::WinExec(pVal, SW_SHOW);
                }
			}
			else if( (pVal = CheckValKey("Shell=*>"))!=NULL ){
				CWaitCursor w;
				LPSTR pBF = StrDupe(GetMacroStr(as, pVal));
                LPSTR tt, pp;
				pp = StrDlm(tt, pBF);
				LPCSTR pDoc = GetMacroStr(as, tt);
                AnsiString ts;
				pVal = GetMacroStr(ts, pp);
            	if( *pDoc ){
					char bf[256];
					LPSTR pDefDir = bf;
					if( (pDoc[1] == ':')||(pDoc[0]=='\\') ){
    	                SetDirName(pDefDir, pDoc);
                    }
                    else {
						pDefDir = NULL;
                    }
                    if( !*pVal ) pVal = NULL;
					::ShellExecute(Handle, "open", pDoc, pVal, pDefDir, SW_SHOWDEFAULT);
                }
                delete pBF;
			}
			else if( (pVal = CheckValKey("YesNo=*>"))!=NULL ){
				if( pButton && pButton->Down ) pButton->Down = FALSE;
				DettachFocus();
				f = YesNoMB(GetMacroStr(as, pVal));
                AttachFocus();
                sprintf(t, "%d", f);
			}
			else if( (pVal = CheckValKey("YesNoCancel=*>"))!=NULL ){
				if( pButton && pButton->Down ) pButton->Down = FALSE;
				DettachFocus();
				f = YesNoCancelMB(GetMacroStr(as, pVal));
                AttachFocus();
                sprintf(t, "%d", f);
			}
			else if( (pVal = CheckValKey("OkCancel=*>"))!=NULL ){
				if( pButton && pButton->Down ) pButton->Down = FALSE;
				DettachFocus();
				f = OkCancelMB(GetMacroStr(as, pVal));
                AttachFocus();
                sprintf(t, "%d", f);
			}
			else if( (pVal = CheckValKey("Error=*>"))!=NULL ){
				if( pButton && pButton->Down ) pButton->Down = FALSE;
				DettachFocus();
				ErrorMB(GetMacroStr(as, pVal));
                AttachFocus();
			}
			else if( (pVal = CheckValKey("Warning=*>"))!=NULL ){
				if( pButton && pButton->Down ) pButton->Down = FALSE;
				DettachFocus();
				WarningMB(GetMacroStr(as, pVal));
                AttachFocus();
			}
			else if( (pVal = CheckValKey("Message=*>"))!=NULL ){
				if( pButton && pButton->Down ) pButton->Down = FALSE;
				DettachFocus();
				InfoMB(GetMacroStr(as, pVal));
                AttachFocus();
			}
			else if( (pVal = CheckValKey("WaterMsg=*>"))!=NULL ){
				LPSTR pBF = StrDupe(GetMacroValue(pVal));
                LPSTR pp, tt;
                pp = SkipSpace(StrDlm(tt, pBF));
                int pos = 4;
				if( *pp ){
					sscanf(GetMacroStr(as, tt), "%d", &pos);
                    if( pos < 0 ) pos = 0;
                    if( pos > 4 ) pos = 4;
                }
                else {
					pp = tt;
                }
				if( *pp ) SetInfoMsg(GetMacroStr(as, pp), pos);
                delete pBF;
			}
			else if( (pVal = CheckValKey("RxStatus=*>"))!=NULL ){
				RxStatus(&m_RxSet[0], GetMacroStr(as, pVal));
            }
			else if( CheckKey("SetFocus>") ){
				PCTX->SetFocus();
            }
			else if( CheckKey("Menu>") ){
				sprintf(t, "%d", m_MacroMenuNo);
            }
			else if( (pVal = CheckValKey("Menu=*>"))!=NULL ){
				DoMacroMenu(pVal, pButton, FALSE);
			}
			else if( (pVal = CheckValKey("MenuB=*>"))!=NULL ){
				DoMacroMenu(pVal, pButton, TRUE);
            }
			else if( (pVal = CheckValKey("AddMenu=*>"))!=NULL ){
				LPSTR pBF = StrDupe(GetMacroValue(pVal));
                LPSTR pp, tt;
                pp = SkipSpace(StrDlm(tt, pBF));
				AnsiString Title = GetMacroStr(as, tt);
                pp = SkipSpace(StrDlm(tt, pp));
                pVal = GetMacroStr(as, tt);
//				if( !Title.IsEmpty() && *pVal && (*pp || !strcmp(pVal, "-")) ){
				if( !Title.IsEmpty() ){
					AddExtensionMenu(Title.c_str(), pVal, pp);
                }
                delete pBF;
			}
			else if( (pVal = CheckValKey("InsertMenu=*>"))!=NULL ){
				LPSTR pBF = StrDupe(GetMacroValue(pVal));
                LPSTR pp, tt;
                pp = SkipSpace(StrDlm(tt, pBF));
				AnsiString Title = GetMacroStr(as, tt);
                pp = SkipSpace(StrDlm(tt, pp));
				AnsiString Pos = GetMacroStr(as, tt);
                pp = SkipSpace(StrDlm(tt, pp));
                pVal = GetMacroStr(as, tt);
				if( !Title.IsEmpty() && !Pos.IsEmpty() && *pVal && (*pp || !strcmp(pVal, "-")) ){
					InsExtensionMenu(Title.c_str(), Pos.c_str(), pVal, pp);
                }
                delete pBF;
			}
			else if( (pVal = CheckValKey("ShowMenu=*>"))!=NULL ){
				TMenuItem *pMenu = GetMenuArg(as, pVal, TRUE);
                if( pMenu ){
					f = GetMacroOnOff(as.c_str());
					switch(f){
						case 0:
                        case 1:
                        	pMenu->Visible = f;
                            break;
                        case 2:
							pMenu->Visible = !pMenu->Visible;
                            break;
                    }
                }
			}
			else if( (pVal = CheckValKey("EnableMenu=*>"))!=NULL ){
				TMenuItem *pMenu = GetMenuArg(as, pVal, TRUE);
                if( pMenu ){
					f = GetMacroOnOff(as.c_str());
					switch(f){
						case 0:
                        case 1:
                        	pMenu->Enabled = f;
                            break;
                        case 2:
							pMenu->Enabled = !pMenu->Enabled;
                            break;
                    }
                }
			}
			else if( (pVal = CheckValKey("ShortCut=*>"))!=NULL ){
				TMenuItem *pMenu = GetMenuArg(as, pVal, TRUE);
                if( pMenu ) ShortCutExtensionMenu(pMenu, as.c_str());
			}
			else if( (pVal = CheckValKey("CheckMenu=*>"))!=NULL ){
				TMenuItem *pMenu = GetMenuArg(as, pVal, TRUE);
                if( pMenu ){
					f = GetMacroOnOff(as.c_str());
					switch(f){
						case 0:
                        case 1:
                        	pMenu->Checked = f;
                        	break;
                        case 2:
							pMenu->Checked = !pMenu->Checked;
                        	break;
                    }
                }
			}
			else if( (pVal = CheckValKey("DeleteMenu=*>"))!=NULL ){
				TMenuItem *pMenu = GetMenuArg(as, pVal, FALSE);
				if( pMenu ){
	                TMenuItem *pMainMenu = pMenu->Parent;
    	            if( pMainMenu ){
						if( (pMenu == KF)||(pMenu == KE)||(pMenu == KV)||(pMenu == KO)||(pMenu == KH) ){
							if( !sys.m_MacroError ){
								sys.m_MacroError = TRUE;
								InfoMB("[%s] is not deleted...", pMenu->Caption.c_str());
                            }
						}
						else {
							pMainMenu->Delete(pMainMenu->IndexOf(pMenu));
				        }
            	    }
                }
            }
			else if( (pVal = CheckValKey("TableCount=*>"))!=NULL ){
				LPSTR pBF = StrDupe(GetMacroStr(as, pVal));
                LPSTR tt, p;
				int n = 0;
                p = pBF;
                while(*p){
					p = StrDlm(tt, SkipSpace(p));
                    n++;
                }
				sprintf(t, "%d", n);
                delete pBF;
			}
			else if( (pVal = CheckValKey("TableStr=*>"))!=NULL ){
				LPSTR pBF = new char[8192];
                StrCopy(pBF, GetMacroStr(as, pVal), 8191);
                LPSTR tt, p;
				p = StrDlm(tt, pBF);
                f = GetMacroInt(tt);
                if( f > 0 ){
					strcpy(p, GetMacroStr(as, p));
	                int n = 1;
    	            while(*p){
						p = StrDlm(tt, SkipSpace(p));
						if( n == f ){
							strcpy(t, GetMacroStr(as, tt));
                    	    break;
	                    }
    	                n++;
        	        }
                }
                delete pBF;
			}
			else if( CheckKey("Events>") ){
				for( f = 0; f < macOnEnd; f++ ){
					if( f ) *t++ = ',';
					strcpy(t, g_tMacEvent[f]); t += strlen(t);
                }
			}
			else if( (pVal = CheckValKey("Table=*>"))!=NULL ){
				LPSTR pBF = new char[8192];
                StrCopy(pBF, GetMacroStr(as, pVal), 8191);
                LPSTR tt, p;
				p = StrDlm(tt, pBF);
                pVal = GetMacroStr(as, tt);
                AnsiString ts;
				strcpy(p, GetMacroStr(ts, p));
                int n = 1;
                while(*p){
					p = StrDlm(tt, SkipSpace(p));
					if( !strcmpi(pVal, tt) ){
						sprintf(t, "%d", n);
                        n = -1;
                        break;
                    }
                    n++;
                }
                if( n >= 0 ) strcpy(t, "0");
                delete pBF;
            }
			else if( (pVal = CheckValKey("Input=*>"))!=NULL ){
				if( pButton && pButton->Down ) pButton->Down = FALSE;
				DettachFocus();
                LPSTR pBF = StrDupe(GetMacroStr(as, pVal));
                LPSTR tt, pp;
                pp = SkipSpace(StrDlm(tt, pBF));
                AnsiString rs = *pp ? GetMacroStr(as, pp) : "";
				if( InputMB("MMVARI", GetMacroStr(as, tt), rs) ){
        	        ConvMacro_(m_MacroInput, rs.c_str(), pButton);
					strcpy(t, m_MacroInput.c_str());
                }
                delete pBF;
                AttachFocus();
			}
			else if( (pVal = CheckValKey("ButtonContents=*>"))!=NULL ){
				LPSTR pBF = StrDupe(GetMacroValue(pVal));
                LPSTR tt, p;
				p = SkipSpace(StrDlm(tt, pBF));
				f = GetMacroInt(tt);
				if( (f >= 1) && (f <= MACBUTTONALL) ){
                	f--;
                    MACBUTTON *pList = &m_tMacButton[f];
	            	as = p;
					Yen2CrLf(pList->Text, as);
                    if( pList->pButton ){
   		                pList->pButton->Font->Color = pList->Text.IsEmpty() ? clGrayText : pList->Color;
                    }
                }
                delete pBF;
			}
			else if( (pVal = CheckValKey("ButtonName=*>"))!=NULL ){
				LPSTR pBF = StrDupe(GetMacroValue(pVal));
                LPSTR tt, p;
				p = SkipSpace(StrDlm(tt, pBF));
				f = GetMacroInt(tt);
				if( (f >= 1) && (f <= MACBUTTONALL) ){
                	f--;
                    MACBUTTON *pList = &m_tMacButton[f];
                    pList->Name = GetMacroStr(as, p);
					if( pList->pButton ){
						pList->pButton->Caption = ConvAndChar(m_TextBuff, pList->Name.c_str());
                    }
                }
                delete pBF;
			}
			else if( (pVal = CheckValKey("EditMacro=*>"))!=NULL ){
				if( pButton && pButton->Down ) pButton->Down = FALSE;
				LPCSTR pp = GetMacroStr(as, pVal);
                if( !strcmpi(GetEXT(pp), "TXT") ){
					DettachFocus();
					TMacEditDlg *pBox = new TMacEditDlg(this);
					pBox->Execute(pp);
   		            delete pBox;
       		        AttachFocus();
                }
                else if( !strcmp(pp, "AS(CW)") ){
					DettachFocus();
					TMacEditDlg *pBox = new TMacEditDlg(this);
					if( pBox->Execute(sys.m_AS, "AS(CW)") ){
                       	AdjustAS(&sys.m_AS);
                    }
                    delete pBox;
       		        AttachFocus();
				}
                else if( (f = FindStringTableStrictly(g_tMacEvent, pp, macOnEnd)) != 0 ){
					DettachFocus();
					TMacEditDlg *pBox = new TMacEditDlg(this);
					if( pBox->Execute(sys.m_MacEvent[f], g_tMacEvent[f]) ){
                       	AdjustAS(&sys.m_MacEvent[f]);
                    }
                    delete pBox;
       		        AttachFocus();
                }
                else {
					f = GetMacroInt(pVal);
					if( (f >= 1) && (f <= MACBUTTONALL) ){
        	        	f--;
            	        MACBUTTON *pList = &m_tMacButton[f];
						DettachFocus();
						TMacEditDlg *pBox = new TMacEditDlg(this);
						if( pBox->Execute(pList, f) ){
        	                if( pList->pButton ){
					           	pList->pButton->Caption = ConvAndChar(m_TextBuff, pList->Name.c_str());
    		    	            pList->pButton->Font->Color = pList->Text.IsEmpty() ? clGrayText : pList->Color;
								TFontStyles fs = Code2FontStyle(pList->Style);
        					    pList->pButton->Font->Style = fs;
	                        }
		                }
    		            delete pBox;
        		        AttachFocus();
                	}
                }
			}
			else if( (pVal = CheckValKey("DoMacro=*>"))!=NULL ){
				f = GetMacroInt(pVal);
				if( (f >= 1) && (f <= MACBUTTONALL) ){
					::PostMessage(Handle, WM_WAVE, waveDoMacro, f-1);
                }
			}
			else if( (pVal = CheckValKey("DoEvent=*>"))!=NULL ){
				pVal = GetMacroStr(as, pVal);
				f = FindStringTable(g_tMacEvent, pVal, macOnEnd);
                if( f >= 0 ) DoEvent(f);
			}
			else if( (pVal = CheckValKey("DoButton=*>"))!=NULL ){
				if( pButton && pButton->Down ) pButton->Down = FALSE;
                ClickButton(FindButton(this, GetMacroStr(as, pVal), pButton, TRUE));
            }
			else if( (pVal = CheckValKey("DoMenu=*>"))!=NULL ){
				if( pButton && pButton->Down ) pButton->Down = FALSE;
				TMenuItem *pMenu = GetMenuArg(as, pVal, FALSE);
                if( pMenu && pMenu->Enabled ) pMenu->Click();
			}
			else if( (pVal = CheckValKey("HEX2DEC=*>"))!=NULL ){
				pVal = GetMacroStr(as, pVal);
				if( *pVal ){
					sprintf(t, "%u", htoin(pVal, -1));
                }
			}
			else if( (pVal = CheckValKey("CHARX=*>"))!=NULL ){
				f = *pVal++;
				if( m_RxSet[0].m_MBCS.IsLead(BYTE(f)) ){
					f = f << 8;
                    f |= *pVal;
                }
               	sprintf(t, "%02X", f); t += strlen(t);
			}
			else if( (pVal = CheckValKey("CHAR=*>"))!=NULL ){
				f = *pVal++;
				if( m_RxSet[0].m_MBCS.IsLead(BYTE(f)) ){
					f = f << 8;
                    f |= *pVal;
                }
               	sprintf(t, "%u", f); t += strlen(t);
			}
			else if( (pVal = CheckValKey("KeyStroke=*>"))!=NULL ){
				pVal = GetMacroStr(as, pVal);
				f = htoin(pVal, -1);
				keybd_event(BYTE(f), 0, 0, 0);
				keybd_event(BYTE(f), 0, KEYEVENTF_KEYUP, 0);
			}
			else if( (pVal = CheckValKey("KeyDown=*>"))!=NULL ){
				pVal = GetMacroStr(as, pVal);
				f = htoin(pVal, -1);
				keybd_event(BYTE(f), 0, 0, 0);
			}
			else if( (pVal = CheckValKey("KeyUp=*>"))!=NULL ){
				pVal = GetMacroStr(as, pVal);
				f = htoin(pVal, -1);
				keybd_event(BYTE(f), 0, KEYEVENTF_KEYUP, 0);
			}
			else if( (pVal = CheckValKey("Cond=*>"))!=NULL ){
            	f = GetDataConds(SkipSpace(pVal), FALSE, pButton);
				sprintf(t, "%d", f);
			}
			else if( (pVal = CheckValKey("ATCSpeed=*>"))!=NULL ){
				f = GetMacroInt(pVal);
				if( (f >= 0) && (f <= 6) ){
					SetATCSpeed(f);
                }
			}
			else if( (pVal = CheckValKey("ATCLevel=*>"))!=NULL ){
				f = GetMacroInt(pVal);
				if( (f >= 3) && (f <= 40) ){
					m_ATCLevel = f;
                }
			}
			else if( (pVal = CheckValKey("ATCLimit=*>"))!=NULL ){
				f = GetMacroInt(pVal);
				if( (f > 1000) && (f <= 100000) ){
					SetATCLimit(f);
                }
			}
			else if( CheckKey("TxShift>") ){
				StrDbl(t, m_ModFSK.m_RTTYShift);
            }
			else if( CheckKey("RxShift>") ){
				StrDbl(t, m_RxSet[0].m_pDem->m_RTTYShift);
            }
			else if( (pVal = CheckValKey("TxShift=*>"))!=NULL ){
				double d = GetMacroDouble(pVal);
                if( (d >= 10.0) && (d <= 450.0) ){
					m_ModFSK.SetRTTYShift(d);
#if DEBUG
					m_ModTest.SetRTTYShift(d);
#endif
                }
			}
			else if( (pVal = CheckValKey("RxShift=*>"))!=NULL ){
				double d = GetMacroDouble(pVal);
                if( (d >= 10.0) && (d <= 450.0) ){
					m_RxSet[0].m_pDem->SetRTTYShift(d);
                }
			}
			else if( CheckKey("RTTYWaitC>") ){
				sprintf(t, "%d", m_ModFSK.m_Encode.GetWaitC());
			}
			else if( CheckKey("RTTYWaitD>") ){
				sprintf(t, "%d", m_ModFSK.m_Encode.GetWaitD());
			}
			else if( (pVal = CheckValKey("RTTYWaitC=*>"))!=NULL ){
				f = GetMacroInt(pVal);
				if( (f >= 0) && (f <= 100) ){
					m_ModFSK.m_Encode.SetWaitC(f);
#if DEBUG
					m_ModTest.m_Encode.SetWaitC(f);
#endif
                }
			}
			else if( (pVal = CheckValKey("RTTYWaitD=*>"))!=NULL ){
				f = GetMacroInt(pVal);
				if( (f >= 0) && (f <= 100) ){
					m_ModFSK.m_Encode.SetWaitD(f);
#if DEBUG
					m_ModTest.m_Encode.SetWaitD(f);
#endif
                }
			}
			else if( (pVal = CheckValKey("RTTYWordOut=*>"))!=NULL ){
				f = GetMacroOnOff(pVal);
                switch(f){
					case 0:
                    case 1:
		                m_fRttyWordOut = f;
                        break;
                    case 2:
						m_fRttyWordOut = !m_fRttyWordOut;
                    	break;
                }
			}
			else if( CheckKey("DIDDLE>") ){
				pVal = m_ModFSK.m_Encode.GetDiddle() == diddleLTR ? "LTR":"BLK";
				strcpy(t, pVal);
			}
			else if( (pVal = CheckValKey("DIDDLE=*>"))!=NULL ){
				pVal = GetMacroStr(as, pVal);
                if( !strcmpi(pVal, "LTR") ){
					m_ModFSK.m_Encode.SetDiddle(diddleLTR);
                }
                else if( !strcmpi(pVal, "BLK") ){
					m_ModFSK.m_Encode.SetDiddle(diddleBLK);
                }
			}
			else if( CheckKey("RTTYDEM>") ){
				pVal = m_RxSet[0].m_RTTYFFT ? "FFT":"IIR";
				strcpy(t, pVal);
			}
			else if( (pVal = CheckValKey("RTTYDEM=*>"))!=NULL ){
				pVal = GetMacroStr(as, pVal);
                if( !strcmpi(pVal, "FFT") ){
					SetRTTYFFT(TRUE);
                }
                else if( !strcmpi(pVal, "IIR") ){
					SetRTTYFFT(FALSE);
                }
			}
			else if( (pVal = CheckValKey("UOS=*>"))!=NULL ){
				f = GetMacroOnOff(pVal);
                m_RxSet[0].m_pDem->m_Decode.SetUOS(f);
			}
			else if( CheckKey("MetricMFSK>") ){
				sprintf(t, "%d", m_RxSet[0].m_pDem->GetMFSKMetric(0));
            }
			else if( (pVal = CheckValKey("MetricMFSK=*>"))!=NULL ){
				pVal = GetMacroStr(as, pVal);
                f = 0;
				if( !strcmpi(pVal, "EVEN") ){
					f = 1;
                }
                else if( !strcmpi(pVal, "ODD") ){
					f = 2;
                }
				sprintf(t, "%d", m_RxSet[0].m_pDem->GetMFSKMetric(f));
			}
			else if( CheckKey("COMFSK>") ){
				sprintf(t, "%d", sys.m_bFSKOUT);
            }
			else if( (pVal = CheckValKey("COMFSK=*>"))!=NULL ){
				f = GetMacroOnOff(pVal);
				sys.m_bFSKOUT = f;
				if( m_pCom ) m_pCom->SetFSK(sys.m_bFSKOUT, sys.m_bINVFSK);
			}
			else if( CheckKey("COMFSKINV>") ){
				sprintf(t, "%d", sys.m_bINVFSK);
            }
			else if( (pVal = CheckValKey("COMFSKINV=*>"))!=NULL ){
				f = GetMacroOnOff(pVal);
				sys.m_bINVFSK = f;
				m_pCom->SetFSK(sys.m_bFSKOUT, sys.m_bINVFSK);
			}
			else if( (pVal = CheckValKey("String=*>"))!=NULL ){
				strcpy(t, GetMacroStr(as, pVal));
			}
			else if( (pVal = CheckValKey("CallProc=*>"))!=NULL ){
				LPSTR pBF = StrDupe(GetMacroValue(pVal));
                LPSTR tt;
				pVal = GetMacroValue(StrDlm(tt, SkipSpace(pBF)));
                tt = SkipSpace(tt);
				if( *tt && !isdigit(*tt) ){
					f = m_MacroVal.FindName(tt, _VAL_PROC);
					if( f >= 0 ){
						ConvDummy(as, m_MacroVal.GetString(f), m_MacroVal.GetPara(f), pVal);
						r |= ConvMacro(t, as.c_str(), pButton);
					}
                    else if( !sys.m_MacroError ){
						sys.m_MacroError = TRUE;
						InfoMB("Procedure [%s] is not found...", tt);
                    }
				}
                delete pBF;
			}
			else if( (pVal = CheckValKey("DebugProc=*>"))!=NULL ){
				LPSTR pBF = StrDupe(GetMacroValue(pVal));
                LPSTR tt;
				pVal = GetMacroValue(StrDlm(tt, SkipSpace(pBF)));
                tt = SkipSpace(tt);
				if( *tt && !isdigit(*tt) ){
					f = m_MacroVal.FindName(tt, _VAL_PROC);
					if( f >= 0 ){
						ConvDummy(as, m_MacroVal.GetString(f), m_MacroVal.GetPara(f), pVal);
                        strcpy(t, as.c_str());
					}
                    else if( !sys.m_MacroError ){
						sys.m_MacroError = TRUE;
						InfoMB("Procedure [%s] is not found...", tt);
                    }
				}
                delete pBF;
			}
			else if( (pVal = CheckValKey("VALTIME=*>"))!=NULL ){
				LPSTR pBF = StrDupe(GetMacroStr(as, pVal));
                LPSTR tt;
				pVal = StrDlm(tt, SkipSpace(pBF));
                CalcI(f, pVal);
				GPS2SystemTime(f, &now);
				if( !strcmpi(tt, "YEAR") ){
					sprintf(t, "%04u", now.wYear);
                }
                else if( !strcmpi(tt, "MONTH") ){
					sprintf(t, "%u", now.wMonth);
                }
                else if( !strcmpi(tt, "DAY") ){
					sprintf(t, "%u", now.wDay);
                }
                else if( !strcmpi(tt, "HOUR") ){
					sprintf(t, "%02u", now.wHour);
                }
                else if( !strcmpi(tt, "MINUTE") ){
                   	sprintf(t, "%02u", now.wMinute);
                }
                else if( !strcmpi(tt, "SECOND") ){
					sprintf(t, "%02u", now.wSecond);
				}
                else if( !strcmp(tt, "LMonth") ){
					strcpy(t, MONT2[now.wMonth]);
                }
                else if( !strcmp(tt, "LMONTH") ){
					strcpy(t, MONT1[now.wMonth]);
                }
                else if( !strcmpi(tt, "ALL") ){
					sprintf(t, "%04u,%02u,%02u,%02u,%02u,%02u,%s,%s",
                    	now.wYear,now.wMonth,now.wDay,
                        now.wHour,now.wMinute,now.wSecond,
                        MONT2[now.wMonth],MONT1[now.wMonth]
                    );
                }
                else {
                    strcpy(t, "<%VALTIME=...> year/month/day/hour/minute/second/LMonth/LMONTH/ALL");
                }
                delete pBF;
            }
			else if( (pVal = CheckValKey("Inv=*>"))!=NULL ){
				sprintf(t, "%d", GetMacroInt(pVal)==0);
			}
			else if( (pVal = CheckValKey("Sin=*>"))!=NULL ){
               	double d;
				Calc(d, GetMacroStr(as, pVal));
				sprintf(t, "%.16lG", sin(d));
			}
			else if( (pVal = CheckValKey("Cos=*>"))!=NULL ){
               	double d;
				Calc(d, GetMacroStr(as, pVal));
				sprintf(t, "%.16lG", cos(d));
			}
			else if( (pVal = CheckValKey("Tan=*>"))!=NULL ){
               	double d;
				Calc(d, GetMacroStr(as, pVal));
				sprintf(t, "%.16lG", tan(d));
			}
			else if( (pVal = CheckValKey("ArcTan=*>"))!=NULL ){
               	double d;
				Calc(d, GetMacroStr(as, pVal));
				sprintf(t, "%.16lG", atan(d));
			}
			else if( (pVal = CheckValKey("Exp=*>"))!=NULL ){
               	double d;
				Calc(d, GetMacroStr(as, pVal));
				sprintf(t, "%.16lG", exp(d));
			}
			else if( (pVal = CheckValKey("Log=*>"))!=NULL ){
               	double d;
				Calc(d, GetMacroStr(as, pVal));
				sprintf(t, "%.16lG", log(d));
			}
			else if( (pVal = CheckValKey("Sqrt=*>"))!=NULL ){
               	double d;
				Calc(d, GetMacroStr(as, pVal));
				sprintf(t, "%.16lG", sqrt(d));
			}
			else if( (pVal = CheckValKey("Pow=*>"))!=NULL ){
				LPSTR pBF = StrDupe(GetMacroStr(as, pVal));
				LPSTR pp, tt;
                pp = StrDlm(tt, pBF);
                double x, y;
				Calc(x, GetMacroStr(as, tt));
				Calc(y, GetMacroStr(as, pp));
				sprintf(t, "%.16lG", pow(x, y));
                delete pBF;
			}
			else if( (pVal = CheckValKey("Floor=*>"))!=NULL ){
               	double d;
				Calc(d, GetMacroStr(as, pVal));
				sprintf(t, "%d", int(floor(d)));
			}
			else if( (pVal = CheckValKey("Format=*>"))!=NULL ){
				LPSTR pBF = StrDupe(GetMacroStr(as, pVal));
                LPSTR tt;
				pVal = StrDlm(tt, SkipSpace(pBF));
				if( strchr(tt, 'c') ){
	                pVal = GetMacroStr(as, pVal);
					sprintf(t, tt, *pVal);
                }
				else if( strchrl(tt, "spn") ){
	                pVal = GetMacroStr(as, pVal);
					sprintf(t, tt, pVal);
                }
				else if( strchrl(tt, "eEfgG") ){
                	double d;
                    Calc(d, pVal);
					sprintf(t, tt, d);
                }
                else {
					CalcI(f, pVal);
					sprintf(t, tt, f);
                }
                delete pBF;
			}
			else if( (pVal = CheckValKey("Find$=*>"))!=NULL ){
				LPSTR pBF = StrDupe(GetMacroStr(as, pVal));
                LPSTR tt, pp;
                pp = StrDlm(tt, pBF);
                if( (pVal=strstr(GetMacroStr(as, pp), tt)) != NULL ){
					strcpy(t, pVal);
                }
                delete pBF;
			}
			else if( (pVal = CheckValKey("Skip$=*>"))!=NULL ){
				LPSTR pBF = StrDupe(GetMacroStr(as, pVal));
                LPSTR tt;
				pVal = GetMacroStr(as, StrDlm(tt, SkipSpace(pBF)));
                f = GetMacroInt(tt);
				int l;
				for( l = 0; (l < f) && *pVal; l++, pVal++ );
                strcpy(t, pVal);
                delete pBF;
			}
			else if( (pVal = CheckValKey("MSkip$=*>"))!=NULL ){
				LPSTR pBF = StrDupe(GetMacroStr(as, pVal));
                LPSTR tt;
				pVal = GetMacroStr(as, StrDlm(tt, SkipSpace(pBF)));
                f = GetMacroInt(tt);
				int l;
				for( l = 0; (l < f) && *pVal; l++, pVal++ ){
					if( m_RxSet[0].m_MBCS.IsLead(*pVal) ){
                    	pVal++;
                        if( !*pVal ) break;
                    }
                }
                strcpy(t, pVal);
                delete pBF;
			}
			else if( (pVal = CheckValKey("StrLen=*>"))!=NULL ){
				sprintf(t, "%d", strlen(GetMacroStr(as, pVal)));
			}
			else if( (pVal = CheckValKey("MStrLen=*>"))!=NULL ){
				sprintf(t, "%d", _mbslen((const unsigned char *)GetMacroStr(as, pVal)));
			}
			else if( CheckKey("CodePage>") ){
				sprintf(t, "%d", GetACP());
            }
			else if( CheckKey("LanguageID>") ){
				sprintf(t, "%d", sys.m_wLang);
            }
			else if( CheckKey("Font>") ){
				sprintf(t, "%s,%d,%d", PCRX->Font->Name.c_str(), PCRX->Font->Size, PCRX->Font->Charset);
			}
			else if( (pVal = CheckValKey("Font=*>"))!=NULL ){
				LPSTR pBF = StrDupe(GetMacroStr(as, pVal));
                LPSTR tt, pp;
				pp = StrDlm(tt, SkipSpace(pBF));
				pVal = GetMacroStr(as, tt);
                if( *pVal ){
					PCRX->Font->Name = pVal;
                }
                pp = StrDlm(tt, pp);
                tt = SkipSpace(tt);
				if( *tt ){
                    f = GetMacroInt(tt);
					if( (f >= 1) && (f < 100) ){
						PCRX->Font->Size = f;
                    }
                }
                pp = StrDlm(tt, pp);
                tt = SkipSpace(tt);
				if( *tt ){
					PCRX->Font->Charset = BYTE(GetMacroInt(tt));
                }
				DettachFocus();
			    PCTX->Font->Name = PCRX->Font->Name;
		    	PCTX->Font->Charset = PCRX->Font->Charset;
			    PCTX->Font->Size = PCRX->Font->Size;
			    OnFontChange(FALSE);
			    OnFontChange(TRUE);
			    UpdateUI();
		    	AttachFocus();
                delete pBF;
			}
			else if( CheckKey("WaterNoise>") ){
				double db = m_StgWater.Sum;
                if( m_FFT.m_FFTGain ) db = SqrtToDB(db);
				sprintf(t, "%.0lf", (db*0.01) + 5.0);
			}
			else if( (pVal = CheckValKey("WaterNoise=*>"))!=NULL ){
				LPSTR pBF = StrDupe(GetMacroStr(as, pVal));
                LPSTR pp, tt;
                pp = StrDlm(tt, pBF);
				f = GetMacroInt(tt);
                if( (f > 0) && (f <= 100) ) m_WaterNoiseL = f;
                if( *pp ){
					f = GetMacroInt(pp);
    	            if( (f > 0) && (f <= 100) ) m_WaterNoiseH = f;
                }
                InitWater(iniwLIMIT);
                delete pBF;
			}
			else if( CheckKey("WaterLevels>") ){
				for( f = 0; f < AN(m_tWaterLevel); f++ ){
					if( f ) *t++ = ',';
					sprintf(t, "%d", m_tWaterLevel[f]); t += strlen(t);
                }
            }
			else if( (pVal = CheckValKey("WaterLevels=*>"))!=NULL ){
				LPSTR pBF = StrDupe(GetMacroStr(as, pVal));
                LPSTR pp, tt;
                pp = SkipSpace(pBF);
                f = 0;
                while(*pp && (f < AN(m_tWaterLevel))){
	                pp = SkipSpace(StrDlm(tt, pp));
					int d = GetMacroInt(tt);
                    if( d < 0 ) d = 0;
                    if( d > 255 ) d = 255;
                    m_tWaterLevel[f] = d;
                    f++;
                }
				CreateWaterColors();
                delete pBF;
			}
			else if( CheckKey("WaterColors>") ){
				EncodeColors(t, m_tWaterColset, AN(m_tWaterColset));
            }
			else if( (pVal = CheckValKey("WaterColors=*>"))!=NULL ){
				if( DecodeColors(pVal, m_tWaterColset, AN(m_tWaterColset)) ){
					CreateWaterColors();
                }
			}
			else if( CheckKey("SpectrumColors>") ){
				EncodeColors(t, m_tFFTColset, AN(m_tFFTColset));
            }
			else if( (pVal = CheckValKey("SpectrumColors=*>"))!=NULL ){
				DecodeColors(pVal, m_tFFTColset, AN(m_tFFTColset));
			}
			else if( CheckKey("RxColors>") ){
				EncodeColors(t, m_Dump.m_Color, AN(m_Dump.m_Color));
            }
			else if( (pVal = CheckValKey("RxColors=*>"))!=NULL ){
				if( DecodeColors(pVal, m_Dump.m_Color, AN(m_Dump.m_Color)) ){
					PCRX->Color = m_Dump.m_Color[0].c;
                }
			}
			else if( CheckKey("TxColors>") ){
				EncodeColors(t, m_Edit[0].m_Color, AN(m_Edit[0].m_Color));
            }
			else if( (pVal = CheckValKey("TxColors=*>"))!=NULL ){
				if( DecodeColors(pVal, m_Edit[0].m_Color, AN(m_Edit[0].m_Color)) ){
					for( f = 1; f < AN(m_Edit); f++ ){
						for( int i = 0; i < AN(m_Edit[0].m_Color); i++ ){
							m_Edit[f].m_Color[i] = m_Edit[0].m_Color[i];
            	        }
                	}
			    	PCTX->Color = m_Edit[0].m_Color[0].c;
                }
			}
			else if( CheckKey("Capture>") ){
				if( m_Dump.GetWindowCallsign(as) ){
					strcpy(t, as.c_str());
                }
			}
			else if( CheckKey("SetCaptureLimit>") ){
				m_Dump.UpdateCaptureLimit();
			}
			else if( CheckKey("ClearCaptureLimit>") ){
				m_Dump.ClearCaptureLimit();
			}
			else if( (pVal = CheckValKey("SendMessage=*>"))!=NULL ){
				m_MacroInput = "";
				LPSTR pBF = StrDupe(GetMacroStr(as, pVal));
                LPSTR pp, tt;
                pp = StrDlm(tt, pBF);
                if( *tt ){
					HWND hWnd = HWND_BROADCAST;
					UINT uMsg;
					pVal = GetMacroStr(as, tt);
                    if( !strcmpi(pVal, "this") ){
	                    pp = StrDlm(tt, pp);
    	                uMsg = GetMacroInt(tt);
                        hWnd = Handle;
                    }
                    else {
						uMsg = ::RegisterWindowMessage(pVal);
                    }
                    pp = StrDlm(tt, pp);
                    DWORD wParam = GetMacroInt(tt);
                    StrDlm(tt, pp);
                    DWORD lParam = GetMacroInt(tt);
                    f = ::SendMessage(hWnd, uMsg, wParam, lParam);
					char bf[32];
                    sprintf(bf, "%d", f);
                    m_MacroInput = bf;
                }
                delete pBF;
			}
			else if( (pVal = CheckValKey("ShowHTML=*>"))!=NULL ){
				m_WebRef.ShowHTML(GetMacroStr(as, pVal));
			}
			else if( (pVal = CheckValKey("CallDLL=*>"))!=NULL ){
				LPSTR pBF = StrDupe(GetMacroStr(as, pVal));
                LPSTR pp, tt;
                pp = StrDlm(tt, pBF);
                if( *tt ){
					pVal = GetMacroStr(as, tt);
					HANDLE hLib = m_LibDLL.LoadLibrary(pVal);	//ja7ude 0522
					if( hLib ){
						pp = StrDlm(tt, pp);
						pVal = GetMacroStr(as, tt);
						tmmMacro pFunc = (tmmMacro)::GetProcAddress((HINSTANCE)hLib, pVal);
						if( pFunc ){
			                pp = StrDlm(tt, pp);
							pFunc(Handle, t, GetMacroStr(as, tt));
                        }
                    }
                }
                delete pBF;
			}
			else if( CheckKey("OnTimerInterval>") ){
				sprintf(t, "%d", sys.m_OnTimerInterval);
            }
			else if( (pVal = CheckValKey("OnTimerInterval=*>"))!=NULL ){
				f = GetMacroInt(pVal);
				if( f > 0 ){
					sys.m_OnTimerInterval = f;
					if( m_pMacroOnTimer && (m_pMacroOnTimer->Interval != UINT(f)) ){
			    		m_pMacroOnTimer->Interval = f;
                    }
                }
			}
			else if( (pVal = CheckValKey("ONOFF=*>"))!=NULL ){
				strcpy(t, GetMacroInt(pVal) ? "ON":"OFF");
			}
			else if( CheckKey("BuffSize>") ){
				sprintf(t, "%d", sys.m_MacBuffSize);
			}
			else if( (pVal = CheckValKey("BuffSize=*>"))!=NULL ){
				f = GetMacroInt(pVal);
                if( (f >= 512) && (f <= 65536) ){
					sys.m_MacBuffSize = f;
                }
			}
			else if( CheckKey("Memory>") ){
				MEMORYSTATUS mem;
                ::GlobalMemoryStatus(&mem);
                sprintf(t, "%u,%.2lf,%.2lf",
                	mem.dwMemoryLoad,
                    double(mem.dwTotalPhys)/(1024*1024),
                    double(mem.dwAvailPhys)/(1024*1024)
                );
			}
			else if( CheckKey("Folder>") ){
				strcpy(t, sys.m_BgnDir);
			}
			else if( CheckKey("EOF>") ){
				break;
            }
#if DEBUG
			else if( CheckKey("Stack>") ){
				sprintf(t, "%08X", &f);
			}
			else if( CheckKey("ListDLL>") ){
				for( f = 0; f < m_LibDLL.m_Count; f++ ){
					sprintf(t, "%s\r\n", m_LibDLL.m_pBase[f].pName);
                    t += strlen(t);
                }
			}
			else if( CheckKey("AFCCount>") ){
				if( m_RxSet[0].IsMFSK() ){
                	f = m_RxSet[0].m_AFCTimerMFSK;
                }
                else {
                	f = m_RxSet[0].m_pDem->m_AFCCount;
                }
				sprintf(t, "%d", f);
            }
			else if( CheckKey("LostRX>") ){
				m_LostSoundRX = LOSTMSGTIME * int(SAMPFREQ) / m_BufferSize;
	            m_fShowMsg = TRUE;
            }
            else if( CheckKey("Repeat>") ){
				m_pDebugButton = pButton;
            }
            else if( CheckKey("StopRepeat>") ){
				m_pDebugButton = NULL;
            }
            else if( CheckKey("TestSignal>") ){
				if( pButton && pButton->Down ) pButton->Down = FALSE;
				TestSignal();
            }
			else if( CheckKey("TestMod>") ){
				CMODFSK *pMod = new CMODFSK;
				pMod->SetSampleFreq(SAMPFREQ);
				pMod->SetCarrierFreq(1900.0);
				pMod->SetSpeed(m_RxSet[0].m_Speed);
				pMod->SetType(CBMode->ItemIndex);
				for( int i = 0; i < m_BufferSize; i++ ) pMod->Do();
                sprintf(t, "%08X : %d", pMod, pMod->m_DEBUG);
                delete pMod;
			}
			else if( (pVal = CheckValKey("RandMem=*>"))!=NULL ){
				f = GetMacroInt(pVal);
				if( (f > 1) && (f <= 1024*1024) ){
					BYTE *bp = new BYTE[f];
                    BYTE *t = bp;
                    for( int i = 0; i < f; i++ ) *t++ = (BYTE)rand();
                    delete bp;
                }
            }
			else if( CheckKey("ShowTxBPF>") ){
				DettachFocus();
				TFreqDispDlg *pBox = new TFreqDispDlg(this);
				pBox->Execute(m_ModFSK.GetFIR(), 6000, SAMPFREQ);
		        delete pBox;
		        AttachFocus();
            }
			else if( CheckKey("ShowDECM>") ){
				DettachFocus();
				TFreqDispDlg *pBox = new TFreqDispDlg(this);
#if DECFIR
				pBox->Execute(m_Dec2.GetFIR(), 6000, SAMPFREQ);
#else
				pBox->Execute(m_Dec2.GetIIR(), 6000, SAMPFREQ);
#endif
		        delete pBox;
		        AttachFocus();
            }
			else if( CheckKey("ShowDemLPF>") ){
				DettachFocus();
				TFreqDispDlg *pBox = new TFreqDispDlg(this);
				pBox->Execute(&m_RxSet[0].m_pDem->m_OutLPF, 300, SAMPFREQ);
		        delete pBox;
		        AttachFocus();
            }
			else if( CheckKey("ShowNOTCH>") ){
				if( !m_Notches.m_Count ){
					m_Notches.Add(m_NotchFreq);
                }
				DettachFocus();
				TFreqDispDlg *pBox = new TFreqDispDlg(this);
				pBox->Execute(&m_Notches.m_FIR, 3000, m_Notches.m_FIR.GetSampleFreq());
	    	    delete pBox;
	        	AttachFocus();
            }
			else if( CheckKey("ShowMFSK>") ){
                CFIR2 *pH = m_RxSet[0].m_pDem->GetMFSKHIL();
				if( pH ){
					DettachFocus();
					TFreqDispDlg *pBox = new TFreqDispDlg(this);
					pBox->Execute(pH, 3000, DEMSAMPFREQ);
		    	    delete pBox;
		        	AttachFocus();
                }
            }
/*
			else if( CheckKey("ShowPHASEX>") ){
				TFreqDispDlg *pBox = new TFreqDispDlg(this);
				pBox->Execute(&m_RxSet[0].m_pDem->m_PhaseX.m_LPF, 400, DEMSAMPFREQ);
		        delete pBox;
			}
*/
			else if( CheckKey("ShowQPSK1>") ){
				TFreqDispDlg *pBox = new TFreqDispDlg(this);
				pBox->Execute(&m_RxSet[0].m_pDem->m_DecPSK.m_LPF1, 400, DEMSAMPFREQ);
		        delete pBox;
			}
			else if( CheckKey("ShowQPSK2>") ){
				TFreqDispDlg *pBox = new TFreqDispDlg(this);
				pBox->Execute(&m_RxSet[0].m_pDem->m_DecPSK.m_LPF2, 200, DEMSAMPFREQ/QPSK_SUBFACTOR);
		        delete pBox;
			}
			else if( (pVal = CheckValKey("RxFile=*>"))!=NULL ){
				pVal = GetMacroStr(as, pVal);
            	if( *pVal ){
					FILE *fp = fopen(pVal, "rb");
					if( fp ){
						while(1){
		                    int len = fread(m_TextBuff, 1, sizeof(m_TextBuff), fp);
							if( len <= 0 ) break;
							LPCSTR p = m_TextBuff;
    	        	        while(len--){
								m_Dump.PutKey(*p++, 1);
            	        	}
	                    }
    	                fclose(fp);
                    }
                }
			}
			else if( CheckKey("ShowTitle>") ){
	            UpdateTitle();
            }
			else if( (pVal = CheckValKey("SimBAUD=*>"))!=NULL ){
				double b = GetMacroDouble(pVal);
				if( (b >= MIN_SPEED) && (b <= MAX_SPEED) ){
                	m_ModTest.SetSpeed(b);
                }
            }
#endif
            else {
				BOOL f = FALSE;
                for( int i = 0; i < macOnEnd; i++ ){
					char bf[256];
                    sprintf(bf, "%s=*>", g_tMacEvent[i]);
					if( (pVal = CheckValKey(bf))!=NULL ){
						f = TRUE;
		            	as = pVal;
						Yen2CrLf(sys.m_MacEvent[i], as);
                		UpdateMacroOnTimer();
                        break;
                    }
                    else {
	                    sprintf(bf, "%s>", g_tMacEvent[i]);
    	                if( CheckKey(bf) ){
							f = TRUE;
                            DettachFocus();
							TMacEditDlg *pBox = new TMacEditDlg(this);
							if( pBox->Execute(sys.m_MacEvent[i], g_tMacEvent[i]) ){
                            	AdjustAS(&sys.m_MacEvent[i]);
                            }
                            delete pBox;
                            AttachFocus();
                            break;
            	        }
                    }
                }
				if( !f ){
					*t++ = '<';
    	            *t++ = '%';
					strcpy(t, pKey);
                }
            }
            t += strlen(t);
        }
        else {
			if( *p != '\n' ){
				if( (*p == '\r') && cDisCR ){
					cDisCR--;
                    p++;
                }
                else if( (*p == '\t') && fDisTAB ){
					p++;
                }
                else if( (*p == ' ') && fDisSP ){
					p++;
                }
                else {
					*t++ = *p++;
                }
            }
            else {
            	p++;
            }
        }
    }
    *t = 0;
    return r;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::DoMacroReturn(int f)
{
    if( f & macTX ){
		if( !m_TX || m_fTone ) ToTX();
    }
    else if( f & macTXRX ){
		SBTX->Down = !SBTX->Down;
        SBTXClick(NULL);
    }
    else if( f & macTONE ){
		if( !m_TX ) ToTone();
    }
    if( f & macRX ){
		m_fReqRX = TRUE;
		m_ModFSK.m_Encode.m_fReqRX = TRUE;
        m_Edit[m_CurrentEdit].MoveCursor(dmpMoveLAST);
		SetTXCaption();
    }
	if( f & macTXOFF ){
		if( m_TX ) ToRX();
    }
	if( f & macAUTOCLEAR ) m_ReqAutoClear = TRUE;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::DoMacro(LPCSTR pMacro, TSpeedButton *pButton)
{
	int len = strlen(pMacro) + 1;
    if( len < sys.m_MacBuffSize ) len = sys.m_MacBuffSize;
    LPSTR bp = new char[len];
	int f = ConvMacro(bp, pMacro, pButton);
    BOOL fCW = FALSE;
    LPCSTR p = bp;
	if( f & macRX ) m_Edit[m_CurrentEdit].MoveCursor(dmpMoveLAST);
	for( f; *p; p++ ){
		if( (BYTE(*p) == button1ST) && (BYTE(*(p+1)) == button2ND) && (*(p+2)) ){
			p+=2;
			switch(*p){
				case buttonCWON:
                	fCW = TRUE;
                	break;
                case buttonCWOFF:
                	fCW = FALSE;
                    break;
                case buttonMOVETOP:
   					m_Edit[m_CurrentEdit].MoveCursor(dmpMoveTOP);
					break;
                case buttonMOVEEND:
   					m_Edit[m_CurrentEdit].MoveCursor(dmpMoveLAST);
					break;
				case buttonIDLE:
					m_Edit[m_CurrentEdit].PutChar(0x0200, 3);
                	break;
                default:
					m_Edit[m_CurrentEdit].PutKey(button1ST, 1);
					m_Edit[m_CurrentEdit].PutKey(button2ND, 1);
					m_Edit[m_CurrentEdit].PutKey(*p, 1);
                	break;
            }
        }
        else if( fCW ){
			m_Edit[m_CurrentEdit].PutChar(*p+0x0100, 3);
        }
        else {
			m_Edit[m_CurrentEdit].PutKey(*p, 1);
        }
    }
	DoMacroReturn(f);
   	if( pButton && !(f & macSEEK) ){
		pButton->Down = m_ReqMacroTimer != 0;
    }
    delete bp;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SendButton(int n)
{
	if( (n < 0) || (n >= MACBUTTONALL) ) return;

	sys.m_MacroError = FALSE;
	DeleteMacroTimer();
	m_CurrentMacro = n;

	MACBUTTON *pList = &m_tMacButton[n];
	if( pList->Text.IsEmpty() ){
		if( pList->pButton ) pList->pButton->Down = FALSE;
    	return;
    }
	DoMacro(pList->Text.c_str(), pList->pButton);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KOAIClick(TObject *Sender)
{
	InvMenu(KOAI);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KOAOClick(TObject *Sender)
{
	InvMenu(KOAO);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SBInitClick(TObject *Sender)
{
	AnsiString strBand = LogFreq->Text;
	if( Log.IsOpen() ){
		if( SBQSO->Down ){
			Log.DeleteLast();
			Log.m_Find.Clear();
			Log.InitCur();
			SBQSO->Down = FALSE;
			UpdateTextData();
		}
		else {
			Log.InitCur();
			Log.m_sd.call[0] = 0;
			Log.m_sd.name[0] = 0;
			Log.m_sd.qth[0] = 0;
			UpdateTextData();
			UpdateCallsign();
		}
		if( !strBand.IsEmpty() ){
			LogFreq->Text = strBand;
			Log.SetFreq(&Log.m_sd, strBand.c_str());
		}
		HisCallChange(NULL);
		LogLink.Clear();
	    if( HisCall->CanFocus() ) HisCall->SetFocus();
    }
    else {
		HisCall->Text = "";
        HisName->Text = "";
        HisRST->Text = "599";
        MyRST->Text = "599";
		SetTXFocus();
    }
}
//---------------------------------------------------------------------------

void __fastcall TMainVARI::KOClick(TObject *Sender)
{
	KOA->Enabled = !m_TX;
	KORS->Visible = IsRTTY();
    KORSC->Checked = !m_fRttyWordOut;
    KORSW->Checked = m_fRttyWordOut;
    OnMenuProc(KO, "&Options");
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KHAClick(TObject *Sender)
{
	char fname[256];

	BOOL ww = TRUE;
	LPCSTR pName;
    if( Sender == KHA ){
		pName = "eproject.txt";
        if( !sys.m_MsgEng ) pName++;
    }
    else if( Sender == KHO ){
		pName = "emmvari.txt";
        if( !sys.m_MsgEng ) pName++;
    }
    else if( Sender == KHS ){
		pName = "samples.txt";
        ww = FALSE;
    }
    else {
		pName = "ehistory.txt";
        if( !sys.m_MsgEng ) pName++;
    }

	if( !m_pHelp ){
		m_pHelp = new TFileEdit(this);
        m_pHelp->SetEvent(Handle, WM_WAVE, 2);
    }
	if( m_pHelp ){
        m_pHelp->SetWordWrap(ww);
		sprintf(fname, "%s%s", sys.m_BgnDir, pName);
#if DEBUG
		m_pHelp->Execute(fname, FALSE);
#else
		m_pHelp->Execute(fname, TRUE);
#endif
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KHVClick(TObject *Sender)
{
	DettachFocus();
	TVerDspDlg *pBox = new TVerDspDlg(this);
	pBox->ShowModal();
    delete pBox;
    AttachFocus();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::FormPaint(TObject *Sender)
{
	if( m_fInitFirst ){
		m_fInitFirst = FALSE;
	    Draw(FALSE);
		if( sys.m_fShowLangMsg && !sys.m_fBaseMBCS ){
            AnsiString as = "This program was made for MBCS(JA/HL/BV/BY) languages.  "
		                    "SBCS languages are out of scope of this project, and there is no merit to run this program."
                            "\r\n\r\nDo you continue to run it?";
           	if( YesNoMB(as.c_str()) != ID_YES ){
    	    	Close();
        	    return;
            }
	        sys.m_fShowLangMsg = FALSE;
        }
		OpenSound(FALSE);
		int f1stInst = FALSE;
		if( !strcmp(sys.m_CallSign.c_str(), "NOCALL") ){
            f1stInst = TRUE;
        	AnsiString as;
			DettachFocus();
			if( InputMB("MMVARI", sys.m_MsgEng ? "Enter your callsign (for Macros)":"ÉRÅ[ÉãÉTÉCÉìÇì¸óÕÇµÇƒâ∫Ç≥Ç¢", as) && !as.IsEmpty() ){
				strupr(as.c_str());
				sys.m_CallSign = as;
				Log.MakePathName(ClipCall(as.c_str()));
            }
        }
		sprintf(m_TextBuff, "%sARRL.DX", sys.m_BgnDir);
		Cty.Load(m_TextBuff);
		sprintf(m_TextBuff, "%sMMCG.DEF", sys.m_BgnDir);
		mmcg.LoadDef(m_TextBuff);
		if( !f1stInst ) Log.DoBackup();
		Log.Open(NULL, !f1stInst);
		m_LogBand = Log.m_sd.band;
		UpdateLogLink();
		UpdateTextData();
		if( Log.m_sd.btime ) SBQSO->Down = TRUE;
        UpdateTitle();
#if !DEBUG
		srand(::GetTickCount());
#endif
		if( sys.m_fRestoreSubChannel ){
			for( int i = 1; i < RXMAX; i++ ){
				if( m_RxSet[i].m_fShowed ){
					ShowSubChannel(i, TRUE);
	            }
	        }
        }
		DoEvent(macOnStart);
        AttachFocus();
		UpdateMacroOnTimer();
    }
    else {
		DoResume();
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KVCRClick(TObject *Sender)
{
	m_Dump.Clear();
    AttachFocus();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KVCTClick(TObject *Sender)
{
	m_ReqAutoClear = FALSE;
	m_Edit[m_CurrentEdit].Clear();
	if( m_fReqRX ){
       	m_fReqRX = FALSE;
		m_ModFSK.m_Encode.m_fReqRX = FALSE;
		SetTXCaption();
    }
    AttachFocus();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SBATCMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if( Button == mbRight ){
		if( m_TX != txINTERNAL ){
			m_fDisEvent++;
			EATC->Text = "0";
			m_RxSet[0].m_pDem->m_Decode.SetTmg(0);
            m_RxSet[0].m_pDem->m_Decode.ClearLPF();
        	m_fDisEvent--;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::CreateWaterColors(void)
{
	int i, O, W;
    UCOL c;

    W = m_tWaterLevel[0];
	for( i = 0; i < W; i++ ){
		c = GetGrade2(m_tWaterColset, i, W);
        m_tWaterColors[i] = c.c;
    }
	UCOL col[2];
	W = m_tWaterLevel[1] - m_tWaterLevel[0];
    O = m_tWaterLevel[0];
    col[0].c = m_tWaterColset[1].c;
    col[1].c = m_tWaterColset[6].c;
    for( i = 0; i < W; i++ ){
		c = GetGrade2(col, i, W);
        m_tWaterColors[i+O] = c.c;
    }
	W = m_tWaterLevel[2] - m_tWaterLevel[1];
    O = m_tWaterLevel[1];
    col[0].c = m_tWaterColset[6].c;
    col[1].c = m_tWaterColset[7].c;
    for( i = 0; i < W; i++ ){
		c = GetGrade2(col, i, W);
        m_tWaterColors[i+O] = c.c;
    }
	W = m_tWaterLevel[3] - m_tWaterLevel[2];
    O = m_tWaterLevel[2];
    col[0].c = m_tWaterColset[7].c;
    col[1].c = m_tWaterColset[8].c;
    for( i = 0; i < W; i++ ){
		c = GetGrade2(col, i, W);
        m_tWaterColors[i+O] = c.c;
    }
	W = m_tWaterLevel[4] - m_tWaterLevel[3];
    O = m_tWaterLevel[3];
    col[0].c = m_tWaterColset[8].c;
    col[1].c = m_tWaterColset[9].c;
    for( i = 0; i < W; i++ ){
		c = GetGrade2(col, i, W);
        m_tWaterColors[i+O] = c.c;
    }
	W = m_tWaterLevel[5] - m_tWaterLevel[4];
    O = m_tWaterLevel[4];
    col[0].c = m_tWaterColset[9].c;
    col[1].c = m_tWaterColset[10].c;
    for( i = 0; i < W; i++ ){
		c = GetGrade2(col, i, W);
        m_tWaterColors[i+O] = c.c;
    }
	W = 256 - m_tWaterLevel[5];
    O = m_tWaterLevel[5];
    col[0].c = m_tWaterColset[10].c;
    col[1].c = m_tWaterColset[11].c;
    for( i = 0; i < W; i++ ){
		c = GetGrade2(col, i, W);
        m_tWaterColors[i+O] = c.c;
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::DrawStatusBar(const TRect &Rect, LPCSTR pText, TColor col)	//JA7UDE 0428
{
	if( (WindowState != wsMinimized) && Rect.Right ){
		TCanvas *pCanvas = StatusBar->Canvas;
		pCanvas->Brush->Color = col;
		pCanvas->TextRect(Rect, Rect.Left+1, Rect.Top+1, pText);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::DrawHint(void)
{
	DrawStatus(statusHint, GetHintStatus(m_HintKey.c_str()));
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::DrawStatus(int n, LPCSTR pText)
{
	if( strcmp(pText, m_strStatus[n].c_str()) ){
		m_strStatus[n] = pText;
        DrawStatus(n);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::DrawStatus(int n)
{
	const TRect &Rect = m_rcStatus[n];	//JA7UDE 0428
	switch(n){
		case statusCOM:
        	{
				TColor col  = clBtnFace;
				if( strcmpi(sys.m_PTTCOM.c_str(), "NONE") && !m_pCom ){
        			col = clRed;
		    	}
    		    DrawStatusBar(Rect, sys.m_PTTCOM.c_str(), col);
            }
        	break;
        default:
    	    DrawStatusBar(Rect, m_strStatus[n].c_str(), clBtnFace);
			if( (n == statusHint) && (WindowState == wsNormal) ){
				RECT rc;
                rc.right = StatusBar->Width;
                rc.bottom = StatusBar->Height;
				rc.left = rc.right - 14;
				rc.top = rc.bottom - 14;
				::DrawFrameControl(StatusBar->Canvas->Handle, &rc, DFC_SCROLL, DFCS_SCROLLSIZEGRIP);
            }
			break;
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::StatusBarDrawPanel(TStatusBar *StatusBar,
      TStatusPanel *Panel, const TRect &Rect)
{
	for( int i = 0; i < statusEND; i++ ){
		if( Panel == StatusBar->Panels->Items[i] ){
			m_rcStatus[i] = Rect;
			DrawStatus(i);
            break;
        }
    }
}
//---------------------------------------------------------------------------
int __fastcall TMainVARI::GetStatusIndex(int x)
{
	int N = -1;
	TRect *pRect = m_rcStatus;
    for( int i = 0; i <= statusHint; i++, pRect++ ){
		if( (x > pRect->Left) && (x < pRect->Right) ){
			N = i;
			break;
        }
    }
    return N;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::StatusBarMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	switch(GetStatusIndex(X)){
		case statusPAGE:
			PupPage->Popup(Left+X, Top+StatusBar->Top + Y);
        	break;
        case statusSAMP:
			if( Button == mbLeft ){
				KOOClick((TObject *)2);
            }
            else {
				KOAClick(NULL);
            }
        	break;
        case statusCOM:
        	KOOClick((TObject *)1);
            break;
		case statusVARI:
			PupCharset->PopupComponent = StatusBar;
			PupCharset->Popup(Left+X, Top+StatusBar->Top + Y);
            break;
        case statusHint:
			if( m_pPlayBox ) KVSPClick(NULL);
        	break;
        default:
        	break;
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::StatusBarMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
	int N = GetStatusIndex(X);
    if( (N >= statusPAGE) && (N <= statusHint) ){

	    const LPCSTR _tt[][2]={
			{ "ëóêMâÊñ ÉyÅ[ÉW", "Page of the TX window" },
	        { "éÛêMêMçÜÇÃS/Nî‰", "S/N ratio of the RX signals" },
    	    { "ÉTÉEÉìÉhÉJÅ[Éh Clock", "Sound card clock" },
        	{ "PTTÉ|Å[Ég", "PTT Port" },
	        { "ì`ëóåæåÍ", "Transmission language" },
	        { "", "" },
    	};
	    DrawStatus(statusHint, _tt[N][sys.m_MsgEng]);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KANSIClick(TObject *Sender)
{
	TMenuItem *pMenu = (TMenuItem *)Sender;
    if( pMenu->Checked ) return;

	WORD wLang;
	if( pMenu == KANSI ){
		wLang = 0;
    }
    else if( pMenu == KJA ){
		wLang = 0x0411;
    }
    else if( pMenu == KHL ){
		wLang = 0x0412;
    }
    else if( pMenu == KBV ){
		wLang = 0x0404;
    }
    else if( pMenu == KBY ){
		wLang = 0x0804;
    }
	int n = GetPopupIndex(PupCharset->PopupComponent);
	if( n ){
		TRxViewDlg *pView = m_RxSet[n].m_pView;
		SetLangFont(pView->PC->Font, wLang);
        pView->OnUpdateFont();
    }
    else {
		DettachFocus();
    	SetLangFont(PCRX->Font, wLang);
	    PCTX->Font->Name = PCRX->Font->Name;
    	PCTX->Font->Charset = PCRX->Font->Charset;
	    OnFontChange(FALSE);
	    OnFontChange(TRUE);
	    UpdateUI();
    	AttachFocus();
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KMISCClick(TObject *Sender)
{
	int n = GetPopupIndex(PupCharset->PopupComponent);
    if( n ){
		m_RxSet[n].m_pView->ChangeFont();
    }
    else {
		TFontDialog *pBox = new TFontDialog(this);
		DettachFocus();
	    pBox->Font = PCRX->Font;
    	pBox->Font->Color = PCRX->Color;
		OnWave();
    	if( pBox->Execute() ){
			PCRX->Font = pBox->Font;
			PCRX->Color = pBox->Font->Color;
		    PCTX->Font->Name = PCRX->Font->Name;
    		PCTX->Font->Charset = PCTX->Font->Charset;
	    	OnFontChange(FALSE);
    		OnFontChange(TRUE);
		}
    	delete pBox;
	    UpdateUI();
    	AttachFocus();
    }
}
//---------------------------------------------------------------------------
int __fastcall TMainVARI::GetPopupIndex(TComponent *pComponent)
{
    if( (pComponent == StatusBar)||(pComponent == this) ){
		return 0;
    }
    else {
		CRxSet *pRxSet = &m_RxSet[1];
	    for( int i = 1; i < RXMAX; i++, pRxSet++ ){
			if( pRxSet->IsActive() && pRxSet->m_pView ){
			    if( (pComponent == pRxSet->m_pView->StatusBar)||
					(pComponent == pRxSet->m_pView)
                ){
					return i;
                }
            }
        }
    }
	return 0;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::PupCharsetPopup(TObject *Sender)
{
	CheckFontCharset();
	KJA->Enabled = sys.m_tFontFam[fmJA];
	KHL->Enabled = sys.m_tFontFam[fmHL] || sys.m_tFontFam[fmJOHAB];
	KBV->Enabled = sys.m_tFontFam[fmBV];
	KBY->Enabled = sys.m_tFontFam[fmBY];

	int charset;
	int n = GetPopupIndex(PupCharset->PopupComponent);
	if( n ){
		charset = m_RxSet[n].m_pView->PC->Font->Charset;
	}
    else {
		charset = PCRX->Font->Charset;
    }
    switch(charset){
		case SHIFTJIS_CHARSET:
            KJA->Checked = TRUE;
			break;
        case JOHAB_CHARSET:
		case HANGEUL_CHARSET:
            KHL->Checked = TRUE;
			break;
		case CHINESEBIG5_CHARSET:   //
            KBV->Checked = TRUE;
			break;
		case 134:       // ä»ó™
            KBY->Checked = TRUE;
			break;
        case ANSI_CHARSET:
            KANSI->Checked = TRUE;
			break;
        case SYMBOL_CHARSET:
            KMISC->Checked = TRUE;
            break;
		default:
            KMISC->Checked = TRUE;
			break;
	}
	OnMenuProc(PupCharset->Items, "PopCHARSET");
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::HisRSTChange(TObject *Sender)
{
	if( m_fDisEvent ) return;

    if( m_fDrop ){
		m_fDrop = FALSE;
		SetTXFocus();
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::OnLogFreq(BOOL fLink)
{
	Log.SetFreq(&Log.m_sd, AnsiString(LogFreq->Text).c_str());	//JA7UDE 0428
	if( fLink ) LogLink.SetFreq(AnsiString(LogFreq->Text).c_str());	//JA7UDE 0428
	if( Log.m_sd.band != m_LogBand ){
		m_LogBand = Log.m_sd.band;
        DoEvent(macOnBand);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::LogFreqChange(TObject *Sender)
{
	if( m_fDisEvent ) return;

	OnLogFreq(TRUE);
    if( m_fDrop ){
		m_fDrop = FALSE;
        if( (ActiveControl != PCTX) && PCTX->CanFocus() ) SetTXFocus();
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KOVClick(TObject *Sender)
{
	char fname[256];

    sprintf(fname, "%sVARICODE.TXT", sys.m_BgnDir);
	g_VariCode.SaveTable(fname);
    if( !m_pEdit ){
		m_pEdit = new TFileEdit(this);
        m_pEdit->SetEvent(Handle, WM_WAVE, 2);
    }
	m_pEdit->SetDelFile(TRUE);
    m_pEdit->Execute(fname, TRUE);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KECClick(TObject *Sender)
{
	DettachFocus();
	TCodeView *pBox = new TCodeView(this);
	pBox->Execute(Handle, WM_WAVE, 3, PCRX->Font);
    delete pBox;
    AttachFocus();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KFWSTClick(TObject *Sender)
{
	if( m_pPlayBox ){
		delete m_pPlayBox;
       	m_pPlayBox = NULL;
   	}
	::GetLocal(&m_LocalTime);
    sprintf(m_TextBuff, "%s%02u%02u%02u%02u%02u%02u.mmv",
		sys.m_SoundDir,
    	m_LocalTime.wYear % 100,
        m_LocalTime.wMonth,
        m_LocalTime.wDay,
        m_LocalTime.wHour,
        m_LocalTime.wMinute,
        m_LocalTime.wSecond
    );
	m_WaveFile.Rec(m_TextBuff);
    KFWST->Checked = TRUE; KFWS->Checked = FALSE;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KFWSClick(TObject *Sender)
{
	TSaveDialog *pBox = new TSaveDialog(this);
	pBox->Options << ofOverwritePrompt;
    pBox->Options << ofNoReadOnlyReturn;
	if( sys.m_MsgEng ){
		pBox->Title = "Recode sound";
		pBox->Filter = "MMV Files(*.mmv)|*.mmv|";
	}
	else {
		pBox->Title = "ÉTÉEÉìÉhÇÃãLò^";
		pBox->Filter = "MMV Files(*.mmv)|*.mmv|";
	}
	pBox->FileName = "";
	pBox->DefaultExt = "mmv";
	pBox->InitialDir = sys.m_SoundDir;
	DettachFocus();
	OnWave();
	if( pBox->Execute() == TRUE ){
		SetDirName(sys.m_SoundDir, AnsiString(pBox->FileName).c_str());	//JA7UDE 0428
		OnWave();
		if( m_pPlayBox ){
			delete m_pPlayBox;
        	m_pPlayBox = NULL;
    	}
        OnWave();
		m_WaveFile.Rec(AnsiString(pBox->FileName).c_str());	//JA7UDE 0428
	    KFWST->Checked = FALSE; KFWS->Checked = TRUE;
	}
	delete pBox;
	AttachFocus();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KFRSClick(TObject *Sender)
{
	TOpenDialog *pBox = new TOpenDialog(this);
	pBox->Options >> ofCreatePrompt;
	pBox->Options << ofFileMustExist;
	if( sys.m_MsgEng ){
		pBox->Title = "Play sound";
		pBox->Filter = "MMV Files(*.mmv)|*.mmv|";
	}
	else {
		pBox->Title = "ÉTÉEÉìÉhÇÃçƒê∂";
		pBox->Filter = "MMV Files(*.mmv)|*.mmv|";
	}
	pBox->FileName = "";
	pBox->DefaultExt = "mmv";
	pBox->InitialDir = sys.m_SoundDir;
	DettachFocus();
	OnWave();
	if( pBox->Execute() == TRUE ){
		SetDirName(sys.m_SoundDir, AnsiString(pBox->FileName).c_str());	//JA7UDE 0428
		OnWave();
		m_Wave.InClose();
		if( m_WaveFile.Play(AnsiString(pBox->FileName).c_str()) ){	//JA7UDE 0428
	        ReOutOpen();
			if( !m_pPlayBox ){
				m_pPlayBox = new TPlayDlgBox(this);
	        }
    	    m_pPlayBox->Execute(&m_WaveFile);
        }
        else {
	        OpenSound(FALSE);
        }
	}
	delete pBox;
	if( m_pPlayBox ){
		m_pPlayBox->Visible = TRUE;
        m_pPlayBox->SetFocus();
    }
    else {
		AttachFocus();
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KFESClick(TObject *Sender)
{
	if( m_pPlayBox ){
		delete m_pPlayBox;
        m_pPlayBox = NULL;
    }
    int mode = m_WaveFile.m_mode;
	m_WaveFile.FileClose();
    if( mode == 1 ){
		m_Wave.OutAbort();
        OpenSound(FALSE);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KVSPClick(TObject *Sender)
{
	if( m_pPlayBox ){
		m_pPlayBox->Visible = TRUE;
        m_pPlayBox->SetFocus();
    }
}
//---------------------------------------------------------------------------

void __fastcall TMainVARI::KVLClick(TObject *Sender)
{
	TLogListDlg *pBox = new TLogListDlg(this);
	OnWave();
	pBox->Execute();
	delete pBox;
	UpdateTitle();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::UpdateCallsign(void)
{
	if( HisCall->Text.IsEmpty() ){
    	m_Dupe = 0;
        SBQSO->Enabled = FALSE;
    }
    else {
        SBQSO->Enabled = TRUE;
    }
	HisCall->Font->Color = m_Dupe ? clRed : clBlack;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::UpdateLogMode(void)
{
	LPCSTR pMode;
	if( m_strLogMode.IsEmpty() ){
		pMode = g_tLogModeTable[CBMode->ItemIndex];
    }
    else {
		pMode = m_strLogMode.c_str();
    }
	Log.SetMode(&Log.m_sd, pMode);
    LogLink.SetMode(pMode);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::UpdateLogData(void)
{
	StrCopy(Log.m_sd.call, AnsiString(HisCall->Text).c_str(), MLCALL);	//JA7UDE 0428
	clipsp(Log.m_sd.call);
	jstrupr(Log.m_sd.call);
	StrCopy(Log.m_sd.name, AnsiString(HisName->Text).c_str(), MLNAME);	//JA7UDE 0428
	StrCopy(Log.m_sd.qth, AnsiString(HisQTH->Text).c_str(), MLQTH);	//JA7UDE 0428
	StrCopy(Log.m_sd.my, AnsiString(MyRST->Text).c_str(), MLRST);	//JA7UDE 0428
	jstrupr(Log.m_sd.my);
	StrCopy(Log.m_sd.ur, AnsiString(HisRST->Text).c_str(), MLRST);	//JA7UDE 0428
	jstrupr(Log.m_sd.ur);
	StrCopy(Log.m_sd.rem, AnsiString(EditNote->Text).c_str(), MLREM);	//JA7UDE 0428
	StrCopy(Log.m_sd.qsl, AnsiString(EditQSL->Text).c_str(), MLQSL);	//JA7UDE 0428
	Log.SetFreq(&Log.m_sd, AnsiString(LogFreq->Text).c_str());	//JA7UDE 0428
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::UpdateTextData(void)
{
	HisCall->Text = Log.m_sd.call;
	HisName->Text = Log.m_sd.name;
	HisQTH->Text = Log.m_sd.qth;
	HisRST->Text = Log.m_sd.ur;
	MyRST->Text = Log.m_sd.my;
	EditNote->Text = Log.m_sd.rem;
	EditQSL->Text = Log.m_sd.qsl;
	LogFreq->Text = Log.GetFreqString(Log.m_sd.band, Log.m_sd.fq);
	Log.m_CurChg = 0;
    UpdateLogPanel();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::FindCall(void)
{
	CWaitCursor w;
	UpdateLogMode();
	m_Dupe = 0;
	StrCopy(Log.m_sd.call, AnsiString(HisCall->Text).c_str(), MLCALL);	//JA7UDE 0428
    clipsp(Log.m_sd.call);
    AddCall(Log.m_sd.call);
//	if( IsCall(Log.m_sd.call) ) AddCall(Log.m_sd.call);
	AnsiString  strHis = HisRST->Text;
	if( Log.FindSet(&Log.m_Find, Log.m_sd.call) ){  // å©Ç¬Ç©Ç¡ÇΩéû
		OnWave();
		Log.SetFreq(&Log.m_sd, AnsiString(LogFreq->Text).c_str());	//JA7UDE 0428
		SDMMLOG	sd;
		Log.GetData(&sd, Log.m_Find.pFindTbl[0]);	// àÍî‘ç≈êVÇÃÉfÅ[É^
		if( SBQSO->Down ){
			if( (sd.btime == Log.m_sd.btime) && Log.m_Find.GetCount() ){
				Log.GetData(&sd, Log.m_Find.pFindTbl[1]);	// ìÒî‘ñ⁄ÇÃÉfÅ[É^
	        }
        }
        else {
			Log.m_sd.btime = 0;
		}
		if( sd.btime != Log.m_sd.btime ){
			if( Log.m_Find.m_FindCmp1Max ){
				switch(Log.m_LogSet.m_CheckBand){
					case 1:
						if( Log.FindSameBand(FALSE) ) m_Dupe = 1;
						break;
					case 2:
						if( Log.FindSameBand(TRUE) ) m_Dupe = 1;
						break;
					default:
						m_Dupe = 1;
						break;
				}
			}
        }
		if( (!SBQSO->Down || !Log.m_sd.name[0] ) && Log.m_LogSet.m_CopyName ) strcpy(Log.m_sd.name, sd.name);
		if( (!SBQSO->Down || !Log.m_sd.qth[0] ) && Log.m_LogSet.m_CopyQTH ) strcpy(Log.m_sd.qth, sd.qth);
		if( (!SBQSO->Down || !Log.m_sd.rem[0] ) && Log.m_LogSet.m_CopyREM ) strcpy(Log.m_sd.rem, sd.rem);
		if( (!SBQSO->Down || !Log.m_sd.qsl[0] ) && Log.m_LogSet.m_CopyQSL ) strcpy(Log.m_sd.qsl, sd.qsl);
		UpdateTextData();
		if( SBQSO->Down ){
			Log.m_Find.Ins(Log.m_CurNo);
			UpdateTextData();
			Log.PutData(&Log.m_sd, Log.m_CurNo);
		}
	}
	else {                                          // å©Ç¬Ç©ÇÁÇ»Ç©Ç¡ÇΩéû
		if( !SBQSO->Down && Log.m_LogSet.m_CopyName ) Log.m_sd.name[0] = 0;
		if( !SBQSO->Down && Log.m_LogSet.m_CopyQTH ) Log.m_sd.qth[0] = 0;
		if( !SBQSO->Down && Log.m_LogSet.m_CopyREM ) Log.m_sd.rem[0] = 0;
		if( !SBQSO->Down && Log.m_LogSet.m_CopyQSL ) Log.m_sd.qsl[0] = 0;
		UpdateTextData();
	}
	LPCSTR pCC = ClipCC(Log.m_sd.call);
	Log.SetOptStr(0, &Log.m_sd, Cty.GetCountry(pCC));
	Log.SetOptStr(1, &Log.m_sd, Cty.GetCont(pCC));
	if( !strHis.IsEmpty() ) HisRST->Text = strHis;
	UpdateCallsign();
    UpdateLogPanel();
	if( !SBQSO->Down ){
		LogLink.SetFreq(AnsiString(LogFreq->Text).c_str());
	    LogLink.SetMode(g_tLogModeTable[CBMode->ItemIndex]);
		LogLink.FindCall(Log.m_sd.call);
	}
	else {
		LogLink.Write(&Log.m_sd, 1);
	}
	DoEvent(macOnFind);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::AutoLogSave(void)
{
	if( Log.IsOpen() && Log.m_LogSet.m_AutoSave && Log.IsEdit() ) KFLFClick(NULL);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::UpdateLogLink(void)
{
	LogLink.UpdateLink(sys.m_LogLink);
    UpdateTitle();
    if( sys.m_LogLink ){
		if( !m_pLogLinkTimer ){
			m_pLogLinkTimer = new TTimer(this);
            m_pLogLinkTimer->OnTimer = LogLinkTimer;
            m_pLogLinkTimer->Interval = LINKINTERVAL;
        }
    }
    else if( m_pLogLinkTimer ){
		delete m_pLogLinkTimer;
        m_pLogLinkTimer = NULL;
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KFLFClick(TObject *Sender)
{
	Log.Close();
	Log.Open(NULL, TRUE);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::HisCallChange(TObject *Sender)
{
    UpdateLogPanel();
}
//---------------------------------------------------------------------------

void __fastcall TMainVARI::SBDataClick(TObject *Sender)
{
	SDMMLOG sd;

	UpdateLogData();
	if( strcmp(Log.m_Find.GetText(), Log.m_sd.call) ){
		CWaitCursor w;
		Log.FindSet(&Log.m_Find, Log.m_sd.call);
	}
	TQSODlgBox *pBox = new TQSODlgBox(this);
	OnWave();
	memcpy(&sd, &Log.m_sd, sizeof(sd));
	DettachFocus();
	if( pBox->Execute(&Log.m_Find, &Log.m_sd, Log.m_CurNo) == TRUE ){
		if( memcmp(&sd, &Log.m_sd, sizeof(sd)) ){
			UpdateTextData();
			HisCallChange(NULL);
			LogLink.Write(&Log.m_sd, 0);
		}
	}
	delete pBox;
	AttachFocus();
    UpdateLogPanel();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SBFindClick(TObject *Sender)
{
	AnsiString strName = HisName->Text;
    AnsiString strMy = MyRST->Text;
	if( !Log.IsOpen() ){
		if( Log.Open(NULL, TRUE) == FALSE ){
		    UpdateLogPanel();
			return;
		}
	}
	FindCall();
	DettachFocus();
	TQSODlgBox *pBox = new TQSODlgBox(this);
	pBox->ShowFind(&Log.m_Find);
	delete pBox;
	if( HisName->Text.IsEmpty() ) HisName->Text = strName;
    if( MyRST->Text.IsEmpty() ) MyRST->Text = strMy;
	AutoLogSave();
    UpdateLogPanel();
    AttachFocus();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KOLClick(TObject *Sender)
{
	DettachFocus();
	TLogSetDlg *pBox = new TLogSetDlg(this);
	pBox->Execute();
	delete pBox;
	UpdateLogLink();
    AttachFocus();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::CheckLogLink(void)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KFLOClick(TObject *Sender)
{
	TOpenDialog *pBox = new TOpenDialog(this);
	pBox->Options >> ofCreatePrompt;
	pBox->Options >> ofFileMustExist;
	if( sys.m_MsgEng ){
		pBox->Title = "Open LogData File";
		pBox->Filter = "MMLOG Data Files(*.mdt)|*.mdt|";
	}
	else {
		pBox->Title = "ÉçÉOÉtÉ@ÉCÉãÇÃÉIÅ[ÉvÉì";
		pBox->Filter = "MMLOGÉfÅ[É^ÉtÉ@ÉCÉã(*.mdt)|*.mdt|";
	}
	pBox->FileName = "";
	pBox->DefaultExt = "mdt";
	pBox->InitialDir = sys.m_LogDir;
	DettachFocus();
	if( pBox->Execute() == TRUE ){
		Log.Close();
        Log.DoBackup();
		Log.MakeName(AnsiString(pBox->FileName).c_str());	//JA7UDE 0428
		Log.Open(NULL, TRUE);
		UpdateTextData();
		UpdateTitle();
	}
    UpdateLogPanel();
	delete pBox;
    if( Active ) AttachFocus();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SBListClick(TObject *Sender)
{
	DettachFocus();
	TLogListDlg *pBox = new TLogListDlg(this);
	OnWave();
	pBox->Execute();
	delete pBox;
	AutoLogSave();
	UpdateLogLink();
	UpdateTitle();
    AttachFocus();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::CMMML(TMessage &Message)
{
	if( sys.m_LogLink != 2 ) return;
	if( LogLink.m_pLink == NULL ) return;

	switch(Message.WParam){
		case MML_NOTIFYSESSION:
			LogLink.NotifySession((LPCSTR)Message.LParam);
			UpdateLogLink();
			break;
		case MML_QRETURN:
			if( !LogLink.IsLink() ) return;
			if( LogLink.QReturn(&Log.m_sd, (const mmLOGDATA *)Message.LParam) ){
				UpdateTextData();
			}
			break;
		case MML_VFO:
			if( !LogLink.IsLink() ) return;
			if( Message.LParam ){
				LogFreq->Text = (LPCSTR)Message.LParam;
				OnLogFreq(FALSE);
			}
			break;
	}
	Message.Result = TRUE;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::CMMMR(TMessage &Message)
{
	if( m_pRadio == NULL ) return;

	switch(Message.WParam){
		case MMR_DEFCOMMAND:
			{
				LPCSTR p;
				switch(Message.LParam){
					case 1:
						p = RADIO.CmdRx.c_str();
						break;
					case 2:
						p = RADIO.CmdTx.c_str();
						break;
					default:
						p = RADIO.CmdInit.c_str();
						break;
				}
				m_pRadio->SendCommand(p);
			}
			break;
		case MMR_VFO:
			m_pRadio->UpdateFreq(double(Message.LParam)/10000.0);
			break;
	}
	Message.Result = TRUE;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KVWAClick(TObject *Sender)
{
	InvMenu(KVWA);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::InitDefKey(void)
{
	sys.m_DefKey[kkTX] = VK_F12;
    sys.m_DefKey[kkTXOFF] = VK_PAUSE;
}
//---------------------------------------------------------------------------
// èââÒÇÃèâä˙âª
__fastcall CRxSet::CRxSet()
{
	m_fShowed = FALSE;
	m_Mode = sys.m_DefaultMode;
    m_fTWO = 0;
    m_fMBCS = 0;
    m_fAFC = TRUE;
    m_SQLevel = 300;
	m_SQ = FALSE;
    m_SQTimer = 0;
    m_Speed = SPEED;
    m_AFCTimerW = 0;
    m_AFCTimerN = 0;
    m_AFCTimerW2 = 0;
    m_AFCFQ = 0;
    m_AFCSQ = 0;
    m_fATC = FALSE;
    m_WaterW = 400;
    m_pFFT = &MainVARI->m_FFT;
	m_CarrierFreq = 1750;
	m_MFSK_TYPE = typMFSK16;
	m_AFCTimerPSK = m_AFCTimerMFSK = 0;
	m_RTTYFFT = FALSE;

	memset(&m_StgFFT, 0, sizeof(m_StgFFT));
	m_StgFFT.VW = 100;
	InitStgFFT();

	m_FontData.m_Name = "";
    m_FontData.m_Charset = 0;
    m_FontData.m_Height = 0;
    m_FontData.m_Style = 0;

    m_cAutoTS1 = m_cAutoTS2 = 0;
	m_AvgAFC.Create(8);

	m_pDem = NULL;
    m_pView = NULL;
}
//---------------------------------------------------------------------------
__fastcall CRxSet::~CRxSet()
{
	Delete();
}
//---------------------------------------------------------------------------
void __fastcall CRxSet::SetMFSKType(int type)
{
	m_MFSK_TYPE = type;
	if( IsActive() ) m_pDem->SetMFSKType(type);
}
//---------------------------------------------------------------------------
double __fastcall CRxSet::GetSpeed(void)
{
	if( IsMFSK() ){
		const double _tt[]={15.625,7.8125,31.25,10.767,21.533,32.0,62.5,3.9063};
		return _tt[m_MFSK_TYPE];
//		return IsActive() ? m_pDem->m_MFSK_SPEED : 15.625;
    }
    else {
		return m_Speed;
    }
}
//---------------------------------------------------------------------------
void __fastcall CRxSet::SetSpeed(double b)
{
	if( b < MIN_SPEED ) return;
   	if( b > MAX_SPEED ) return;
	if( m_Speed != b ){
		m_Speed = b;
   		if( IsActive() ) m_pDem->SetSpeed(b);
    }
}
//---------------------------------------------------------------------------
void __fastcall CRxSet::SetMode(int mode)
{
	int Offset = 0;
    double Speed = 0.0;
	if( m_Mode != mode ){
		if( ::IsRTTY(mode) ){
			if( !::IsRTTY(m_Mode) ){
				Speed = 45.45;
            }
        }
		else if( ::IsMFSK(mode) ){
			if( !::IsMFSK(m_Mode) ){
				Speed = 15.625;
            }
        }
        else {
			if( ::IsRTTY(m_Mode) || ::IsMFSK(m_Mode) ){
				Speed = 31.25;
            }
        }
        if( ::IsMFSK(mode) && ::IsMFSK(m_Mode) && !sys.m_MFSK_Center ){
			int bw = m_pDem ? m_pDem->m_MFSK_BW : 234.375;
			Offset = (mode == MODE_mfsk_U) ? -bw : bw;
        }
    }
	m_Mode = mode;
	if( Speed > 1.0 ) MainVARI->UpdateSpeed(this, Speed);
	MainVARI->InitCollect(this, ::IsRTTY(m_Mode) ? 7 : 14);
	switch(m_Mode){
		case MODE_RTTY:
        case MODE_U_RTTY:
		case MODE_GMSK:
        case MODE_FSK:
        case MODE_FSKW:
        case MODE_BPSK:
			m_fTWO = FALSE;
           	break;
        case MODE_N_BPSK:
        case MODE_mfsk_L:
        case MODE_mfsk_U:
		case MODE_qpsk_L:
        case MODE_qpsk_U:
			m_fTWO = TRUE;
           	break;
    }
	m_fMBCS = FALSE;
	m_AFCTimerPSK = m_AFCTimerMFSK = 0;
    if( IsActive() ) m_pDem->SetType(m_Mode);
	m_AvgAFC.Create(IsMFSK() ? 4 : 8);
    m_AvgAFC.Reset(m_pDem->m_CarrierFreq);
	MainVARI->UpdateMode(this, Offset);
}
//---------------------------------------------------------------------------
void __fastcall CRxSet::SetCarrierFreq(double f)
{
	if( IsActive() ){
		m_pDem->SetCarrierFreq(f);
        m_CarrierFreq = f;
    }
    m_AvgAFC.Reset(f);
	m_AFCTimerPSK = 0;
    m_AFCFQ = f;
}
//---------------------------------------------------------------------------
void __fastcall CRxSet::Create(BOOL fView)
{
    m_SQLevel = 300;
	m_SQ = FALSE;
    m_SQTimer = 0;
    m_AFCTimerW = 0;
    m_AFCTimerN = 0;
    m_AFCTimerW2 = 0;
    m_AFCFQ = 0;
    m_AFCSQ = 0;
    m_fATC = FALSE;
    m_pFFT = &MainVARI->m_FFT;

	m_StgFFT.dBSum = 0;
    m_StgFFT.dBMax = 0;
	m_StgFFT.dBWMax = 0;
	m_StgFFT.VW = 100;
    m_StgFFT.DispSig = 0;
    m_StgFFT.Timer = 0;
	InitStgFFT();

    m_cAutoTS1 = m_cAutoTS2 = 0;

    m_PeakSig = 0;
    m_AvgSig.Create(32);
	LimitInt(&m_CarrierFreq, MIN_CARRIER, sys.m_MaxCarrier);
	LimitInt(&m_SQLevel, 0, LEVELMAX);
    LimitDbl(&m_Speed, MIN_SPEED, MAX_SPEED);

	if( IsRTTY() ) m_Speed = 45.45;

	EPHASE(P_SUBCREATE);
	if( !m_pDem ){
		m_pDem = new CDEMFSK;
		if( !m_pDem ){
			MainVARI->SetErrorMsg("Out of memory");
			return;
        }
		m_pDem->m_fRTTYFFT = m_RTTYFFT;
        m_pDem->SetSampleFreq(DEMSAMPFREQ);
        m_pDem->SetSpeed(m_Speed);
        m_pDem->SetType(m_Mode);
		m_pDem->SetMFSKType(m_MFSK_TYPE);
        m_pDem->m_Decode.SetATCSpeed(MainVARI->m_ATCSpeed);
        m_pDem->m_Decode.SetATCLimit(MainVARI->m_ATCLimit);
		if( fView ){			// ÉTÉuÉEÉCÉìÉhÉEÇÃèÍçá
            m_pDem->m_Decode.m_fATC = TRUE;
            m_pDem->m_fAFC = m_fAFC;
            m_pDem->m_fEnableAFC = TRUE;
			CDEMFSK *pDem = MainVARI->m_RxSet[0].m_pDem;
            m_pDem->MakeBPF(pDem->m_PreBPFTaps);
            m_pDem->m_pBPF = &pDem->m_inBPF;
			SetCarrierFreq(m_CarrierFreq);
//            m_fJA = MainVARI->m_RxSet[0].m_fJA;
        }
        InitStgFFT();
	    m_StgFFT.Timer = 0;
    }
	EPHASE(P_SUBVIEW);
    if( fView && !m_pView ){
        m_pView = new TRxViewDlg(MainVARI);
		if( !m_pView ){
			delete m_pDem; m_pDem = NULL;
			MainVARI->SetErrorMsg("Out of memory");
			return;
        }
        m_pView->m_pRxSet = this;
 		m_pView->SetBounds(m_rcView.left, m_rcView.top, m_rcView.right, m_rcView.bottom);
        m_pView->UpdateWaterWidth();
		m_pView->m_Dump.SetRTTY(IsRTTY());
		m_pView->m_Dump.ShowCtrl(sys.m_fShowCtrlCode);
		if( m_FontData.m_Height ){
			FontData2Font(m_pView->PC->Font, &m_FontData);
        }
		m_pView->OnUpdateFont();
        memcpy(m_pView->m_Dump.m_Color, MainVARI->m_Dump.m_Color, sizeof(MainVARI->m_Dump.m_Color));
//        ::SetWindowPos(m_pView->Handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
    }
	EPHASE(P_NULL);
	SetMode(m_Mode);
}
//---------------------------------------------------------------------------
void __fastcall CRxSet::Delete(void)
{
	if( m_pDem ){
		m_CarrierFreq = m_pDem->m_CarrierFreq + 0.5;
    	delete m_pDem;
        m_pDem = NULL;
    }
	if( m_pView ){
		MainVARI->CloseWheelTimer(&m_pView->m_Dump);
		m_rcView.left = m_pView->Left;
        m_rcView.top = m_pView->Top;
        m_rcView.right = m_pView->Width;
        m_rcView.bottom = m_pView->Height;
        Font2FontData(&m_FontData, m_pView->PC->Font);
        delete m_pView;
        m_pView = NULL;
		m_fShowed = TRUE;
	}
    else {
		m_fShowed = FALSE;
    }
}
//---------------------------------------------------------------------------
void __fastcall CRxSet::SetSampleFreq(double f)
{
	if( !IsActive() ) return;
	m_pDem->SetSampleFreq(f);
    if( m_pView ) m_pView->UpdateWaterWidth();
}
//---------------------------------------------------------------------------
void __fastcall CRxSet::InitStgFFT(void)
{
	if( m_pFFT->m_FFTGain ){
		m_StgFFT.Sum = 1024;
	    m_StgFFT.Max = 1024;
    	m_StgFFT.WMax = 1024;
    }
    else {
        m_StgFFT.Sum = 5000;
   	    m_StgFFT.Max = 5000;
   	    m_StgFFT.WMax = 5000;
    }
    m_StgFFT.Sig = 0;
}
//---------------------------------------------------------------------------
void __fastcall CRxSet::ClearWindow(void)
{
	if( !IsActive() ) return;
    if( m_pView ) m_pView->m_Dump.Clear();
}
//---------------------------------------------------------------------------
double __fastcall CRxSet::GetBandWidth(void)
{
	switch(m_Mode){
		case MODE_GMSK:
        case MODE_BPSK:
        case MODE_N_BPSK:
		case MODE_qpsk_L:
        case MODE_qpsk_U:
			return m_Speed;
        case MODE_FSK:
			return m_Speed*2;
        case MODE_FSKW:
		case MODE_RTTY:
        case MODE_U_RTTY:
			return m_pDem->m_RTTYShift;
        case MODE_mfsk_L:
        case MODE_mfsk_U:
			if( m_pDem ){
				return m_pDem->m_MFSK_BW;
            }
            else {
	        	return 234.375;
            }
    }
    return 0;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::UpdateSubWindow(void)
{
	m_fSubWindow = FALSE;
	int i;
	for( i = 1; i < RXMAX; i++ ){
		if( m_RxSet[i].IsActive() ) m_fSubWindow = TRUE;
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::ShowSubChannel(int n, int sw)
{
    DettachFocus();
	CRxSet *pRxSet = &m_RxSet[n];
	if( sw == 2 ){
		sw = !pRxSet->IsActive();
    }
	if( sw ){
	   	if( !pRxSet->IsActive() ){
			pRxSet->Create(TRUE);
			sprintf(m_TextBuff, sys.m_MsgEng ? "Channel - %u":"É`ÉÉÉìÉlÉã - %u", n);
           	pRxSet->m_pView->Caption = m_TextBuff;
    	}
   	    pRxSet->m_pView->Visible = TRUE;
    }
    else {
		if( pRxSet->IsActive() ) pRxSet->Delete();
	}
    UpdateSubWindow();
    AttachFocus();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::PBoxFFTMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
	if( SBWave->Down ) return;

    // FFTÇ‹ÇΩÇÕWaterFallÇÃèÍçá
	if( m_MouseDown ){
		int fq = m_FFTB + X * m_FFTW / m_fftXW;
        if( fq < MIN_CARRIER ) fq = MIN_CARRIER;
        if( fq > sys.m_MaxCarrier ) fq = sys.m_MaxCarrier;
		if( m_MouseNotch ){
			m_Notches.SetFreq(m_MouseNotch-1, fq);
			if( m_MouseNotch == 1 ) m_NotchFreq = fq;
			PBoxFFTPaint(NULL);
        }
		else if( m_MouseSubChannel ){
			CRxSet *pRxSet = &m_RxSet[m_MouseSubChannel];
			if( pRxSet->IsActive() ){
				pRxSet->m_pDem->m_Decode.Reset();
				pRxSet->m_pDem->ResetMeasMFSK();
				pRxSet->SetCarrierFreq(fq);
				PBoxFFTPaint(NULL);
            }
        }
        else {
			m_RxSet[0].m_pDem->m_Decode.Reset();
			SetRxFreq(fq);
   	    }
    }
    else if( m_fSubWindow || m_Notches.m_Count ){
		m_MouseNotch = FALSE;
    	m_MouseSubChannel = FALSE;
        int i;
		if( m_Notches.m_Count ){
			for( i = 0; i < m_Notches.m_Count; i++ ){
				int xx = X - m_Notches.m_pBase[i].m_MX;
    		    int yy = Y - m_Notches.m_pBase[i].m_MY;
	        	int d = (xx * xx + yy * yy);
		        if( d < 64 ){
					m_MouseNotch = i + 1;
        		}
            }
        }
		if( !m_MouseNotch ){
			CRxSet *pRxSet = &m_RxSet[RXMAX-1];
			for( i = RXMAX-1; i > 0; i--, pRxSet-- ){
				if( pRxSet->IsActive() ){
					int xx = X - pRxSet->m_X;
        	       	int yy = Y - pRxSet->m_Y;
	                int d = (xx * xx + yy * yy);
   		            if( d < 64 ){
						m_MouseSubChannel = i;
	           	        break;
    	           	}
        	    }
            }
   	    }
        PBoxFFT->Cursor = (m_MouseNotch || m_MouseSubChannel) ? crSizeWE : crDefault;
    }
    else {
		m_MouseSubChannel = FALSE;
        m_MouseNotch = 0;
//		int XO = (UdRxCarrier->Position - m_FFTB) * m_fftXW / m_FFTW;
//		XO -= X; XO *= XO;
//        PBoxFFT->Cursor = (XO < 64) ? crSizeWE : crDefault;
        PBoxFFT->Cursor = crDefault;
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::PBoxFFTMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if( m_MouseDown && !SBFFT500->Down ){
		int fq = m_FFTB + X * m_FFTW / m_fftXW;
        if( fq < MIN_CARRIER ) fq = MIN_CARRIER;
        if( fq > sys.m_MaxCarrier ) fq = sys.m_MaxCarrier;
		if( m_MouseNotch ){
			m_Notches.SetFreq(m_MouseNotch-1, fq);
			if( m_MouseNotch == 1 ) m_NotchFreq = fq;
			PBoxFFTPaint(NULL);
        }
    	else if( m_MouseSubChannel ){
			CRxSet *pRxSet = &m_RxSet[m_MouseSubChannel];
			if( pRxSet->IsActive() ){
				fq = GetSignalFreq(fq, SBFFT3K->Down ? 50 : 32, pRxSet);
				pRxSet->m_pDem->m_Decode.Reset();
				pRxSet->m_pDem->ResetMeasMFSK();
                pRxSet->m_PeakSig = 0;
				pRxSet->SetCarrierFreq(fq);
                pRxSet->m_AFCTimerMFSK = (SBFFT500->Down || SBFFT1K->Down) ? MFSKAFC_MAX : 0;
				PBoxFFTPaint(NULL);
            }
		}
        else {
			fq = GetSignalFreq(fq, SBFFT3K->Down ? 50 : 32, &m_RxSet[0]);
			m_RxSet[0].m_pDem->m_Decode.Reset();
			SetRxFreq(fq);
			m_RxSet[0].m_AFCTimerMFSK = (SBFFT500->Down || SBFFT1K->Down) ? MFSKAFC_MAX : 0;
            m_RxSet[0].m_PeakSig = 0;
        }
	}
	m_MouseDown = FALSE;
    m_MouseSubChannel = FALSE;
    m_MouseNotch = 0;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KCallClick(TObject *Sender)
{
	TMenuItem *pm = (TMenuItem *)Sender;
	if( strcmp(AnsiString(HisCall->Text).c_str(), (AnsiString(pm->Caption).c_str()+6)) ){	//JA7UDE 0428
		HisCall->Text = pm->Caption.c_str() + 6;
		FindCall();
		UpdateUI();
        SetTXFocus();
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::AddCall(LPCSTR p)
{
	SYSTEMTIME	now;
	GetLocal(&now);
	char bf[128];
	sprintf(bf, "%02u:%02u\t%s", now.wHour, now.wMinute, p);

	int i;
	TMenuItem *pm;
	for( i = 0; i < PupCalls->Items->Count; i++ ){
		pm = PupCalls->Items->Items[i];
		if( !strcmp(AnsiString(pm->Caption).c_str()+6, p) ){	//JA7UDE 0428
			if( !i ){
				pm->Caption = bf;
				return;
			}
			else {
				PupCalls->Items->Delete(i);
				break;
			}
		}
	}
	pm = new TMenuItem (this);
	pm->Caption = bf;
	pm->RadioItem = FALSE;
	pm->OnClick = KCallClick;
	pm->Checked = FALSE;
	pm->Enabled = TRUE;
	if( (i >= PupCalls->Items->Count) && (PupCalls->Items->Count == STGCALLMAX) ){
		PupCalls->Items->Delete(STGCALLMAX-1);
	}
	PupCalls->Items->Insert(0, pm);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SetTXFocus(void)
{
	if( PCTX->CanFocus() ) PCTX->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SBMClick(TObject *Sender)
{
	if( PupCalls->Items->Count ){
		RECT rc;
		::GetWindowRect(PLog->Handle, &rc);
		PupCalls->Popup(rc.left+SBM->Left + SBM->Width/2, rc.top+SBM->Top+SBM->Height/2);
	}
}
//---------------------------------------------------------------------------
BOOL __fastcall TMainVARI::DeleteMacroTimerS(void)
{
	BOOL r = FALSE;
	if( m_ReqMacroTimer || m_pMacroTimer ){
		m_ReqMacroTimer = 0;
		if( m_CurrentMacro >= 0 ){
			MACBUTTON *pList = &m_tMacButton[m_CurrentMacro];
			if( pList->pButton ) pList->pButton->Down = FALSE;
        }
		if( m_pMacroTimer ){
		    m_pMacroTimer->Enabled = FALSE;
			delete m_pMacroTimer;
        	m_pMacroTimer = NULL;
        }
		DrawHint();
        r = TRUE;
    }
    m_pCurrentMacroMenu = NULL;
    return r;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::DeleteMacroTimer(void)
{
	if( DeleteMacroTimerS() ){
        if( m_TX ) ToRX();
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::CreateMacroTimer(int n)
{
	if( n <= 100 ) n = 100;
	if( m_pMacroTimer ){
    	delete m_pMacroTimer;
        m_pMacroTimer = NULL;
    }
	if( ((m_CurrentMacro >= 0) && (m_CurrentMacro < MACBUTTONALL)) || (m_CurrentMacro == -1) ){
	    m_pMacroTimer = new TTimer(this);
    	m_pMacroTimer->Interval = n;
		m_pMacroTimer->OnTimer = MacroTimer;
    	m_pMacroTimer->Enabled = TRUE;
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KVLAClick(TObject *Sender)
{
	InvMenu(KVLA);
    UpdateLogHeight();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::UpdateLogHeight(void)
{
	BOOL f = KVLA->Checked;
	if( f ){
		PLog->Height = 52;
    }
    else {
		PLog->Height = 26;
    }
	L16->Visible = f;
    L17->Visible = f;
    L18->Visible = f;
	HisQTH->Visible = f;
	EditNote->Visible = f;
    EditQSL->Visible = f;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::PLogMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if( Button == mbRight ) KVLAClick(NULL);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::UpdateMacButtonVW(int n)
{
	if( n < 2 ) n = 2;
    if( n > 4 ) n = 4;

    m_MacButtonVW = n;
	n *= 16;
	PCMac->SetBounds(0, StatusBar->Top - n, ClientWidth, n);
    UdMac->Height = n;
	SetMacButtonMax();
   	CreateMacButton();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KVM2Click(TObject *Sender)
{
	int n = 3;
    if( Sender == KVM2 ){
		n = 2;
    }
    else if( Sender == KVM4 ){
		n = 4;
    }
	UpdateMacButtonVW(n);
}
//---------------------------------------------------------------------------

void __fastcall TMainVARI::UdMacMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if( Button == mbRight ){
		UdMac->Position = 0;
        CreateMacButton();
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KOAClick(TObject *Sender)
{
	if( m_TX ) return;

	if( !m_pClockView ) m_pClockView = new TClockAdjDlg(this);
    m_pClockView->Execute();
}
//---------------------------------------------------------------------------

void __fastcall TMainVARI::SBDataMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if( Button == mbRight ){
		int n = Log.m_CurNo - 1;
		if( n < 0 ){
			SBDataClick(NULL);
			return;
		}

		TQSODlgBox *pBox = new TQSODlgBox(this);
		SDMMLOG	sd;
		Log.GetData(&sd, n);
		CLogFind	Find;

		Log.FindSet(&Find, sd.call);

		DettachFocus();
		pBox->Execute(&Find, &sd, n);
		delete pBox;
        AttachFocus();
		if( !SBQSO->Down && Log.m_CurNo && Log.IsEdit() ){
			Log.SetLastPos();
			UpdateTextData();
		}
		AutoLogSave();
	}
}
//---------------------------------------------------------------------------

void __fastcall TMainVARI::SBFindMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if( Button == mbRight ){
		AnsiString as;
		if( InputMB("MMVARI", "Callsign", as) == TRUE ){
			jstrupr(as.c_str());
			CLogFind find;
			Log.FindSet(&find, as.c_str() );
			TQSODlgBox *pBox = new TQSODlgBox(this);
			DettachFocus();
			pBox->ShowFind(&find);
			delete pBox;
            AttachFocus();
        }
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KFLClick(TObject *Sender)
{
	if( m_Dump.IsLogging() ){
		m_Dump.CloseLogFile();
    }
    else {
		::SetCurrentDirectory(sys.m_BgnDir);
		char bf[256];
	    sprintf(bf, "%sRxLog", sys.m_BgnDir);
    	mkdir(bf);
		::GetLocal(&m_LocalTime);
    	sprintf(bf, "%sRxLog\\%04u%02u%02u.txt",
        	sys.m_BgnDir,
	    	m_LocalTime.wYear,
    	    m_LocalTime.wMonth,
	        m_LocalTime.wDay
    	);
	    m_Dump.OpenLogFile(bf);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KFLRClick(TObject *Sender)
{
	char bf[256];
    sprintf(bf, "%sRxLog", sys.m_BgnDir);
	TOpenDialog *pBox = new TOpenDialog(this);
	pBox->Options >> ofCreatePrompt;
	pBox->Options << ofFileMustExist;
	if( sys.m_MsgEng ){
		pBox->Title = "Load text file";
		pBox->Filter = "Text files(*.txt)|*.txt|";
	}
	else {
		pBox->Title = "ÉeÉLÉXÉgÉtÉ@ÉCÉãéQè∆";
		pBox->Filter = "ÉeÉLÉXÉgÉtÉ@ÉCÉã(*.txt)|*.txt|";
	}
	pBox->FileName = "";
	pBox->DefaultExt = "txt";
	pBox->InitialDir = bf;
	DettachFocus();
	OnWave();
	if( pBox->Execute() == TRUE ){
		m_Dump.FlushLogFile();
		OnWave();
		if( !m_pEdit ){
			m_pEdit = new TFileEdit(this);
        	m_pEdit->SetEvent(Handle, WM_WAVE, 2);
	    }
		if( m_pEdit ){
			m_pEdit->Execute(AnsiString(pBox->FileName).c_str(), FALSE);	//JA7UDE 0428
	    }
	}
    else {
		AttachFocus();
    }
	delete pBox;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KVSFTClick(TObject *Sender)
{
	if( (Sender == KVSFR) && (!m_pRadio || !RADIO.PollType) ){
		if( YesNoMB(sys.m_MsgEng ? "Radio command must be installed, do you open the setup window?":"ÉäÉOÉRÉìÉgÉçÅ[Éã(é¸îgêîéÊÇËçûÇ›)Çê›íËÇ∑ÇÈïKóvÇ™Ç†ÇËÇ‹Ç∑. ê›íËâÊñ ÇäJÇ´Ç‹Ç∑Ç©?") == IDYES ){
			KORClick(NULL);
        }
//        if( (!m_pRadio || !RADIO.PollType) ) return;
        if( !m_pRadio ) return;
    }
	m_ScaleAsRigFreq = (Sender == KVSFT) ? FALSE : TRUE;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KORClick(TObject *Sender)
{
	DettachFocus();
	TRADIOSetDlg *pBox = new TRADIOSetDlg(this);
	RADIO.change = pBox->Execute();
    delete pBox;
	AttachFocus();
    if( RADIO.change ) OpenRadio();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::UpdateSpeedList(int mode)
{
	if( IsMFSK(mode) ){
		SetComboBox(CBSpeed, "3.9063,7.8125,10.767,15.625,21.533,31.25,32.0,62.5");
    }
	else if( ::IsRTTY(mode) ){
		SetComboBox(CBSpeed, "45.45,75.0,110.0");
    }
    else {
		SetComboBox(CBSpeed, m_ListBAUD.c_str());
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SetMode(int mode)
{
	if( m_RxSet[0].m_Mode != mode ){
		if( (::IsMFSK(mode) != ::IsMFSK(m_RxSet[0].m_Mode)) ||
        	(::IsRTTY(mode) != ::IsRTTY(m_RxSet[0].m_Mode))
        ){
			UpdateSpeedList(mode);
        }
		m_RxSet[0].SetMode(mode);
	    m_ModFSK.SetType(mode);
#if DEBUG
		m_ModTest.SetType(mode);
#endif
		m_fDisEvent++;
		CBMode->ItemIndex = mode;
        m_fDisEvent--;
		m_Dump.SetRTTY(IsRTTY());
        UpdateLogMode();
        UpdateUI();
		switch(mode){
			case MODE_GMSK:
				SetInfoMsg(sys.m_MsgEng ? "GMSK : For MBCS language":"GMSK : MBCSì`ëóé¿å±óp(HF)");
            	break;
			case MODE_FSK:
				SetInfoMsg(sys.m_MsgEng ? "FSK : For V/UHF":"FSK : V/UHFë—ópÉÇÅ[Éh");
            	break;
			case MODE_FSKW:
				SetInfoMsg(sys.m_MsgEng ? "FSK-W : For V/UHF and satellite":"FSK-W : V/UHFë—/âqêØí êMópÉÇÅ[Éh");
            	break;
            case MODE_BPSK:
            	SetInfoMsg(sys.m_MsgEng ? "BPSK : No compatible on MBCS":"BPSK : è]óàï˚éÆÇ∆å›ä∑ê´Ç»Çµ(VariJAÇÕégópíçà”)");
                break;
            case MODE_N_BPSK:
            	SetInfoMsg(sys.m_MsgEng ? "bpsk : Compatible with conventionally VARICODE":"bpsk : è]óàï˚éÆÇ∆å›ä∑ê´Ç†ÇË");
                break;
            case MODE_RTTY:
            	SetInfoMsg("rtty(LSB) : BAUDOT");
                break;
            case MODE_U_RTTY:
            	SetInfoMsg("rtty(USB) : BAUDOT");
                break;
            case MODE_mfsk_L:
            case MODE_mfsk_U:
				sprintf(m_TextBuff, "mfsk%d (%s)", int((250.0/m_RxSet[0].m_pDem->m_MFSK_TONES)+0.5), mode == MODE_mfsk_L ? "LSB":"USB");
            	SetInfoMsg(m_TextBuff);
                break;
			case MODE_qpsk_L:
	        case MODE_qpsk_U:
            	SetInfoMsg(sys.m_MsgEng ? "qpsk : Compatible with conventionally VARICODE":"qpsk : è]óàï˚éÆÇ∆å›ä∑ê´Ç†ÇË");
				break;
        }
		UpdateLogMode();
		DoEvent(macOnMode);
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::CBModeChange(TObject *Sender)
{
	if( m_fDisEvent ) return;

    SetMode(CBMode->ItemIndex);
    SetTXFocus();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KECPClick(TObject *Sender)
{
	AnsiString as;

    if( m_Edit[m_CurrentEdit].GetSelText(as) ){
		Clipboard()->AsText = as;
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KEXClick(TObject *Sender)
{
	AnsiString as;

    if( m_Edit[m_CurrentEdit].GetSelText(as) ){
		Clipboard()->AsText = as;
		m_Edit[m_CurrentEdit].DeleteSelText();
        m_Edit[m_CurrentEdit].Paint();
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::PBoxTXMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if( Button == mbLeft ){
		if( m_fpText ) return;
		m_Edit[m_CurrentEdit].MoveCursor(X, Y, TRUE);
        m_Edit[m_CurrentEdit].OpenSelect();
        m_MouseDown = TRUE;
    }
    else if( Button == mbRight ){
		RECT rc;
		::GetWindowRect(PCTX->Handle, &rc);
        rc.left += X; rc.top += Y+8;
		PupTX->Popup(rc.left, rc.top);
    }
}
//---------------------------------------------------------------------------

void __fastcall TMainVARI::PBoxTXMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
	if( m_MouseDown ){
		m_Edit[m_CurrentEdit].MoveCursor(X, Y, FALSE);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::PBoxTXMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
    m_Edit[m_CurrentEdit].CloseSelect();
	m_MouseDown = FALSE;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::DoSuspend(BOOL fMinimize)
{
	if( !m_fSuspend ){
		DeleteMacroTimer();
		if( m_TX ) ToRX();
		m_Wave.InClose();
    	m_Wave.OutAbort();
        if( m_pCom ){
			delete m_pCom;
            m_pCom = NULL;
        }
        if( m_pRadio ){
			delete m_pRadio;
            m_pRadio = NULL;
        }
		if( m_fpText ) fflush(m_fpText);
        Log.Close();
        UpdateLogPanel();
		m_fSuspend = TRUE;
		m_fShowMsg = TRUE;
    }
    if( fMinimize && (WindowState != wsMinimized) ) Application->Minimize();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::DoResume(void)
{
	if( m_fSuspend ){
		if( OpenSound(FALSE) ){
			OpenCom();
            OpenRadio();
			Log.Open(NULL, TRUE);
            UpdateLogPanel();
            m_fSuspend = FALSE;
            if( WindowState == wsMinimized ) Application->Restore();
        }
        else {
			Draw(FALSE);
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KECallClick(TObject *Sender)
{
	SetTXFocus();
	TMenuItem *pItem = (TMenuItem *)Sender;
	StrCopy(m_TextBuff, AnsiString(pItem->Caption).c_str(), sizeof(m_TextBuff)-1);	//JA7UDE 0428
	LPSTR p = m_TextBuff;
    for( ; *p; p++ ){
		if( *p == '\t' ){
			*p = 0;
            break;
        }
    }
	DoMacro(m_TextBuff, NULL);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SetEditPage(int cno)
{
	if( cno == m_CurrentEdit ) return;

	DettachFocus();
	CDump *pEdit = &m_Edit[cno];
    CDump *pOld = &m_Edit[m_CurrentEdit];
	m_CurrentEdit = cno;
	if( !pEdit->IsCreate() ){
	    pEdit->Create(PCTX->Handle, PCTX, PBoxTX, PCTX->Font, SBarTX, 512);
	    pEdit->SetCursorType(csCARET);
    }
    if( memcmp(pEdit->GetLogFontP(), pOld->GetLogFontP(), sizeof(LOGFONT)) ||
    	pEdit->GetWindowSize() != pOld->GetWindowSize()
    ){
		pEdit->SetFont(PCTX->Font);
		pEdit->Resize();
    }
    else {
		pEdit->SetScrollBar();
    }
    pEdit->SetMBCS(&m_RxSet[0].m_MBCS);
    pEdit->m_fConvAlpha = m_fConvAlpha;
	PBoxTX->Invalidate();
    AttachFocus();
	PageStatus();
}
//---------------------------------------------------------------------------

void __fastcall TMainVARI::KPG1Click(TObject *Sender)
{
	int cno = 0;
    if( Sender == KPG2 ){
		cno = 1;
    }
    else if( Sender == KPG3 ){
		cno = 2;
    }
    else if( Sender == KPG4 ){
		cno = 3;
    }
    SetEditPage(cno);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::PupPagePopup(TObject *Sender)
{
	switch(m_CurrentEdit){
		case 1:
			KPG2->Checked = TRUE;
        	break;
        case 2:
			KPG3->Checked = TRUE;
        	break;
        case 3:
			KPG4->Checked = TRUE;
        	break;
        default:
			KPG1->Checked = TRUE;
        	break;
    }
	OnMenuProc(PupPage->Items, "PopPAGE");
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::InitCollect(CRxSet *pRxSet, int n)
{
	if( pRxSet != &m_RxSet[0] ) return;

	if( n < 4 ) n = 4;
	if( n > 24 ) n = 24;
	m_WaveBitMax = n;
	InitCollect();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::InitCollect(void)
{
	double speed = m_RxSet[0].m_Speed;
    if( m_RxSet[0].IsMFSK() ) speed = m_RxSet[0].m_pDem->m_MFSK_SPEED;
    m_Collect1.Create(DEMSAMPFREQ*m_WaveBitMax/speed);
    m_Collect2.Create(DEMSAMPFREQ*m_WaveBitMax/speed);
	if( m_RxSet[0].IsActive() ){
	    int n = m_WaveBitMax;
    	if( !(n & 1) ) n++;
	    m_RxSet[0].m_pDem->m_Decode.m_cBWave = n;
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SBWaveMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if( Button == mbRight ){
		m_WaveType = m_WaveType ? 0 : 1;
        UpdateWaveCaption();
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KSASClick(TObject *Sender)
{
	KSTXClick(NULL);
	DoMacro(sys.m_AS.c_str(), NULL);
}
//---------------------------------------------------------------------------
#if DEBUG
void __fastcall TMainVARI::TestSignal(void)
{
	DettachFocus();
	TTestDlg *pBox = new TTestDlg(this);
	AnsiString fName = sys.m_testName.c_str();
	int r = pBox->Execute();
	delete pBox;
    if( r ){
		double snr = sys.m_testSN-20.9;
		if( snr >= (20.9-3.0) ){
			sys.m_testGain = (16384);
			sys.m_testNoiseGain = (16384) / pow(10, snr/20.0);
   	    }
	    else {
			sys.m_testNoiseGain = 2369.5;
   	    	sys.m_testGain = 2369.5 * pow(10, snr/20.0);
   	    }
        sys.m_testGain2 = sys.m_testGain * pow(10, sys.m_testDB2/20.0);
		m_TestHPF.Create(ffHPF, 700, SAMPFREQ+SAMPTXOFFSET, 3, 0, 0);
   	    m_QSB.Create(sys.m_testGain / pow(10, sys.m_testQSBDB/20.0), sys.m_testGain, sys.m_testQSBTime, sys.m_testPhase);
        double baud = m_RxSet[0].m_Speed - m_RxSet[0].m_Speed * sys.m_testClockErr * 1.0e-6;
		m_ModTest.SetSpeed(baud);
		if( sys.m_testCarrier1 ) m_ModTest.SetCarrierFreq(sys.m_testCarrier1);
		if( sys.m_testCarrier2 ) m_VCOTest.SetFreeFreq(sys.m_testCarrier2);
		if( sys.m_testCarrier1 ){
			int fq = sys.m_testCarrier1;
			if( !sys.m_MFSK_Center ){
				switch( m_RxSet[0].m_Mode ){
					case MODE_mfsk_L:
						fq -= 125;
            	    	break;
                	case MODE_mfsk_U:
						fq += 125;
                		break;
            	}
            }
			int bw = (sys.m_test500 == 1) ? 250 : 135;
            int tap = (sys.m_test500 == 1) ? 96 : 128;
        	m_BPF500.Create(tap, ffBPF, SAMPFREQ+SAMPTXOFFSET, fq-bw, fq+bw, 60.0, 1.0);
        }
		if( r == 2 ){		// èIóπ
			if( m_fpTest ){
				fclose(m_fpTest);
    	    	m_fpTest = NULL;
            }
			if( sys.m_test ){
    		    m_Wave.OutAbort();
				OpenSound(FALSE);
    	    	sys.m_test = FALSE;
    		}
        }
        else {				// äJén
			if( m_fpTest && (fName != sys.m_testName) ){
				fclose(m_fpTest);
    	    	m_fpTest = NULL;
				m_ModTest.m_Encode.Reset(FALSE);
            }
			::SetCurrentDirectory(sys.m_BgnDir);
    		if( !m_fpTest ) m_fpTest = fopen(sys.m_testName.c_str(), "rb");
			if( !sys.m_test ){
				sys.m_test = TRUE;
				m_ModTest.m_Encode.Reset(FALSE);
                m_ModTest.Reset();
   		    	m_TestTimer = 5 * SAMPFREQ / m_BufferSize;
			    if( m_TX ) ToRX();
       		    m_Wave.InClose();
				OpenSound(TRUE);
       		    for( int i = 0; i < m_Wave.m_OutFifoSize; i++ ){
					DoDem();
       		    	m_Wave.OutWrite(m_wBuffer);
       		    }
            }
		}
    }
	AttachFocus();
}
#endif
//---------------------------------------------------------------------------
void __fastcall TMainVARI::OpenWheelTimer(CDump *pDump)
{
	if( m_pDump != pDump ){
		m_pDump = pDump;
    }
    if( !m_pWheelTimer ){
		m_pWheelTimer = new TTimer(this);
	    m_pWheelTimer->OnTimer = WheelTimer;
    	m_pWheelTimer->Interval = 500;
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::CloseWheelTimer(CDump *pDump)
{
	if( m_pDump == pDump ){
    	m_pDump = NULL;
        if( m_pWheelTimer ){
			delete m_pWheelTimer;
            m_pWheelTimer = NULL;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::OnMouseWheel(TMessage &Message)
{
	if( !sys.m_EnableMouseWheel ) return;
    if( m_Dump.OnMouseWheel(Message.WParam >> 16) ){
		OpenWheelTimer(&m_Dump);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SetATCSpeed(int f)
{
	m_ATCSpeed = f;
    CRxSet *pRxSet = m_RxSet;
	for( int i = 0; i < RXMAX; i++, pRxSet++ ){
    	if( pRxSet->IsActive() ) pRxSet->m_pDem->m_Decode.SetATCSpeed(f);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SetATCLimit(int f)
{
	m_ATCLimit = f;
    CRxSet *pRxSet = m_RxSet;
	for( int i = 0; i < RXMAX; i++, pRxSet++ ){
    	if( pRxSet->IsActive() ) pRxSet->m_pDem->m_Decode.SetATCLimit(f);
    }
}
//---------------------------------------------------------------------------
int __fastcall TMainVARI::GetSignalFreq(int fo, int fm, CRxSet *pRxSet)
{
	return GetSignalFreq(fo, fm, pRxSet, 600);
}
//---------------------------------------------------------------------------
int __fastcall TMainVARI::GetSignalFreq(int fo, int fm, CRxSet *pRxSet, int th)
{
	if( pRxSet->IsMFSK() ){
		return fo;
    }
	else if( pRxSet->Is170() ){
    	if( (pRxSet != &m_RxSet[0]) || pRxSet->m_fAFC ){
			fm = pRxSet->m_pDem->m_RTTYShift + 30;
        }
        else {
        	return fo;
        }
    }
	int xl = (fo - fm) * FFT_SIZE / m_FFTSampFreq;
	int xh = (fo + fm) * FFT_SIZE / m_FFTSampFreq;
	int x;

    if( xl < 0 ){
		x = xl;
        xl = 0;
        xh -= x;
    }

    if( xl >= xh ) return fo;

	int d;
    int avg = 0;
    int max = 0;
    for(x = xl; x < xh; x++ ){
    	d = m_fftout[x];
		avg += d;
        if( max < d ) max = d;
    }
    avg /= (xh - xl);
	int maxdb, avgdb;
    if( m_FFT.m_FFTGain ){
		avgdb = SqrtToDB(avg);
		maxdb = SqrtToDB(max);
    }
    else {
		double k = 100.0 / m_fftSC;
        avgdb = avg * k;
        maxdb = max * k;
	}
	if( (maxdb - avgdb) >= th ){
		max = (max + (avg * 2)) / 3;
	    int fl = 0;
    	int fh = 0;
        for(x = xl; x <= xh; x++ ){
			if( m_fftout[x] >= max ){
				if( !fl ) fl = x;
                fh = x;
            }
        }
		if( pRxSet->Is170() ){
			xl = (pRxSet->m_pDem->m_RTTYShift - 20) * FFT_SIZE/m_FFTSampFreq;
    	    xh = (pRxSet->m_pDem->m_RTTYShift + 30) * FFT_SIZE/m_FFTSampFreq;
        }
        else {
			xl = 0;
    	    xh = (pRxSet->m_Speed + 10) * FFT_SIZE/m_FFTSampFreq;
        }
       	max = ABS(fh-fl);
		if( ((max >= xl) && (max <= xh)) ){
	    	x = (fl + fh) / 2;
			fo = x * m_FFTSampFreq / FFT_SIZE;
        }
	}
    return fo;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::LoadMacro(LPCSTR pName)
{
	AnsiString as;
    GetFullPathName(as, pName);
	TMemIniFile	*pIniFile = new TMemIniFile(as.c_str());
	LoadMacro(pIniFile);
	delete pIniFile;
	CreateMacButton();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SaveMacro(LPCSTR pName)
{
	AnsiString as;
    GetFullPathName(as, pName);
	TMemIniFile	*pIniFile = new TMemIniFile(as.c_str());
	SaveMacro(pIniFile);
	pIniFile->UpdateFile();
	delete pIniFile;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::LoadMacro(TMemIniFile *pIniFile)
{
	char bf[64];
	AnsiString as, ws;
	MACBUTTON *pList = m_tMacButton;
    for( int i = 0; i < MACBUTTONALL; i++, pList++ ){
		sprintf(bf, "MB%u", i+1);
		pList->Name = pIniFile->ReadString(bf, "Name", pList->Name);
		CrLf2Yen(as, pList->Text);
		ws = pIniFile->ReadString(bf, "Text", as);
		Yen2CrLf(pList->Text, ws);
        pList->Color = (TColor)pIniFile->ReadInteger(bf, "Color", pList->Color);
        pList->Style = pIniFile->ReadInteger(bf, "Style", pList->Style);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SaveMacro(TMemIniFile *pIniFile)
{
	char bf[64];
    AnsiString as;
	MACBUTTON *pList = m_tMacButton;
    for( int i = 0; i < MACBUTTONALL; i++, pList++ ){
		sprintf(bf, "MB%u", i+1);
		pIniFile->WriteString(bf, "Name", pList->Name);
		CrLf2Yen(as, pList->Text);
		pIniFile->WriteString(bf, "Text", as);
        pIniFile->WriteInteger(bf, "Color", pList->Color);
        pIniFile->WriteInteger(bf, "Style", pList->Style);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KFMSClick(TObject *Sender)
{
	TSaveDialog *pBox = new TSaveDialog(this);
	pBox->Options << ofOverwritePrompt;
    pBox->Options << ofNoReadOnlyReturn;
	if( sys.m_MsgEng ){
		pBox->Title = "Save Macros";
		pBox->Filter = "Macro Files(*.mac)|*.mac|";
	}
	else {
		pBox->Title = "É}ÉNÉçÇÃÉZÅ[Éu";
		pBox->Filter = "É}ÉNÉçÉtÉ@ÉCÉã(*.mac)|*.mac|";
	}
	pBox->FileName = "Macros";
	pBox->DefaultExt = "mac";
	pBox->InitialDir = sys.m_BgnDir;
	DettachFocus();
	OnWave();
	if( pBox->Execute() == TRUE ){
		OnWave();
		SaveMacro(AnsiString(pBox->FileName).c_str());	//JA7UDE 0428
	}
	delete pBox;
	AttachFocus();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KFMLClick(TObject *Sender)
{
	TOpenDialog *pBox = new TOpenDialog(this);
	pBox->Options >> ofCreatePrompt;
	pBox->Options << ofFileMustExist;
	if( sys.m_MsgEng ){
		pBox->Title = "Load Macros";
		pBox->Filter = "Macro Files(*.mac)|*.mac|";
	}
	else {
		pBox->Title = "É}ÉNÉçÇÃÉçÅ[Éh";
		pBox->Filter = "É}ÉNÉçÉtÉ@ÉCÉã(*.mac)|*.mac|";
	}
	pBox->FileName = "";
	pBox->DefaultExt = "mac";
	pBox->InitialDir = sys.m_BgnDir;
	DettachFocus();
	OnWave();
	if( pBox->Execute() == TRUE ){
		OnWave();
		LoadMacro(AnsiString(pBox->FileName).c_str());	//JA7UDE 0428
	}
	delete pBox;
	AttachFocus();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::PupRXWPopup(TObject *Sender)
{
	int n = GetPopupIndex(PupRXW->PopupComponent);
	TScrollBar *pBar;
    CDump *pDump;
    if( n ){
        pBar = m_RxSet[n].m_pView->SBar;
        pDump = &m_RxSet[n].m_pView->m_Dump;
    }
    else {
        pBar = SBarRX;
        pDump = &m_Dump;
    }
	BOOL f;
	KRWC->Enabled = (f=pDump->IsPrinted());
	KRWE->Enabled = f;
	KRWT->Enabled = (f=pBar->Enabled);
    KRWB->Enabled = f;
    KRSC->Checked = sys.m_fShowCtrlCode;
	OnMenuProc(PupRXW->Items, "PopRXW");
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KRWCClick(TObject *Sender)
{
	int n = GetPopupIndex(PupRXW->PopupComponent);
    if( n ){
		m_RxSet[n].m_pView->m_Dump.Clear();
    }
    else {
		m_Dump.Clear();
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KRWTClick(TObject *Sender)
{
	int n = GetPopupIndex(PupRXW->PopupComponent);
	TScrollBar *pBar;
    if( n ){
        pBar = m_RxSet[n].m_pView->SBar;
    }
    else {
        pBar = SBarRX;
    }
	if( pBar->Enabled ){
		pBar->Position = (Sender == KRWT) ? 0 : pBar->Max;
		if( Sender == KRWB ) SetTXFocus();
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KRWEClick(TObject *Sender)
{
	int n = GetPopupIndex(PupRXW->PopupComponent);
	CDump *pDump;
    if( n ){
        pDump = &m_RxSet[n].m_pView->m_Dump;
    }
    else {
        pDump = &m_Dump;
    }
	AnsiString	as;
    pDump->GetWindowText(as);
    if( !m_pEdit ){
		m_pEdit = new TFileEdit(this);
        m_pEdit->SetEvent(Handle, WM_WAVE, 2);
    }
    m_pEdit->Execute(as, "Received text", "Received.txt");
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::PupSpecPopup(TObject *Sender)
{
	BOOL fs = m_fSubWindow && m_MouseSubChannel;
    BOOL f = fs || m_MouseNotch;
	KSAS->Visible = !f;
    KSTX->Visible = !f;
    KSRX->Visible = !f;
    KSN->Visible = !f;
    KSNR->Visible = !fs && m_MouseNotch && m_Notches.m_Count;
    KSNRA->Visible = !fs && m_Notches.m_Count;
    KSS->Visible = !f;
    N21->Visible = !f;
    N23->Visible = !f;
	KSW->Visible = fs;
    KSSR->Visible = fs;
    KSSM->Visible = fs;
	if( fs ){
		sprintf(m_TextBuff, sys.m_MsgEng ? "CH.%d <=swap=> main channel":"CH.%d <=ì¸ÇÍë÷Ç¶=> ÉÅÉCÉìÉ`ÉÉÉìÉlÉã", m_MouseSubChannel);
	    KSW->Caption = m_TextBuff;
		sprintf(m_TextBuff, sys.m_MsgEng ? "CH.%d Close window":"CH.%d ÉEÉCÉìÉhÉEÇï¬Ç∂ÇÈ", m_MouseSubChannel);
        KSSR->Caption = m_TextBuff;
		sprintf(m_TextBuff, sys.m_MsgEng ? "CH.%d Set mode":"CH.%d ÉÇÅ[ÉhÇê›íË", m_MouseSubChannel);
        KSSM->Caption = m_TextBuff;

		BOOL fNew = !KSSM->Count;
		TRxViewDlg *pView = m_RxSet[m_MouseSubChannel].m_pView;
		pView->PupModePopup(NULL);
		TMenuItem *pMenu = pView->PupMode->Items;
		int i;
        for( i = 0; i < pMenu->Count; i++ ){
            	TMenuItem *pm;
                if( fNew ){
					pm = new TMenuItem(KSSM);
                }
                else {
					pm = KSSM->Items[i];
                }
                pm->GroupIndex = pMenu->Items[i]->GroupIndex;
                pm->RadioItem = pMenu->Items[i]->RadioItem;
                pm->Caption = pMenu->Items[i]->Caption;
                pm->OnClick = pMenu->Items[i]->OnClick;
                pm->Visible = pMenu->Items[i]->Visible;
                pm->Checked = pMenu->Items[i]->Checked;
				if( fNew ) KSSM->Add(pm);
        }
    }
	for( int i = 1; i < RXMAX; i++ ){
		TMenuItem *pm = KSS->Items[i-1];
        pm->Checked = m_RxSet[i].IsActive();
    }
	OnMenuProc(PupSpec->Items, "PopWATER");
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KSWClick(TObject *Sender)
{
	::PostMessage(Handle, WM_WAVE, waveSwapRxView, DWORD(m_RxSet[m_MouseSubChannel].m_pView));
}
//---------------------------------------------------------------------------
int __fastcall TMainVARI::GetOverlayTop(void)
{
	int y = 0;
	int i;
    CRxSet *pRxSet = &m_RxSet[1];
    for( i = 1; i < RXMAX; i++, pRxSet ){
		if( pRxSet->IsActive() ){
			if( pRxSet->m_pView->Top < 32 ){
				int yy = pRxSet->m_pView->Top + pRxSet->m_pView->Height;
                if( y < yy ) y = yy;
            }
        }
    }
    return y;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SetConvAlpha(BOOL f)
{
	m_fConvAlpha = f;
    m_Edit[m_CurrentEdit].m_fConvAlpha = f;
}
//---------------------------------------------------------------------------

void __fastcall TMainVARI::KVSDClick(TObject *Sender)
{
	m_ScaleDetails = m_ScaleDetails ? FALSE : TRUE;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KORSCClick(TObject *Sender)
{
	m_fRttyWordOut = (Sender == KORSW);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KSNClick(TObject *Sender)
{
	m_Notches.Add(m_RightFreq);
	m_NotchFreq = m_RightFreq;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KSTXClick(TObject *Sender)
{
	if( m_TX || !SBNET->Down ){
		SetTxFreq(m_RightFreq);
    }
    else if( SBNET->Down ){
		SBNET->Down = FALSE;
		SetTxFreq(m_RightFreq);
	}
	UpdateUI();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KSRXClick(TObject *Sender)
{
	m_RxSet[0].m_pDem->m_Decode.Reset();
	SetRxFreq(m_RightFreq);
    m_RxSet[0].m_AFCTimerMFSK = MFSKAFC_MAX;
    UpdateUI();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KSNRClick(TObject *Sender)
{
	if( m_MouseNotch ){
		m_Notches.Delete(m_MouseNotch-1);
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KSNRAClick(TObject *Sender)
{
	m_Notches.Delete();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KSSRClick(TObject *Sender)
{
	ShowSubChannel(m_MouseSubChannel, 0);
}
//---------------------------------------------------------------------------
BOOL __fastcall TMainVARI::IsFreqErr(double d)
{
	if( IsBPSK() ){
		return fabs(d) > 0.30;		// 0.15 X 2.0
    }
	else {
		return fabs(d) > 0.20;
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::DrawPF(BOOL fPaint)
{
	if( !m_PFTimer ){
		TCanvas *pCanvas = m_pBitmapPF->Canvas;
    	TRect rc;
	   	rc.Left = 0; rc.Top = 0; rc.Right = m_pfXW; rc.Bottom = m_pfYW;
   		pCanvas->Brush->Color = clBlack;
	   	pCanvas->Pen->Color = clBlack;
		pCanvas->Pen->Style = psSolid;
	   	pCanvas->FillRect(rc);

	    if( m_Wave.IsInOpen() || m_Wave.IsOutOpen() ){
			rc.Top ++;
            rc.Bottom--;
			BOOL fTX = (m_TX == txINTERNAL);
			CRxSet *pRxSet = m_RxSet;
			CDEMFSK *pDem = pRxSet->m_pDem;
			double d = pDem->GetFreqErr();
			if( IsBPSK() ){
				if( m_AFCWidth && !fTX ){
					double f = double(pRxSet->m_AFCFQ - UdRxCarrier->Position)/pRxSet->GetBandWidth();
					if( fabs(f) >= 0.2 ) d = f;
                }
                d *= 2.0;
            }
            if( fTX ) d *= 0.05;
			if( d > 0.0 ){
				rc.Left = m_pfXC + (d * m_pfXW);
    		    rc.Right = m_pfXC;
	    	}
	    	else {
				rc.Left = m_pfXC;
    		    rc.Right = m_pfXC + (d * m_pfXW);
		    }
			if( SBAFC->Down || fTX ){
	    		pCanvas->Brush->Color = TColor(RGB(144,144,144));
	        }
    	    else {
	    		pCanvas->Brush->Color = IsFreqErr(d) ? clRed : clYellow;
	        }
		   	pCanvas->FillRect(rc);
	    }
	    pCanvas->Pen->Color = SBAFC->Down ? TColor(RGB(0,255,0)) : clWhite;
    	pCanvas->MoveTo(m_pfXC, 0);
	    pCanvas->LineTo(m_pfXC, m_pfYW);
    	if( fPaint ) PBoxPFPaint(NULL);
	}
    else {
		m_PFTimer--;
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::PBoxPFPaint(TObject *Sender)
{
	PBoxPF->Canvas->Draw(0, 0, m_pBitmapPF);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::OutYaesuVU(int hz)
{
	if( !m_pRadio ) return;

	char bf[256];
	int hz1 = hz / 10000000;
    hz = hz % 10000000;
    int hz2 = hz / 100000;
    hz = hz % 100000;
    int hz3 = hz / 1000;
    hz = hz % 1000;
	sprintf(bf, "\\$%02u%02u%02u%02u01", hz1, hz2, hz3, hz/10);
	m_pRadio->SendCommand(bf);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::OutYaesuHF(int hz)
{
	if( !m_pRadio ) return;

	char bf[256];
	int hz1 = hz / 10000000;
    hz = hz % 10000000;
    int hz2 = hz / 100000;
    hz = hz % 100000;
    int hz3 = hz / 1000;
    hz = hz % 1000;
	sprintf(bf, "\\$%02u%02u%02u%02u0A", hz/10, hz3, hz2, hz1);
	m_pRadio->SendCommand(bf);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::OutCIV(int hz)
{
	if( !m_pRadio ) return;

	char bf[256];
	int hz1 = hz / 100000000;
	hz = hz % 100000000;
	int hz2 = hz / 1000000;
	hz = hz % 1000000;
	int hz3 = hz / 10000;
	hz = hz % 10000;
    int hz4 = hz / 100;
    hz = hz % 100;
	sprintf(bf, "\\$FEFExxE005%02u%02u%02u%02u%02uFD", hz, hz4, hz3, hz2, hz1);
	WaitICOM();
	m_pRadio->SendCommand(bf);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::OutCIV4(int hz)
{
	if( !m_pRadio ) return;

	char bf[256];
	if( hz >= 100000000 ){	// 100MHz and up
		int hz1 = hz / 100000000;
		hz = hz % 100000000;
		int hz2 = hz / 1000000;
		hz = hz % 1000000;
		int hz3 = hz / 10000;
		hz = hz % 10000;
        int hz4 = hz / 100;
        hz = hz % 100;
		sprintf(bf, "\\$FEFExxE005%02u%02u%02u%02u%02uFD", hz, hz4, hz3, hz2, hz1);
    }
    else {
		int hz1 = hz / 1000000;
		hz = hz % 1000000;
		int hz2 = hz / 10000;
		hz = hz % 10000;
		int hz3 = hz / 100;
		hz = hz % 100;
		sprintf(bf, "\\$FEFExxE005%02u%02u%02u%02uFD", hz, hz3, hz2, hz1);
	}
	WaitICOM();
	m_pRadio->SendCommand(bf);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::OutKENWOOD(int hz)
{
	if( !m_pRadio ) return;

	char bf[256];
    sprintf(bf, "FA0%010lu;", hz);
    m_pRadio->SendCommand(bf);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::OutJST245(int hz)
{
	if( !m_pRadio ) return;

	char bf[256];
    sprintf(bf, "H1\\rF%08luA\\rH0\\rI1\\r", hz);
    m_pRadio->SendCommand(bf);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::OutModeYaesuHF(LPCSTR pMode)
{
	if( !m_pRadio ) return;

    const BYTE _td[]={
		0x00, 0x01, 0x02, 0x04, 0x06, 0x08, 0x0a,
    };
    int n = FindStringTable(g_tRadioMode, pMode, AN(_td));
	if( n >= 0 ){
		char bf[256];
		sprintf(bf, "\\$000000%02X0C", _td[n]);
		m_pRadio->SendCommand(bf);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::OutModeYaesuVU(LPCSTR pMode)
{
	if( !m_pRadio ) return;

    const BYTE _td[]={
		0x00, 0x01, 0x82, 0x04, 0x08,
    };
    int n = FindStringTable(g_tRadioMode, pMode, AN(_td));
	if( n >= 0 ){
		char bf[256];
		sprintf(bf, "\\$%02X00000007", _td[n]);
		m_pRadio->SendCommand(bf);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::OutModeCIV(LPCSTR pMode)
{
	if( !m_pRadio ) return;

    const BYTE _td[]={
		0x00, 0x01, 0x03, 0x02, 0x05, 0x04,
    };
    int n = FindStringTable(g_tRadioMode, pMode, AN(_td));
	if( n >= 0 ){
		char bf[256];
		sprintf(bf, "\\$FEFExxE006%02XFD", _td[n]);
		WaitICOM();
		m_pRadio->SendCommand(bf);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::OutModeKENWOOD(LPCSTR pMode)
{
	if( !m_pRadio ) return;

    const BYTE _td[]={
		1, 2, 3, 5, 4, 6,
    };
    int n = FindStringTable(g_tRadioMode, pMode, AN(_td));
	if( n >= 0 ){
		char bf[256];
		sprintf(bf, "MD%d;", int(_td[n]));
		m_pRadio->SendCommand(bf);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::OutModeJST245(LPCSTR pMode)
{
	if( !m_pRadio ) return;

    const BYTE _td[]={
		3, 2, 1, 4, 5, 0,
    };
    int n = FindStringTable(g_tRadioMode, pMode, AN(_td));
	if( n >= 0 ){
		char bf[256];
		sprintf(bf, "H1\rD%d\rH0\rI1\r", int(_td[n]));
		m_pRadio->SendCommand(bf);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::WaitICOM(void)
{
	if( m_pRadio && m_pRadioTimer ){
		m_pRadio->WaitICOM(m_pRadioTimer->Interval);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KVMXClick(TObject *Sender)
{
	if( KVMX->Checked ){
		DeleteMacExButton();
    }
    else {
		CreateMacExButton();
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::DeleteMacExButton(void)
{
	KVMX->Checked = FALSE;
	if( PBoxFFT->Align != alClient ){
		PBoxFFT->Align = alClient;
        m_fftYW = PBoxFFT->Height;
		m_pBitmapFFT->Canvas->Brush->Color = m_tWaterColset[0].c;
		m_pBitmapFFT->Height = m_fftYW;
    }
	MACBUTTON *pList = &m_tMacButton[MACBUTTONMAX];
    for( int i = 0; i < MACEXBUTTONMAX; i++, pList++ ){
		TSpeedButton *pButton = pList->pButton;
       	if( pButton ){
			PFFT->RemoveControl(pList->pButton);
            delete pList->pButton;
            pList->pButton = NULL;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::CreateMacExButton(void)
{
	KVMX->Checked = TRUE;
	MACBUTTON *pList = &m_tMacButton[MACBUTTONMAX];
	if( PBoxFFT->Align == alClient ){
		PBoxFFT->Align = alTop;
        PBoxFFT->Height = PFFT->Height - 24;
        m_fftYW = PBoxFFT->Height;
		m_pBitmapFFT->Height = m_fftYW;
    }
	int x = PBoxFFT->Left;
    int max = (PBoxFFT->Width / 60);
    if( max > MACEXBUTTONMAX ) max = MACEXBUTTONMAX;
    int xw = PBoxFFT->Width / max;
    int xx = PBoxFFT->Width % max;
    int y = PBoxFFT->Top + PBoxFFT->Height+2;
    int yw = 16;
    int i;
    for( i = 0; i < max; i++, pList++ ){
		TSpeedButton *pButton = pList->pButton;
       	if( !pButton ){
			pButton = new TSpeedButton(PFFT);
			PFFT->InsertControl(pButton);
           	pButton->Parent = PFFT;
			pButton->OnClick = OnMacButtonClick;
            pButton->OnMouseDown = OnMacButtonDown;
            pButton->GroupIndex = 1;
            pButton->AllowAllUp = TRUE;
			char bf[16];
            sprintf(bf, "%%M%u", i+MACBUTTONMAX);
            pButton->Hint = bf;
            pButton->Down = (i == m_CurrentMacro) && (m_ReqMacroTimer || m_pMacroTimer);
            pList->pButton = pButton;
       	}
        else {
			pButton->Font->Name = sys.m_FontName;
            pButton->Font->Charset = sys.m_FontCharset;
        }
       	pButton->Caption = ConvAndChar(m_TextBuff, pList->Name.c_str());
		int xxw = xw;
        if( xx ){
			xxw++;
            xx--;
        }
        pButton->SetBounds(x, y, xxw, yw);
		pButton->Font->Height = -(yw-4);
        pButton->Font->Color = pList->Text.IsEmpty() ? clGrayText : pList->Color;
		TFontStyles fs = Code2FontStyle(pList->Style);
        pButton->Font->Style = fs;
        x += xxw;
    }
    for( ; i < MACEXBUTTONMAX; i++, pList++ ){
		TSpeedButton *pButton = pList->pButton;
       	if( pButton ){
			PFFT->RemoveControl(pList->pButton);
            delete pList->pButton;
            pList->pButton = NULL;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::DoMacroMenu(LPCSTR pVal, TSpeedButton *pButton, BOOL fRadio)
{
	DettachFocus();
	AnsiString as;
    LPSTR pBF = new char[8192];
	StrCopy(pBF, GetMacroStr(as, pVal), 8191);
	LPSTR t, p;
	p = pBF;
	if( m_pMacroPopup ){
		delete m_pMacroPopup;
	}
	m_pMacroPopup = new TPopupMenu(this);
	int nInit = -1;
	if( fRadio ){
		p = StrDlm(t, SkipSpace(p));
        nInit = GetMacroInt(t);
    }
    strcpy(p, GetMacroStr(as, p));
	int n = 0;
    BYTE gIndex = 1;
	while(*p){
		p = StrDlm(t, SkipSpace(p));
		if( *t ){
			TMenuItem *pMenu = new TMenuItem(m_pMacroPopup);
			pVal = GetMacroStr(as, t);
			pMenu->Caption = pVal;
			if( strcmp(pVal, "-") ){
				pMenu->OnClick = OnMacroMenuClick;
				if( fRadio ){
		            n++;
					pMenu->GroupIndex = gIndex;
                    pMenu->RadioItem = TRUE;
					pMenu->Checked = (n == nInit);
            	}
            }
            else {
				gIndex++;
				if( fRadio ){
					p = StrDlm(t, SkipSpace(p));
			        nInit = GetMacroInt(t);
                    n = 0;
			    }
            }
			m_pMacroPopup->Items->Add(pMenu);
		}
	}
	delete pBF;
	BOOL fButton = pButton && pButton->Down;
	if( fButton ) pButton->Down = FALSE;
	RECT rc;
	HWND hWnd;
	if( fButton ){
		hWnd = GetMacButtonNo(pButton) >= MACBUTTONMAX ? PFFT->Handle : PCMac->Handle;
	}
	else {
		hWnd = PCTX->Handle;
	}
	::GetWindowRect(hWnd, &rc);
	int X, Y;
	if( fButton ){
		rc.left += pButton->Left + pButton->Width/2;
		rc.top += pButton->Top + pButton->Height/2;
	}
	else {
		m_Edit[m_CurrentEdit].GetCursorPos(X, Y);
		rc.left += X; rc.top += Y+8;
	}
	m_MacroInput = "";
	m_MacroMenuNo = 0;
	m_pMacroPopup->Popup(rc.left, rc.top);
	Application->ProcessMessages();
	delete m_pMacroPopup;
	m_pMacroPopup = NULL;
	AttachFocus();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SBP60Click(TObject *Sender)
{
	TSpeedButton *pButton = (TSpeedButton *)Sender;

    int s =  m_PlayBackTime[0];
    if( Sender == SBP30 ){
		s = m_PlayBackTime[1];
    }
    else if( Sender == SBP15 ){
		s = m_PlayBackTime[2];
    }
	DoPlayBack(pButton->Down ? s : 0);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::UpdatePlayBack(void)
{
	SBP15->Caption = m_PlayBackTime[2];
	SBP30->Caption = m_PlayBackTime[1];
	SBP60->Caption = m_PlayBackTime[0];
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::DoPlayBack(int s)
{
	if( !m_PlayBack.IsActive() || !SBP60->Enabled ) return;

    if( s ){
		if( !m_PlayBack.StartPlaying(s) ){
			StopPlayBack();
        }
        else {
			DrawWater(FALSE, TRUE);
            if( s <= m_PlayBackTime[2] ){
				SBP15->Down = TRUE;
                if( m_PlayBackTime[2] != s ) SBP15->Caption = s;
            }
            else if( s <= m_PlayBackTime[1] ){
				SBP30->Down = TRUE;
                if( m_PlayBackTime[1] != s ) SBP30->Caption = s;
            }
            else {
				SBP60->Down = TRUE;
                if( m_PlayBackTime[0] != s ) SBP60->Caption = s;
            }
        }
    }
    else {
		m_PlayBack.StopPlaying();
        UpdatePlayBack();
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::StopPlayBack(void)
{
	if( m_PlayBack.IsActive() ){
		m_PlayBack.StopPlaying();
		SBP60->Down = FALSE;
		SBP30->Down = FALSE;
		SBP15->Down = FALSE;
		UpdatePlayBack();
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::UpdateShowCtrl(void)
{
	int i;
	CRxSet *pRxSet = &m_RxSet[1];
    for( i = 1; i < RXMAX; i++ ){
		if( pRxSet->m_pView ){
			pRxSet->m_pView->m_Dump.ShowCtrl(sys.m_fShowCtrlCode);
        }
    }
    m_Dump.ShowCtrl(sys.m_fShowCtrlCode);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KRSCClick(TObject *Sender)
{
	sys.m_fShowCtrlCode = !sys.m_fShowCtrlCode;
    UpdateShowCtrl();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::PupSQPopup(TObject *Sender)
{
	int n = GetPopupIndex(PupSQ->PopupComponent);
    n = m_RxSet[n].m_SQLevel;
    switch(n){
		case 100:
			KS1->Checked = TRUE;
			break;
		case 200:
			KS2->Checked = TRUE;
			break;
        case 300:
			KS3->Checked = TRUE;
			break;
        case 400:
			KS4->Checked = TRUE;
			break;
        default:
			if( n < 50 ){
				KS0->Checked = TRUE;
            }
            else {
				KS0->Checked = FALSE;
				KS1->Checked = FALSE;
				KS2->Checked = FALSE;
				KS3->Checked = FALSE;
				KS4->Checked = FALSE;
            }
        	break;
    }
	OnMenuProc(PupSQ->Items, "PopSQ");
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KS0Click(TObject *Sender)
{
	int sq = 0;
    if( Sender == KS1 ){
		sq = 100;
    }
    else if( Sender == KS2 ){
		sq = 200;
    }
    else if( Sender == KS3 ){
		sq = 300;
    }
    else if( Sender == KS4 ){
		sq = 400;
    }
	int n = GetPopupIndex(PupSQ->PopupComponent);
	m_RxSet[n].m_SQLevel = sq;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KHJClick(TObject *Sender)
{
	AnsiString as = ((TMenuItem *)Sender)->Hint;
	m_WebRef.ShowHTML(as.c_str());
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::DoEvent(int n)
{
	if( !sys.m_MacEvent[n].IsEmpty() ) DoMacro(sys.m_MacEvent[n].c_str(), NULL);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::MacroOnTimer(TObject *Sender)
{
	AnsiString as = sys.m_MacEvent[macOnTimer].c_str();
    if( !sys.m_MacroError ){
		DoMacro(as.c_str(), NULL);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::UpdateMacroOnTimer(void)
{
	if( sys.m_MacEvent[macOnTimer].IsEmpty() ){
		if( m_pMacroOnTimer ){
    		delete m_pMacroOnTimer;
        	m_pMacroOnTimer = NULL;
	    }
    }
    else {
		if( !m_pMacroOnTimer ){
		    m_pMacroOnTimer = new TTimer(this);
    		m_pMacroOnTimer->Interval = sys.m_OnTimerInterval;
			m_pMacroOnTimer->OnTimer = MacroOnTimer;
	    	m_pMacroOnTimer->Enabled = TRUE;
        }
    }
	sys.m_MacroError = FALSE;
}
//---------------------------------------------------------------------------
// ÉVÉXÉeÉÄÉVÉÉÉbÉgÉ_ÉEÉì
void __fastcall TMainVARI::ShutDown(void)
{

	DoSuspend(FALSE);
	WriteRegister();

	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;
	int WinNT = FALSE;
	// ÉAÉNÉZÉXÉgÅ[ÉNÉìÇÉIÅ[ÉvÉì
	if(OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)){
		WinNT = TRUE;

		// ì¡å†ñºÇÃLUIDÇéÊìæÇ∑ÇÈ
		LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);

		tkp.PrivilegeCount = 1;  /* one privilege to set      */
		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		// ÉAÉNÉZÉXÉgÅ[ÉNÉìÇÃì¡å†ÇìæÇÈ
		AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES) NULL, 0);
	}
	::ExitWindowsEx(sys.m_WinNT ? EWX_POWEROFF : EWX_SHUTDOWN, 0);
	Close();
	if( WinNT == TRUE ){
		// ÉAÉNÉZÉXÉgÅ[ÉNÉìÇÃì¡å†Çï˙ä¸Ç∑ÇÈ
		tkp.Privileges[0].Attributes = 0;
		AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES) NULL, 0);
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::UpdateModGain(void)
{
	m_ModGainR = m_ModGain;
    if( m_ModGainR < 1024 ) m_ModGainR = 1024;
    if( m_ModGainR > 31774 ) m_ModGainR = 31774;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::DoBench(LPSTR t, int type)
{
	CWaitCursor w;
	if( m_CPUBENCHType != type ){
		m_CPUBENCHType = type;
		m_CPUBENCH.Create(32);
	}
	LARGE_INTEGER ltFreq, ltOld;
	if( ::QueryPerformanceFrequency(&ltFreq) && ::QueryPerformanceCounter(&ltOld) ){
		int i;
		double d;
        if( type ){
			CDECM2	dec;
            CVCO	vco;
            vco.SetSampleFreq(SAMPFREQ);
            vco.SetFreeFreq(1000.0);
            dec.SetSampleFreq(m_fDec, SAMPFREQ);
			for( i = 0; i < 65536; i++ ){
				dec.Do(vco.Do());
           	}
        }
        else {
			for( i = 0; i < 23400; i++ ){
				d = double(rand() % int(2 * PI * 1000)) / 1000.0;
       	        sin(d*d);
           	}
        }
		LARGE_INTEGER ltNow;
        ::QueryPerformanceCounter(&ltNow);
        d = double(ltNow.QuadPart - ltOld.QuadPart)/double(ltFreq.QuadPart);
		d = m_CPUBENCH.Do(d);
		sprintf(t, "%.1lf", d * 1.0e6);
    }
}
//---------------------------------------------------------------------------
// ÉCÉìÉfÉbÉNÉXÇ©ÇÁÉÅÉjÉÖÅ[ÇìæÇÈ
TMenuItem* __fastcall TMainVARI::GetMenuItem(TMenuItem *pMainMenu, LPCSTR pTitle)
{
	int index;
	if( IsNumbAll(pTitle) && ((index = GetMacroInt(pTitle)) <= pMainMenu->Count) && (index >= 1) ){
		int n = 1;
        TMenuItem *pMenu;
    	for( int i = 0; i < pMainMenu->Count; i++ ){
        	pMenu = pMainMenu->Items[i];
			if( pMenu->Visible && (pMenu->Caption != '-') ){
				if( n == index ) return pMenu;
        	    n++;
	        }
    	}
    }
	return NULL;
}
//---------------------------------------------------------------------------
TMenuItem* __fastcall TMainVARI::FindMenu(TMenuItem *pMainMenu, LPCSTR pTitle)
{
	AnsiString as;
	int i;
    BOOL fAcc = (*pTitle == '&') && (strlen(pTitle)==2);
	TMenuItem *pMenu;
	if( (pMenu = GetMenuItem(pMainMenu, pTitle)) != NULL ){
		return pMenu;
    }
    else {
	    for( i = pMainMenu->Count - 1; i >= 0; i-- ){
        	pMenu = pMainMenu->Items[i];
			as = pMenu->Caption;
			if( !strcmp(as.c_str(), pTitle) ){
				return pMenu;
			}
            else if( fAcc ){
				for( LPCSTR p = as.c_str(); *p; p++ ){
					if( *p == '&' ){
                    	if( *(p+1) == '&' ){
							p++;
                        }
                        else if( !strncmpi(p, pTitle, 2) ){
							return pMenu;
                        }
                    }
                }
            }
	    }
    }
    return NULL;
}
//---------------------------------------------------------------------------
TMenuItem* __fastcall TMainVARI::GetMainMenu(LPCSTR pTitle, BOOL fNew)
{
	TMenuItem *pMenu = GetSubMenu(pTitle);
    if( pMenu ) return pMenu;

	TMenuItem *pMainMenu = MainMenu->Items;
	pMenu = FindMenu(pMainMenu, pTitle);
    if( !pMenu ){
		if( fNew ){
			pMenu = new TMenuItem(pMainMenu);
    	    pMenu->Caption = pTitle;
            pMenu->OnClick = OnXmClick;
        	pMainMenu->Insert(pMainMenu->IndexOf(KH), pMenu);
        }
        else {
			pMenu = NULL;
        }
    }
	return pMenu;
}
//---------------------------------------------------------------------------
TMenuItem* __fastcall TMainVARI::GetSubMenu(LPCSTR pTitle)
{
	if( !strcmp(pTitle, "PopWATER") ){
		return PupSpec->Items;
    }
	else if( !strcmp(pTitle, "PopPAGE") ){
		return PupPage->Items;
    }
	else if( !strcmp(pTitle, "PopRX") ){
		return PupRX->Items;
    }
	else if( !strcmp(pTitle, "PopRXW") ){
		return PupRXW->Items;
    }
	else if( !strcmp(pTitle, "PopSQ") ){
		return PupSQ->Items;
    }
	else if( !strcmp(pTitle, "PopTX") ){
		return PupTX->Items;
    }
	else if( !strcmp(pTitle, "PopCHARSET") ){
		return PupCharset->Items;
    }
	else if( !strcmp(pTitle, "PopCALLS") ){
		return PupCalls->Items;
    }
    return NULL;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::OnXmClick(TObject *Sender)
{
	if( !m_MacroVal.IsHandleProc() ) return;

	TMenuItem *pMenu = (TMenuItem *)Sender;
	AnsiString as = pMenu->Caption;
	OnMenuProc(pMenu, as.c_str());
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::AddExtensionMenu(LPCSTR pTitle, LPCSTR pCaption, LPCSTR pHint)
{
	TMenuItem *pMainMenu = GetMainMenu(pTitle, TRUE);
	if( !*pCaption ) return;

	AnsiString as;
	TMenuItem *pMenu = NULL;
	if( strcmp(pCaption, "-") ){
		if( !*pHint ) return;
		pMenu = FindMenu(pMainMenu, pCaption);
    }
    else if( pMainMenu->Count ){
		as = pMainMenu->Items[pMainMenu->Count - 1]->Caption;
        if( !strcmp(as.c_str(), "-") ) return;
    }
	BOOL fAdd = FALSE;
	if( !pMenu ){
		pMenu = new TMenuItem(pMainMenu);
		pMenu->Caption = pCaption;
        fAdd = TRUE;
    }
    else if( pMenu->Count ){
		if( !sys.m_MacroError ){
			sys.m_MacroError = TRUE;
			as = pMenu->Caption;
			InfoMB("[%s] is not overrode...", as.c_str());
        }
		return;
    }
	pMenu->Hint = pHint;
	pMenu->OnClick = OnXClick;
	if( fAdd ){
    	pMainMenu->Add(pMenu);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::InsExtensionMenu(LPCSTR pTitle, LPCSTR pPos, LPCSTR pCaption, LPCSTR pHint)
{
	TMenuItem *pMainMenu = GetMainMenu(pTitle, TRUE);
	TMenuItem *pPosMenu = FindMenu(pMainMenu, pPos);
	if( !pPosMenu ) return;

	BOOL fIns = FALSE;
	TMenuItem *pMenu = NULL;
	if( strcmp(pCaption, "-") ){
	    pMenu = FindMenu(pMainMenu, pCaption);
    }
	if( !pMenu ){
		pMenu = new TMenuItem(pMainMenu);
		pMenu->Caption = pCaption;
        fIns = TRUE;
    }
    else if( pMenu->Count ){
		if( !sys.m_MacroError ){
			sys.m_MacroError = TRUE;
			AnsiString as = pMenu->Caption;
			InfoMB("[%s] is not overrode...", as.c_str());
        }
		return;
    }
	pMenu->Hint = pHint;
	pMenu->OnClick = OnXClick;
	if( fIns ){
    	pMainMenu->Insert(pMainMenu->IndexOf(pPosMenu), pMenu);
    }
}
//---------------------------------------------------------------------------
static void __fastcall DeleteShortCut(TMenuItem *pMain, TShortCut sKey)
{
	for( int i = 0; i < pMain->Count; i++ ){
		TMenuItem *pMenu = pMain->Items[i];
		if( pMenu->ShortCut == sKey ){
			pMenu->ShortCut = 0;
        }
        if( pMenu->Count ){
			DeleteShortCut(pMenu, sKey);
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::ShortCutExtensionMenu(TMenuItem *pMenu, LPCSTR pKey)
{
    if( pMenu ){
		AnsiString as;
		GetMacroStr(as, pKey);
		TShortCut sKey = 0;
		if( strcmpi(as.c_str(), "NONE") ){
			sKey = TextToShortCut(as);
        }
        if( sKey ){
        	DeleteShortCut(MainMenu->Items, sKey);
            DeleteShortCut(PupTX->Items, sKey);
        }
		pMenu->ShortCut = sKey;
    }
}
//---------------------------------------------------------------------------
BOOL __fastcall TMainVARI::IsXMenu(TMenuItem *pItem)
{
	if( !pItem ) return FALSE;

	TMenuItem *pMainMenu = MainMenu->Items;
	for( int i = 0; i < pMainMenu->Count; i++ ){
		if( pMainMenu->Items[i]->IndexOf(pItem) >= 0 ) return TRUE;
    }
    return FALSE;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::OnXClick(TObject *Sender)
{
	TMenuItem *pMenu = (TMenuItem *)Sender;
	AnsiString ps = pMenu->Hint.c_str();
	LPSTR tt, pp;
	pp = StrDlm(tt, ps.c_str());
	if( !OnMenuProc(pMenu, tt, pp, TRUE) ){
	    if( MainMenu->Items->IndexOf(pMenu) < 0 ){
			if( !sys.m_MacroError ){
				sys.m_MacroError = TRUE;
				InfoMB("Procedure [%s] is not found...", tt);
            }
    	}
    }
}
//---------------------------------------------------------------------------
BOOL __fastcall TMainVARI::OnMenuProc(TMenuItem *pMenu, LPCSTR pProc, LPCSTR pPara, BOOL fStop)
{
	int r = m_MacroVal.FindName(pProc, _VAL_PROC);
	if( r >= 0 ){
		if( fStop ){
			sys.m_MacroError = FALSE;
			DeleteMacroTimer();
			m_pCurrentMacroMenu = pMenu;
	        m_CurrentMacro = -1;
        }
		m_MacroInput = pMenu->Caption;
		AnsiString as;
		ConvDummy(as, m_MacroVal.GetString(r), m_MacroVal.GetPara(r), pPara);
        DoMacro(as.c_str(), NULL);
        return TRUE;
	}
    else {
		return FALSE;
    }
}
//---------------------------------------------------------------------------
// ÉÅÉjÉÖÅ[ÇÃçXêVÉnÉìÉhÉâ
void __fastcall TMainVARI::OnMenuProc(TMenuItem *pMenu, LPCSTR pCaption)
{
	if( !m_MacroVal.IsHandleProc() ) return;

	m_ParentMenu = pMenu;
	char bf[256];
    sprintf(bf, "On$%sClick", pCaption);
    OnMenuProc(pMenu, bf, "", FALSE);
	m_ParentMenu = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::DoParentClick(TMenuItem *pMenu)
{
	TMenuItem *pMainMenu = pMenu->Parent;
    if( pMainMenu ){
		DoParentClick(pMainMenu);
		if( (pMainMenu != m_ParentMenu) && pMainMenu->OnClick ) pMainMenu->Click();
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KHClick(TObject *Sender)
{
	OnMenuProc(KH, "&Help");
}
//---------------------------------------------------------------------------
TMenuItem* __fastcall TMainVARI::GetMenuArg(AnsiString &arg, LPCSTR pVal, BOOL fArg)
{
	BOOL f = !fArg;
	AnsiString as;
	LPSTR pBF = StrDupe(GetMacroStr(as, pVal));
    LPSTR tt, p;
	p = StrDlm(tt, pBF);
    TMenuItem *pMenu = FindMenu(MainMenu->Items, GetMacroStr(as, tt));
	while( *p && pMenu ){
		p = StrDlm(tt, SkipSpace(p));
        tt = SkipSpace(tt);
		if( *p || (!fArg) ){
			if( *tt ){
				pMenu = FindMenu(pMenu, GetMacroStr(as, tt));
			}
		}
		else {
			arg = tt;
            f = TRUE;
		}
	}
	delete pBF;
    return f ? pMenu : NULL;
}
//---------------------------------------------------------------------------
TSpeedButton* __fastcall TMainVARI::FindButton(TComponent *pMainControl, LPCSTR pCaption, TSpeedButton *pButton, BOOL fErrMsg)
{
	const LPCSTR _bt[]={"BPF1","BPF2","BPF3","BPF4"};
    int i = FindStringTable(_bt, pCaption, AN(_bt));
    if( i >= 0 ){
		TSpeedButton *_tt[]={SBBPFW, SBBPFM, SBBPFN, SBBPFS};
		return _tt[i];
    }

	TComponent *pComponent;
	TControl *pControl;
	for( i = 0; i < pMainControl->ComponentCount; i++ ){
		pComponent = pMainControl->Components[i];
        pControl = (TControl *)pComponent;
		if( pControl && (pButton != pControl) ){
			if(  pControl->ClassNameIs("TSpeedButton") ){
				TSpeedButton *pButton = (TSpeedButton *)pControl;
				AnsiString as = pButton->Caption;
				if( !strcmp(as.c_str(), pCaption) ){
					return pButton;
                }
			}
            else if( pControl->ComponentCount ){
				pControl = FindButton(pControl, pCaption, pButton, FALSE);
                if( pControl ) return (TSpeedButton *)pControl;
            }
		}
	}
	if( fErrMsg ){
		if( !sys.m_MacroError ){
			sys.m_MacroError = TRUE;
    	    InfoMB("Button [%s] is not found...", pCaption);
        }
    }
    return NULL;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::ClickButton(TSpeedButton *pButton)
{
	if( !pButton ) return;

	if( pButton->Visible && pButton->Enabled ){
	    pButton->Down = !pButton->Down;
    	pButton->Click();
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SetRTTYFFT(int f)
{
	m_RxSet[0].m_RTTYFFT = f;
	m_RxSet[0].m_pDem->m_fRTTYFFT = f;
#if 0
	int i;
	for( i = 1; i < RXMAX; i++ ){
		if( m_RxSet[i].m_pDem ){
			m_RxSet[i].m_pDem->m_fRTTYFFT = f;
        }
    }
#endif
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::CreateSubMenu(void)
{
	char bf[256];
	TMenuItem *pm;
    for( int i = 1; i < RXMAX; i++ ){
    	sprintf(bf, sys.m_MsgEng ? "CH &%d" : "É`ÉÉÉìÉlÉã &%d", i);
		if( (i - 1) >= KVS->Count ){
			pm = new TMenuItem(this);
	        KVS->Add(pm);
        }
		pm = KVS->Items[i-1];
        pm->OnClick = KVSClick;
        pm->Caption = bf;
        pm->Checked = m_RxSet[i].IsActive();

		if( (i - 1) >= KSS->Count ){
			pm = new TMenuItem(this);
	        KSS->Add(pm);
        }
		pm = KSS->Items[i-1];
        pm->OnClick = KSSClick;
        pm->Caption = bf;
        pm->Checked = m_RxSet[i].IsActive();
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KVSClick(TObject *Sender)
{
	int RN = KVS->IndexOf((TMenuItem *)Sender);
	if( RN >= 0 ){
		RN++;
		ShowSubChannel(RN, 2);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KSSClick(TObject *Sender)
{
	int RN = KSS->IndexOf((TMenuItem *)Sender);
	if( RN >= 0 ){
		RN++;
		ShowSubChannel(RN, 1);

		CRxSet *pRxSet = &m_RxSet[RN];
		double fq = GetSignalFreq(m_RightFreq, SBFFT3K->Down ? 50 : 32, pRxSet);
		pRxSet->m_pDem->m_Decode.Reset();
		pRxSet->SetCarrierFreq(fq);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::AdjustRadioMenu(void)
{
    int max = KR->IndexOf(NR);
	TMenuItem *pm;
	int i;
	for( i = 0; i < m_nRadioMenu; i++ ){
		if( i >= max ){
			TMenuItem *pm = new TMenuItem(this);
			KR->Insert(i, pm);
            max++;
        }
		pm = KR->Items[i];
		if( pm->Caption != m_RadioMenu[i].strTTL ){
			pm->Caption = m_RadioMenu[i].strTTL;
        }
        pm->OnClick = KRadioCmdClick;
        pm->Enabled = m_pRadio != NULL;
    }
	int N = i;
    for( ; i < max; i++ ){
		KR->Delete(N);
    }

    max = KRM->IndexOf(NRE);
	for( i = 0; i < m_nRadioMenu; i++ ){
		if( i >= max ){
			TMenuItem *pm = new TMenuItem (this);
			KRM->Insert(i, pm);
            max++;
        }
		pm = KRM->Items[i];
		if( pm->Caption != m_RadioMenu[i].strTTL ){
			pm->Caption = m_RadioMenu[i].strTTL;
        }
        pm->OnClick = KRadioEditClick;
    }
	N = i;
    for( ; i < max; i++ ){
		KRM->Delete(N);
    }
	BOOL f = m_nRadioMenu != 0;
	NR->Visible = f;
	NRE->Visible = f;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KRadioCmdClick(TObject *Sender)
{
	if( !m_pRadio ) return;

    int n = KR->IndexOf((TMenuItem *)Sender);
    if( (n >= 0) && (n < RADIOMENUMAX) ){
		LPCSTR p = m_RadioMenu[n].strCMD.c_str();
        if( CheckEXT(p, "RCM") && IsFile(p) ){
			LoadRadioDef(p);
        }
        else {
			m_pRadio->SendCommand(p);
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KRadioEditClick(TObject *Sender)
{
    int n = KRM->IndexOf((TMenuItem *)Sender);
    if( (n >= 0) && (n < RADIOMENUMAX) ){
		AnsiString strTTL = m_RadioMenu[n].strTTL;
        AnsiString strCMD = m_RadioMenu[n].strCMD;

		TRMenuDialog *pBox = new TRMenuDialog(this);
		int r = pBox->Execute(strTTL, strCMD);
        BOOL fNull = strTTL.IsEmpty() && strCMD.IsEmpty();
		if( (r == IDOK) && !fNull ){
			if( !strCMD.IsEmpty() ){
				if( strTTL.IsEmpty() ){
					char bf[64];
			        sprintf(bf, "Radio command #d", n + 1);
					strTTL = bf;
	            }
	            m_RadioMenu[n].strTTL = strTTL;
	            m_RadioMenu[n].strCMD = strCMD;
	            AdjustRadioMenu();
	        }
	    }
        else if( (r == 1024) || fNull ){
			for( int i = n; i < (m_nRadioMenu - 1); i++ ){
	            m_RadioMenu[i].strTTL = m_RadioMenu[i+1].strTTL;
	            m_RadioMenu[i].strCMD = m_RadioMenu[i+1].strCMD;
            }
            m_nRadioMenu--;
            AdjustRadioMenu();
        }
        else if( r == 1025 ){
			if( m_nRadioMenu < RADIOMENUMAX ){
				for( int i = m_nRadioMenu; i > n; i-- ){
		            m_RadioMenu[i].strTTL = m_RadioMenu[i-1].strTTL;
		            m_RadioMenu[i].strCMD = m_RadioMenu[i-1].strCMD;
	            }
	            m_RadioMenu[n].strTTL = strTTL;
	            m_RadioMenu[n].strCMD = strCMD;
	            m_nRadioMenu++;
	            AdjustRadioMenu();
            }
        }
        delete pBox;
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KRClick(TObject *Sender)
{
	AdjustRadioMenu();
    KRADD->Enabled = m_nRadioMenu < RADIOMENUMAX;
    OnMenuProc(KR, "Radio&Command");
}
//---------------------------------------------------------------------------

void __fastcall TMainVARI::KRADDClick(TObject *Sender)
{
	if( m_nRadioMenu >= RADIOMENUMAX ) return;

	AnsiString strTTL, strCMD;

	TRMenuDialog *pBox = new TRMenuDialog(this);
	if( pBox->Execute(strTTL, strCMD, TRUE) ){
		if( !strCMD.IsEmpty() ){
			if( strTTL.IsEmpty() ){
				char bf[64];
		        sprintf(bf, "Radio command #d", m_nRadioMenu + 1);
				strTTL = bf;
            }
            m_RadioMenu[m_nRadioMenu].strTTL = strTTL;
            m_RadioMenu[m_nRadioMenu].strCMD = strCMD;
            m_nRadioMenu++;
            AdjustRadioMenu();
        }
    }
    delete pBox;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::LoadRadioDef(LPCSTR pName)
{
	try {
		CWaitCursor w;
		TMemIniFile *pIniFile = new TMemIniFile(pName);
		RADIO.CmdInit = pIniFile->ReadString("RADIO", "CmdInit", RADIO.CmdInit);
		RADIO.CmdRx = pIniFile->ReadString("RADIO", "CmdRx", RADIO.CmdRx);
		RADIO.CmdTx = pIniFile->ReadString("RADIO", "CmdTx", RADIO.CmdTx);
		char bf[32];
		sprintf( bf, "%02X", RADIO.Cmdxx);
		AnsiString as = pIniFile->ReadString("RADIO", "Cmdxx", bf);
		int d;
	    sscanf(as.c_str(), "%X", &d);
	    RADIO.Cmdxx = d;
		RADIO.PollType = pIniFile->ReadInteger("RADIO", "PollType", 0);
		RADIO.PollInterval = pIniFile->ReadInteger("RADIO", "PollInterval", 0);
		delete pIniFile;
		OpenRadio();
    }
    catch(...){
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KRLClick(TObject *Sender)
{
	TOpenDialog *pBox = new TOpenDialog(this);
	pBox->Options >> ofCreatePrompt;
	pBox->Options << ofFileMustExist;
	if( sys.m_MsgEng ){
		pBox->Title = "Open command file";
		pBox->Filter = "Command files(*.rcm)|*.rcm|";
	}
	else {
		pBox->Title = "ÉRÉ}ÉìÉhíËã`ÉtÉ@ÉCÉãÇäJÇ≠";
		pBox->Filter = "ÉRÉ}ÉìÉhíËã`ÉtÉ@ÉCÉã(*.rcm)|*.rcm|";
	}
	pBox->FileName = "";
	pBox->DefaultExt = "rcm";
	pBox->InitialDir = sys.m_BgnDir;
	if( pBox->Execute() == TRUE ){
		LoadRadioDef(AnsiString(pBox->FileName).c_str());	//JA7UDE 0428
	}
    delete pBox;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KROClick(TObject *Sender)
{
	TRADIOSetDlg *pBox = new TRADIOSetDlg(this);

	if( (sys.m_PTTCOM != "NONE") && !strcmp(sys.m_PTTCOM.c_str(), RADIO.StrPort) ){
		strcpy(RADIO.StrPort, "NONE");
		RADIO.change = 1;
	}
	if( pBox->Execute() == TRUE ){
		if( (sys.m_PTTCOM != "NONE") && !strcmp(sys.m_PTTCOM.c_str(), RADIO.StrPort) ){
			sys.m_PTTCOM = "NONE";
            COMM.change = 1;
		}
	}
	delete pBox;
	if( COMM.change ) OpenCom();
    if( RADIO.change ) OpenRadio();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::PupCallsPopup(TObject *Sender)
{
	OnMenuProc(PupCalls->Items, "PopPAGE");
}
//---------------------------------------------------------------------------

