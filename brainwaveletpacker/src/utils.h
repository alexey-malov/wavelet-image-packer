// utils.h: interface for the utils class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UTILS_H__BE47BB19_0CA5_4FF4_9CEE_3EBB3C27C50F__INCLUDED_)
#define AFX_UTILS_H__BE47BB19_0CA5_4FF4_9CEE_3EBB3C27C50F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../include/IWaveletPacker.h"

namespace bwlpacker
{

// copies block from source buffer to destination buffer
void CopyBlock(
		const IWaveletPacker::data_t * pSrcData,	// source data pointer
		size_t srcDataPitch,		// source data pitch
		size_t srcBlockWidth,		// source block width
		size_t srcBlockHeight,		// source block height
		size_t srcX,				// source x
		size_t srcY,				// source y
		IWaveletPacker::data_t *pDstData,			// destination data pointer
		size_t dstDataPitch,		// destination data pitch
		size_t dstX,				// destination x
		size_t dstY);				// destination y


};
/*
inline void FillFloatBlock(float *pBlock, float value, size_t count)
{
	__asm
	{
		mov edi, pBlock
		mov ecx, count
		mov eax, value
		cld
		rep stosd
	};
}
*/
#endif // !defined(AFX_UTILS_H__BE47BB19_0CA5_4FF4_9CEE_3EBB3C27C50F__INCLUDED_)
