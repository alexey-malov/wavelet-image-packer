// IWaveletPacker.h: interface for the IWaveletPacker class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IWAVELETPACKER_H__BD254C86_A526_4B2A_AE3D_A5DADFA16754__INCLUDED_)
#define AFX_IWAVELETPACKER_H__BD254C86_A526_4B2A_AE3D_A5DADFA16754__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace bwlpacker
{

class IWaveletPacker  
{
public:
	typedef float data_t;			// data type
	typedef unsigned int uint_t;
public:
	static IWaveletPacker* Create(size_t initialPackBufferSize = 0);
	virtual bool Depack(const void* pPackedData, data_t *pOutputData)const = 0;
	virtual size_t GetPackedSize()const = 0;
	virtual const void* GetPackedData()const = 0;
	virtual bool Pack(const data_t* pInputData, size_t dataSize, data_t quantizer) = 0;
	virtual bool IsTilingEnabled() const = 0;
	virtual void DisableTiling() = 0;
	virtual void EnableTiling(size_t tileSize = 128) = 0;
	virtual size_t GetTileSize()const = 0;
	
	virtual ~IWaveletPacker();
protected:
	IWaveletPacker();
private:
};


};


#endif // !defined(AFX_IWAVELETPACKER_H__BD254C86_A526_4B2A_AE3D_A5DADFA16754__INCLUDED_)
