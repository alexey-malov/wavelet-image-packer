// BWImage.cpp: implementation of the CBWImage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BWImage.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

std::auto_ptr<bwPng::IPngCodec>	CBWImage::m_pPngCodec(NULL);
std::auto_ptr<bwlpacker::IPicturePacker>	CBWImage::m_pPicturePacker(NULL);

CBWImage::CBWImage()
:m_nWidth(0)
,m_nHeight(0)
,m_nPixelFormat(bwlpacker::IPicturePacker::PDF_BGR)
,m_bCreated(false)
{
}

CBWImage::~CBWImage()
{
}

bool CBWImage::LoadPng(LPCTSTR strFileName)
{
	m_bCreated = false;
	if (!m_pPngCodec.get()) 
	{
		m_pPngCodec = std::auto_ptr<bwPng::IPngCodec>(bwPng::IPngCodec::Create());	// create instance of png codec
		m_pPngCodec->SetPaletteToRGB(true);				// convert palette to RGB
	}

	std::auto_ptr<bwPng::IDataStream>	pDataStream(bwPng::IDataStream::CreateFileDataStream(strFileName));
	if (!pDataStream.get()) 
	{
		// file open error
		return false;
	}

	size_t nWidth;
	size_t nHeight;
	size_t nBitDepth;
	size_t nBufferSize;
	bwPng::IPngCodec::PixelDataFormat pdf;
	
	// get image info
	if (!m_pPngCodec->GetImageInfo(pDataStream.get(), &nWidth, &nHeight, &nBitDepth, &pdf, &nBufferSize))
	{
		return false;
	}

	m_Data.resize(nBufferSize);

	// Load image
	if (!m_pPngCodec->Decode(pDataStream.get(), &m_Data[0]))
	{
		return false;
	}

	// set corresponding pixel data format
	switch(pdf) 
	{
	case bwPng::IPngCodec::PDF_GRAY :
		m_nPixelFormat = bwlpacker::IPicturePacker::PDF_GRAY;
		break;
	case bwPng::IPngCodec::PDF_GRAY_ALPHA :
		m_nPixelFormat = bwlpacker::IPicturePacker::PDF_GRAY_ALPHA;
		break;
	case bwPng::IPngCodec::PDF_RGB :
		m_nPixelFormat = bwlpacker::IPicturePacker::PDF_RGB;
		break;
	case bwPng::IPngCodec::PDF_RGBA :
		m_nPixelFormat = bwlpacker::IPicturePacker::PDF_RGBA;
		break;
	}
	m_nWidth = nWidth;
	m_nHeight = nHeight;

	m_RGBXbuf.clear();

	CreateBitmap(false);

	m_bCreated = true;
	return true;
}

CBWImage& CBWImage::operator=(const CBWImage& other)
{
	Assign(other);
	return *this;
}

CBWImage::CBWImage(const CBWImage& other)
:m_bCreated(false)
{
	Assign(other);
}

void CBWImage::Assign(const CBWImage &other)
{
	
	m_nWidth = other.m_nWidth;
	m_nHeight = other.m_nHeight;
	m_nPixelFormat = other.m_nPixelFormat;
	m_Data = other.m_Data;
	m_bCreated = other.m_bCreated;
	m_BitmapInfo = other.m_BitmapInfo;
	m_RGBXbuf.clear();
}

bool CBWImage::CreateBitmap(bool bPacked)
{
	if (bPacked) 
	{
		return CreatePackedBitmap();
	}
	else
	{
		return CreateUnpackedBitmap();
	}
	
}

bool CBWImage::Draw(HDC destDC, int xDest, int yDest, int width, int height, int xSrc, int ySrc) 
{
/*
	if (!m_Bitmap) 
	{
		if (!CreateBitmap())
		{
			return false;
		}
	}
	return BitBlt(destDC, xDest, yDest, width, height, m_BitmapDC, xSrc, ySrc, SRCCOPY)!=0;
*/
	if (!IsCreated()) 
	{
		return false;
	}
	if (!m_RGBXbuf.size()) 
	{
		return false;
	}
	return SetDIBitsToDevice(destDC, xDest, yDest, width, height, xSrc, ySrc, 0, m_nHeight, &m_RGBXbuf[0], &m_BitmapInfo, DIB_RGB_COLORS) != 0;
}

bool CBWImage::IsCreated() const
{
	return m_bCreated;
}



