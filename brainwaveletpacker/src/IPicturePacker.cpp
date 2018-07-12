// IPicturePacker.cpp: implementation of the IPicturePacker class.
//
//////////////////////////////////////////////////////////////////////

#include "bwl_includes.h"
#include "..\INCLUDE\IPicturePacker.h"
#include "BWLPicturePacker.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace bwlpacker
{


IPicturePacker::IPicturePacker()
{

}

IPicturePacker::~IPicturePacker()
{

}

IPicturePacker* IPicturePacker::Create(size_t nPackBufferSize)
{
	return CBWLPicturePacker::Create(nPackBufferSize);
}


};

