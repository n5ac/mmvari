//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "Main.h"
//---------------------------------------------------------------------------
extern "C" void __declspec(dllexport) __stdcall extfskClose(void);
//---------------------------------------------------------------------------
USEFORM("Main.cpp", ComFSK);
//---------------------------------------------------------------------------
TComFSK *g_pMain;
//---------------------------------------------------------------------------
int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void*)
{
	switch(reason){
		case DLL_PROCESS_ATTACH:
			g_pMain = NULL;

			memset(&sys, 0, sizeof(sys));
			::GetModuleFileName(hinst, sys.m_ModuleName, sizeof(sys.m_ModuleName));
			::SetDirName(sys.m_BgnDir, sys.m_ModuleName);
			sys.m_dwVersion = ::GetVersion();
			sys.m_WinNT = (sys.m_dwVersion < 0x80000000) ? TRUE : FALSE;
			break;
		case DLL_PROCESS_DETACH:
			extfskClose();
			break;
	}
	return 1;
}
//---------------------------------------------------------------------------
extern "C" LONG __declspec(dllexport) __stdcall
extfskOpen(LONG para)
{
	extfskClose();

	g_pMain = new TComFSK(NULL);
	ASSERT(g_pMain);
	if( g_pMain != NULL ){
		g_pMain->SetPara(para);
		g_pMain->Show();
		return TRUE;
	}
	else {
		return FALSE;
	}
}

//---------------------------------------------------------------------------
extern "C" void __declspec(dllexport) __stdcall
extfskClose(void)
{
	if( g_pMain != NULL ){
		g_pMain->Close();
		delete g_pMain;
		g_pMain = NULL;
	}
}

//---------------------------------------------------------------------------
extern "C" LONG __declspec(dllexport) __stdcall
extfskIsTxBusy(void)
{
	if( g_pMain == NULL ) return FALSE;

	return g_pMain->IsBusy();
}

//---------------------------------------------------------------------------
extern "C" void __declspec(dllexport) __stdcall
extfskPutChar(BYTE c)
{
	if( g_pMain == NULL ) return;

	g_pMain->PutChar(c);
}

//---------------------------------------------------------------------------
extern "C" void __declspec(dllexport) __stdcall
extfskSetPTT(LONG tx)
{
	if( g_pMain == NULL ) return;

	g_pMain->SetPTT(tx & 1, TRUE);
}

