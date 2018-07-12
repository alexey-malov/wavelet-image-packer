// pwc_codec.h: interface for the CPWCCodec class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PWC_CODEC_H__5AADD42B_0B48_4418_BECB_F80E97DB7BD7__INCLUDED_)
#define AFX_PWC_CODEC_H__5AADD42B_0B48_4418_BECB_F80E97DB7BD7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WaveLet.h"
#include <vector>
#include <iostream>

//#define BRAINWAVELETS_USE_FAST_READBITS

/*
	BlockHeader:
		[nEncodedCoefficients]	- uint_t: number of coefficients
		[encodedSize]			- uint_t: encoded size (in bytes)
		[numberOfBitplanes]		- uint_t: number of encoded bitplanes
		[packedData]

 */

namespace brainwavelets
{


class CPWCCodec  
{
public:
	// typedefs
	//typedef unsigned char	byte_t;
	//typedef vector<byte_t>	byte_vector_t;
	typedef unsigned int	uint_t;
	typedef unsigned int	size_t;
	typedef std::vector<uint_t>	uint_vector_t;
private:
	enum 
	{
		uint_bits = 32,	// number of bits in uint_t
		uint_shift = 5,
		uint_high = 1 << (uint_bits - 1)
	};

public:
	static const void* GetPackedData()
	{
		return &m_EncodedData[0];
	}

	static size_t GetPackedDataSize()
	{
		if (m_EncodedData.size() >= GetHeaderSize()) 
		{
			return GetHeaderSize() + m_EncodedData[2];
		}
	}

	template <class T>
	static size_t EncodeMacroblock(const CWavelet2D<T>::blk_desc_t &macroblock)
	{
		return Encode(&macroblock[0], macroblock.GetCoefficientCount(), macroblock.GetBitplaneCount());
	}

	template <class data_t>
	static size_t Decode(const void* pPackedData, data_t *pOutputCoeffs)
	{
		const uint_t* pData = (const uint_t*)pPackedData;
		size_t nCoeffs = pData[0];
		size_t nPackedSize = pData[1];
		size_t nBitplanes = pData[2];
		
		// Clear memory
		memset(pOutputCoeffs, 0, sizeof(data_t) * nCoeffs);

		if (!nBitplanes || !nCoeffs) 
		{
			return nCoeffs;
		}

		ResizeMagnitude(nCoeffs);

		data_t nLayerMask = 1 << (nBitplanes - 1);
		
		data_t * const pEndOfOutput = pOutputCoeffs + nCoeffs;

		InitPackedStream(&pData[3], nPackedSize);

		while (nLayerMask) 
		{
			DepackBitplane(pOutputCoeffs, pEndOfOutput, nLayerMask);
			nLayerMask >>= 1;
		}

		return nCoeffs;
	}

	template <class data_t>
	static size_t Encode(const data_t *pInputCoeffs, size_t nCoeffs, size_t nBitplanes)
	{
		m_EncodedData.clear();
		m_EncodedData.push_back(nCoeffs);		// number of Coefficients
		m_EncodedData.push_back(0);				// packed size
		m_EncodedData.push_back(nBitplanes);	// number of bit planes

		if (!nBitplanes || !nCoeffs) 
		{
			return WriteEncodedSize() + GetHeaderSize();
		}

		ResizeMagnitude(nCoeffs);

		data_t nLayerMask = 1 << (nBitplanes - 1);
		
		const data_t *pEndOfInput = pInputCoeffs + nCoeffs - 1;

		InitStream();

		while (nLayerMask)
		{
			PackBitplane(pInputCoeffs, pEndOfInput, nLayerMask);
			nLayerMask >>= 1;
		}
		Flush();
		return WriteEncodedSize() + GetHeaderSize();
	}

	// returns header size (in bytes)
	static inline size_t GetHeaderSize()
	{
		if (m_EncodedData.size() < 3)
		{
			return 0;
		}
		return 3 * sizeof(uint_t);
	}


public:
	CPWCCodec();
	virtual ~CPWCCodec();
private:
	static void ResizeMagnitude(size_t nCoeffs);
	static size_t WriteEncodedSize();

	static inline void InitStream()
	{
		m_strmValue = 0;
		m_strmCounter = 0;
	}

	static inline void Write0()
	{
		WriteBit(0);
	}

	static inline void WriteBit(uint_t nValue)
	{
		m_strmValue <<= 1;
		if (nValue) 
		{
			m_strmValue++;
		}

		if ((++m_strmCounter) == uint_bits) 
		{
			m_EncodedData.push_back(m_strmValue);
			m_strmCounter = 0;
		}
	}

#if 1
	static inline void WriteBits(uint_t nValue, size_t nBits)
	{
		nValue <<= uint_bits - nBits;
		while (nBits--) 
		{
			WriteBit(nValue & uint_high);
			nValue <<= 1;
		}
	}
#else
	static inline void WriteBits(uint_t nValue, size_t nBits)
	{
		nValue <<= uint_bits - nBits;
		while (true) 
		{
			size_t freeBits = uint_bits - m_strmCounter;
			if (nBits < freeBits) 
			{
				// we can write all nBits
				m_strmValue = (m_strmValue << nBits) | (nValue >> (uint_bits - nBits));
				m_strmCounter += nBits;
				return;
			}
			else
			{
				// we can write only freeBits
				m_EncodedData.push_back((m_strmValue << freeBits) | (nValue >> (uint_bits - freeBits)));
				nValue <<= freeBits;
				m_strmCounter = 0;
				nBits -= freeBits;
				if (!nBits)
				{
					return; // we wrote all bits
				}
			}
		}
	}
#endif

