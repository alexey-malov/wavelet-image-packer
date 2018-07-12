// utils.cpp: implementation of the utils class.
//
//////////////////////////////////////////////////////////////////////

#include "bwl_includes.h"
#include "utils.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace bwlpacker
{

void CopyBlock(
	const IWaveletPacker::data_t  * pSrcData, 
		size_t srcDataPitch, 
		size_t srcBlockWidth, 
		size_t srcBlockHeight, 
		size_t srcX, 
		size_t srcY,
	IWaveletPacker::data_t  *pDstData,
		size_t dstDataPitch,
		size_t dstX, 
		size_t dstY)
{
	pSrcData += srcDataPitch * srcY + srcX;
	pDstData += dstDataPitch * dstY + dstX;

	for (size_t y = 0; y < srcBlockHeight; y++) 
	{
		memcpy(pDstData, pSrcData, srcBlockWidth * sizeof(IWaveletPacker::data_t));
		pSrcData += srcDataPitch;
		pDstData += dstDataPitch;
	}
}



};