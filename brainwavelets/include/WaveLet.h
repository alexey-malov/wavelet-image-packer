// WaveLet.h: interface for the CWaveLet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WAVELET_H__FCF58915_C3ED_43A8_8996_45C78020A734__INCLUDED_)
#define AFX_WAVELET_H__FCF58915_C3ED_43A8_8996_45C78020A734__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "defines.h"
#include <vector>

//#define BRAINWAVELETS_USE_FAST_LIFTING 
#define BRAINWAVELETS_FAST_WAVELET_RESIZE
#define BRAINWAVELETS_FAST_TRANSPOSE_LIFTING

namespace brainwavelets
{


template <class T>
class CLifting
{
public:
	typedef T data_t;
	typedef unsigned int size_t;
	enum extension_t
	{
		CYCLIC_EXT,
		SYMMETRIC_EXT,
		SYMMETRIC_EXT2,
	};
public:
	// Construction / destruction

	CLifting():m_Extension(SYMMETRIC_EXT){}
	virtual ~CLifting(){}

	// Attributes

	inline void SetExtension(extension_t Extend)
	{
		m_Extension = Extend;
	}
	inline extension_t GetExtension()const
	{
		return m_Extension;
	}

	// Methods
	// lifting
	virtual bool Lift(data_t *pData, size_t numCoeff, size_t nDataStep, bool bInverse) const = 0;

	//virtual void InverseLiftEvenLoop(data_t *pData, size_t count, size_t nDataStep) const = 0;

//	// lifting with nDataStep = 1
	virtual bool LiftTranspose(data_t *pData, size_t numCoeff, bool bInverse) const = 0;
	
	static inline bool IsPowerOfTwo(size_t n)
	{
		return (n & (n - 1)) == 0;
	}
private:
	extension_t m_Extension;
};

template <class T>
class CLinearLifting : public CLifting<T>
{
public:
	virtual bool Lift(data_t *pData, size_t numCoeff, size_t nDataStep, bool bInverse) const;
	virtual bool LiftTranspose(data_t *pData, size_t numCoeff, bool bInverse) const;
};

/*
 *	Wavelet class
 */
template <class T>
class CWavelet2D
{
public:
	typedef CLifting<T>					lifting_t;		// lifting algorithm type
	typedef lifting_t::data_t			data_t;			// data type
	typedef lifting_t::size_t			size_t;			// size type
	typedef unsigned int				uint_t;			// int type
	typedef std::vector<data_t>			data_vector_t;	// data vector type
	typedef std::vector<uint_t>			uint_vector_t;	// uint vector type

	class blk_desc_t
	{
	public:
		blk_desc_t()
			:m_Quantizer((data_t)1)
			,m_nBitplanes(0)
			,m_nSize(0)
			,m_nCoeffsUsed(0)
		{
		}

		void Assign(const blk_desc_t& other)
		{
			m_Quantizer =			other.m_Quantizer;
			m_nBitplanes =			other.m_nBitplanes;
			m_nSize =				other.m_nSize;
			m_nCoeffsUsed =			other.m_nCoeffsUsed;
			m_QuantizedCoeffs =		other.m_QuantizedCoeffs;
		}

		blk_desc_t& operator=(const blk_desc_t& other)
		{
			Assign(other);
			return *this;
		}

		blk_desc_t(const blk_desc_t& other)
		{
			Assign(other);
		}

		// Set Quantizer
		inline void SetQuantizer(data_t quantizer)
		{
			m_Quantizer = quantizer;
		}

		// Get Quantizer
		inline data_t GetQuantizer()const
		{
			return m_Quantizer;
		}

		// Set Bitplane Count
		inline void SetBitplaneCount(size_t numBitplanes)
		{
			m_nBitplanes = numBitplanes;
		}

		// Get bitplane count
		inline size_t GetBitplaneCount()const
		{
			return m_nBitplanes;
		}

		// get macroblock size
		inline size_t GetSize()const
		{
			return m_nSize;
		}

		// return an element of macroblock
		inline uint_t& operator[](size_t index)
		{
			return m_QuantizedCoeffs[index];
		}

