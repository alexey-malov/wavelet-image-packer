// pwc_codec.cpp: implementation of the CPWCCodec class.
//
//////////////////////////////////////////////////////////////////////

#include "../include/pwc_codec.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace brainwavelets
{


CPWCCodec::uint_t CPWCCodec::m_nMagMask;
CPWCCodec::uint_t CPWCCodec::m_nMagVal;
CPWCCodec::uint_t* CPWCCodec::m_pMagPos;
int CPWCCodec::m_nZeroMagnitudeCount;
int CPWCCodec::m_nZeroMagnitudeSize;
int CPWCCodec::m_nMagnitudeSize;

CPWCCodec::uint_t CPWCCodec::m_strmValue;
CPWCCodec::size_t CPWCCodec::m_strmCounter;
CPWCCodec::size_t CPWCCodec::m_nStreamSize;


CPWCCodec::uint_vector_t CPWCCodec::m_Magnitude;
CPWCCodec::uint_vector_t CPWCCodec::m_EncodedData;

const CPWCCodec::uint_t*	CPWCCodec::m_pPackedBitStream = NULL;
const CPWCCodec::uint_t*	CPWCCodec::m_pEndOfPackedBitStream = NULL;

CPWCCodec::CPWCCodec()
{
	m_nMagnitudeSize = 0;
}

CPWCCodec::~CPWCCodec()
{

}

CPWCCodec::size_t CPWCCodec::WriteEncodedSize()
{
	if (m_EncodedData.size() <= 3)
	{
		return 0;
	}

	return (m_EncodedData[1] = (m_EncodedData.size() - 3) * sizeof(uint_t));
}

void CPWCCodec::ResizeMagnitude(size_t nCoeffs)
{
	m_nZeroMagnitudeSize = nCoeffs;
	size_t nMagSize = ((nCoeffs + (uint_bits - 1)) >> uint_shift) + 1;
	if (nMagSize > m_Magnitude.size()) 
	{
		m_Magnitude.resize(nMagSize);
	}

	m_nMagnitudeSize = nMagSize;

	memset(&m_Magnitude[0], 0, nMagSize * sizeof(uint_t));
}

void CPWCCodec::InitPackedStream(uint_t const *pPackedStream, uint_t nStreamSize)
{
	m_pPackedBitStream = pPackedStream;
	m_pEndOfPackedBitStream = (const uint_t *)((size_t)pPackedStream + nStreamSize);
	m_strmCounter = 0;
}


};
