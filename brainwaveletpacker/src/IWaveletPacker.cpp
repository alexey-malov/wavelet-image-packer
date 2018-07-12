// IWaveletPacker.cpp: implementation of the IWaveletPacker class.
//
//////////////////////////////////////////////////////////////////////

#include "bwl_includes.h"
#include "..\INCLUDE\IWaveletPacker.h"
#include "BWLWaveletPacker.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace bwlpacker
{


IWaveletPacker::IWaveletPacker()
{

}

IWaveletPacker::~IWaveletPacker()
{

}

IWaveletPacker* IWaveletPacker::Create(size_t initialPackBufferSize)
{
	return CBWLWaveletPacker::Create(initialPackBufferSize);
}

};

