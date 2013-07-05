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



//---------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "Option.h"
#include "ComLib.h"
#include "Main.h"
#include "dsp.h"
#include "CodeVw.h"
#include "RadioSet.h"
#include "cradio.h"
#include "MacEdit.h"
//---------------------------------------------------------------------
#pragma resource "*.dfm"
//TOptDlgBox *OptDlgBox;
//---------------------------------------------------------------------
__fastcall TOptDlgBox::TOptDlgBox(TComponent* AOwner)
	: TForm(AOwner)
{
	m_fDisEvent = TRUE;
	FormStyle = ((TForm *)AOwner)->FormStyle;
	Font->Name = ((TForm *)AOwner)->Font->Name;
	Font->Charset = ((TForm *)AOwner)->Font->Charset;
	if( sys.m_MsgEng ){
		Caption = "Setup MMVARI";
		CancelBtn->Caption = "Cancel";

		TabRX->Caption = "RX";
        TabTX->Caption = "TX";
        TabMisc->Caption = "Misc";

		// RX
        GB10->Caption = "HPF (DC ingredients cutter)";
		GB10->Hint = "It is unnecessary with most sound cards";
        CBHPF->Caption = "Use HPF";
        L10->Caption = "FREQ width +/-";
        L13->Caption = "Sense level(S/N)";
        GB6->Hint = "Narrow (less than 50Hz) is FB in HF";
		GB15->Hint = "You do not need to change the value";
		L20->Caption = "Speed";
        L21->Caption = "Slow ---- Fast";
        L22->Caption = "Sense level(S/N)";
		L23->Caption = "Max range +/-";
		CBMC->Caption = "Handling center frequency";
		CBMM->Caption = "Use metric level squelch";
		CBMM->Hint = "It may be FB, when you use narrow filter in the Rig";
		GB17->Caption = "Misc";
        CBSWL->Caption = "Show Time stamp for SWL";
		CBPB->Caption = "Enabled Sound Playback";
		LPB->Caption = "Playback speed\r\nx1 -------------- x20";
		RGRTTY->Caption = "RTTY Demodulator";

		// TX
        CBComLock->Caption = "Exclusive lock";
		CBFSK->Caption = "Output";
		CBFSK->Hint = "Use multi-media timer function";
        CBFSKINV->Caption = "Invert logic";
		CBFSKINV->Hint = "Invert mark and space";
    
        SBRadio->Caption = "Radio command";
		GB8->Caption = "Digital output level";
		RGLoopBack->Caption = "LoopBack";
			RGLoopBack->Items->Strings[0] = "Internal";
			RGLoopBack->Items->Strings[1] = "External (full-duplex)";
		GBKey->Caption = "Key assign";
        GB5->Caption = "Macro";
		GB16->Caption = "Misc";
        CBCA->Caption = "Convert MBCS alphabet to ASCII";
		SBAS->Caption = "Edit AS(CW)";
		SBAS->Hint = "Edit AS(CW) macro - Right click on the Spectrum scope";
        GB33->Caption = "Event macro";
        L33->Caption = "Name";
		SBEvent->Caption = "Edit";
        SBEventC->Caption = "Clear";
        CBEvent->Hint = "Choose event to edit";
        SBEvent->Hint = "Edit this event";
        SBEventC->Hint = "Clear this event";
		CBST->Caption = "Output single tone when the TX ends (PSK)";

		// Misc
        GB2->Caption = "Soundcard";
		RGSoundIn->Caption = "Input channel";
			RGSoundIn->Items->Strings[0] = "Mono";
			RGSoundIn->Items->Strings[1] = "Left";
			RGSoundIn->Items->Strings[2] = "Right";
		RGPriority->Caption = "Priority";
			RGPriority->Items->Strings[0] = "Normal";
			RGPriority->Items->Strings[1] = "Higher";
			RGPriority->Items->Strings[2] = "Highest";
        GB11->Caption = "RX window";
		SBRxFont->Caption = "Font";
        SBRxChar->Caption = "Code list";
        SBRxChar->Hint = "Character code list";
		CBRxUTC->Caption = "Time stamp = UTC";
        CBMW->Caption = "Scroll with mouse wheel";
        CBShowCtrl->Caption = "Show CTRL code";
		GB12->Caption = "TX window";
        SBTxFont->Caption = "Font";
		GBWindow->Caption = "Size and position";
			RBW1->Caption = "Windows default";
			RBW2->Caption = "Save size && pos.";
            CBRestoreSub->Caption = "Restore sub channels";
		GB13->Caption = "Spectrum scope";
        GB14->Caption = "Waterfall scope";
			L30->Caption = "Level";
            L31->Caption = "Sigs";

        SBJA->Caption = "Japanese";
        SBEng->Caption = "English";
        GB9->Caption = "Message-Language";

		OKBtn->Hint = "Save all update && close this window";
		CancelBtn->Hint = "Unsave all update && close this window";
		EATCL->Hint = "Default = 15dB";
        TBATC->Hint = "Default = Slow";
        CBATCM->Hint = "Default = 25000ppm";
        CBComPTT->Hint = "";
        CBComLock->Hint = "Uncheck this, when the port shares with other software";
		RGLoopBack->Hint = "Choose 'Internal' for usually operation";
        ECallSign->Hint = "Your callsign";
        GB8->Hint = "The signal may be a warp, when it is high (It depend on soundcard)";
        CBKTX->Hint = "Switch TX/RX";
        CBKTXOFF->Hint = "Abort TX";
        CBCA->Hint = "Some transmission efficiency will be improved when does this conv.";
        CBFifoRX->Hint = "Enlarge the value if you often break the sound in RX";
        CBFifoTX->Hint = "Enlarge the value if you often break the sound in TX";
        CBClock->Hint = "RX sampling FREQ (Put the same value of MMSSTV)";
        CBTxOffset->Hint = "TX (offset) sampling FREQ (Put the same value of MMSSTV)";
        CBSoundID->Hint = "Input sound device (device name or number)";
        CBSoundIDTX->Hint = "Output sound device (device name or number)";
		PCRx1->Hint = "Back color";
        PCRx2->Hint = "Character color (RX)";
        PCRx3->Hint = "Character color (TX)";
        PCRx4->Hint = "Status color";
        PCRx5->Hint = "Back color with scrolling";
        PCTx1->Hint = "Back color";
        PCTx2->Hint = "Character color";
        PCTx3->Hint = "Character color (TX)";
        PCTx4->Hint = "CWID/Control code color";
		PS1->Hint = "Back color";
        PS2->Hint = "Signal color";
        PS3->Hint = "Text color";
        PS4->Hint = "Scale";
        PS5->Hint = "RX Marker";
        PS6->Hint = "TX Marker";
        PW1->Hint = "Back color";
        PW2->Hint = "Level1";
        PW3->Hint = "Text color";
        PW4->Hint = "Marker frame";
        PW5->Hint = "RX Marker";
        PW6->Hint = "TX Marker";
        PW7->Hint = "Level2";
        PW8->Hint = "Level3";
        PW9->Hint = "Level4";
        PW10->Hint = "Level5";
        PW11->Hint = "Level6";
        PW12->Hint = "Level7";
        PL->Hint = "Set level (Drag && drop)";
        SBWLCD->Hint = "Return to the LCD's default (Level and Colors)";
        SBWCRT->Hint = "Return to the CRT's default (Level and Colors)";
        GBWindow->Hint = "Window size and position (when program will be invoked).";
        SBFont->Hint = "Choose your favorite font for the program except TX/RX window";
	}
    else {
		LPB->Caption = "ﾌﾟﾚｰﾊﾞｯｸ速度\r\nx1 <---------> x20";
    }
    SBJA->Hint = "All menu messages are displayed in Japanese";
    SBEng->Hint = "All menu messages are displayed in English";

	double mb = 120.0 * SAMPBASE / (1024*1024);
    char bf[256];
	sprintf(bf, sys.m_MsgEng ? "To use memory of %.1lfMB for the function":"この機能は%.1lfMBのメモリを使います.", mb);
    CBPB->Hint = bf;

    UdATCL->Hint = EATCL->Hint;
    m_fComChange = FALSE;
	m_fLangChange = FALSE;

	UdClock->Hint = CBClock->Hint;
    UdTxOffset->Hint = CBTxOffset->Hint;
    OnWave();
    const DEFKEYTBL *pKey = KEYTBL;
    for( ; pKey->Key; pKey++ ){
		CBKTX->Items->Add(pKey->pName);
    }
	CBEvent->DropDownCount = macOnEnd;
	for( int i = 0; i < macOnEnd; i++ ){
		CBEvent->Items->Add(g_tMacEvent[i]);
    }
    CBEvent->ItemIndex = sys.m_EventIndex;
    OnWave();
    CBKTXOFF->Items->Assign(CBKTX->Items);
    FormCenter(this);
	m_MouseDown = 0;
	m_MouseWLN = -1;
	CWave *pw = &MainVARI->m_Wave;
    pw->GetDeviceList();
    for( int i = 0; i < pw->m_InDevs; i++ ){
		if( !pw->m_tInDevName[i].IsEmpty() ){
			CBSoundID->Items->Add(pw->m_tInDevName[i]);
        }
    }
    for( int i = 0; i < pw->m_OutDevs; i++ ){
		if( !pw->m_tOutDevName[i].IsEmpty() ){
			CBSoundIDTX->Items->Add(pw->m_tOutDevName[i]);
        }
    }
	CBSoundID->DropDownCount = CBSoundID->Items->Count;
	CBSoundIDTX->DropDownCount = CBSoundIDTX->Items->Count;
}
//---------------------------------------------------------------------
void __fastcall TOptDlgBox::UpdateButton(void)
{
	if( m_FontCharset == SHIFTJIS_CHARSET ){
		SBJA->Down = TRUE;
    }
    else {
		SBEng->Down = TRUE;
    }
}
//---------------------------------------------------------------------
void __fastcall TOptDlgBox::UpdateHint(void)
{
	AnsiString as = PCRx2->Font->Name;
	TFontStyles fs = PCRx2->Font->Style;
    AddStyle(as, PCRx2->Font->Charset, FontStyle2Code(fs));
    as += sys.m_MsgEng ? " (VariCode type is decided by this font)":" (このフォントでVariCodeタイプが決まります)";
    SBRxFont->Hint = as;
    as = PCTx2->Font->Name;
	fs = PCTx2->Font->Style;
    AddStyle(as, PCTx2->Font->Charset, FontStyle2Code(fs));
    SBTxFont->Hint = as;
}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::DisplayHint(TObject *Sender)
{
	LH->Caption = GetLongHint(Application->Hint);
}
//---------------------------------------------------------------------
void __fastcall TOptDlgBox::UpdateUI(void)
{
	int dd;
	BOOL f = (sscanf(AnsiString(CBSoundID->Text).c_str(), "%d", &dd) == 1 );	//JA7UDE 0428
	if( !f ){
		if( !strcmpi(AnsiString(CBSoundID->Text).c_str(), "Default") ){	//JA7UDE 0428
			f = TRUE;
        }
        else {
	   		CWave *pw = &MainVARI->m_Wave;
			for(int i = 0; i < pw->m_InDevs; i++ ){
				if( !strcmp(AnsiString(CBSoundID->Text).c_str(), AnsiString(pw->m_tInDevName[i]).c_str()) ){	//JA7UDE 0428
					f = TRUE;
	                break;
	            }
            }
        }
    }
	LO->Enabled = f;
    CBSoundIDTX->Enabled = f;

	GB3->Enabled = f;
    SetGroupEnabled(GB3);
    RGSoundIn->Enabled = f;

	f = CBPB->Checked;
    TBPB->Visible = f;
    LPB->Visible = f;
    SBEventC->Enabled = !m_MacEvent[CBEvent->ItemIndex].IsEmpty();

	AnsiString as = CBComPTT->Text;
    f = strncmpi(as.c_str(), "COM", 3);
    if( f ){		// COMxx
    	f = strcmp(as.c_str(), "NONE");
    }
    else {
		LPCSTR p = as.c_str();
		f = isdigit(p[3]) ? 0 : 1;
    }
	CBFSKINV->Enabled = f ? 0 : 1;
}
//---------------------------------------------------------------------
int __fastcall TOptDlgBox::Execute(DWORD dwPage)
{
	m_fDisEvent = TRUE;
    int i;
	char bf[256];
	// RX
	CBAFCW->Text = MainVARI->m_AFCWidth;
    UdAFCL->Position = short(MainVARI->m_AFCLevel);
	CBHPF->Checked = MainVARI->m_fHPF;
	TBATC->Position = short(MainVARI->m_ATCSpeed);
    UdATCL->Position = short(MainVARI->m_ATCLevel);
	CBATCM->Text = MainVARI->m_ATCLimit;
	CBSWL->Checked = sys.m_fAutoTS;
	CBPB->Checked = sys.m_fPlayBack;
	TBPB->Position = sys.m_PlayBackSpeed;
	RGRTTY->ItemIndex = MainVARI->m_RxSet[0].m_RTTYFFT;
    // TX
	CBComPTT->Text = sys.m_PTTCOM;
	CBFSK->Checked = sys.m_bFSKOUT;
    CBFSKINV->Checked = sys.m_bINVFSK;

    CBComLock->Checked =  sys.m_PTTLock;
	RGLoopBack->ItemIndex = sys.m_LoopBack;

    ECallSign->Text = sys.m_CallSign;
    m_AS = sys.m_AS.c_str();
	for( i = 0; i < macOnEnd; i++ ) m_MacEvent[i] = sys.m_MacEvent[i];
	TBModGain->Position = MainVARI->m_ModGain;

    CBKTX->ItemIndex = CBKTX->Items->IndexOf(GetKeyName(sys.m_DefKey[kkTX]));
    CBKTXOFF->ItemIndex = CBKTXOFF->Items->IndexOf(GetKeyName(sys.m_DefKey[kkTXOFF]));
	CBCA->Checked = MainVARI->m_fConvAlpha;
	CBMC->Checked = sys.m_MFSK_Center;
    CBMM->Checked = sys.m_MFSK_SQ_Metric;
	CBST->Checked = sys.m_fSendSingleTone;

    //Misc
	CBFifoRX->Text = MainVARI->m_Wave.m_InFifoSize;
	CBFifoTX->Text = MainVARI->m_Wave.m_OutFifoSize;
    RGSoundIn->ItemIndex = MainVARI->m_Wave.m_SoundStereo;

	int d;
	if( MainVARI->m_Wave.m_SoundID == -2 ){
		CBSoundID->Text = sys.m_SoundMMW;
    }
    else {
		if( (sscanf(sys.m_SoundIDRX.c_str(), "%d", &d ) == 1) && (d == -1) ){
			CBSoundID->Text = "Default";
        }
        else {
			CBSoundID->Text = sys.m_SoundIDRX;
        }
    }
	if( (sscanf(sys.m_SoundIDTX.c_str(), "%d", &d ) == 1) && (d == -1) ){
		CBSoundIDTX->Text = "Default";
	}
    else {
        CBSoundIDTX->Text = sys.m_SoundIDTX;
    }
    sprintf(bf, "%.2lf", SAMPFREQ);
    CBClock->Text = bf;
    sprintf(bf, "%.2lf", SAMPTXOFFSET);
    CBTxOffset->Text = bf;
	UdTxOffset->Position = 0;
	RGPriority->ItemIndex = MainVARI->m_Priority;

	PCRx2->Font = MainVARI->PCRX->Font;
	long cz;
	cz = MainVARI->m_Dump.m_Color[0].d;
	PCRx1->Color = cz;
	cz = MainVARI->m_Dump.m_Color[1].d;
	PCRx2->Color = cz;
	cz = MainVARI->m_Dump.m_Color[2].d;
	PCRx3->Color = cz;
	cz = MainVARI->m_Dump.m_Color[3].d;
	PCRx4->Color = cz;
	cz = MainVARI->m_Dump.m_Color[4].d;
	PCRx5->Color = cz;

	//PCRx1->Color = MainVARI->m_Dump.m_Color[0].c;
	//PCRx2->Color = MainVARI->m_Dump.m_Color[1].c;
	//PCRx3->Color = MainVARI->m_Dump.m_Color[2].c;
	//PCRx4->Color = MainVARI->m_Dump.m_Color[3].c;
	//PCRx5->Color = MainVARI->m_Dump.m_Color[4].c;

	CBRxUTC->Checked = MainVARI->m_StatusUTC;
	CBMW->Checked = sys.m_EnableMouseWheel;
	CBShowCtrl->Checked = sys.m_fShowCtrlCode;

	PCTx2->Font = MainVARI->PCTX->Font;
	cz = MainVARI->m_Edit[0].m_Color[0].c;
	PCTx1->Color = cz;
	cz = MainVARI->m_Edit[0].m_Color[1].c;
	PCTx2->Color = cz;
	cz = MainVARI->m_Edit[0].m_Color[2].c;
	PCTx3->Color = cz;
	cz = MainVARI->m_Edit[0].m_Color[3].c;
	PCTx4->Color = cz;

	//PCTx1->Color = MainVARI->m_Edit[0].m_Color[0].c;
	//PCTx2->Color = MainVARI->m_Edit[0].m_Color[1].c;
	//PCTx3->Color = MainVARI->m_Edit[0].m_Color[2].c;
	//PCTx4->Color = MainVARI->m_Edit[0].m_Color[3].c;

	PS1->Color = MainVARI->m_tFFTColset[0].c;
	PS2->Color = MainVARI->m_tFFTColset[1].c;
	PS3->Color = MainVARI->m_tFFTColset[2].c;
	PS4->Color = MainVARI->m_tFFTColset[3].c;
	PS5->Color = MainVARI->m_tFFTColset[4].c;
	PS6->Color = MainVARI->m_tFFTColset[5].c;

	PW1->Color = MainVARI->m_tWaterColset[0].c;
	PW2->Color = MainVARI->m_tWaterColset[1].c;
	PW3->Color = MainVARI->m_tWaterColset[2].c;
	PW4->Color = MainVARI->m_tWaterColset[3].c;
	PW5->Color = MainVARI->m_tWaterColset[4].c;
	PW6->Color = MainVARI->m_tWaterColset[5].c;
	PW7->Color = MainVARI->m_tWaterColset[6].c;
	PW8->Color = MainVARI->m_tWaterColset[7].c;
	PW9->Color = MainVARI->m_tWaterColset[8].c;
	PW10->Color = MainVARI->m_tWaterColset[9].c;
	PW11->Color = MainVARI->m_tWaterColset[10].c;
	PW12->Color = MainVARI->m_tWaterColset[11].c;
	PL->Color = PW1->Color;
	for( i = 0; i < 6; i++ ){
		m_tWaterLevel[i] = MainVARI->m_tWaterLevel[i];
    }

	RBW1->Checked = !sys.m_fFixWindow;
	RBW2->Checked = sys.m_fFixWindow;
	CBRestoreSub->Checked = sys.m_fRestoreSubChannel;

	m_FontName = sys.m_FontName;
    m_FontCharset = sys.m_FontCharset;

	UpdateButton();
	UpdateHint();
    UpdateUI();
	OnWave();
	m_fDisEvent = FALSE;

	if( (dwPage >= 0) && (dwPage <= 2) ){
		sys.m_OptionPage = dwPage;
    }
	SetActiveIndex(Page, sys.m_OptionPage);
	m_fnHintProc = Application->OnHint;
    Application->OnHint = DisplayHint;
    int r = ShowModal();
	Application->OnHint = m_fnHintProc;
	sys.m_OptionPage = GetActiveIndex(Page);
    sys.m_EventIndex = CBEvent->ItemIndex;
    if( r == IDOK ){
		int di;
		double dd;
		// RX
		sscanf(AnsiString(CBAFCW->Text).c_str(), "%u", &di);	//JA7UDE 0428
        if( (di >= 0) && (di <= 2000) ){
			MainVARI->m_AFCWidth = di;
        }
		MainVARI->m_AFCLevel = UdAFCL->Position;
		MainVARI->m_fHPF = CBHPF->Checked;
		MainVARI->SetATCSpeed(TBATC->Position);
		MainVARI->m_ATCLevel = UdATCL->Position;
		sscanf(AnsiString(CBATCM->Text).c_str(), "%u", &di);	//JA7UDE 0428
        if( (di >= 1000) && (di <= 100000) ){
			MainVARI->SetATCLimit(di);
        }
		sys.m_fAutoTS = CBSWL->Checked;
		sys.m_fPlayBack = CBPB->Checked;
		sys.m_PlayBackSpeed = TBPB->Position;

		// TX
		sys.m_PTTCOM = CBComPTT->Text;
		sys.m_bFSKOUT = CBFSK->Checked;
		sys.m_bINVFSK = CBFSKINV->Checked;
		if( MainVARI->m_pCom ) MainVARI->m_pCom->SetFSK(sys.m_bFSKOUT, sys.m_bINVFSK);

        sys.m_PTTLock = CBComLock->Checked;
		sys.m_LoopBack = RGLoopBack->ItemIndex;
		sys.m_CallSign = ECallSign->Text;
	    sys.m_AS = m_AS.c_str();
		for( i = 0; i < macOnEnd; i++ ) sys.m_MacEvent[i] = m_MacEvent[i];
		MainVARI->m_ModGain = TBModGain->Position;
		MainVARI->UpdateModGain();

		sys.m_DefKey[kkTX] = GetKeyCode(AnsiString(CBKTX->Text).c_str());	//JA7UDE 0428
		sys.m_DefKey[kkTXOFF] = GetKeyCode(AnsiString(CBKTXOFF->Text).c_str());	//JA7UDE 0428
		MainVARI->SetConvAlpha(CBCA->Checked);
		sys.m_fSendSingleTone = CBST->Checked;

		PCTx2->Font->Charset = PCRx2->Font->Charset;
		LOGFONT l1;
        LOGFONT l2;
        GetLogFont(&l1, PCRx2->Font);
        GetLogFont(&l2, MainVARI->PCRX->Font);
		if( memcmp(&l1, &l2, sizeof(l1)) ){
			MainVARI->PCRX->Font = PCRx2->Font;
            MainVARI->OnFontChange(FALSE);
        }
		MainVARI->m_Dump.m_Color[0].c = PCRx1->Color;
		MainVARI->m_Dump.m_Color[1].c = PCRx2->Color;
		MainVARI->m_Dump.m_Color[2].c = PCRx3->Color;
		MainVARI->m_Dump.m_Color[3].c = PCRx4->Color;
		MainVARI->m_Dump.m_Color[4].c = PCRx5->Color;
		MainVARI->m_StatusUTC = CBRxUTC->Checked;
		sys.m_EnableMouseWheel = CBMW->Checked;
		sys.m_fShowCtrlCode = CBShowCtrl->Checked;

        GetLogFont(&l1, PCTx2->Font);
        GetLogFont(&l2, MainVARI->PCTX->Font);
		if( memcmp(&l1, &l2, sizeof(l1)) ){
			MainVARI->PCTX->Font = PCTx2->Font;
            MainVARI->OnFontChange(TRUE);
        }
		for( int i = 0; i < 4; i++ ){
			MainVARI->m_Edit[i].m_Color[0].c = PCTx1->Color;
			MainVARI->m_Edit[i].m_Color[1].c = PCTx2->Color;
			MainVARI->m_Edit[i].m_Color[2].c = PCTx3->Color;
			MainVARI->m_Edit[i].m_Color[3].c = PCTx4->Color;
			MainVARI->m_Edit[i].m_Color[4].c = PCTx1->Color;
		}
		MainVARI->m_tFFTColset[0].c = PS1->Color;
		MainVARI->m_tFFTColset[1].c = PS2->Color;
		MainVARI->m_tFFTColset[2].c = PS3->Color;
		MainVARI->m_tFFTColset[3].c = PS4->Color;
		MainVARI->m_tFFTColset[4].c = PS5->Color;
		MainVARI->m_tFFTColset[5].c = PS6->Color;

		MainVARI->m_tWaterColset[0].c = PW1->Color;
		MainVARI->m_tWaterColset[1].c = PW2->Color;
		MainVARI->m_tWaterColset[2].c = PW3->Color;
		MainVARI->m_tWaterColset[3].c = PW4->Color;
		MainVARI->m_tWaterColset[4].c = PW5->Color;
		MainVARI->m_tWaterColset[5].c = PW6->Color;
		MainVARI->m_tWaterColset[6].c = PW7->Color;
		MainVARI->m_tWaterColset[7].c = PW8->Color;
		MainVARI->m_tWaterColset[8].c = PW9->Color;
		MainVARI->m_tWaterColset[9].c = PW10->Color;
		MainVARI->m_tWaterColset[10].c = PW11->Color;
		MainVARI->m_tWaterColset[11].c = PW12->Color;
		for( i = 0; i < 6; i++ ){
			MainVARI->m_tWaterLevel[i] = m_tWaterLevel[i];
	    }

		sys.m_fFixWindow = RBW2->Checked;
        sys.m_fRestoreSubChannel = CBRestoreSub->Checked;

		if( RGRTTY->ItemIndex != MainVARI->m_RxSet[0].m_RTTYFFT ){
			MainVARI->SetRTTYFFT(RGRTTY->ItemIndex);
        }

	    sys.m_MFSK_SQ_Metric = CBMM->Checked;
		sscanf(AnsiString(CBClock->Text).c_str(), "%lf", &dd);	//JA7UDE 0428
		di = CBMC->Checked != sys.m_MFSK_Center;
		if( (dd != SAMPFREQ) || di ){
			sys.m_MFSK_Center = CBMC->Checked;
			MainVARI->SetSampleFreq(dd, di);
		}
		sscanf(AnsiString(CBTxOffset->Text).c_str(), "%lf", &dd);	//JA7UDE 0428
		if( dd != SAMPTXOFFSET ){
			MainVARI->SetTxOffset(dd);
		}
		sscanf(AnsiString(CBFifoRX->Text).c_str(), "%u", &di);	//JA7UDE 0428
		if( (di >= 4) && (di <= 32) ){
			MainVARI->SetRXFifo(di);
		}
		sscanf(AnsiString(CBFifoTX->Text).c_str(), "%u", &di);	//JA7UDE 0428
        if( (di >= 4) && (di <= 32) ){
			MainVARI->SetTXFifo(di);
        }
		MainVARI->SetSoundCard(RGSoundIn->ItemIndex, AnsiString(CBSoundID->Text).c_str(), AnsiString(CBSoundIDTX->Text).c_str());	//JA7UDE 0428
		MainVARI->UpdatePriority(RGPriority->ItemIndex);

		if( (m_FontCharset != sys.m_FontCharset) || (m_FontName != sys.m_FontName) ){
			m_fLangChange = TRUE;
            sys.m_FontCharset = m_FontCharset;
	        sys.m_FontName = m_FontName;
        }
        return TRUE;
    }
	OnWave();
    return FALSE;
}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::UdTxOffsetClick(TObject *Sender,
      TUDBtnType Button)
{
	if( m_fDisEvent ) return;

	double f;
	sscanf(AnsiString(CBTxOffset->Text).c_str(), "%lf", &f);	//JA7UDE 0428
	if( Button == Comctrls::btNext ){
		f += 0.02;
	}
	else {
		f -= 0.02;
	}
	char bf[128];
	sprintf(bf, "%.2lf", f);
	m_fDisEvent++;
	CBTxOffset->Text = bf;
	m_fDisEvent--;
}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::CBTxOffsetChange(TObject *Sender)
{
	if( m_fDisEvent ) return;
	double fq;
	sscanf(AnsiString(CBTxOffset->Text).c_str(), "%lf", &fq);	//JA7UDE 0428
    if( (fq >= -300.0) && (fq <= 300.0) ){
		m_fDisEvent++;
		UdTxOffset->Position = short(fq * 100.0);
        m_fDisEvent--;
    }
}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::SBRxFontClick(TObject *Sender)
{
	TFontDialog *pBox = new TFontDialog(this);
	TPanel *pPanel = Sender == SBRxFont ? PCRx2 : PCTx2;
	pBox->Font = pPanel->Font;
    pBox->Font->Color = pPanel->Color;
	OnWave();
    if( pBox->Execute() ){
		pPanel->Font = pBox->Font;
		pPanel->Color = pBox->Font->Color;
		if( pPanel == PCRx2 ){
			PCTx2->Font->Name = PCRx2->Font->Name;
			PCTx2->Font->Charset = PCRx2->Font->Charset;
        }
		UpdateHint();
    }
    delete pBox;
}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::SBRxCharClick(TObject *Sender)
{
	LOGFONT l1;
    GetLogFont(&l1, PCRx2->Font);
	TCodeView *pBox = new TCodeView(this);
	Application->OnHint = m_fnHintProc;
	pBox->Execute(NULL, 0, 0, PCRx2->Font);
	Application->OnHint = DisplayHint;
	LOGFONT l2;
    GetLogFont(&l2, PCRx2->Font);
	if( memcmp(&l1, &l2, sizeof(l1)) ){
		PCTx2->Font->Name = PCRx2->Font->Name;
		PCTx2->Font->Charset = PCRx2->Font->Charset;
        UpdateHint();
    }
    delete pBox;
}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::PCRx1Click(TObject *Sender)
{
	TColorDialog *pBox = new TColorDialog(this);
	pBox->Options << cdFullOpen;
	TPanel *pPanel = (TPanel *)Sender;
    pBox->Color = pPanel->Color;
	OnWave();
    if( pBox->Execute() ){
		pPanel->Color = pBox->Color;
		if( pPanel == PW1 ) PL->Color = PW1->Color;
		PBL->Invalidate();
    }
    delete pBox;
}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::UdClockClick(TObject *Sender,
      TUDBtnType Button)
{
	if( m_fDisEvent ) return;

	double f;
	sscanf(AnsiString(CBClock->Text).c_str(), "%lf", &f);	//JA7UDE 0428
	if( Button == Comctrls::btNext ){
		f += 0.02;
	}
	else {
		f -= 0.02;
	}
	char bf[128];
    sprintf(bf, "%.2lf", f);
	m_fDisEvent++;
    CBClock->Text = bf;
    m_fDisEvent--;
}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::CBComPTTChange(TObject *Sender)
{
	if( m_fDisEvent ) return;

	m_fComChange = TRUE;
    UpdateUI();
}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::SBRadioClick(TObject *Sender)
{
	TRADIOSetDlg *pBox = new TRADIOSetDlg(this);
	RADIO.change = pBox->Execute();
    delete pBox;
}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::SBEngClick(TObject *Sender)
{
    if( SBJA->Down ){
		m_FontName = "ＭＳ Ｐゴシック";
		m_FontCharset = SHIFTJIS_CHARSET;
		if( sys.m_MsgEng ){
			Application->MessageBox(L"MMVARIを再起動すると日本語モードになります.\r\n(Please restart MMVARI for Japanese mode)", L"MMVARI", MB_ICONINFORMATION|MB_OK);
        }
    }
    else {
		m_FontName = "Arial";
		m_FontCharset = ANSI_CHARSET;
    }
}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::SBFontClick(TObject *Sender)
{
	TFontDialog *pBox = new TFontDialog(this);
	pBox->Font->Name = m_FontName;
    pBox->Font->Charset = m_FontCharset;
	OnWave();
    if( pBox->Execute() ){
		m_FontName = pBox->Font->Name;
        m_FontCharset = pBox->Font->Charset;
        UpdateButton();
    }
    delete pBox;
}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::PBLPaint(TObject *Sender)
{
	int xw = PBL->Width;
    int yw = PBL->Height;
	int x;

    TCanvas *pCanvas = PBL->Canvas;

	TRect rc;
    rc.Left = 0; rc.Top = 0;
    rc.Right = xw; rc.Bottom = yw;
	pCanvas->Brush->Color = PW1->Color;
    pCanvas->FillRect(rc);

	pCanvas->Pen->Width = 2;
	TColor col[6];
    col[0] = PW2->Color;
    col[1] = PW7->Color;
    col[2] = PW8->Color;
    col[3] = PW9->Color;
    col[4] = PW10->Color;
    col[5] = PW11->Color;

    for( int i = 0; i < 6; i++ ){
		x = m_tWaterLevel[i] * xw / 256;
		pCanvas->Pen->Color = col[i];
	    pCanvas->MoveTo(x, 0);
    	pCanvas->LineTo(x, yw);
	}
}
//---------------------------------------------------------------------------
int __fastcall TOptDlgBox::GetWLNo(int x)
{
	int i;
	int *ip = m_tWaterLevel;
    for( i = 0; i < 6; i++, ip++ ){
		int X = *ip * PBL->Width / 256;
		if( (x > (X-3)) && (x < (X+3)) ){
			return i;
        }
    }
    return -1;
}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::PBLMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if( Button != mbLeft ) return;

	if( m_MouseWLN >= 0 ){
	    m_MouseDown = TRUE;
		switch(m_MouseWLN){
			case 0:
				m_MinWL = 0;
                m_MaxWL = m_tWaterLevel[1];
				break;
            case 1:
            case 2:
            case 3:
            case 4:
				m_MinWL = m_tWaterLevel[m_MouseWLN-1];
                m_MaxWL = m_tWaterLevel[m_MouseWLN+1];
				break;
            case 5:
				m_MinWL = m_tWaterLevel[4];
                m_MaxWL = 256;
				break;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::PBLMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
	if( m_MouseDown ){
		int x = X * 256 / PBL->Width;
		if( (x > m_MinWL) && (x < m_MaxWL) ){
			m_tWaterLevel[m_MouseWLN] = x;
    	    PBLPaint(NULL);
        }
    }
    else {
		m_MouseWLN = GetWLNo(X);
        PBL->Cursor = m_MouseWLN >= 0 ? crSizeWE : crDefault;
    }
}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::PBLMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	m_MouseDown = FALSE;
}
//---------------------------------------------------------------------------

