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
USEUNIT("mfsk.cpp");
USEUNIT("CLX.cpp");
USEUNIT("DSP.cpp");
USEFORM("Main.cpp", MainVARI);
USEUNIT("Wave.cpp");
USEUNIT("Fft.cpp");
USEUNIT("ComLib.cpp");
USEUNIT("Dump.cpp");
USEFORM("MacEdit.cpp", MacEditDlg);
USEFORM("FreqDisp.cpp", FreqDispDlg);
USEUNIT("Comm.cpp");
USEFORM("Option.cpp", OptDlgBox);
USEFORM("MacroKey.cpp", MacroKeyDlg);
USEFORM("FEdit.cpp", FileEdit);
USEFORM("VerDsp.cpp", VerDspDlg);
USEFORM("CodeVw.cpp", CodeView);
USEFORM("InputWin.cpp", InputWinDlg);
USEFORM("PlayDlg.cpp", PlayDlgBox);
USEFORM("LogSet.cpp", LogSetDlg);
USEUNIT("LogFile.cpp");
USEUNIT("Loglink.cpp");
USEFORM("LogList.cpp", LogListDlg);
USEUNIT("MMLink.cpp");
USEUNIT("LogConv.cpp");
USEUNIT("country.cpp");
USEFORM("Qsodlg.cpp", QSODlgBox);
USEUNIT("Mmcg.cpp");
USEFORM("MmcgDlg.cpp", MmcgDlgBox);
USEFORM("RadioSet.cpp", RADIOSetDlg);
USEUNIT("cradio.cpp");
USEFORM("RxView.cpp", RxViewDlg);
USERES("mmvari.res");
USEFORM("ClockAdj.cpp", ClockAdjDlg);
USEFORM("TrackDlg.cpp", TrackDlgBox);
USEFORM("Test.cpp", TestDlg);
USEUNIT("Hamlog5.cpp");
USEFORM("TH5Len.cpp", TH5LenDlg);
USEFORM("RMenuDlg.cpp", RMenuDialog);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	try
	{
		Application->Initialize();
		Application->CreateForm(__classid(TMainVARI), &MainVARI);
		Application->Run();
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	return 0;
}
//---------------------------------------------------------------------------
