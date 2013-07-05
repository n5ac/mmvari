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
#ifndef LogSetH
#define LogSetH
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
#include <vcl\ComCtrls.hpp>
*/
//----------------------------------------------------------------------------
#include "LogFile.h"
#include "MMLink.h"
#include <Classes.hpp>
#include <ComCtrls.hpp>
#include <Controls.hpp>
#include <ExtCtrls.hpp>
#include <StdCtrls.hpp>
//----------------------------------------------------------------------------
class TLogSetDlg : public TForm
{
__published:        
	TButton *OKBtn;
	TButton *CancelBtn;
	TPageControl *Page;
	TTabSheet *TabFlag;
	TGroupBox *GrpConv;
	TCheckBox *UpperName;
	TCheckBox *UpperQTH;
	TCheckBox *UpperREM;
	TCheckBox *UpperQSL;
	TGroupBox *GrpCopy;
	TCheckBox *CopyName;
	TCheckBox *CopyQTH;
	TCheckBox *CopyREM;
	TCheckBox *CopyQSL;
	TRadioGroup *CopyHis;
	TRadioGroup *CopyFreq;
	TGroupBox *GB5;
	TCheckBox *DefMyRST;
	TRadioGroup *TimeZone;
	TTabSheet *TabConv;
	TGroupBox *GB1;
	TGroupBox *GB2;
	TCheckBox *ClipRSTADIF;
	TTabSheet *TabMisc;
	TGroupBox *GTime;
	TEdit *TimeOff;
	TUpDown *UDOffset;
	TRadioGroup *DateType;
	TEdit *MinOff;
	TUpDown *UDMin;
	TLabel *Label4;
	TLabel *Label5;
	TButton *ClearOff;
	TCheckBox *AutoSave;
	TCheckBox *AutoZone;
	TGroupBox *GB3;
	TRadioGroup *RGLink;
	TCheckBox *CBPoll;
	TRadioGroup *THTZ;
	TRadioGroup *RGDupe;
	TLabel *LLink;
	TCheckBox *CBRemC;
	TComboBox *CBMMLink;
	TLabel *LT;
	TCheckBox *CBBackup;
	TLabel *LH;
	TCheckBox *CBPTT;
	TGroupBox *GB9;
	TLabel *Label2;
	TEdit *THRTTY;
	TLabel *Label3;
	TEdit *THSSTV;
	TLabel *L10;
	TEdit *THGMSK;
	void __fastcall ClearOffClick(TObject *Sender);
	void __fastcall AutoZoneClick(TObject *Sender);
	
	
	
	void __fastcall RGLinkClick(TObject *Sender);
	
private:
	int			m_DisEvent;
    TNotifyEvent 	m_fnHintProc;
//	AnsiString	m_MacroStr[5];
//	WORD		m_MacroKey[5];

	CMMList		m_MMList;

	void __fastcall MacroBtnClick(int n);
	void __fastcall UpdateUI(void);

public:
	virtual __fastcall TLogSetDlg(TComponent* AOwner);
	void __fastcall DisplayHint(TObject *Sender);

	int __fastcall Execute(void);

};
//----------------------------------------------------------------------------
//extern TLogSetDlg *LogSetDlg;
//----------------------------------------------------------------------------
#endif    
