// IRectangularWaveletPacker.h: interface for the IRectangularWaveletPacker class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IRECTANGULARWAVELETPACKER_H__82A54407_C65A_48FE_85AD_E91E1E6476FC__INCLUDED_)
#define AFX_IRECTANGULARWAVELETPACKER_H__82A54407_C65A_48FE_85AD_E91E1E6476FC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IWaveletPacker.h"

namespace bwlpacker
{


class IRectangularWaveletPacker  
{
public:
	typedef IWaveletPacker::data_t data_t;
public:

	virtual void DisableTiling() = 0;
	virtual size_t GetTileSize()const = 0;
	virtual bool IsTilingEnabled()const = 0;
	virtual void EnableTiling(size_t tileSize=128) = 0;

	
	// returns packed data size
	virtual size_t GetPackedSize()const = 0;

	// returns packed data pointer
	virtual const void* GetPackedData()const = 0;

	// depack packed data to destination
	virtual bool Depack(const void* pPackedData, data_t* pOutputCoefficients) = 0; 

	// pack rectangular wavelet data
	virtual bool Pack(const data_t* pInputData, size_t width, size_t height,  data_t quantizer) = 0;

	static IRectangularWaveletPacker* Create(size_t initialPackBufferSize = 0);

	virtual ~IRectangularWaveletPacker();
protected:
	IRectangularWaveletPacker();

};


};


#endif // !defined(AFX_IRECTANGULARWAVELETPACKER_H__82A54407_C65A_48FE_85AD_E91E1E6476FC__INCLUDED_)
