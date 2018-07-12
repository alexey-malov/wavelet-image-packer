// IRectangularWaveletPacker.cpp: implementation of the IRectangularWaveletPacker class.
//
//////////////////////////////////////////////////////////////////////

#include "bwl_includes.h"
#include "..\INCLUDE\IRectangularWaveletPacker.h"
#include "RectangularWaveletPacker.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


namespace bwlpacker
{


IRectangularWaveletPacker::IRectangularWaveletPacker()
{

}

IRectangularWaveletPacker::~IRectangularWaveletPacker()
{

}

IRectangularWaveletPacker* IRectangularWaveletPacker::Create(size_t initialPackBufferSize)
{
	return CRectangularWaveletPacker::Create(initialPackBufferSize);
}



};


