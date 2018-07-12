#ifndef __STATIC_WAVELET2D_H_INCLUDED__324897239487238947239847__
#define __STATIC_WAVELET2D_H_INCLUDED__324897239487238947239847__

#include "defines.h"

namespace brainwavelets
{

class CStaticWavelet2D
{
public:
	typedef float data_t;
public:
	// constructor
	CStaticWavelet2D(){}


	// direct transform
private:
	CStaticWavelet2D(CStaticWavelet2D const& wavelet){}
	CStaticWavelet2D& operator=(CStaticWavelet2D const& wavelet){return *this;}
private:
	static inline bool IsPowerOfTwo(unsigned n)
	{
		return (n & (n - 1)) == 0;
	}
public:
	static void LiftTranspose(data_t *pData, unsigned numCoeffs)
	{
		BRAINWAVELETS_ASSERT(numCoeffs);
		BRAINWAVELETS_ASSERT(IsPowerOfTwo(numCoeffs));
		if (numCoeffs == 1) return;

		// calculate odd indices
		{
			unsigned lastRowOffset = numCoeffs * (numCoeffs - 1);
			data_t *p, *pBaseRow, * pRowEnd, *const pDataEnd = pData + lastRowOffset; 
			for (pBaseRow = pData + numCoeffs; pBaseRow < pDataEnd; pBaseRow += numCoeffs << 1)
			{
				for (p = pBaseRow, pRowEnd = pBaseRow + numCoeffs;p < pRowEnd;++p)
				{
					*p -= (p[-(int)numCoeffs] + p[numCoeffs]) * (data_t)0.5;
				}
			}

			for (p = pDataEnd, pRowEnd = pRowEnd + numCoeffs;p < pRowEnd;++p)
			{
				*p -= (p[-(int)numCoeffs]);
			}
		}

		// calculate even indices
		{
			unsigned lastRowOffset = numCoeffs * (numCoeffs - 1);
			data_t *p, *pBaseRow, *pRowEnd, *const pDataEnd = pData + lastRowOffset; 

			for (pBaseRow = pData + numCoeffs; pBaseRow < pDataEnd; pBaseRow += numCoeffs << 1)
			{
				for (p = pBaseRow, pRowEnd = pBaseRow + numCoeffs;p < pRowEnd;++p)
				{
					*p -= (p[-(int)numCoeffs] + p[numCoeffs]) * (data_t)0.5;
				}
			}

			for (p = pDataEnd, pRowEnd = pRowEnd + numCoeffs; p < pRowEnd; ++p)
			{
				*p -= (p[-(int)numCoeffs]);
			}
		}
	}

	static void Lift(data_t *pData, unsigned numCoeffs)
	{
		enum { step = 1, step2 = step * 2 };

		BRAINWAVELETS_ASSERT(numCoeffs);
		BRAINWAVELETS_ASSERT(IsPowerOfTwo(numCoeffs));

		if (numCoeffs == 1) return;

		unsigned size = numCoeffs * step;
		unsigned last = size - step;

		// calculate odd indices
		{
			data_t *p, *pDataEnd = pData + last;
			for (p = pData + step; p < pDataEnd; p += step2)
			{
				*p -= (p[-step] + p[step]) * ((data_t)0.5);
			}
			pData[last] -= pDataEnd[-step];
		}

		// calculate even indices
		{
			data_t *p, *pDataEnd = pData + size;
			for (p = pData + step2; p < pDataEnd; p += step2)
			{
				*p += (p[-step] + p[step]) * ((data_t)0.25);
			}
			pData[0] += (pData[step]) * ((data_t)0.5);
		}
	}

	static void InverseLift(data_t *pData, unsigned numCoeffs)
	{
		enum { step = 1, step2 = step * 2 };

		BRAINWAVELETS_ASSERT(numCoeffs);
		BRAINWAVELETS_ASSERT(IsPowerOfTwo(numCoeffs));

		if (numCoeffs == 1) return;

		unsigned size = numCoeffs * step;
		unsigned last = size - step;

		// calculate even indices
		{
			data_t *p, *pDataEnd = pData + size;
			for (p = pData + step2; p < pDataEnd; p += step2)
			{
				*p -= (p[-step] + p[step]) * ((data_t)0.25);
			}
			pData[0] -= (pData[step]) * ((data_t)0.5);
		}

		// calculate odd indices
		{
			data_t *p, *pDataEnd = pData + last;
			for (p = pData + step; p < pDataEnd; p += step2)
			{
				*p += (p[-step] + p[step]) * ((data_t)0.5);
			}
			pData[last] += pDataEnd[-step];
		}
	}
};



};


#endif

