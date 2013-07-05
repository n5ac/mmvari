//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "Main.h"
SYS		sys;
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
void __fastcall SetDirName(LPSTR t, LPCSTR pName)
{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char name[_MAX_FNAME];
	char ext[_MAX_EXT];
	AnsiString	Dir;

	::_splitpath( pName, drive, dir, name, ext );
	Dir = drive;
	Dir += dir;
	strncpy(t, Dir.c_str(), MAX_PATH-1);
}
//---------------------------------------------------------------------------
void __fastcall SetEXT(LPSTR t, LPCSTR pExt)
{
	LPSTR p = LASTP(t);
	for( ; p > t; p-- ){
		if( *p == '.' ){
			p++;
			strcpy(p, pExt);
			return;
		}
	}
}
//***************************************************************************
// TExtFSK (MainWindow) class
//---------------------------------------------------------------------------
__fastcall TComFSK::TComFSK(TComponent* Owner)
	: TForm(Owner)
{
	m_DisEvent = 1;
	m_Left = 0;
    m_Top = 0;
	m_hPort = INVALID_HANDLE_VALUE;
	m_X = 0;
	m_Para = 45 << 16;

    m_fMsg = TRUE;
	m_fCheckError = FALSE;
    m_fLimitSpeed = FALSE;
	m_LimitMargin = 3;

	m_strIniName = sys.m_ModuleName;
	SetEXT(m_strIniName.c_str(), "ini");
	PortName->ItemIndex = 0;

	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char name[_MAX_FNAME];
	char ext[_MAX_EXT];

	::_splitpath( sys.m_ModuleName, drive, dir, name, ext );
    strupr(name);
    AnsiString as = name;
    as += " "VERNO;
    Caption = as;

	ReadIniFile();

	CBERR->Checked = m_fCheckError;
    CBLMT->Checked = m_fLimitSpeed;

	Left = m_Left;
    Top = m_Top;
	WindowState = TWindowState(m_WindowState);
	m_DisEvent = 0;
}
//---------------------------------------------------------------------------
//	para:	Upper16bits	Speed(eg. 45)
//			Lower16bits	b1-b0	Stop (0-1, 1-1.5, 2-2)
//						b5-b2	Length
void __fastcall TComFSK::SetPara(LONG para)
{
	m_Para = para;
	AnsiString as = int(para >> 16);
	as += " baud";
	LB->Caption = as;
    OpenPort();
}
//---------------------------------------------------------------------------
void __fastcall TComFSK::FormClose(TObject *Sender, TCloseAction &Action)
{
	m_WindowState = WindowState;
    if( m_WindowState == wsNormal ){
		m_Left = Left;
        m_Top = Top;
    }

	if( IsOpen() && m_ptt ){
		SetPTT(FALSE, FALSE);
	}

	ClosePort();
	WriteIniFile();
}
//---------------------------------------------------------------------------
void __fastcall TComFSK::ReadIniFile(void)
{
	TMemIniFile *pIniFile = new TMemIniFile(m_strIniName);

	m_Top = pIniFile->ReadInteger("Window", "Top", m_Top);
	m_Left = pIniFile->ReadInteger("Window", "Left", m_Left);
	if( m_Top < 0 ) m_Top = 0;
	if( m_Left < 0 ) m_Left = 0;

	m_WindowState = (TWindowState)pIniFile->ReadInteger("Window", "State", WindowState);

	AnsiString as = pIniFile->ReadString("Settings", "Port", "COM1");
	int n = PortName->Items->IndexOf(as);
	if( n < 0 ){
		n = atoi(as.c_str());
		if( n < 0 ) n = 0;
	}
	PortName->ItemIndex = n;
	RGPTT->ItemIndex = pIniFile->ReadInteger("Settings", "PTT", RGPTT->ItemIndex);
	RGSTOP->ItemIndex = pIniFile->ReadInteger("Settings", "STOP", RGSTOP->ItemIndex);
	CBInvPTT->Checked = pIniFile->ReadInteger("Settings", "InvPTT", CBInvPTT->Checked);
	m_fCheckError = pIniFile->ReadInteger("Settings", "CheckError", m_fCheckError);
	m_fLimitSpeed = pIniFile->ReadInteger("Settings", "LimitSpeed", m_fLimitSpeed);
	m_LimitMargin = pIniFile->ReadInteger("Settings", "LimitMargin", m_LimitMargin);
	int ver = pIniFile->ReadInteger("Settings", "VER", 0);
	if( ver < VERINI ) RGSTOP->ItemIndex = 1;
	delete pIniFile;
}
//---------------------------------------------------------------------------
void __fastcall TComFSK::WriteIniFile(void)
{
	TMemIniFile *pIniFile = new TMemIniFile(m_strIniName);
	pIniFile->WriteInteger("Window", "Top", m_Top);
	pIniFile->WriteInteger("Window", "Left", m_Left);
	pIniFile->WriteInteger("Window", "State", WindowState);
	pIniFile->WriteInteger("Settings", "VER", VERINI);
	pIniFile->WriteString("Settings", "Port", PortName->Items->Strings[PortName->ItemIndex]);
	pIniFile->WriteInteger("Settings", "PTT", RGPTT->ItemIndex);
	pIniFile->WriteInteger("Settings", "InvPTT", CBInvPTT->Checked);
	pIniFile->WriteInteger("Settings", "STOP", RGSTOP->ItemIndex);
	pIniFile->WriteInteger("Settings", "CheckError", m_fCheckError);
	pIniFile->WriteInteger("Settings", "LimitSpeed", m_fLimitSpeed);
	pIniFile->WriteInteger("Settings", "LimitMargin", m_LimitMargin);
	pIniFile->UpdateFile();
	delete pIniFile;
}
//---------------------------------------------------------------------------
void __fastcall TComFSK::UpdateComStat(void)
{
	char bf[128];

	wsprintf(bf, "Status:%s", m_hPort != INVALID_HANDLE_VALUE ? "OK" : "NG");
	LComStat->Color = m_hPort != INVALID_HANDLE_VALUE ? clBtnFace : clRed;
	LComStat->Caption = bf;
}
//---------------------------------------------------------------------------
void __fastcall TComFSK::OpenPort(void)
{
	ClosePort();
	OpenPort_();
	UpdateComStat();
	SetPTT(FALSE, FALSE);
}
//---------------------------------------------------------------------------
BOOL __fastcall TComFSK::OpenPort_(void)
{
	AnsiString pname = PortName->Items->Strings[PortName->ItemIndex];

	m_hPort = ::CreateFile( pname.c_str(),
					GENERIC_READ | GENERIC_WRITE,
					0, NULL,
					OPEN_EXISTING,
					FILE_ATTRIBUTE_NORMAL,
					NULL
	);
	if( m_hPort == INVALID_HANDLE_VALUE ){
		AnsiString as = "\\\\.\\";
        as += pname;
		m_hPort = ::CreateFile( as.c_str(),
						GENERIC_READ | GENERIC_WRITE,
						0, NULL,
						OPEN_EXISTING,
						FILE_ATTRIBUTE_NORMAL,
						NULL
		);
    }
	if( m_hPort == INVALID_HANDLE_VALUE ) return FALSE;

	if( ::SetupComm( m_hPort, DWORD(256), DWORD(2) ) == FALSE ){
		::CloseHandle(m_hPort);
		m_hPort = INVALID_HANDLE_VALUE;
		return FALSE;
	}
	::PurgeComm( m_hPort, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR );

	COMMTIMEOUTS TimeOut;

	TimeOut.ReadIntervalTimeout = 0xffffffff;
	TimeOut.ReadTotalTimeoutMultiplier = 0;
	TimeOut.ReadTotalTimeoutConstant = 0;
	TimeOut.WriteTotalTimeoutMultiplier = 0;
	TimeOut.WriteTotalTimeoutConstant = 1000;
	if( !::SetCommTimeouts( m_hPort, &TimeOut ) ){
		::CloseHandle( m_hPort );
		m_hPort = INVALID_HANDLE_VALUE;
		return FALSE;
	}

	::GetCommState( m_hPort, &m_dcb );
	m_dcb.fBinary = TRUE;
	m_dcb.BaudRate = m_Para >> 16;	// BAUD
	m_dcb.ByteSize = 5;				// 5 bits length
	m_dcb.StopBits = BYTE(RGSTOP->ItemIndex);
	m_dcb.Parity = NOPARITY;
	m_dcb.XonChar = 0x11;	// XON
	m_dcb.XoffChar = 0x13;	// XOFF
	m_dcb.fParity = 0;
	m_dcb.fOutxCtsFlow = FALSE;
	m_dcb.fInX = m_dcb.fOutX = FALSE;
	m_dcb.fOutxDsrFlow = FALSE;
	m_dcb.EvtChar = 0x0d;
	m_dcb.fRtsControl = RTS_CONTROL_DISABLE;
	m_dcb.fDtrControl = DTR_CONTROL_DISABLE;
	m_dcb.fTXContinueOnXoff = FALSE;
	m_dcb.XonLim = USHORT(256/4);
	m_dcb.XoffLim = USHORT(256*3/4);
	m_dcb.DCBlength = sizeof( DCB );

	if( !::SetCommState( m_hPort, &m_dcb ) ){
		::CloseHandle( m_hPort );
		m_hPort = INVALID_HANDLE_VALUE;
		return FALSE;
	}

	if( !::SetCommMask( m_hPort, EV_RXFLAG ) ){
		::CloseHandle(m_hPort);
		m_hPort = INVALID_HANDLE_VALUE;
		return FALSE;
	}
	return TRUE;
}
//---------------------------------------------------------------------------
void __fastcall TComFSK::ClosePort(void)
{
	DeleteMMTimer();
	if( IsOpen() ){
		::CloseHandle(m_hPort);
		m_hPort = INVALID_HANDLE_VALUE;
	}
	UpdateComStat();
}
//---------------------------------------------------------------------------
void __fastcall TComFSK::SetPort(int port, int sw)
{
	switch(port){
		case ptTXD:
			::EscapeCommFunction(m_hPort, sw ? SETBREAK : CLRBREAK);
			break;
		case ptRTS:
			::EscapeCommFunction(m_hPort, sw ? SETRTS : CLRRTS);
			break;
		case ptDTR:
			::EscapeCommFunction(m_hPort, sw ? SETDTR : CLRDTR);
			break;
	}
}
//---------------------------------------------------------------------------
void __fastcall TComFSK::SetPTT(int sw, int msg)
{
	m_ptt = sw;
	if( CBInvPTT->Checked ){
		sw = sw ? 0 : 1;
    }
    if( !m_ptt && m_ummTimerID ){
		DeleteMMTimer();
    }
    SetPort(RGPTT->ItemIndex ? ptDTR : ptRTS, sw);
	if( m_ptt ){
		m_dwPending = 0;
    }

	m_X = 0;
	if( msg ){
		if( WindowState == wsMinimized) return;
		Memo->Lines->Add(m_ptt ? "PTT ON" : "PTT OFF");
	}
}
//---------------------------------------------------------------------------
void CALLBACK mmTimeProc(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	ASSERT(dwUser);

	TComFSK *pComm = (TComFSK *)dwUser;
	if( uID != pComm->m_ummTimerID ) return;

	pComm->DoTimer();
}
//-----------------------------------------------------------------
void __fastcall TComFSK::DoTimer(void)
{
	if( m_dwPending ){
		if( m_hPort != INVALID_HANDLE_VALUE ){
			::TransmitCommChar(m_hPort, BYTE(m_dwPending & 0x00ff));
        }
        m_dwPending = 0;
    }
}
//-----------------------------------------------------------------
void __fastcall TComFSK::CreateMMTimer(void)
{
	if( !m_fLimitSpeed ) return;

	int baud = m_Para >> 16;
	if( baud <= 0 ) return;

	m_ummTimerID = 0;
	if( ::timeGetDevCaps(&m_TimeCaps, sizeof(m_TimeCaps)) == TIMERR_NOERROR ){
		::timeBeginPeriod(m_TimeCaps.wPeriodMin);
		double dblSymTime = (14 + RGSTOP->ItemIndex) * 500.0 / double(baud);
		double dblLmtTime = dblSymTime + m_LimitMargin;
		int lmttime = int(dblLmtTime + 0.5);
		if( m_fMsg && (WindowState == wsNormal) ){
			char bf[64];
			wsprintf(bf, "S=%d.%02dms, T=%ums", int(dblSymTime), int(dblSymTime * 100.0) % 100, lmttime);
			Memo->Lines->Add(bf);
        }
		m_ummTimerID = ::timeSetEvent(lmttime, 0, mmTimeProc, DWORD(this), TIME_PERIODIC);
	}
	if( !m_ummTimerID ){
		if( m_fMsg ){
			Memo->Lines->Add("MM timer is not supported.");
        }
	}
	m_fMsg = FALSE;
}
//-----------------------------------------------------------------
void __fastcall TComFSK::DeleteMMTimer(void)
{
	if( m_ummTimerID ){
		::timeKillEvent(m_ummTimerID);
		m_ummTimerID = 0;
		::timeEndPeriod(m_TimeCaps.wPeriodMin);
	}
}
//-----------------------------------------------------------------
// 送信ビジーかどうか調べる
int __fastcall TComFSK::IsBusy(void)
{
    if( m_ummTimerID ){
    	return m_dwPending ? 1 : 0;
    }
	if( IsOpen() ){
		if( m_dwPending ){
			if( TransmitCommChar(m_hPort, BYTE(m_dwPending & 0x00ff)) ){
				m_dwPending = 0;
			}
	        return TRUE;
    	}
    }
	if( m_fCheckError || m_fLimitSpeed ) return FALSE;
	if( !IsOpen() ) return FALSE;

	COMSTAT ComStat;
	DWORD	dwErrorFlags;

	ClearCommError(m_hPort, &dwErrorFlags, &ComStat);
	return ComStat.fTxim;
}
//---------------------------------------------------------------------------
void __fastcall TComFSK::PutChar(BYTE c)
{
	if( !m_ptt ) return;

	if( m_fLimitSpeed && !m_ummTimerID ) CreateMMTimer();

	if( m_ummTimerID ){
		m_dwPending = c | 0x8000;
    }
	else if( IsOpen() ){
        if( !TransmitCommChar(m_hPort, c) ){
			if( m_fCheckError ){
				m_dwPending = c | 0x8000;
            }
        }
    }
	if( WindowState == wsMinimized) return;

	char bf[128];
	if( m_X ){
		int n = Memo->Lines->Count;
		if( n ) n--;
		strcpy(bf, AnsiString(Memo->Lines->Strings[n]).c_str());
		wsprintf(&bf[strlen(bf)], " %02X", c);
		if( !m_ptt ) return;
		Memo->Lines->Strings[n] = bf;
	}
	else {
		wsprintf(bf, "%02X", c);
		Memo->Lines->Add(bf);
	}
	m_X++;
	if( m_X >= 8 ) m_X = 0;
}
//---------------------------------------------------------------------------
void __fastcall TComFSK::PortNameChange(TObject *Sender)
{
	if( m_DisEvent ) return;

	OpenPort();
}
//---------------------------------------------------------------------------
void __fastcall TComFSK::SBMinClick(TObject *Sender)
{
	if( m_DisEvent ) return;

    if( WindowState == wsNormal ){
		m_Left = Left;
        m_Top = Top;
    }

	WindowState = wsMinimized;
	m_WindowState = wsMinimized;

	Memo->Lines->Clear();
	m_X = 0;
}
//---------------------------------------------------------------------------
void __fastcall TComFSK::CBInvPTTClick(TObject *Sender)
{
	if( m_DisEvent ) return;
	SetPTT(m_ptt, FALSE);
}
//---------------------------------------------------------------------------
void __fastcall TComFSK::CBERRClick(TObject *Sender)
{
	m_fCheckError = CBERR->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TComFSK::CBLMTClick(TObject *Sender)
{
	m_fLimitSpeed = CBLMT->Checked;
	if( m_ptt && m_fLimitSpeed ){
		CreateMMTimer();
    }
    else {
		DeleteMMTimer();
    }
}
//---------------------------------------------------------------------------