		// return an element of macroblock
		inline const uint_t& operator[](size_t index)const
		{
			return m_QuantizedCoeffs[index];
		}

		// returns number of coefficeint
		inline size_t GetCoefficientCount()const
		{
			return m_nCoeffsUsed;
		}

		static size_t GetQuantizedCoefficientsCountBySize(size_t sz)
		{
			if (sz == 0) 
			{
				return 0;
			}
			if (sz == 1) 
			{
				return 1;
			}
			return (sz * sz * 3) >> 2;
		}

		// Resizes block
		bool Resize(size_t sz)
		{
			m_nSize = sz;
			if (sz == 0) 
			{
				return false;
			}
			ResizeCoefficients(GetQuantizedCoefficientsCountBySize(sz));
			return true;
		}

	private:
		void ResizeCoefficients(size_t newSize)
		{
			if (newSize > m_QuantizedCoeffs.size()) 
			{
				m_QuantizedCoeffs.resize(newSize);
			}
			m_nCoeffsUsed = newSize;
		}

		data_t			m_Quantizer;		// quantizer value
		size_t			m_nBitplanes;		// number of bitplanes
		size_t			m_nSize;			// size of block
		uint_vector_t	m_QuantizedCoeffs;	// quantized coefficients
		size_t			m_nCoeffsUsed;		// number of used quantized coeffs
	};

	typedef std::vector<blk_desc_t>	desc_vector_t;


	// Constructor
	CWavelet2D(size_t nSize, lifting_t* pLifting)
		:m_pLifting(pLifting)
		,m_nSize(nSize)
		,m_WaveletCoeffs(nSize * nSize)
		,m_nUsedMacroblocks(0)
		,m_nCoeffsCount(nSize * nSize)
	{
	}

	CWavelet2D()
		:m_pLifting(NULL)
		,m_nSize(0)
		,m_WaveletCoeffs(0)
		,m_nUsedMacroblocks(0)
		,m_nCoeffsCount(0)
	{
	}

	// Removes all macroblocks from wavelet
	inline void RemoveAllMacroblocs()
	{
		m_nUsedMacroblocks = 0;
	}

	// destructor
	virtual ~CWavelet2D()
	{
	}


	// Change lifting algorithm
	inline void SetLifting(lifting_t* pLifting)
	{
		m_pLifting = pLifting;
	}

	// Get lifting algorithm
	inline lifting_t* GetLifting()const
	{
		return m_pLifting;
	}

	// return number of coefficients for wavelet
	inline size_t GetCoefficientsCount()
	{
		return m_nCoeffsCount;
	}

	/*
	 *	returns an wavelet coefficient (index must be >= 0 and < GetCoefficientsCount())
	 */
	inline data_t& operator[](size_t index)
	{
		BRAINWAVELETS_ASSERT(index < GetCoefficientsCount());
		return m_WaveletCoeffs[index];
	}

	// Get wavelet size
	inline size_t GetSize()const
	{
		return m_nSize;
	}

	// Resize wavelet
	bool Resize(size_t newSize)
	{
		m_nSize = newSize;
#ifdef BRAINWAVELETS_FAST_WAVELET_RESIZE
		ResizeCoefficients(newSize * newSize);
#else
		m_WaveletCoeffs.resize(newSize * newSize);
#endif
		return true;
	}

	/*
	 *	Quantize and split wavelet
	 *		return macroblock index
	 */
	int QuantizeAndSplit(data_t quantizer);

	// dequantize wavelet and join
	bool DequantizeAndJoin(size_t nMacroblockIndex);

	inline void Load(const data_t *pSource)
	{
		memcpy(&m_WaveletCoeffs[0], pSource, m_nSize * m_nSize * sizeof(data_t));
	}

	inline void Load(const data_t *pSource, size_t sourceWidth)
	{
		for (size_t y = 0; y < m_nSize; ++y)
		{
			memcpy(&m_WaveletCoeffs[0] + (m_nSize * y), pSource + (sourceWidth * y), m_nSize * sizeof(data_t));
		}
		
	}

