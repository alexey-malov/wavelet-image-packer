// ArbitraryWaveletPacker.h: interface for the CArbitraryWaveletPacker class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ARBITRARYWAVELETPACKER_H__831394FC_03B5_4D1A_8E49_E82780C21644__INCLUDED_)
#define AFX_ARBITRARYWAVELETPACKER_H__831394FC_03B5_4D1A_8E49_E82780C21644__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\INCLUDE\IArbitraryWaveletPacker.h"
#include "../include/IRectangularWaveletPacker.h"
#include "PackBuffer.h"

namespace bwlpacker
{

class CArbitraryWaveletPacker : public IArbitraryWaveletPacker  
{
public:
	typedef unsigned char				ubyte_t;
	typedef std::vector<ubyte_t>		ubyte_vector_t;
	
	struct arbitrary_header_type 
	{
		size_t	width;
		size_t	height;
		size_t	packedSize;
	};
public:
	virtual void DisableTiling();
	virtual size_t GetTileSize()const;
	virtual bool IsTilingEnabled()const;
	virtual void EnableTiling(size_t tileSize=128);

	virtual bool Depack(const void* pPackedData, data_t* pOutput);
	virtual bool Pack(const data_t* pInputCoefficients, size_t width, size_t height, data_t quantizer);
	virtual const void *GetPackedData() const;
	virtual size_t GetPackedSize() const;

	static CArbitraryWaveletPacker* Create(size_t initialPackBufferSize=0);
protected:
	CArbitraryWaveletPacker(size_t initialPackBufferSize);
	virtual ~CArbitraryWaveletPacker();
private:
	inline data_t* GetExpandBuffer()const
	{
		return &m_ExpandedCoefficients[0];
	}
	void OpenPackedStream(const void *pPackedStream)const;
	const void *ReadPackedData(size_t numBytesToRead)const;
	size_t WriteToPackBuffer(const void *pSrcData, size_t bytesToWrite);
	void RewritePackBuffer();
	static size_t ExpandValue(size_t value);
	void ResizeExpandedCoefficients(size_t width, size_t height, bool bClear);
	std::auto_ptr<IRectangularWaveletPacker>	m_pRectWaveletPacker;

	mutable std::vector<data_t>		m_ExpandedCoefficients;

	//ubyte_vector_t					m_PackBuffer;
	CPackBuffer						m_PackBuffer;

	mutable ubyte_t					*m_pPackedDataPointer;
};



};

#endif // !defined(AFX_ARBITRARYWAVELETPACKER_H__831394FC_03B5_4D1A_8E49_E82780C21644__INCLUDED_)

