// BWImage.h: interface for the CBWImage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BWIMAGE_H__18E5B7EB_3969_4253_830E_98E0D751A0B7__INCLUDED_)
#define AFX_BWIMAGE_H__18E5B7EB_3969_4253_830E_98E0D751A0B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CBWImage  
{
	typedef bwlpacker::IPicturePacker::TypeBGRX	bgrx_t;
	typedef bwlpacker::IPicturePacker::TypeRGB	rgb_t;
	typedef bwlpacker::IPicturePacker::TypeRGBA	rgba_t;
	typedef bwlpacker::IPicturePacker::TypeL	gray_t;
	typedef bwlpacker::IPicturePacker::TypeLA	gray_alpha_t;

public:
	inline bwlpacker::IPicturePacker::PixelDataFormat  GetPixelDataFormat()const;
	inline size_t GetHeight()const;
	inline size_t GetWidth()const;
	bool IsCreated()const;
	bool Draw(HDC destDC, int xDest, int yDest, int width, int height,  int  xSrc, int ySrc);
	void Assign(const CBWImage &other);
	bool LoadPng(LPCTSTR strFileName);
	CBWImage();
	CBWImage(const CBWImage& other);
	virtual ~CBWImage();
	CBWImage& operator=(const CBWImage& other);
	bool CreateBitmap(bool bPacked = false);
	inline bwlpacker::IPicturePacker * GetPicturePacker()
	{
		return m_pPicturePacker.get();
	}
private:
	bool CreatePackedBitmap();
	bool CreateUnpackedBitmap(bool bCreateFromPackedData = false);

	size_t	m_nWidth;
	size_t	m_nHeight;
	bwlpacker::IPicturePacker::PixelDataFormat	m_nPixelFormat;
	std::vector<char>	m_Data;
	BITMAPINFO			m_BitmapInfo;
	bool	m_bCreated;

	std::vector<bgrx_t> m_RGBXbuf;

	
	// png codec
	static std::auto_ptr<bwPng::IPngCodec>	m_pPngCodec;
	static std::auto_ptr<bwlpacker::IPicturePacker>	m_pPicturePacker;
};

inline size_t CBWImage::GetWidth() const
{
	return m_nWidth;
}

inline size_t CBWImage::GetHeight() const
{
	return m_nHeight;
}

inline bwlpacker::IPicturePacker::PixelDataFormat CBWImage::GetPixelDataFormat() const
{
	return m_nPixelFormat;
}

#endif // !defined(AFX_BWIMAGE_H__18E5B7EB_3969_4253_830E_98E0D751A0B7__INCLUDED_)