	inline void Save(data_t *pDestination)
	{
		memcpy(pDestination, &m_WaveletCoeffs[0], m_nSize * m_nSize * sizeof(data_t));
	}

	inline void Save(data_t *pDestination, size_t destinationWidth)
	{
		for (size_t y = 0; y < m_nSize; ++y)
		{
			memcpy(pDestination + (destinationWidth * y), &m_WaveletCoeffs[0] + (m_nSize * y), m_nSize * sizeof(data_t));
		}
	}

	// Direct Wavelet transform
	bool Transform();

	// Inverse Wavelet Transform
	bool InverseTransform();

	/*
	 *	performs multiple quantizations
	 *	returns number of passes
	 */
	size_t TransformAndQuantizeMultipass(data_t quantizer, size_t numPasses = 0)
	{
		size_t cnt = 0;
		if (numPasses == 0) 
		{
			while (m_nSize) 
			{
				Transform();
				QuantizeAndSplit(quantizer);
				quantizer += quantizer;
				cnt++;
			}
		}
		else
		{
			while (m_nSize && numPasses)
			{
				Transform();
				QuantizeAndSplit(quantizer);
				quantizer += quantizer;
				numPasses--;
				cnt++;
			}

		}
		return cnt;
	}

	size_t InverseTransformAndDequantizeMultipass(size_t startMacroblock, int numPasses)
	{
		size_t cnt = 0;
		if (numPasses > 0) 
		{
			while (numPasses && (startMacroblock < m_nUsedMacroblocks))
			{
				DequantizeAndJoin(startMacroblock++);
				InverseTransform();
				cnt++;
				numPasses++;
			}
		}
		else
		{
			while (numPasses && (startMacroblock != (size_t)-1))
			{
				DequantizeAndJoin(startMacroblock--);
				InverseTransform();
				cnt++;
				numPasses--;
			}
		}
		return cnt;
	}

	// Get macroblock by it's index
	inline blk_desc_t& GetMacroblock(size_t nIndex)
	{
		BRAINWAVELETS_ASSERT(nIndex < m_nUsedMacroblocks);
		return m_Macroblocks[nIndex];
	}

	// Get macroblock count
	inline size_t GetMacroblockCount()const
	{
		return m_nUsedMacroblocks;
	}

	blk_desc_t& AllocateMacroblock(data_t quantizer, size_t blockSize);

private:
	// Wavelet transform
	bool Transform(bool bInverse);

	static inline size_t GetBitCount(uint_t value);
	static inline void QuantizeValue(const data_t* pInitialValue, uint_t *pQuantizedValue, uint_t &curMaxValue, data_t Scale);
	static inline void DequantizeValue(const uint_t *pQuantizedValue, data_t* pRestoredValue, data_t Scale);

	void ResizeCoefficients(size_t coeffsCount);

	lifting_t		*m_pLifting;				// lifting algorithm pointer
	data_vector_t	m_WaveletCoeffs;			// wavelet coeffitients
	size_t			m_nSize;					// wavelet size
	size_t			m_nCoeffsCount;				// number of coefficients

