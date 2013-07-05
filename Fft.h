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



#ifndef FFT_H
#define FFT_H
//---------------------------------------------------------------------------
extern int FFT_SIZE;
//#define	FFT_SIZE	2048
#define FFT_BUFSIZE 2048
#define SQRT_FFT_SIZE 46//sqrt(2048)

class CFFT
{
public:
	__fastcall CFFT();
	virtual __fastcall ~CFFT();
	void	__fastcall InitFFT(void);

    int		m_FFTGain;
	int		m_FFTDIS;
	void	__fastcall Calc(double * InBuf, int size, double gain, int stg, int* OutBuf);
    inline void __fastcall ClearStg(void){
		memset(pStgBuf, 0, sizeof(double[FFT_BUFSIZE]));
    }
private:
	BOOL	m_StgSW;
	int		m_StgSize;
	double	m_StgScale;
	double	m_StgK;

	double	*m_tSinCos;
	double	*m_tWindow;
	double	*pStgBuf;
	int		*m_Work;
	void __fastcall makewt(int nw, int *ip, double *w);
	void __fastcall makect(int nc, int *ip, double *c);
	void __fastcall bitrv2(int n, int *ip, double *a);
	void __fastcall cftfsub(int n, double *a, double *w);
	void __fastcall rftfsub(int n, double *a, int nc, double *c);
	void __fastcall cft1st(int n, double *a, double *w);
	void __fastcall cftmdl(int n, int l, double *a, double *w);
};

#endif