	static inline void InitMagnitudePointer()
	{
		m_nZeroMagnitudeCount = m_nZeroMagnitudeSize;
		m_pMagPos = &m_Magnitude[0];
		m_nMagMask = uint_high;
		m_nMagVal = *m_pMagPos;
	}

	static inline uint_t NextMagnitudeBit()
	{
		if (!m_nMagMask) 
		{
			m_nMagMask = uint_high;
			m_nMagVal = *(++m_pMagPos);
		}
		uint_t result = m_nMagVal & m_nMagMask;
		if (!result) 
		{
			m_nZeroMagnitudeCount--;
		}
		m_nMagMask >>= 1;
		return result;
	}

	// Check Magnitude bit
	// if it is 0: update it with newBit value and return false
	// if it is 1: return true
	static inline uint_t UpdateNextMagnitudeBit(uint_t newBit)
	{
		if (!m_nMagMask) 
		{
			m_nMagMask = uint_high;
			*(m_pMagPos++) = m_nMagVal;
			m_nMagVal = *m_pMagPos;
		}

		uint_t result = m_nMagVal & m_nMagMask;
		if (!result) 
		{
			m_nZeroMagnitudeCount--;
			BRAINWAVELETS_ASSERT(m_nZeroMagnitudeCount >= 0);
		}

		if (newBit && (!result)) 
		{
			m_nMagVal |= m_nMagMask;
			m_nZeroMagnitudeSize--;
			BRAINWAVELETS_ASSERT(m_nZeroMagnitudeSize >= 0);
		}


		m_nMagMask >>= 1;
		return result;
	}


	static inline size_t GetBitCount(uint_t value)
	{
		size_t nBitCount = 0;
		while (value)
		{
			value >>= 1;
			nBitCount++;

		}
		return nBitCount;
		
	}

	static inline uint_t ReadBit()
	{
		if (!m_strmCounter) 
		{
			m_strmValue = *(m_pPackedBitStream++);
			m_strmCounter = uint_bits;
		}
		uint_t result = m_strmValue & uint_high;
		m_strmValue <<= 1;
		m_strmCounter--;
		return result;
	}

#ifndef BRAINWAVELETS_USE_FAST_READBITS
	static inline uint_t ReadBits(size_t nBits)
	{
		BRAINWAVELETS_ASSERT((nBits >= 1) && (nBits <= 32));

		uint_t result = 0;
		do
		{
			result <<= 1;
			if (ReadBit()) 
			{
				result++;
			}
		}while(--nBits);
		return result;
	}

#else

	static inline uint_t ReadBits(size_t nBits)
	{
		BRAINWAVELETS_ASSERT((nBits >= 1) && (nBits <= uint_bits));

		uint_t result = 0;
		while(true)
		{
			if (nBits <= m_strmCounter) 
			{
				// we have enough bits
				result = (result << nBits) | (m_strmValue >> (uint_bits - nBits));
				m_strmValue <<= nBits;
				m_strmCounter -= nBits;
				return result;
			}
			else
			{
				// we have not enough bits
				if (!m_strmCounter) 
				{
					m_strmValue = *(m_pPackedBitStream++);
					m_strmCounter = uint_bits;
					continue;
				}

				// we can read only m_strmCounter bits
				result = (result << m_strmCounter) | (m_strmValue >> (uint_bits - m_strmCounter));
				nBits -= m_strmCounter;

				m_strmValue = *(m_pPackedBitStream++);
				m_strmCounter = uint_bits;
				continue;
			}

		}
	}

#endif

