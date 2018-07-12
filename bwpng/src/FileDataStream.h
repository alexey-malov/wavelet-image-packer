// FileDataStream.h: interface for the CFileDataStream class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILEDATASTREAM_H__344D54C1_B4B0_41FA_8BD2_F06151D24798__INCLUDED_)
#define AFX_FILEDATASTREAM_H__344D54C1_B4B0_41FA_8BD2_F06151D24798__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\INCLUDE\IDataStream.h"
#include <stdio.h>


namespace bwPng
{

class CFileDataStream : public IDataStream  
{
public:
	CFileDataStream(const char *strFileName);
	virtual int Read(void *pBuffer, int numBytesToRead);
	virtual int Seek(int pos, bool bRelative = false);
	virtual int GetSize()const;
	virtual int GetPosition()const;

	inline bool IsOpened() const
	{
		return m_pFile!=NULL;
	}

	virtual ~CFileDataStream();
protected:
private:
	FILE	*m_pFile;
	int		m_nFileSize;
};


};

#endif // !defined(AFX_FILEDATASTREAM_H__344D54C1_B4B0_41FA_8BD2_F06151D24798__INCLUDED_)
