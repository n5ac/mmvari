//---------------------------------------------------------------------------
#ifndef MainH
#define MainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Buttons.hpp>
#include <Inifiles.hpp>
#include <mmsystem.h>
#include <stdlib.h>
//---------------------------------------------------------------------------
#define NDEBUG      // Remove this symbol, if you would like to do debug
#include <assert.h>
#define ASSERT(c)   assert(c)
//---------------------------------------------------------------------------
#define	VERNO	"1.04"
#define	VERINI	1
//---------------------------------------------------------------------------
template <class T> T LASTP(T p){
	int l = strlen(p);
	if( l ) p += (l-1);
	return p;
};
enum {
	ptTXD,
	ptRTS,
	ptDTR,
};
void __fastcall SetDirName(LPSTR t, LPCSTR pName);

typedef struct {	// Should not put a class into the member
	DWORD	m_dwVersion;
	int		m_WinNT;
	char	m_BgnDir[MAX_PATH];
	char	m_ModuleName[MAX_PATH];
}SYS;
extern SYS	sys;
//---------------------------------------------------------------------------
class TComFSK : public TForm
{
__published:	// IDE
	TMemo *Memo;
	TLabel *L1;
	TComboBox *PortName;
	TLabel *LComStat;
	TRadioGroup *RGPTT;
	TCheckBox *CBInvPTT;
	TSpeedButton *SBMin;
	TLabel *LB;
	TRadioGroup *RGSTOP;
	TCheckBox *CBERR;
	TCheckBox *CBLMT;
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall PortNameChange(TObject *Sender);

	void __fastcall SBMinClick(TObject *Sender);
	void __fastcall CBInvPTTClick(TObject *Sender);
	
	
	
	void __fastcall CBERRClick(TObject *Sender);
	void __fastcall CBLMTClick(TObject *Sender);
private:
	int		m_Left;
    int		m_Top;
	int		m_WindowState;
	int		m_DisEvent;
	int     m_ptt;
	int		m_X;
	AnsiString	m_strIniName;

	LONG	m_Para;
	HANDLE	m_hPort;
	DCB		m_dcb;

	BOOL	m_fCheckError;
    BOOL	m_fLimitSpeed;
	int		m_LimitMargin;

	BOOL	m_fBusy;
	DWORD	m_dwPending;

	BOOL	m_fMsg;
	TIMECAPS	m_TimeCaps;
	UINT	m_ummTimerID;
private:
	void __fastcall ReadIniFile(void);
	void __fastcall WriteIniFile(void);

	inline int __fastcall IsOpen(void){ return m_hPort != INVALID_HANDLE_VALUE;};
	void __fastcall UpdateComStat(void);
	void __fastcall OpenPort(void);
	BOOL __fastcall OpenPort_(void);
	void __fastcall ClosePort(void);
	void __fastcall SetPort(int port, int sw);

public:
	__fastcall TComFSK(TComponent* Owner);
	void __fastcall SetPara(LONG para);
	void __fastcall SetPTT(int sw, int msg);
	int __fastcall IsBusy(void);
	void __fastcall PutChar(BYTE c);
	void __fastcall CreateMMTimer(void);
	void __fastcall DeleteMMTimer(void);
	void __fastcall DoTimer(void);

	friend void CALLBACK mmTimeProc(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2);
};
//---------------------------------------------------------------------------
#endif