	template <class data_t>
	static void DepackBitplane(data_t * const pOutputCoeffs, data_t * const pEndOfOutput, data_t nLayerMask)
	{

		data_t *pOutput = pOutputCoeffs;
		size_t numBits = ReadBits(5);
		/*
		 *	Read RLR encoded data from stream
		 */
		{
			InitMagnitudePointer();

			size_t expNumBits = 1 << numBits;
			int remainder = 0;

			enum 
			{
				data_t_high = 1 << (sizeof(data_t) * 8 - 1)
			};

			while(m_nZeroMagnitudeCount)
			{
				if (ReadBit()) 
				{
					size_t desc = ReadBits(numBits + 1);
					size_t numZeros = desc >> 1;

					if (numBits > 1) 
					{
						numBits--;
						expNumBits >>= 1;
					}
					
					// write numZeros zeros
					while (numZeros) 
					{
						if (!NextMagnitudeBit()) 
						{
							numZeros--;
						}
						pOutput++;
					}


					while (m_nZeroMagnitudeCount)
					{
						if (!NextMagnitudeBit()) 
						{
							BRAINWAVELETS_ASSERT(pOutput < pEndOfOutput);
							if (desc & 0x01) 
							{
								*pOutput |= data_t_high | nLayerMask;
							}
							else
							{
								*pOutput |= nLayerMask;
							}
							pOutput++;
							break;
						}
						pOutput++;
					}
				}
				else
				{
					// write expNumBits zero
					size_t numZeros = expNumBits;
					while (numZeros) 
					{
						if (!NextMagnitudeBit()) 
						{
							numZeros--;
						}
						pOutput++;
					}
					numBits++;
					expNumBits <<= 1;

				}
				
			}
		}

		/*
		 *	Write unpacked bit stream
		 */
		{
			pOutput = pOutputCoeffs;
			InitMagnitudePointer();
			while (pOutput < pEndOfOutput)
			{
				
				if (UpdateNextMagnitudeBit(*pOutput & nLayerMask)) 
				{
					if (ReadBit()) 
					{
						*pOutput |= nLayerMask;
					}
				}
				pOutput++;
			}
		}

		FlushMagnitude();
	}

	template <class data_t>
	static void PackBitplane(const data_t * const pInputCoeffs, const data_t * const pEndOfInput, data_t nLayerMask)
	{
		// calculate initial bit count
		InitMagnitudePointer();

		const data_t *pData = pInputCoeffs;
		uint_t nZeroCounter = 0;

		size_t numBits;

		enum 
		{
			data_t_high = 1 << (sizeof(data_t) * 8 - 1)
		};


		// count number of zeros in current bitplane having magnitude = 0
		do
		{
			uint_t nMagBit = NextMagnitudeBit();
			if (!(nMagBit || (*pData & nLayerMask)))
			{
				nZeroCounter++;
			}
		}while ((pData++) < pEndOfInput);

		numBits = GetBitCount(nZeroCounter);

		if (!numBits) 
		{
			numBits = 1;
		}

		WriteBits(numBits, uint_shift);

		//////////////////////////////////////////////////////////////////////////
		// write bit stream
		//////////////////////////////////////////////////////////////////////////


		/*
		 *	write RLR encoded data to stream
		 */
		{
			size_t expNumBits = 1 << numBits;
			nZeroCounter = 0;

			InitMagnitudePointer();
			pData = pInputCoeffs;
			do 
			{
				if (!NextMagnitudeBit()) 
				{
					// we are packing only bits with 0th magnitude
					if (*pData & nLayerMask) 
					{
						// found a 1
						uint_t value = (1 << (numBits + 1)) | (nZeroCounter << 1);
						if (*pData & data_t_high) 
						{
							value++;
						}
						
						WriteBits(value, numBits + 2);
						if ((numBits > 1))
						{
							numBits--;
							expNumBits >>= 1;
						}
						
						nZeroCounter = 0;
					}
					else
					{
						// found a 0
						if ((++nZeroCounter) == expNumBits)
						{
							expNumBits <<= 1;
							nZeroCounter = 0;
							numBits++;
							Write0();
						}
					}

				}
			} while((pData++) < pEndOfInput);
			
			if (nZeroCounter) 
			{
				uint_t value = (1 << (numBits + 1)) | (nZeroCounter << 1);
				WriteBits(value, numBits + 2);
			}
		}

		/*
		 *	Write plain data to bitstream (also modify magnitude vector)
		 */
		{
			InitMagnitudePointer();

			pData = pInputCoeffs;

			do 
			{
				uint_t bitValue = (*pData & nLayerMask) != 0;
				if (UpdateNextMagnitudeBit(bitValue))
				{
					// copy magnitude data
					WriteBit(bitValue);
				}
			}while((pData++) < pEndOfInput);
			
		}

		FlushMagnitude();
		
	}

	static void FlushMagnitude()
	{
		*m_pMagPos = m_nMagVal;
	}

	static inline void Flush()
	{
		while (m_strmCounter) 
		{
			WriteBit(0);
		}
	}

	static void InitPackedStream(uint_t const *pPackedStream, size_t nStreamSize);

private:

	static uint_vector_t	m_EncodedData;
	static uint_vector_t	m_Magnitude;

	// bitstream variables
	static uint_t	m_strmValue;			// bitstream value
	static size_t	m_strmCounter;			// bitstream bit counter
	static const uint_t*	m_pPackedBitStream;
	static const uint_t*	m_pEndOfPackedBitStream;

	static size_t	m_nStreamSize;				// stream size
	
	// magnitude variables
	static uint_t	*m_pMagPos;	// magnitude address
	static uint_t	m_nMagMask;	// magnitude mask
	static uint_t	m_nMagVal;	// magnitude value
	static int		m_nZeroMagnitudeCount;
	static int		m_nZeroMagnitudeSize;
	static int		m_nMagnitudeSize;
};



};

#endif // !defined(AFX_PWC_CODEC_H__5AADD42B_0B48_4418_BECB_F80E97DB7BD7__INCLUDED_)
