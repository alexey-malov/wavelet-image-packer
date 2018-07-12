// FileDataStream.cpp: implementation of the CFileDataStream class.
//
//////////////////////////////////////////////////////////////////////

#include "FileDataStream.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace bwPng
{


CFileDataStream::CFileDataStream(const char *strFileName)
:m_pFile(NULL)
,m_nFileSize(0)
{
	m_pFile = fopen(strFileName, "rb");
	if (m_pFile)
	{
		fseek(m_pFile, 0, SEEK_END);
		m_nFileSize = ftell(m_pFile);
		fseek(m_pFile, 0, SEEK_SET);
	}	
}

CFileDataStream::~CFileDataStream()
{
	if (m_pFile) 
	{
		fclose(m_pFile);
	}
}


int CFileDataStream::Read(void *pBuffer, int numBytesToRead)
{
	if (!IsOpened())
	{
		return 0;
	}

	return fread(pBuffer, 1, numBytesToRead, m_pFile);
}

int CFileDataStream::Seek(int pos, bool bRelative)
{
	if (!IsOpened())
	{
		return false;
	}

	return fseek(m_pFile, pos, bRelative ? SEEK_CUR : SEEK_SET) == 0;
}

int CFileDataStream::GetSize()const
{
	return m_nFileSize;
}

int CFileDataStream::GetPosition()const
{
	if (!IsOpened())
	{
		return -1;
	}
	return ftell(m_pFile);
}




};