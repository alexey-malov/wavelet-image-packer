// MemoryDataStream.cpp: implementation of the CMemoryDataStream class.
//
//////////////////////////////////////////////////////////////////////

#include "MemoryDataStream.h"
#include <memory.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace bwPng
{



CMemoryDataStream::CMemoryDataStream(const void *pBuffer, int bufferSize)
:m_pBuffer((const char*)pBuffer)
,m_nBufferSize(bufferSize)
,m_nPos(0)
{

}

CMemoryDataStream::~CMemoryDataStream()
{

}


int CMemoryDataStream::Read(void *pBuffer, int numBytesToRead)
{
	if (numBytesToRead < 0) 
	{
		return 0;
	}
	int sizeAvailable = m_nBufferSize - m_nPos;
	int sizeToRead = (numBytesToRead <= sizeAvailable) ? numBytesToRead : sizeAvailable;
	memcpy(pBuffer, m_pBuffer + m_nPos, sizeToRead);

	m_nPos += sizeToRead;
	
	return sizeToRead;
}

int CMemoryDataStream::Seek(int pos, bool bRelative)
{
	if (bRelative) 
	{

		m_nPos += pos;

		m_nPos = 
			(m_nPos < 0) ? 0 :
			(m_nPos > m_nBufferSize) ? m_nBufferSize : m_nPos;
	}
	else
	{
		m_nPos = 
			(pos < 0) ? 0 :
			(pos > m_nBufferSize) ? m_nBufferSize :
			pos;
	}
	return m_nPos;
}

int CMemoryDataStream::GetSize()const
{
	return m_nBufferSize;
}

int CMemoryDataStream::GetPosition()const
{
	return m_nPos;
}


};