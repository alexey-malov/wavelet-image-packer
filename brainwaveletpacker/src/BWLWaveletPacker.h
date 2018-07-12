// BWLWaveletPacker.h: interface for the CBWLWaveletPacker class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BWLWAVELETPACKER_H__1C338A12_426D_4E8D_B34A_22D869CF7605__INCLUDED_)
#define AFX_BWLWAVELETPACKER_H__1C338A12_426D_4E8D_B34A_22D869CF7605__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\INCLUDE\IWaveletPacker.h"
#include "brainwavelets.h"
#include "PackBuffer.h"

namespace bwlpacker
{

	using namespace brainwavelets;

class CBWLWaveletPacker : public IWaveletPacker  
{
	enum CompressionMethod
	{
		COMPRESSION_BEST,
		COMPRESSION_FAST,
	};
	
	typedef CWavelet2D<data_t>			CWaveletType;
	typedef CLinearLifting<data_t>		CLiftingType;
	typedef CWaveletType::blk_desc_t	CMacroblockType;
	typedef unsigned char				ubyte_t;
	typedef std::vector<ubyte_t>		ubyte_vector_t;

	struct main_header
	{
		CompressionMethod	compressionMethod;
		size_t				dataSize;
		size_t				tileSize;
	};

	struct wavelet_header
	{
		data_t				quantizer;		// quantizer
		size_t				dataSize;		// dimension
		size_t				packedSize;		// packed data size
		size_t				numMacroblocks;	// number of macroblocks
	};

	struct macroblock_header
	{
		size_t	packedSize;
		data_t	quantizer;
		size_t	blockSize;
	};

public:
	virtual bool IsTilingEnabled() const;
	virtual void DisableTiling();
	virtual void EnableTiling(size_t tileSize = 128);
	virtual size_t GetTileSize()const;
	static CBWLWaveletPacker* Create(size_t initialPackBufferSize = 0);
	virtual bool Depack(const void* pPackedData, data_t *pOutputData)const;
	virtual size_t GetPackedSize()const;
	virtual const void* GetPackedData()const;
	virtual bool Pack(const data_t* pInputData, size_t dataSize, data_t quantizer);
protected:
	CBWLWaveletPacker(size_t initialPackBufferSize);
	virtual ~CBWLWaveletPacker();
private:
	inline CompressionMethod GetCompressionMethod()const
	{
		return m_CompressionMethod;
	}
	bool DepackBest(data_t *pOutputData) const;
	bool PackBest(const data_t *pInputData, size_t dataSize, data_t quantizer);
	bool DepackFast(data_t *pOutputData, main_header const * pMainHeader)const;
	bool PackFast(const data_t* pInputData, size_t dataSize, data_t quantizer);
	void OpenPackedStream(const void* pPackedStream)const;
	const void* ReadPackedData(size_t numBytesToRead)const;
	size_t WriteToPackBuffer(const void* pSourceData, size_t numBytesToWrite);
	void EmptyPackBuffer();
	
	
	mutable CWaveletType		m_Wavelet;	// wavelet
	mutable CLiftingType		m_Lifting;	// lifting scheme
	mutable CPWCCodec			m_Codec;	// pwc codec

	mutable ubyte_t*			m_pPackedDataPointer;	//packed data pointer (used when depacking)
	
	CPackBuffer					m_PackedData;
	CompressionMethod			m_CompressionMethod;
	size_t						m_nTileSize;
	typedef std::vector<data_t>	data_vector_t;
	mutable data_vector_t		m_TemporaryBuffers[2];
};



};

#endif // !defined(AFX_BWLWAVELETPACKER_H__1C338A12_426D_4E8D_B34A_22D869CF7605__INCLUDED_)
