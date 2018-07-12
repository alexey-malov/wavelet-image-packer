// IArbitraryWaveletPacker.h: interface for the IArbitraryWaveletPacker class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IARBITRARYWAVELETPACKER_H__467D6B8C_0CAB_48D3_B235_AA6D09ACAF1F__INCLUDED_)
#define AFX_IARBITRARYWAVELETPACKER_H__467D6B8C_0CAB_48D3_B235_AA6D09ACAF1F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IRectangularWaveletPacker.h"

namespace bwlpacker
{

class IArbitraryWaveletPacker  
{
public:
	typedef IRectangularWaveletPacker::data_t data_t;
public:

	virtual bool Depack(const void* pPackedData, data_t* pOutput) = 0;
	virtual bool Pack(const data_t* pInputCoefficients, size_t width, size_t height, data_t quantizer) = 0;
	virtual const void *GetPackedData() const = 0;
	virtual size_t GetPackedSize() const = 0;
	virtual void DisableTiling() = 0;
	virtual size_t GetTileSize()const = 0;
	virtual bool IsTilingEnabled()const = 0;
	virtual void EnableTiling(size_t tileSize=128) = 0;
	
	static IArbitraryWaveletPacker* Create(size_t initialPackBufferSize = 0);
	virtual ~IArbitraryWaveletPacker();
protected:
	IArbitraryWaveletPacker();
private:
};


};


#endif // !defined(AFX_IARBITRARYWAVELETPACKER_H__467D6B8C_0CAB_48D3_B235_AA6D09ACAF1F__INCLUDED_)
