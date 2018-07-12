// PackBuffer.h: interface for the CPackBuffer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PACKBUFFER_H__40B427DC_5114_485B_A1CC_F66A1C8D78D9__INCLUDED_)
#define AFX_PACKBUFFER_H__40B427DC_5114_485B_A1CC_F66A1C8D78D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace bwlpacker
{

class CPackBuffer  
{
private:
	typedef unsigned char			ubyte_t;
	typedef std::vector<ubyte_t>	ubyte_vector_t;
public:
	inline const void* GetData()const
	{
		return &m_PackBuffer[0];
	}
	inline void* GetData()
	{
		return &m_PackBuffer[0];
	}
	inline size_t GetSize()const
	{
		return m_PackBuffer.size();
	}
	void Rewrite();
	size_t Write(const void *pSrcData, size_t bytesToWrite);
	CPackBuffer(size_t initial_size = 0);
	virtual ~CPackBuffer();
private:
	ubyte_vector_t	m_PackBuffer;
};

};

#endif // !defined(AFX_PACKBUFFER_H__40B427DC_5114_485B_A1CC_F66A1C8D78D9__INCLUDED_)
