// IDataStream.cpp: implementation of the IDataStream class.
//
//////////////////////////////////////////////////////////////////////

#include "..\INCLUDE\IDataStream.h"
#include "MemoryDataStream.h"
#include "FileDataStream.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace bwPng
{


IDataStream::IDataStream()
{

}

IDataStream::~IDataStream()
{

}

IDataStream* IDataStream::CreateFileDataStream(const char *strFileName)
{
	CFileDataStream *pFileStream = new CFileDataStream(strFileName);
	if (pFileStream->IsOpened()) 
	{
		return pFileStream;
	}
	delete pFileStream;
	return NULL;
}

IDataStream* IDataStream::CreateMemoryDataStream(const void *pBuffer, int nBufferSize)
{
	return new CMemoryDataStream(pBuffer, nBufferSize);
}

};


