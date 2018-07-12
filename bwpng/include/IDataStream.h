// IDataStream.h: interface for the IDataStream class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IDATASTREAM_H__29B70513_5AFE_4DB7_8CF8_97FCB61EB17D__INCLUDED_)
#define AFX_IDATASTREAM_H__29B70513_5AFE_4DB7_8CF8_97FCB61EB17D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace bwPng
{


class IDataStream  
{
public:
public:
	static IDataStream* CreateMemoryDataStream(const void* pBuffer, int nBufferSize);
	static IDataStream* CreateFileDataStream(const char* strFileName);
	virtual int Read(void *pBuffer, int numBytesToRead) = 0;
	virtual int Seek(int pos, bool bRelative = false) = 0;
	virtual int GetSize()const = 0;
	virtual int GetPosition()const = 0;
	
	virtual ~IDataStream();
protected:
	IDataStream();

};


};

#endif // !defined(AFX_IDATASTREAM_H__29B70513_5AFE_4DB7_8CF8_97FCB61EB17D__INCLUDED_)
