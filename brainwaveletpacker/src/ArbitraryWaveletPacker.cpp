// ArbitraryWaveletPacker.cpp: implementation of the CArbitraryWaveletPacker class.
//
//////////////////////////////////////////////////////////////////////

#include "bwl_includes.h"
#include "ArbitraryWaveletPacker.h"
#include "utils.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace bwlpacker
{


CArbitraryWaveletPacker::CArbitraryWaveletPacker(size_t initialPackBufferSize)
:m_pRectWaveletPacker(IRectangularWaveletPacker::Create(initialPackBufferSize))
,m_PackBuffer(initialPackBufferSize)
{

}

CArbitraryWaveletPacker::~CArbitraryWaveletPacker()
{

}

CArbitraryWaveletPacker* CArbitraryWaveletPacker::Create(size_t initialPackBufferSize)
{
	return new CArbitraryWaveletPacker(initialPackBufferSize);
}


bool CArbitraryWaveletPacker::Pack(const data_t *pInputCoefficients, size_t width, size_t height, data_t quantizer)
{
	if (!width || !height) 
	{
		return false;
	}


	size_t exp_width = ExpandValue(width);
	size_t exp_height = ExpandValue(height);

	const data_t* pData = pInputCoefficients;

	if ((exp_height != height) || (width != exp_width)) 
	{
		// we need to clone
		ResizeExpandedCoefficients(exp_width, exp_height, true);
		pData = GetExpandBuffer();
		CopyBlock(
			pInputCoefficients, 
				width, 
				width, height, 
				0, 0, 
			GetExpandBuffer(), 
				exp_width, 
				0, 0);
	}

	if (!m_pRectWaveletPacker->Pack(pData, exp_width, exp_height, quantizer))
	{
		return false;
	}

	arbitrary_header_type header;

	header.height = height;
	header.width = width;
	header.packedSize = m_pRectWaveletPacker->GetPackedSize();

	RewritePackBuffer();
	WriteToPackBuffer(&header, sizeof(header));
	WriteToPackBuffer(m_pRectWaveletPacker->GetPackedData(), m_pRectWaveletPacker->GetPackedSize());

	return true;
}


void CArbitraryWaveletPacker::ResizeExpandedCoefficients(size_t width, size_t height, bool bClear)
{
	size_t newSize = width * height;
	if (bClear) 
	{
		m_ExpandedCoefficients.assign(newSize, 0);
	}
	else
	{
		m_ExpandedCoefficients.resize(newSize);
	}
}

size_t CArbitraryWaveletPacker::ExpandValue(size_t value)
{
	if (value & (value - 1)) 
	{
		while (value & (value + 1)) 
		{
			value |= value >> 1;
		}
		value++;
	}
	return value;

}

void CArbitraryWaveletPacker::RewritePackBuffer()
{
	m_PackBuffer.Rewrite();
}

size_t CArbitraryWaveletPacker::WriteToPackBuffer(const void *pSrcData, size_t bytesToWrite)
{
	return m_PackBuffer.Write(pSrcData, bytesToWrite);
}

bool CArbitraryWaveletPacker::Depack(const void *pPackedData, data_t *pOutput)
{
	OpenPackedStream(pPackedData);
	const arbitrary_header_type* pHeader = (const arbitrary_header_type*)ReadPackedData(sizeof(arbitrary_header_type));

	size_t width = pHeader->width;
	size_t height = pHeader->height;

	size_t exp_width = ExpandValue(width);
	size_t exp_height = ExpandValue(height);

	if ((exp_width != width) || (exp_height != height))
	{
		ResizeExpandedCoefficients(exp_width, exp_height, false);
		m_pRectWaveletPacker->Depack(ReadPackedData(pHeader->packedSize), GetExpandBuffer());
		CopyBlock(
			GetExpandBuffer(),
				exp_width,
				width, height,
				0, 0,
			pOutput,
				width,
				0, 0);
	}
	else
	{
		m_pRectWaveletPacker->Depack(ReadPackedData(pHeader->packedSize), pOutput);
	}
	
	return true;
}

void CArbitraryWaveletPacker::OpenPackedStream(const void *pPackedStream)const
{
	m_pPackedDataPointer = (ubyte_t*)pPackedStream;
}

const void* CArbitraryWaveletPacker::ReadPackedData(size_t numBytesToRead)const
{
	const void* pData = m_pPackedDataPointer;
	m_pPackedDataPointer += numBytesToRead;
	return pData;
}


const void* CArbitraryWaveletPacker::GetPackedData() const
{
	return m_PackBuffer.GetData();
}

size_t CArbitraryWaveletPacker::GetPackedSize() const
{
	return m_PackBuffer.GetSize();
}

void CArbitraryWaveletPacker::EnableTiling(size_t tileSize)
{
	m_pRectWaveletPacker->EnableTiling(tileSize);
}

bool CArbitraryWaveletPacker::IsTilingEnabled() const
{
	return m_pRectWaveletPacker->IsTilingEnabled();
}

size_t CArbitraryWaveletPacker::GetTileSize() const
{
	return m_pRectWaveletPacker->GetTileSize();
}

void CArbitraryWaveletPacker::DisableTiling()
{
	m_pRectWaveletPacker->DisableTiling();
}


};