void __fastcall TOptDlgBox::SBWLCDClick(TObject *Sender)
{
	PW1->Color = clBlack;		// back
	PW3->Color = clWhite;		// char
    PW4->Color = clRed;			// WAKU
	PW5->Color = TColor(RGB(0,255,255));		// RX
    PW6->Color = clBlue;		// TX

	if( Sender == SBWLCD ){
		m_tWaterLevel[0] = 28;
		m_tWaterLevel[1] = 100;
		m_tWaterLevel[2] = 168;
		m_tWaterLevel[3] = 192;
		m_tWaterLevel[4] = 220;
		m_tWaterLevel[5] = 240;

	    PW2->Color = TColor(RGB(0,255,255));	// low
	    PW7->Color = clYellow;		// mid-low
	    PW8->Color = TColor(RGB(255,128,0));	// mid
	    PW9->Color = TColor(RGB(255,0,128));	// mid-high
	    PW10->Color = clRed;		// Peak
	    PW11->Color = clRed;		// Peak
	    PW12->Color = clRed;		// Peak
    }
    else {
		m_tWaterLevel[0] = 10;
		m_tWaterLevel[1] = 60;
		m_tWaterLevel[2] = 134;
		m_tWaterLevel[3] = 192;
		m_tWaterLevel[4] = 220;
		m_tWaterLevel[5] = 240;

	    PW2->Color = TColor(16711680);	// low
	    PW7->Color = TColor(16776960);	// mid-low
	    PW8->Color = TColor(65535);	// mid
	    PW9->Color = TColor(33023);	// mid-high
	    PW10->Color = clRed;		// Peak
	    PW11->Color = clRed;		// Peak
	    PW12->Color = clRed;		// Peak
    }
	PBL->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::CBSoundIDDropDown(TObject *Sender)
{
	if( m_fDisEvent ) return;

    if( !m_MMListW.IsQuery() ){
		m_MMListW.QueryList("MMW");
		for( int i = 0; i < m_MMListW.GetCount(); i++ ){
			CBSoundID->Items->Add(m_MMListW.GetItemName(i));
		}
		CBSoundID->DropDownCount = CBSoundID->Items->Count;
    }
}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::CBSoundIDChange(TObject *Sender)
{
	if( m_fDisEvent ) return;
	UpdateUI();
}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::CBComPTTDropDown(TObject *Sender)
{
	if( m_fDisEvent ) return;

    if( !m_MMListF.IsQuery() ){
		m_MMListF.QueryList("FSK");
		for( int i = 0; i < m_MMListF.GetCount(); i++ ){
			CBComPTT->Items->Add(m_MMListF.GetItemName(i));
		}
		CBComPTT->DropDownCount = CBComPTT->Items->Count;
    }
}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::SBASClick(TObject *Sender)
{
	TMacEditDlg *pBox = new TMacEditDlg(this);
	if( pBox->Execute(m_AS, "AS(CW)") ){
		AdjustAS(&m_AS);
    }
    delete pBox;
}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::SBEventClick(TObject *Sender)
{
	int n = CBEvent->ItemIndex;
	TMacEditDlg *pBox = new TMacEditDlg(this);
	if( pBox->Execute(m_MacEvent[n], g_tMacEvent[n]) ){
		AdjustAS(&m_MacEvent[n]);
    }
    delete pBox;
    UpdateUI();
}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::SBEventCClick(TObject *Sender)
{
	int n = CBEvent->ItemIndex;
	if( YesNoMB(sys.m_MsgEng ? "Clear contents of %s, and stop executing this event macro.":"%sの定義内容をクリアし、このイベントマクロを停止します.", g_tMacEvent[n]) == IDYES ){
		m_MacEvent[n] = "";
		UpdateUI();
    }
}
//---------------------------------------------------------------------------
void __fastcall TOptDlgBox::CBPBClick(TObject *Sender)
{
	UpdateUI();
}
//---------------------------------------------------------------------------


 
