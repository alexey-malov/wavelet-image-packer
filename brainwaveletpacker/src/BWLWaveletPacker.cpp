// BWLWaveletPacker.cpp: implementation of the CBWLWaveletPacker class.
//
//////////////////////////////////////////////////////////////////////

#include "bwl_includes.h"
#include "BWLWaveletPacker.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace bwlpacker
{


CBWLWaveletPacker::CBWLWaveletPacker(size_t initialPackBufferSize)
:m_PackedData(initialPackBufferSize)
,m_CompressionMethod(COMPRESSION_BEST)
,m_nTileSize(128)
{
	m_Wavelet.SetLifting(&m_Lifting);
	
}

CBWLWaveletPacker::~CBWLWaveletPacker()
{

}


bool CBWLWaveletPacker::Pack(const data_t *pInputData, size_t dataSize, data_t quantizer)
{
	if (dataSize & dataSize - 1) 
	{
		return false;
	}
	
	EmptyPackBuffer();
	
	main_header	hdr;
	hdr.dataSize = dataSize;
	if ((GetCompressionMethod() == COMPRESSION_FAST) && (dataSize > GetTileSize()))
	{
		hdr.compressionMethod = COMPRESSION_FAST;
		hdr.tileSize = GetTileSize();
		WriteToPackBuffer(&hdr, sizeof(hdr));
		return PackFast(pInputData, dataSize, quantizer);
	}
	hdr.compressionMethod = COMPRESSION_BEST;
	hdr.tileSize = 0;
	WriteToPackBuffer(&hdr, sizeof(hdr));
	m_Wavelet.RemoveAllMacroblocs();
	PackBest(pInputData, dataSize, quantizer);
	return true;

}

void CBWLWaveletPacker::EmptyPackBuffer()
{
	m_PackedData.Rewrite();
}

size_t CBWLWaveletPacker::WriteToPackBuffer(const void *pSourceData, size_t numBytesToWrite)
{
	return m_PackedData.Write(pSourceData, numBytesToWrite);
}

const void* CBWLWaveletPacker::GetPackedData() const
{
	return m_PackedData.GetData();
}


size_t CBWLWaveletPacker::GetPackedSize() const
{
	return m_PackedData.GetSize();
}


bool CBWLWaveletPacker::Depack(const void *pPackedData, data_t *pOutputData)const
{
	OpenPackedStream(pPackedData);
	const main_header *pMainHeader = (const main_header *)ReadPackedData(sizeof(main_header));
	switch(pMainHeader->compressionMethod) 
	{
	case COMPRESSION_FAST:
		return DepackFast(pOutputData, pMainHeader);
	case COMPRESSION_BEST:
		return DepackBest(pOutputData);
	default:
		BRAINWAVELETS_ASSERT(FALSE);
		return false;
	}
}

const void* CBWLWaveletPacker::ReadPackedData(size_t numBytesToRead) const
{
	const void* pData = m_pPackedDataPointer;
	m_pPackedDataPointer += numBytesToRead;
	return pData;
}

void CBWLWaveletPacker::OpenPackedStream(const void *pPackedStream) const
{
	m_pPackedDataPointer = (ubyte_t*)pPackedStream;
}

CBWLWaveletPacker* CBWLWaveletPacker::Create(size_t initialPackBufferSize)
{
	return new CBWLWaveletPacker(initialPackBufferSize);
}

bool CBWLWaveletPacker::PackFast(const data_t *pInputData, size_t dataSize, data_t quantizer)
{
	BRAINWAVELETS_ASSERT(dataSize != 0);
	BRAINWAVELETS_ASSERT((dataSize & (dataSize - 1)) == 0);
	BRAINWAVELETS_ASSERT(dataSize > GetTileSize());

	
	//m_Wavelet.Load(pInputData);
	
	size_t workDataSize = dataSize;
	m_TemporaryBuffers[0].resize((dataSize * dataSize / 4));
	m_TemporaryBuffers[1].resize((dataSize * dataSize / 16));
	size_t destBuffer = 0;
	const data_t *pInputBuffer = pInputData;
	size_t nPackedSize = 0;
	m_Wavelet.RemoveAllMacroblocs();
	while (workDataSize > GetTileSize()) 
	{
		data_t *pOutputBuffer = &m_TemporaryBuffers[destBuffer][0];
		{
			for (size_t startRow = 0; startRow < workDataSize; startRow += GetTileSize())
			{
				for (size_t startColumn = 0; startColumn < workDataSize; startColumn += GetTileSize())
				{
					// resize wavelet
					m_Wavelet.Resize(GetTileSize());
					// load data into wavelet
					m_Wavelet.Load(pInputBuffer + workDataSize * startRow + startColumn, workDataSize);
					// remove macroblocks from wavelet
					// transform wavelet
					m_Wavelet.Transform();
					// quantize data
					m_Wavelet.QuantizeAndSplit(quantizer);
					// save low-frequency data into output buffer
					m_Wavelet.Save(pOutputBuffer + (workDataSize / 2) * (startRow / 2) + startColumn / 2, workDataSize / 2);
				}
			}
		}
		quantizer += quantizer;
		
		pInputBuffer = pOutputBuffer;
		destBuffer ^= 1;
		workDataSize >>= 1;
	}

	// pack rest data using best method
	size_t firstMacroblock = m_Wavelet.GetMacroblockCount();
	PackBest(pInputBuffer, workDataSize, quantizer);

	size_t numMacroblocks = 4;
	while (workDataSize < dataSize) 
	{
		firstMacroblock -= numMacroblocks;
		for (size_t macroblockIndex = 0; macroblockIndex < numMacroblocks; ++macroblockIndex)
		{
			CMacroblockType const& currentMacroblock = m_Wavelet.GetMacroblock(firstMacroblock + macroblockIndex);
			size_t nPackedMacroblockSize = m_Codec.EncodeMacroblock(currentMacroblock);
			macroblock_header hdr;
			hdr.blockSize = currentMacroblock.GetSize();
			hdr.quantizer = currentMacroblock.GetQuantizer();
			hdr.packedSize = nPackedMacroblockSize;
			WriteToPackBuffer(&hdr, sizeof(hdr));
			WriteToPackBuffer(m_Codec.GetPackedData(), nPackedMacroblockSize);
		}
		workDataSize *= 2;
		numMacroblocks *= 4;
	}
	return true;


}

bool CBWLWaveletPacker::DepackFast(data_t *pOutputData, main_header const * pMainHeader) const
{
	size_t dataSize = pMainHeader->dataSize;
	size_t tileSize = pMainHeader->tileSize;
	BRAINWAVELETS_ASSERT(dataSize != 0);
	BRAINWAVELETS_ASSERT(tileSize != 0);
	BRAINWAVELETS_ASSERT((dataSize & (dataSize - 1)) == 0);
	BRAINWAVELETS_ASSERT((tileSize & (tileSize - 1)) == 0);
	BRAINWAVELETS_ASSERT(tileSize < dataSize);

	m_TemporaryBuffers[0].resize(dataSize * dataSize);
	data_t *pOutputBuffer = &m_TemporaryBuffers[0][0];
	data_t *pInputBuffer = pOutputData;
	size_t workDataSize = tileSize;
	while (workDataSize < dataSize) 
	{
		data_t *pTempBuffer = pInputBuffer;
		pInputBuffer = pOutputBuffer;
		pOutputBuffer = pTempBuffer;
		workDataSize *= 2;
	}

	m_Wavelet.RemoveAllMacroblocs();
	m_Wavelet.AllocateMacroblock(0, tileSize);

	DepackBest(pInputBuffer);


	workDataSize = tileSize;
	while (workDataSize < dataSize) 
	{

		for (size_t startRow = 0; startRow < workDataSize; startRow += (tileSize >> 1))
		{
			for (size_t startColumn = 0; startColumn < workDataSize; startColumn += (tileSize >> 1))
			{
				const macroblock_header *pMBHeader = (const macroblock_header*)ReadPackedData(sizeof(macroblock_header));

				m_Wavelet.RemoveAllMacroblocs();
				CMacroblockType &macroblock = m_Wavelet.AllocateMacroblock(pMBHeader->quantizer, pMBHeader->blockSize);
				m_Codec.Decode(ReadPackedData(pMBHeader->packedSize), &macroblock[0]);

				m_Wavelet.Resize(tileSize >> 1);
				m_Wavelet.Load(pInputBuffer + startRow * workDataSize + startColumn, workDataSize);
				m_Wavelet.DequantizeAndJoin(0);
				m_Wavelet.InverseTransform();
				m_Wavelet.Save(pOutputBuffer + (startRow * 2) * (workDataSize * 2) + startColumn * 2, workDataSize * 2);
			}
		}
		m_Wavelet.RemoveAllMacroblocs();


		data_t *pTempBuffer = pInputBuffer;
		pInputBuffer = pOutputBuffer;
		pOutputBuffer = pTempBuffer;
		workDataSize *= 2;
	}
	
	


	return true;
}

size_t CBWLWaveletPacker::GetTileSize()const
{
	return m_nTileSize;
}

bool CBWLWaveletPacker::PackBest(const data_t *pInputData, size_t dataSize, data_t quantizer)
{
	m_Wavelet.Resize(dataSize);
	m_Wavelet.Load(pInputData);
	size_t numMacroblocks = m_Wavelet.GetMacroblockCount();
	size_t numPasses = m_Wavelet.TransformAndQuantizeMultipass(quantizer);

	wavelet_header	header;
	header.dataSize = dataSize;
	header.quantizer = quantizer;
	header.numMacroblocks = numPasses;
	header.packedSize = 0;
	int buffPos = GetPackedSize();
	WriteToPackBuffer(&header, sizeof(header));
	
	// pack macroblocks
	{
		size_t nPackedSize = 0;
		for (int i = numPasses-1; i >= 0; i--)
		{
			// first we pack high frequency signal then low frequency signal
			CMacroblockType const &macroblock = m_Wavelet.GetMacroblock(i + numMacroblocks);
			size_t nPackedMacroblockSize = m_Codec.EncodeMacroblock(macroblock);
			nPackedSize += nPackedMacroblockSize;

			// write header
			macroblock_header blk;
			blk.packedSize = nPackedMacroblockSize;
			blk.blockSize = macroblock.GetSize();
			blk.quantizer = macroblock.GetQuantizer();

			WriteToPackBuffer(&blk, sizeof(blk));
			WriteToPackBuffer(m_Codec.GetPackedData(), nPackedMacroblockSize);
		}
		// write packed size
		wavelet_header* pHeader = (wavelet_header*)((char *)GetPackedData() + buffPos);
		pHeader->packedSize = nPackedSize;
	}
	
	return true;

}

bool CBWLWaveletPacker::DepackBest(data_t *pOutputData) const
{
	const wavelet_header *pHeader = (const wavelet_header*)ReadPackedData(sizeof(wavelet_header));

	// depack macroblocks
	{
		m_Wavelet.Resize(pHeader->dataSize);
		m_Wavelet.Resize(0);
		m_Wavelet.RemoveAllMacroblocs();
		m_Wavelet.AllocateMacroblock(0, pHeader->dataSize);
		
		size_t numMacroblocks = pHeader->numMacroblocks;
		for (size_t curMacroblock = 0; curMacroblock < numMacroblocks; curMacroblock++) 
		{
			const macroblock_header *pMBHeader = (const macroblock_header*)ReadPackedData(sizeof(macroblock_header));

			m_Wavelet.RemoveAllMacroblocs();
			CMacroblockType &macroblock = m_Wavelet.AllocateMacroblock(pMBHeader->quantizer, pMBHeader->blockSize);
			m_Codec.Decode(ReadPackedData(pMBHeader->packedSize), &macroblock[0]);
			m_Wavelet.DequantizeAndJoin(0);
			m_Wavelet.InverseTransform();
		}
		m_Wavelet.RemoveAllMacroblocs();
		m_Wavelet.Save(pOutputData);
	}
	
	return true;

}

void CBWLWaveletPacker::EnableTiling(size_t tileSize)
{
	BRAINWAVELETS_ASSERT((tileSize & (tileSize - 1)) == 0);
	BRAINWAVELETS_ASSERT(tileSize);
	if ((tileSize != 0) && ((tileSize & (tileSize - 1)) == 0))
	{
		m_CompressionMethod = COMPRESSION_FAST;
		m_nTileSize = tileSize;
	}
}

void CBWLWaveletPacker::DisableTiling()
{
	m_CompressionMethod = COMPRESSION_BEST;
}

bool CBWLWaveletPacker::IsTilingEnabled() const
{
	return m_CompressionMethod == COMPRESSION_FAST;
}

};

