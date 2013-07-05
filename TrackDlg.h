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



//----------------------------------------------------------------------------
#ifndef TrackDlgBox_H
#define TrackDlgBox_H
//----------------------------------------------------------------------------
#include <System.hpp>
#include <Windows.hpp>
#include <SysUtils.hpp>
#include <Classes.hpp>
#include <Graphics.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Controls.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>

//----------------------------------------------------------------------------
#include "ComLib.h"
//----------------------------------------------------------------------------
class TTrackDlgBox : public TForm
{
__published:
	TButton *OKBtn;
	TButton *CancelBtn;
	TTrackBar *TB;
	TLabel *LMin;
	TLabel *LMax;
	TLabel *LV;
	void __fastcall TBChange(TObject *Sender);
private:
	BOOL	m_Changed;

	double	m_K;
    double	m_Min, m_Max;

	void __fastcall ShowLV(void);
public:
	virtual __fastcall TTrackDlgBox(TComponent* AOwner);

	BOOL __fastcall Execute(LPCSTR pTitle, double &d, double min, double max, int sc, double per);
};
//----------------------------------------------------------------------------
//extern PACKAGE TTrackDlgBox *TrackDlgBox;
//----------------------------------------------------------------------------
#endif
 
