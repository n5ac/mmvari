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
#ifndef FreqDispH
#define FreqDispH
//----------------------------------------------------------------------------
/* JA7UDE 0428
#include <vcl\System.hpp>
#include <vcl\Windows.hpp>
#include <vcl\SysUtils.hpp>
#include <vcl\Classes.hpp>
#include <vcl\Graphics.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
#include <vcl\Controls.hpp>
#include <vcl\Buttons.hpp>
#include <vcl\ExtCtrls.hpp>
*/
//----------------------------------------------------------------------------
#include "dsp.h"
//----------------------------------------------------------------------------
class TFreqDispDlg : public TForm
{
__published:
	TButton *CancelBtn;
	TPanel *Panel;
	TPaintBox *PaintBox;
	TTimer *Timer;
	void __fastcall PaintBoxPaint(TObject *Sender);
	
	
private:
	Graphics::TBitmap *pBitmap;

	int			m_type;
	double		m_a10, m_a20;
	double		m_b11, m_b12;
	double		m_b21, m_b22;

	CIIR		*m_piir;

	const double *m_H1;
	int			m_Tap1;
	const double *m_H2;
	int			m_Tap2;
    double		m_SampleFreq;

	double		m_HT[TAPMAX+1];
private:
	void __fastcall MakeBitmap(void);

public:
	virtual __fastcall TFreqDispDlg(TComponent* AOwner);
	__fastcall ~TFreqDispDlg();

	void __fastcall Execute(CFIR2 *pFIR, int max, double SampleFreq);
	void __fastcall Execute(CFIRX *pFIR, int max, double SampleFreq);
	void __fastcall Execute(const double *H, int Tap, int max, double SampleFreq);
	void __fastcall Execute(double a10, double b11, double b12, double a20, double b21, double b22, double SampleFreq);
	void __fastcall Execute(CIIR *ip, double max, double SampleFreq);
	void __fastcall Execute(CIIRTANK &tank, double SampleFreq);

	int			m_Max;
};
//----------------------------------------------------------------------------
//extern TFreqDispDlg *FreqDispDlg;
//----------------------------------------------------------------------------
#endif    
 
