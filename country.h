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



#ifndef CountryH
#define	CountryH

extern LPSTR __fastcall StrDupe(LPCSTR s);

#define	CTMAX	512
typedef struct {
	LPSTR	Name;
	LPSTR	Code;
	LPSTR	QTH;
	LPSTR	Cont;
	LPSTR	TD;
}CTL;

class CCountry
{
private:
	int		cmax;
	CTL		ctl[CTMAX];

	char	wbf[512];
public:
	__fastcall CCountry();
	__fastcall ~CCountry(void);

	inline CTL *__fastcall GetCTL(int n){ return &ctl[n];};
	void 	__fastcall Init(void);
	void 	__fastcall Free(void);
	int 	__fastcall GetNo(LPCSTR s);
	int 	__fastcall GetNoP(LPCSTR p);
	LPCSTR	__fastcall GetCountry(LPCSTR p);
	LPCSTR	__fastcall GetCont(LPCSTR p);
	void	__fastcall Load(LPCSTR fm);
	inline	int __fastcall IsData(void){
		return cmax;
	};
};

extern	CCountry	Cty;
#endif