bool CBWImage::CreateUnpackedBitmap(bool bCreateFromPackedData)
{
	void *pSrcData;
	std::vector<char> bufUnpacked;

	if (bCreateFromPackedData) 
	{
		bufUnpacked.resize(m_pPicturePacker->GetImageBufferSize(m_pPicturePacker->GetPackedData()));
		pSrcData = &bufUnpacked[0];
		m_pPicturePacker->Depack(m_pPicturePacker->GetPackedData(), pSrcData);
	}
	else
	{
		pSrcData = &m_Data[0];
	}
	
	size_t area = m_nWidth * m_nHeight;
	m_RGBXbuf.resize(area);

	bgrx_t	*pBGR = &m_RGBXbuf[0];
	switch(m_nPixelFormat) 
	{
	case bwlpacker::IPicturePacker::PDF_RGB:
		{
			rgb_t	*pRGB = (rgb_t*)pSrcData;
			for (size_t i = 0; i < area; i++) 
			{
				pBGR[i].r = pRGB[i].r;
				pBGR[i].g = pRGB[i].g;
				pBGR[i].b = pRGB[i].b;
			}
		}
		break;
	case bwlpacker::IPicturePacker::PDF_RGBA:
		{
			rgba_t	*pRGBA = (rgba_t*)pSrcData;
			for (size_t y = 0, i = 0; y < m_nHeight; y++) 
			{
				for (size_t x = 0; x < m_nWidth; x++, i++) 
				{
					size_t checker = ((x ^ y) & 0x8) ? 0xff : 0xcc;
					float alpha = pRGBA[i].a * (1.0f/255.0f);
					float one_minus_alpha = 1 - alpha;
					
					pBGR[i].r = (unsigned char)(pRGBA[i].r * alpha + checker * one_minus_alpha);
					pBGR[i].g = (unsigned char)(pRGBA[i].g * alpha + checker * one_minus_alpha);
					pBGR[i].b = (unsigned char)(pRGBA[i].b * alpha + checker * one_minus_alpha);
				}
			}
		}
		break;
	case bwlpacker::IPicturePacker::PDF_GRAY:
		{
			gray_t	*pGray = (gray_t*)pSrcData;
			for (size_t i = 0; i < area; i++) 
			{
				pBGR[i].r = pGray[i].l;
				pBGR[i].g = pGray[i].l;
				pBGR[i].b = pGray[i].l;
			}
		}
		break;
	case bwlpacker::IPicturePacker::PDF_GRAY_ALPHA:
		{
			gray_alpha_t	*pGrayAlpha = (gray_alpha_t*)pSrcData;
			for (size_t y = 0, i = 0; y < m_nHeight; y++) 
			{
				for (size_t x = 0; x < m_nWidth; x++, i++) 
				{
					size_t checker = ((x ^ y) & 0x8) ? 0xff : 0xcc;
					float alpha = pGrayAlpha[i].a * (1.0f/255.0f);
					float one_minus_alpha = 1 - alpha;
					
					pBGR[i].r = (unsigned char)(pGrayAlpha[i].l * alpha + checker * one_minus_alpha);
					pBGR[i].g = pBGR[i].r;
					pBGR[i].b = pBGR[i].r;
				}
			}
		}
		break;
	}

	BITMAPINFOHEADER &bih = m_BitmapInfo.bmiHeader;
	bih.biBitCount = 32;
	bih.biClrImportant = 0;
	bih.biClrUsed = 0;
	bih.biCompression = BI_RGB;
	bih.biHeight = -(int)m_nHeight;
	bih.biWidth = m_nWidth;
	bih.biPlanes = 1;
	bih.biSize = sizeof(bih);
	bih.biSizeImage = 0;
	bih.biXPelsPerMeter = 0;
	bih.biYPelsPerMeter = 0;

	BITMAPINFO bi;
	bi.bmiHeader = bih;
	

	return true;

}

bool CBWImage::CreatePackedBitmap()
{
	if (!m_pPicturePacker.get())
	{
		m_pPicturePacker = std::auto_ptr<bwlpacker::IPicturePacker>(bwlpacker::IPicturePacker::Create(2000000));	// create instance of png codec
	}
	m_pPicturePacker->Pack(&m_Data[0], GetWidth(), GetHeight(), GetPixelDataFormat());
	return CreateUnpackedBitmap(true);
}
