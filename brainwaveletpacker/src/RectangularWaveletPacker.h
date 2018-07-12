// RectangularWaveletPacker.h: interface for the CRectangularWaveletPacker class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RECTANGULARWAVELETPACKER_H__0AD909C6_4A96_4404_A882_9A00D6CA4D45__INCLUDED_)
#define AFX_RECTANGULARWAVELETPACKER_H__0AD909C6_4A96_4404_A882_9A00D6CA4D45__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\INCLUDE\IRectangularWaveletPacker.h"
#include "../include/IWaveletPacker.h"
#include "PackBuffer.h"

namespace bwlpacker
{


class CRectangularWaveletPacker : public IRectangularWaveletPacker  
{
public:
	typedef unsigned char				ubyte_t;
	typedef std::vector<ubyte_t>		ubyte_vector_t;

	struct rect_header_type
	{
		size_t width;			// original width
		size_t height;			// original height
	};

	struct rect_block_header_type
	{
		size_t packedDataSize;
	};

public:
	virtual void DisableTiling();
	virtual size_t GetTileSize()const;
	virtual bool IsTilingEnabled()const;
	virtual void EnableTiling(size_t tileSize=128);

	// returns packed data size
	virtual size_t GetPackedSize()const;

	// returns packed data pointer
	virtual const void* GetPackedData()const;

	// depack packed data to destination
	virtual bool Depack(const void* pPackedData, data_t* pOutputCoefficients);

	// pack rectangular wavelet data
	virtual bool Pack(const data_t* pInputData, size_t width, size_t height,  data_t quantizer);

	// create an instance of object
	static CRectangularWaveletPacker* Create(size_t initialPackBufferSize = 0);
protected:
	CRectangularWaveletPacker(size_t initialPackBufferSize);
	virtual ~CRectangularWaveletPacker();
private:
	const void* ReadPackedData(size_t numBytesToRead)const;
	void OpenPackedStream(const void* pPackedStream)const;
	inline data_t* GetWaveletBuffer()const
	{
		return &m_WaveletCoefficients[0];
	}
	static void ConvertSizes(size_t lowerDim, size_t greaterDim, size_t& lowerDim1, size_t & greaterDim1);

	void ResizeTemporaryCoefficients(size_t size);
	size_t WriteToPackBuffer(const void* pSrcData, size_t bytesToWrite);
	void RewritePackBuffer();

private:
	std::auto_ptr<IWaveletPacker>	m_pWaveletPacker;	// square wavelet packer
	mutable std::vector<data_t>		m_WaveletCoefficients;		// wavelet coefficients
	size_t							m_nWaveletSize;
	size_t							m_nBlockSize;

	mutable ubyte_t*				m_pPackedDataPointer;	//packed data pointer (used when depacking)

	CPackBuffer						m_PackBuffer;
};



};


#endif // !defined(AFX_RECTANGULARWAVELETPACKER_H__0AD909C6_4A96_4404_A882_9A00D6CA4D45__INCLUDED_)
