// PackBuffer.cpp: implementation of the CPackBuffer class.
//
//////////////////////////////////////////////////////////////////////

#include "bwl_includes.h"
#include "PackBuffer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace bwlpacker
{

CPackBuffer::CPackBuffer(size_t initial_size)
{
	m_PackBuffer.reserve(initial_size);
}

CPackBuffer::~CPackBuffer()
{

}

size_t CPackBuffer::Write(const void *pSrcData, size_t bytesToWrite)
{
	m_PackBuffer.insert(m_PackBuffer.end(), (const ubyte_t*)pSrcData, (const ubyte_t*)pSrcData + bytesToWrite);
	return m_PackBuffer.size();
}

void CPackBuffer::Rewrite()
{
	m_PackBuffer.clear();
}


};