	size_t			m_nUsedMacroblocks;			// number of used macroblocks
	desc_vector_t	m_Macroblocks;				// macroblocks array
};



//////////////////////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////////////////////

template<class T>
void CWavelet2D<T>::ResizeCoefficients(CWavelet2D<T>::size_t coeffsCount)
{
	if (coeffsCount > m_WaveletCoeffs.size()) 
	{
		m_WaveletCoeffs.resize(coeffsCount);
	}
	m_nCoeffsCount = coeffsCount;
}

template<class T>
bool CWavelet2D<T>::Transform()
{
	return Transform(false);
}

template<class T>
bool CWavelet2D<T>::InverseTransform()
{
	return Transform(true);
}


template <class T>
void CWavelet2D<T>::DequantizeValue(const CWavelet2D<T>::uint_t *pQuantizedValue, CWavelet2D<T>::data_t* pRestoredValue, CWavelet2D<T>::data_t Scale)
{
	int sample = *pQuantizedValue;
	if (sample < 0) 
	{
		*pRestoredValue = (-(sample & 0x7fffffff)) *  Scale;
	}
	else
	{
		*pRestoredValue = sample * Scale;
	}
}


template<class T>
bool CWavelet2D<T>::DequantizeAndJoin(CWavelet2D<T>::size_t nMacroblockIndex)
{
	BRAINWAVELETS_ASSERT(nMacroblockIndex < m_nUsedMacroblocks);
	if (nMacroblockIndex >= m_nUsedMacroblocks) 
	{
		return false;	// wrong macroblock index
	}

	blk_desc_t& desc = m_Macroblocks[nMacroblockIndex];
	size_t newSize = desc.GetSize();
	BRAINWAVELETS_ASSERT((newSize >> 1) == m_nSize);
	if ((newSize >> 1) != m_nSize) 
	{
		return false;	// wrong size
	}

	size_t oldSize = m_nSize;

	Resize(newSize);

	data_t quantizer = (data_t)1.0 / desc.GetQuantizer();

	if (newSize == 1) 
	{
		DequantizeValue(&desc[0], &m_WaveletCoeffs[0], quantizer);
		return true;
	}


	// unpack ll
	{
		const data_t *pSrc = &m_WaveletCoeffs[oldSize * oldSize - 1];
		data_t *pDestStart = &m_WaveletCoeffs[m_nSize * (m_nSize - 1) - 2];
		data_t *pDestEnd = &m_WaveletCoeffs[0];

		for (data_t *pDest = pDestStart; pDest > pDestEnd; pDest -= (m_nSize << 1)) 
		{
			data_t *pDestLineEnd = pDest - m_nSize;
			for (data_t *p = pDest; p > pDestLineEnd; p -= 2) 
			{
				*p = *(pSrc--);
			}
		}
	}

	
	{
		//uint_vector_t &quantizedCoeffs = desc.m_QuantizedCoeffs;

		const uint_t* pSrc = &desc[0];
		size_t nSize = m_nSize;
		// go lh and hl
		{
			size_t stp = nSize - 1;

			// setup lh base
			data_t* plhBase = &m_WaveletCoeffs[1];
			data_t* plhBaseEnd = &m_WaveletCoeffs[nSize];

			// loop by columns
			for (data_t* pBase = plhBase; pBase < plhBaseEnd; pBase += 2) 
			{
				data_t* pBaseEnd = pBase + nSize * nSize;
				// loop by rows
				for (data_t* p = pBase; p < pBaseEnd; p += (nSize * 2))
				{
					DequantizeValue(pSrc, p, quantizer);
					DequantizeValue(pSrc + 1, p + stp, quantizer);
					pSrc += 2;
				}
			}
		}

		// go hh
		{
			data_t* phhBase = &m_WaveletCoeffs[nSize + 1];
			data_t* phhBaseEnd = &m_WaveletCoeffs[nSize * 2 + 1];
			// loop by columns
			for (data_t* pBase = phhBase; pBase < phhBaseEnd; pBase += 2) 
			{
				data_t* pBaseEnd = pBase + nSize * nSize;
				// loop by rows
				for (data_t* p = pBase; p < pBaseEnd; p += (nSize * 2))
				{
					DequantizeValue(pSrc, p, quantizer);
					pSrc++;
				}
			}
		}
	}

	return true;
}


template<class T>
CWavelet2D<T>::size_t CWavelet2D<T>::GetBitCount(CWavelet2D<T>::uint_t value)
{
	size_t nBitCount = 0;
	while (value)
	{
		value >>= 1;
		nBitCount++;
	}
	return nBitCount;
}


template<class T>
CWavelet2D<T>::blk_desc_t& CWavelet2D<T>::AllocateMacroblock(CWavelet2D<T>::data_t quantizer, CWavelet2D<T>::size_t blockSize)
{
	if (m_nUsedMacroblocks >= m_Macroblocks.size()) 
	{
		m_Macroblocks.resize(m_nUsedMacroblocks + 1);
	}

	m_Macroblocks[m_nUsedMacroblocks].Resize(blockSize);

	m_Macroblocks[m_nUsedMacroblocks].SetQuantizer(quantizer);
	m_Macroblocks[m_nUsedMacroblocks].SetBitplaneCount(0);
	return m_Macroblocks[m_nUsedMacroblocks++];
}

template<class T>
void CWavelet2D<T>::QuantizeValue(const CWavelet2D<T>::data_t* pInitialValue, CWavelet2D<T>::uint_t *pQuantizedValue, CWavelet2D<T>::uint_t &curMaxValue, data_t Scale)
{
	if (*pInitialValue < 0) 
	{
		uint_t sample = -(int)(*pInitialValue * Scale - 0.5);
		if (sample > curMaxValue) 
		{
			curMaxValue = sample;
		}
		if (sample) 
		{
			*pQuantizedValue = sample | 0x80000000;
		}
		else
		{
			*pQuantizedValue = sample;
		}
	}
	else
	{
		*pQuantizedValue = (int)(*pInitialValue * Scale + 0.5);
		if (*pQuantizedValue > curMaxValue) 
		{
			curMaxValue = *pQuantizedValue;
		}
	}
}




template<class T>
int CWavelet2D<T>::QuantizeAndSplit(CWavelet2D<T>::data_t quantizer)
{
	if (m_nSize == 0) 
	{
		return -1;
	}

	if (m_nSize == 1) 
	{
		blk_desc_t &desc = AllocateMacroblock(quantizer, 1);
		uint_t curMax = 0;
		QuantizeValue(&m_WaveletCoeffs[0], &desc[0], curMax, quantizer);
		desc.SetBitplaneCount(GetBitCount(curMax));
		Resize(0);
		return m_nUsedMacroblocks - 1;
	}

	blk_desc_t &desc = AllocateMacroblock(quantizer, m_nSize);

	// copy and quantize coefficients

	uint_t nAbsMax = 0;
	uint_t* pDest = &desc[0];
	size_t nSize = m_nSize;
	// go lh and hl
	{
		size_t stp = nSize - 1;

		// setup lh base
		const data_t* plhBase = &m_WaveletCoeffs[1];
		const data_t* plhBaseEnd = &m_WaveletCoeffs[nSize];
		const data_t* pBuffEnd = &m_WaveletCoeffs[nSize * nSize - 1];

		// loop by columns
		for (const data_t* pBase = plhBase; pBase < plhBaseEnd; pBase += 2) 
		{
			//const data_t* pBaseEnd = pBase + nSize * nSize;
			const data_t* pBaseEnd = pBuffEnd;	
			// loop by rows
			for (const data_t* p = pBase; p <= pBaseEnd; p += (nSize * 2))
			{
				QuantizeValue(p, pDest, nAbsMax, quantizer);
				QuantizeValue(p + stp, pDest + 1, nAbsMax, quantizer);
				pDest += 2;
			}
		}
	}

	// go hh
	{
		const data_t* phhBase = &m_WaveletCoeffs[nSize + 1];
		const data_t* phhBaseEnd = &m_WaveletCoeffs[nSize * 2 + 1];
		// loop by columns
		for (const data_t* pBase = phhBase; pBase < phhBaseEnd; pBase += 2) 
		{
			const data_t* pBaseEnd = pBase + nSize * nSize;
			// loop by rows
			for (const data_t* p = pBase; p < pBaseEnd; p += (nSize * 2))
			{
				QuantizeValue(p, pDest, nAbsMax, quantizer);
				pDest++;
			}
		}
	}

	desc.SetBitplaneCount(GetBitCount(nAbsMax));

	// pack ll
	{
		const data_t* const pllBase = &m_WaveletCoeffs[0];
		const data_t* const pllBaseEnd = &m_WaveletCoeffs[nSize * nSize];
		data_t* pPacked = &m_WaveletCoeffs[0];
		// loopy by rows
		for (const data_t* pBase = pllBase; pBase < pllBaseEnd; pBase += (nSize * 2))
		{
			const data_t* pBaseEnd = pBase + nSize;
			for (const data_t* p = pBase; p < pBaseEnd; p += 2) 
			{
				*(pPacked++) = *p;
			}
		}
	}

	// resize wavelet
	Resize(m_nSize>>=1);

	return m_nUsedMacroblocks - 1;
}


template<class T>
bool CLinearLifting<T>::Lift(
	CLifting<T>::data_t *pData, 
	CLifting<T>::size_t numCoeff, 
	CLifting<T>::size_t nDataStep, 
	bool bInverse) const
{
	if (numCoeff == 0) 
	{
		return false;
	}

	if (numCoeff == 1) 
	{
		return true;
	}

	if (!IsPowerOfTwo(numCoeff))
	{
		return false;
	}

	size_t size = numCoeff * nDataStep;
	size_t dataStep2 = (nDataStep << 1);
	size_t cnt = size - nDataStep;
	if (!bInverse) 
	{
		/*
		 *	pData[2*i + 1] -= 0.5 * (pData[2*i] + pData[2*i + 2])
		 */
		data_t *pDataEnd = pData + cnt;
		data_t* p;
		//data_t data_1 = pData[0] - (pData[0] + pData[nDataStep]) * (data_t)0.5;
		//data_t datacnt = pData[cnt];
		// calculate odd indices
		for (p = pData + nDataStep; p < pDataEnd; p += dataStep2)
		{
			*p -= (p[-(int)nDataStep] + p[nDataStep]) * ((data_t)0.5);
		}
		switch(GetExtension()) 
		{
		case CYCLIC_EXT:
			pData[cnt] -= (pDataEnd[-(int)nDataStep] + pData[0]) * ((data_t)0.5);
			break;
		case SYMMETRIC_EXT:
			pData[cnt] -= *(pDataEnd - nDataStep);
			break;
		case SYMMETRIC_EXT2:
			return false;
			pData[cnt] -= (*(pDataEnd - nDataStep) + pData[cnt]) * ((data_t)0.5);
			break;
		default:
			return false;
		}

		// Calculate even indices
		/*
		 *	pData[2*i] += 0.25 * (pData[2*i - 1] + pData[2*i + 1])
		 */
		pDataEnd = pData + size;
		// calculate even indices
		for (p = pData + dataStep2; p < pDataEnd; p += dataStep2)
		{
			*p += (p[-(int)nDataStep] + p[nDataStep]) * ((data_t)0.25);
		}
		switch(GetExtension()) 
		{
		case CYCLIC_EXT:
			pData[0] += (pData[cnt] + pData[nDataStep]) * ((data_t)0.25);
			break;
		case SYMMETRIC_EXT:
			pData[0] += (pData[nDataStep]) * ((data_t)0.5);
			break;
		case SYMMETRIC_EXT2:
//			pData[0] += (data_1 + pData[nDataStep]) * ((data_t)0.25);
			break;
		default:
			return false;
		}
	}
	else
	{

		// Calculate even indices
		/*
		 *	pData[2*i] += 0.25 * (pData[2*i - 1] + pData[2*i + 1])
		 */
		data_t *pDataEnd = pData + size;
		data_t *p;
		
		// calculate even indices
		for (p = pData + dataStep2; p < pDataEnd; p += dataStep2)
		{
			*p -= (p[-(int)nDataStep] + p[nDataStep]) * ((data_t)0.25);
		}
		
		switch(GetExtension()) 
		{
		case CYCLIC_EXT:
			pData[0] -= (pData[cnt] + pData[nDataStep]) * ((data_t)0.25);
			break;
		case SYMMETRIC_EXT:
			pData[0] -= (pData[nDataStep]) * ((data_t)0.5);
			break;
		case SYMMETRIC_EXT2:
			return false;
			pData[0] -= (pData[0] + pData[nDataStep]) * ((data_t)0.25);
			break;
		default:
			return false;
		}


		/*
		 *	pData[2*i + 1] -= 0.5 * (pData[2*i] + pData[2*i + 2])
		 */
		pDataEnd = pData + cnt;
		// calculate odd indices
		for (p = pData + nDataStep; p < pDataEnd; p += dataStep2)
		{
			*p += (p[-(int)nDataStep] + p[nDataStep]) * ((data_t)0.5);
		}
		switch(GetExtension()) 
		{
		case CYCLIC_EXT:
			pData[cnt] += (pDataEnd[-(int)nDataStep] + pData[0]) * ((data_t)0.5);
			break;
		case SYMMETRIC_EXT:
			pData[cnt] += *(pDataEnd - nDataStep);
			break;
		case SYMMETRIC_EXT2:
			return false;
			pData[cnt] += (*(pDataEnd - nDataStep) + pData[cnt]) * ((data_t)0.5);
			break;
		default:
			return false;
		}


	}

	return true;
}




template <class T>
bool CWavelet2D<T>::Transform(bool bInverse)
{
	size_t numCoeff = GetSize();

	if (numCoeff == 0) 
	{
		return false;
	}
	if (numCoeff == 1) 
	{
		return true;
	}
	if (!lifting_t::IsPowerOfTwo(numCoeff))
	{
		return false;
	}
	if (!m_pLifting) 
	{
		return false;
	}
	data_t *pData = &m_WaveletCoeffs[0];

	if (!bInverse) 
	{
		// lift rows
		for (size_t row = 0; row < numCoeff; row++) 
		{
			m_pLifting->Lift(pData + row * numCoeff, numCoeff, 1, false);
		}
#ifdef BRAINWAVELETS_FAST_TRANSPOSE_LIFTING
		m_pLifting->LiftTranspose(pData, numCoeff, false);
#else
		// lift columns
		for (size_t column = 0; column < numCoeff; column++) 
		{
			m_pLifting->Lift(pData + column, numCoeff, numCoeff, false);
		}
#endif
	}
	else
	{
		// inverse lift columns
		for (size_t column = 0; column < numCoeff; column++) 
		{
			m_pLifting->Lift(pData + column, numCoeff, numCoeff, true);
		}

		// inverse lift rows
		for (size_t row = 0; row < numCoeff; row++) 
		{
			m_pLifting->Lift(pData + row * numCoeff, numCoeff, 1, true);
		}
	}
	return true;
}



template<class T>
bool CLinearLifting<T>::LiftTranspose(
	CLifting<T>::data_t *pData, 
	CLifting<T>::size_t numCoeff, 
	bool bInverse) const
{
	if (numCoeff == 0) 
	{
		return false;
	}

	if (numCoeff == 1) 
	{
		return true;
	}

	if (!IsPowerOfTwo(numCoeff))
	{
		return false;
	}

	if (!bInverse) 
	{
		// direct transform
		//////////////////////////////////////////////////////////////////////////
		// Direct ODD indices
		//////////////////////////////////////////////////////////////////////////
		{
			data_t *pBaseRow;
			data_t *p;
			size_t lastRowOffset = numCoeff * (numCoeff - 1);
			data_t * const pDataEnd = pData + lastRowOffset;
			pBaseRow = pData + numCoeff; 
			if (pBaseRow < pDataEnd) 
			{
				do
				{
					data_t * const pRowEnd = pBaseRow + numCoeff;
					p = pBaseRow;
					do
					{
						*p -= (p[-(int)numCoeff] + p[numCoeff]) * (data_t)0.5;
					}while (++p < pRowEnd);
				}while((pBaseRow += numCoeff << 1) < pDataEnd);
			}

			switch(GetExtension()) 
			{
			case CYCLIC_EXT:
				{
					data_t * const pRowEnd = pDataEnd + numCoeff;
					p = pDataEnd;
					do
					{
						*p -= (p[-(int)numCoeff] + p[-(int)lastRowOffset]) * (data_t)0.5;
					}while (++p < pRowEnd);
					
				}
				//pData[cnt] -= (pDataEnd[-(int)nDataStep] + pData[0]) * ((data_t)0.5);
				break;
			case SYMMETRIC_EXT:
				{
					data_t * const pRowEnd = pDataEnd + numCoeff;
					p = pDataEnd;
					do
					{
						*p -= (p[-(int)numCoeff]);
					}while (++p < pRowEnd);
				}
				//pData[cnt] -= *(pDataEnd - nDataStep);
				break;
			case SYMMETRIC_EXT2:
				return false;
				break;
			default:
				return false;
			}
		}
		//////////////////////////////////////////////////////////////////////////
		// Direct EVEN indices
		//////////////////////////////////////////////////////////////////////////
		//	pData[2*i] += 0.25 * (pData[2*i - 1] + pData[2*i + 1])
		{
			data_t *pBaseRow;
			data_t *p;
			size_t lastRowOffset = numCoeff * (numCoeff - 1);
			data_t * const pDataEnd = pData + lastRowOffset;
			pBaseRow = pData + (numCoeff * 2);
			if (pBaseRow < pDataEnd) 
			{
				do
				{
					data_t * const pRowEnd = pBaseRow + numCoeff;
					p = pBaseRow;
					do
					{
						*p += (p[-(int)numCoeff] + p[numCoeff]) * (data_t)0.25;
					}while (++p < pRowEnd);
				}while((pBaseRow += numCoeff << 1) < pDataEnd);
			}

			switch(GetExtension()) 
			{
			case CYCLIC_EXT:
				{
					data_t * const pRowEnd = pData + numCoeff;
					p = pData;
					do
					{
						*p += (p[numCoeff] + p[lastRowOffset]) * (data_t)0.25;
					}while (++p < pRowEnd);
					
				}
				//pData[0] += (pData[cnt] + pData[nDataStep]) * ((data_t)0.25);
				break;
			case SYMMETRIC_EXT:
				{
					data_t * const pRowEnd = pData + numCoeff;
					p = pData;
					do
					{
						*p += (p[numCoeff])*(data_t)0.5;
					}while (++p < pRowEnd);
				}
				//pData[0] += (pData[nDataStep] + pData[nDataStep]) * ((data_t)0.25);
				break;
			case SYMMETRIC_EXT2:
				return false;
				break;
			default:
				return false;
			}
		}
		

	}
	else
	{
		// inverse transform
		//size_t size
	}

/*
	size_t size = numCoeff * nDataStep;
	size_t dataStep2 = (nDataStep << 1);
	size_t cnt = size - nDataStep;
	{

		// Calculate even indices
		data_t *pDataEnd = pData + size;
		data_t *p;
		// calculate even indices
		for (p = pData + dataStep2; p < pDataEnd; p += dataStep2)
		{
			*p -= (p[-(int)nDataStep] + p[nDataStep]) * ((data_t)0.25);
		}
		switch(GetExtension()) 
		{
		case CYCLIC_EXT:
			pData[0] -= (pData[cnt] + pData[nDataStep]) * ((data_t)0.25);
			break;
		case SYMMETRIC_EXT:
			pData[0] -= (pData[nDataStep]) * ((data_t)0.5);
			break;
		case SYMMETRIC_EXT2:
			return false;
			pData[0] -= (pData[0] + pData[nDataStep]) * ((data_t)0.25);
			break;
		default:
			return false;
		}


		pDataEnd = pData + cnt;
		// calculate odd indices
		for (p = pData + nDataStep; p < pDataEnd; p += dataStep2)
		{
			*p += (p[-(int)nDataStep] + p[nDataStep]) * ((data_t)0.5);
		}
		switch(GetExtension()) 
		{
		case CYCLIC_EXT:
			pData[cnt] += (pDataEnd[-(int)nDataStep] + pData[0]) * ((data_t)0.5);
			break;
		case SYMMETRIC_EXT:
			pData[cnt] += *(pDataEnd - nDataStep);
			break;
		case SYMMETRIC_EXT2:
			pData[cnt] += (*(pDataEnd - nDataStep) + pData[cnt]) * ((data_t)0.5);
			break;
		default:
			return false;
		}


	}
	*/
	return true;
}








};

#endif // !defined(AFX_WAVELET_H__FCF58915_C3ED_43A8_8996_45C78020A734__INCLUDED_)
