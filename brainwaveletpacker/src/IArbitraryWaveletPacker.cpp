// IArbitraryWaveletPacker.cpp: implementation of the IArbitraryWaveletPacker class.
//
//////////////////////////////////////////////////////////////////////

#include "bwl_includes.h"
#include "..\INCLUDE\IArbitraryWaveletPacker.h"
#include "ArbitraryWaveletPacker.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace bwlpacker
{


IArbitraryWaveletPacker::IArbitraryWaveletPacker()
{

}

IArbitraryWaveletPacker::~IArbitraryWaveletPacker()
{

}

IArbitraryWaveletPacker* IArbitraryWaveletPacker::Create(size_t initialPackBufferSize)
{
	return CArbitraryWaveletPacker::Create(initialPackBufferSize);
}

};


