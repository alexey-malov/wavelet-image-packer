// MemoryDataStream.h: interface for the CMemoryDataStream class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MEMORYDATASTREAM_H__F40DDF53_F8A8_404D_AE79_0C71713B2839__INCLUDED_)
#define AFX_MEMORYDATASTREAM_H__F40DDF53_F8A8_404D_AE79_0C71713B2839__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\INCLUDE\IDataStream.h"

namespace bwPng
{


class CMemoryDataStream : public IDataStream  
{
public:
public:
	virtual int Read(void *pBuffer, int numBytesToRead);
	virtual int Seek(int pos, bool bRelative = false);
	virtual int GetSize()const;
	virtual int GetPosition()const;

	CMemoryDataStream(const void *pBuffer, int bufferSize);
	virtual ~CMemoryDataStream();
private:
	const char	*m_pBuffer;
	int		m_nBufferSize;
	int		m_nPos;
};

};

#endif // !defined(AFX_MEMORYDATASTREAM_H__F40DDF53_F8A8_404D_AE79_0C71713B2839__INCLUDED_)
