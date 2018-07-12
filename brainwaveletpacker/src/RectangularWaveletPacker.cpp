// RectangularWaveletPacker.cpp: implementation of the CRectangularWaveletPacker class.
//
//////////////////////////////////////////////////////////////////////

#include "bwl_includes.h"
#include "RectangularWaveletPacker.h"
#include "utils.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace bwlpacker
{



CRectangularWaveletPacker::CRectangularWaveletPacker(size_t initialPackBufferSize)
:m_pWaveletPacker(IWaveletPacker::Create(initialPackBufferSize))
,m_PackBuffer(initialPackBufferSize)
,m_nWaveletSize(0)
,m_nBlockSize(0)
{

}

CRectangularWaveletPacker::~CRectangularWaveletPacker()
{

}

CRectangularWaveletPacker* CRectangularWaveletPacker::Create(size_t initialPackBufferSize)
{
	return new CRectangularWaveletPacker(initialPackBufferSize);
}

bool CRectangularWaveletPacker::Pack(const data_t *pInputData, size_t width, size_t height, data_t quantizer)
{
	size_t blockSize;

	if (width == height) 
	{
		rect_header_type header;
		header.width = width;
		header.height = height;
		blockSize = width;
		RewritePackBuffer();
		WriteToPackBuffer(&header, sizeof(header));

		rect_block_header_type blkHeader;
		m_pWaveletPacker->Pack(pInputData, blockSize, quantizer);
		blkHeader.packedDataSize = m_pWaveletPacker->GetPackedSize();

		WriteToPackBuffer(&blkHeader, sizeof(blkHeader));
		WriteToPackBuffer(m_pWaveletPacker->GetPackedData(), blkHeader.packedDataSize);
	}
	else
	{
		size_t width1, height1;

		if (width < height) 
		{
			ConvertSizes(width, height, width1, height1);
			blockSize = width1;
			
			ResizeTemporaryCoefficients(blockSize);
			// copy data
		
			/*
				[a]      [a][b][ ][ ]
				[c]  --\ [c][d][ ][ ]
				[e]  --/ [e][f][ ][ ]
				[g]      [g][h][ ][ ]
				[h]
				[i]
				[j]
				[k]
				
			 */
			RewritePackBuffer();
			rect_header_type header;
			header.width = width;
			header.height = height;
			WriteToPackBuffer(&header, sizeof(header));
			{
				for (size_t dstX = 0, srcY = 0; dstX < blockSize; dstX += width, srcY += blockSize) 
				{
					CopyBlock(
						pInputData, 
							width, 
							width, blockSize, 
							0, srcY, 
						GetWaveletBuffer(),
							blockSize,
							dstX, 0);
				}
			}
			rect_block_header_type blkHeader;
			m_pWaveletPacker->Pack(GetWaveletBuffer(), blockSize, quantizer);
			blkHeader.packedDataSize = m_pWaveletPacker->GetPackedSize();
			WriteToPackBuffer(&blkHeader, sizeof(blkHeader));
			WriteToPackBuffer(m_pWaveletPacker->GetPackedData(), blkHeader.packedDataSize);
			if (width1 != height1)
			{
				{
					for (size_t dstX = 0, srcY = height >> 1; dstX < blockSize; dstX += width, srcY += blockSize) 
					{
						CopyBlock(
							pInputData, 
								width, 
								width, blockSize, 
								0, srcY, 
							GetWaveletBuffer(),
								blockSize,
								dstX, 0);
					}
				}
				m_pWaveletPacker->Pack(GetWaveletBuffer(), blockSize, quantizer);
				blkHeader.packedDataSize = m_pWaveletPacker->GetPackedSize();
				WriteToPackBuffer(&blkHeader, sizeof(blkHeader));
				WriteToPackBuffer(m_pWaveletPacker->GetPackedData(), blkHeader.packedDataSize);
			}

		}
		else // width > height
		{
			ConvertSizes(height, width, height1, width1);
			blockSize = height1;

			ResizeTemporaryCoefficients(blockSize);
			// copy data

			/*
				[a][b][c][d][e][f][g][h]

				[a][b]
				[c][d]

				[e][f]
				[g][h]
			*/
			RewritePackBuffer();
			rect_header_type header;
			header.width = width;
			header.height = height;
			WriteToPackBuffer(&header, sizeof(header));
			{
				for (size_t dstY = 0, srcX = 0; dstY < blockSize; dstY += height, srcX += blockSize) 
				{
					CopyBlock(
						pInputData, 
							width, 
							blockSize, height, srcX, 0, 
						GetWaveletBuffer(), 
							blockSize, 
							0, dstY);
				}
			}

			rect_block_header_type blkHeader;
			m_pWaveletPacker->Pack(GetWaveletBuffer(), blockSize, quantizer);
			blkHeader.packedDataSize = m_pWaveletPacker->GetPackedSize();
			WriteToPackBuffer(&blkHeader, sizeof(blkHeader));
			WriteToPackBuffer(m_pWaveletPacker->GetPackedData(), blkHeader.packedDataSize);

			if (width1 != height1)
			{
				{
					for (size_t dstY = 0, srcX = width >> 1; dstY < blockSize; dstY += height, srcX += blockSize) 
					{
						CopyBlock(
							pInputData, 
								width, 
								blockSize, height, srcX, 0, 
							GetWaveletBuffer(), 
								blockSize, 
								0, dstY);
					}
				}
				m_pWaveletPacker->Pack(GetWaveletBuffer(), blockSize, quantizer);
				blkHeader.packedDataSize = m_pWaveletPacker->GetPackedSize();
				WriteToPackBuffer(&blkHeader, sizeof(blkHeader));
				WriteToPackBuffer(m_pWaveletPacker->GetPackedData(), blkHeader.packedDataSize);
			}
		}

	}

	return true;
}

void CRectangularWaveletPacker::ConvertSizes(size_t lowerDim, size_t greaterDim, size_t &lowerDim1, size_t &greaterDim1)
{
	lowerDim1 = lowerDim;
	greaterDim1 = greaterDim;
	while (lowerDim1 < (greaterDim1 >> 1))
	{
		lowerDim1 <<= 1;
		greaterDim1 >>= 1;
	}
}


void CRectangularWaveletPacker::ResizeTemporaryCoefficients(size_t size)
{
	//m_pWaveletPacker
	//*
	if (size > m_nWaveletSize) 
	{
		m_WaveletCoefficients.resize(size * size);
		m_nWaveletSize = size;
	}
	
	m_nBlockSize = size;

}

size_t CRectangularWaveletPacker::WriteToPackBuffer(const void *pSrcData, size_t bytesToWrite)
{
	return m_PackBuffer.Write(pSrcData, bytesToWrite);
}

void CRectangularWaveletPacker::RewritePackBuffer()
{
	m_PackBuffer.Rewrite();
}

bool CRectangularWaveletPacker::Depack(const void *pPackedData, data_t *pOutputCoefficients)
{
	OpenPackedStream(pPackedData);
	const rect_header_type *pHeader = (const rect_header_type *)ReadPackedData(sizeof(rect_header_type));
	
	const size_t width = pHeader->width;
	const size_t height = pHeader->height;

	if (width == height) 
	{
		const rect_block_header_type *pBlockHeader = (const rect_block_header_type *)ReadPackedData(sizeof(rect_block_header_type));
		m_pWaveletPacker->Depack(ReadPackedData(pBlockHeader->packedDataSize), pOutputCoefficients);
	}
	else
	{
		size_t width1, height1, blockSize;

		if (width < height) 
		{
			ConvertSizes(width, height, width1, height1);
			blockSize = width1;
			
			ResizeTemporaryCoefficients(blockSize);

			const rect_block_header_type *pBlockHeader = (const rect_block_header_type *)ReadPackedData(sizeof(rect_block_header_type));
			m_pWaveletPacker->Depack(ReadPackedData(pBlockHeader->packedDataSize), GetWaveletBuffer());

			/*
				[a]      [a][b][ ][ ]
				[c]  /-- [c][d][ ][ ]
				[e]  \-- [e][f][ ][ ]
				[g]      [g][h][ ][ ]
				[h]
				[i]
				[j]
				[k]
				
ae
cg

hj
ik
*/
			{
				for (size_t srcX = 0, dstY = 0; srcX < blockSize; srcX += width, dstY += blockSize) 
				{
					CopyBlock(
						GetWaveletBuffer(),
							blockSize,
							width, blockSize, 
							srcX, 0,
						pOutputCoefficients, 
							width, 
							0, dstY);
				}
				
			}

			if (width1 != height1)
			{
				pBlockHeader = (const rect_block_header_type *)ReadPackedData(sizeof(rect_block_header_type));
				m_pWaveletPacker->Depack(ReadPackedData(pBlockHeader->packedDataSize), GetWaveletBuffer());

				{
					for (size_t srcX = 0, dstY = height >> 1; srcX < blockSize; srcX += width, dstY += blockSize) 
					{
						CopyBlock(
							GetWaveletBuffer(),
								blockSize,
								width, blockSize, 
								srcX, 0,
							pOutputCoefficients, 
								width, 
								0, dstY);
					}
					
				}
			}
			
		}
		else	// width > height
		{

			ConvertSizes(height, width, height1, width1);
			blockSize = height1;
			
			ResizeTemporaryCoefficients(blockSize);

			const rect_block_header_type *pBlockHeader = (const rect_block_header_type *)ReadPackedData(sizeof(rect_block_header_type));


			m_pWaveletPacker->Depack(ReadPackedData(pBlockHeader->packedDataSize), GetWaveletBuffer());


			/*
				[a][b][c][d]

				[a][b][c][d]
				[e][f][g][h]
				[ ][ ][ ][ ]
				[ ][ ][ ][ ]
			*/
			{
				for (size_t dstX = 0, srcY = 0;  srcY < blockSize;  dstX += blockSize, srcY += height) 
				{
					CopyBlock(
						GetWaveletBuffer(),
							blockSize,
							blockSize, height, 
							0, srcY,
						pOutputCoefficients, 
							width, 
							dstX, 0);
				}
			}

			if (width1 != height1)
			{
				pBlockHeader = (const rect_block_header_type *)ReadPackedData(sizeof(rect_block_header_type));
				m_pWaveletPacker->Depack(ReadPackedData(pBlockHeader->packedDataSize), GetWaveletBuffer());

				{
					for (size_t dstX = width >> 1, srcY = 0;  srcY < blockSize;  dstX += blockSize, srcY += height) 
					{
						CopyBlock(
							GetWaveletBuffer(),
								blockSize,
								blockSize, height, 
								0, srcY,
							pOutputCoefficients, 
								width, 
								dstX, 0);
					}
				}
			}

		}
	}

	return true;
}

void CRectangularWaveletPacker::OpenPackedStream(const void *pPackedStream)const
{
	m_pPackedDataPointer = (ubyte_t*)pPackedStream;
}

const void* CRectangularWaveletPacker::ReadPackedData(size_t numBytesToRead)const
{
	const void* pData = m_pPackedDataPointer;
	m_pPackedDataPointer += numBytesToRead;
	return pData;
}


const void* CRectangularWaveletPacker::GetPackedData() const
{
	return m_PackBuffer.GetData();
}

size_t CRectangularWaveletPacker::GetPackedSize() const
{
	return m_PackBuffer.GetSize();
}

void CRectangularWaveletPacker::EnableTiling(size_t tileSize)
{
	m_pWaveletPacker->EnableTiling(tileSize);
}

bool CRectangularWaveletPacker::IsTilingEnabled() const
{
	return m_pWaveletPacker->IsTilingEnabled();
}

size_t CRectangularWaveletPacker::GetTileSize() const
{
	return m_pWaveletPacker->GetTileSize();
}

void CRectangularWaveletPacker::DisableTiling()
{
	m_pWaveletPacker->DisableTiling();
}

};


