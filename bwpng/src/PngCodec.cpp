// PngCodec.cpp: implementation of the CPngCodec class.
//
//////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include "PngCodec.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace bwPng
{


CPngCodec::CPngCodec()
:m_bPaletteToRGB(false)
,m_nErrorCode(EC_NOERROR)
,m_bConvert16BitTo8(true)
,m_bForce8BitGrayscale(true)
,m_bConvertToScreenGamma(true)
,m_bTransparencyToAlpha(true)
,m_fScreenGamma(2.2)
,m_bConvertGrayscaleToRGB(true)
{

}

CPngCodec::~CPngCodec()
{

}

void CPngCodec::PngReadFromFile(png_structp pngnfo, png_bytep address, png_size_t readsize)
{
	IDataStream* pFile = (IDataStream*)(pngnfo->io_ptr);
	if (pFile->Read(address, readsize) != (int)readsize)
	{
		// an error occured
		png_error(pngnfo, "Read error");
	}
}

bool CPngCodec::Decode(IDataStream *pInputStream, void *pOutputData)
{
	CPngReadStruct png;
	if (!ReadPngSignature(pInputStream, png)) 
	{
		return false;
	}

	{
		CPngInfoStruct	pngInfo(png);
		
		UpdatePngInfo(png, pngInfo);

		ReadImage(png, pngInfo, pOutputData);
	}


	return SignError(EC_NOERROR);
}


void CPngCodec::UpdatePngInfo(CPngReadStruct &png, CPngInfoStruct &pngInfo)
{
	pngInfo.ReadInfo();
	png_uint_32 nWidth, nHeight;
	int nBitDepth, nColorType, nInterlaceMethod, nCompression, nFilterMethod;
	pngInfo.GetIHDR(nWidth, nHeight, nBitDepth, nColorType, nInterlaceMethod, nCompression, nFilterMethod);
	if ((nBitDepth == 16) && m_bConvert16BitTo8)
	{
		png.SetStrip16();
	}

	switch(nColorType) 
	{
	case PNG_COLOR_TYPE_PALETTE:
		if (m_bPaletteToRGB) 
		{
			png.SetPaletteToRGB();
		}
		break;
	case PNG_COLOR_TYPE_GRAY:
		if ((nBitDepth < 8) && (m_bForce8BitGrayscale))
		{
			png.UpgradeGrayscaleTo8();
		}
	}

	if (pngInfo.GetValid(PNG_INFO_tRNS) && m_bTransparencyToAlpha) 
	{
		png.SettRNSToAlpha();
	}

	if (
		((nColorType == PNG_COLOR_TYPE_GRAY) || (nColorType == PNG_COLOR_TYPE_GRAY_ALPHA))
		&& m_bConvertGrayscaleToRGB
		)
	{
		png.SetGrayscaleToRGB();
	}

	double fGamma;
	if (pngInfo.GetgAMA(fGamma) && m_bConvertToScreenGamma)
	{
		png.SetGamma(m_fScreenGamma, fGamma);
	}

	pngInfo.ReadUpdateInfo();


}

bool CPngCodec::ReadPngSignature(IDataStream *pInputStream, CPngReadStruct &png)
{
	png.SetReadFn(pInputStream, PngReadFromFile);
	
	png_byte sig[8];	// signature
	if (pInputStream->Read(sig, sizeof(sig)) != sizeof(sig))
	{
		return SignError(EC_END_OF_DATA);
	}
	
	if (!png_check_sig(sig, sizeof(sig))) 
	{
		return SignError(EC_WRONG_PNG_SIGNATURE);
	}

	png.SetSigBytes(sizeof(sig));
	return true;
}

bool CPngCodec::ReadImage(CPngReadStruct &png, CPngInfoStruct &pngInfo, void *pOutputData)
{
	png_uint_32 nWidth, nHeight;
	int nBitDepth, nColorType, nInterlaceMethod, nCompression, nFilterMethod;
	
	pngInfo.GetIHDR(nWidth, nHeight, nBitDepth, nColorType, nInterlaceMethod, nCompression, nFilterMethod);

	png_uint_32 nRowBytes = pngInfo.GetRowBytes();

	png_byte* pData =(png_byte*)pOutputData;

	m_rowPtrs.resize(nHeight);

	{
		
		for (png_uint_32 i = 0; i < nHeight; i++) 
		{
			m_rowPtrs[i] = pData + i * nRowBytes;
		}
	}

	png.ReadImage(&m_rowPtrs[0]);

	return true;
}

void CPngCodec::GetOutputImageInfo(CPngInfoStruct &pngInfo, size_t *pWidth, size_t *pHeight, size_t *pBitDepth,  PixelDataFormat *pPixelDataFormat, size_t *pMemorySize)
{
	png_uint_32 nWidth, nHeight;
	int nBitDepth, nColorType, nInterlaceMethod, nCompression, nFilterMethod;
	
	pngInfo.GetIHDR(nWidth, nHeight, nBitDepth, nColorType, nInterlaceMethod, nCompression, nFilterMethod);

	if (pWidth) 
	{
		*pWidth = nWidth;
	}

	if (pHeight) 
	{
		*pHeight = nHeight;
	}

	if (pPixelDataFormat) 
	{
		switch(nColorType) 
		{
		case PNG_COLOR_TYPE_GRAY:
			*pPixelDataFormat = PDF_GRAY;
			break;
		case PNG_COLOR_TYPE_PALETTE:
			*pPixelDataFormat = PDF_PALETTE;
			break;
		case PNG_COLOR_TYPE_RGB:
			*pPixelDataFormat = PDF_RGB;
			break;
		case PNG_COLOR_TYPE_RGB_ALPHA:
			*pPixelDataFormat = PDF_RGBA;
			break;
		case PNG_COLOR_TYPE_GRAY_ALPHA:
			*pPixelDataFormat = PDF_GRAY_ALPHA;
			break;
		}
	}

	if (pMemorySize) 
	{
		*pMemorySize = pngInfo.GetRowBytes() * nHeight;
	}

	if (pBitDepth) 
	{
		*pBitDepth = nBitDepth;
	}
	
}

bool CPngCodec::GetImageInfo(IDataStream *pInputStream, size_t *pWidth, size_t *pHeight, size_t *pBitDepth, PixelDataFormat *pPixelDataFormat, size_t *pMemorySize) 
{
	CPngReadStruct png;
	int pos = pInputStream->GetPosition();
	if (!ReadPngSignature(pInputStream, png)) 
	{
		pInputStream->Seek(pos);
		return false;
	}

	{
		CPngInfoStruct	pngInfo(png);
		
		UpdatePngInfo(png, pngInfo);

		GetOutputImageInfo(pngInfo, pWidth, pHeight, pBitDepth, pPixelDataFormat, pMemorySize);
		
	}

	pInputStream->Seek(pos);
	return SignError(EC_NOERROR);
}

IPngCodec::ErrorCode CPngCodec::GetLastError()const
{
	return m_nErrorCode;
}

bool CPngCodec::GetPaletteToRGB()const
{
	return m_bPaletteToRGB;
}

bool CPngCodec::GetConvert16BitTo8() const
{
	return m_bConvert16BitTo8;
}


bool CPngCodec::GetForce8BitGrayscale() const
{
	return m_bForce8BitGrayscale;
}

bool CPngCodec::GetTransparencyToAlpha() const
{
	return m_bTransparencyToAlpha;
}

bool CPngCodec::GetConvertGrayscaleToRGB() const
{
	return m_bConvertGrayscaleToRGB;
}

bool CPngCodec::GetConvertToScreenGamma() const
{
	return m_bConvertToScreenGamma;
}

double CPngCodec::GetScreenGamma() const
{
	return m_fScreenGamma;
}

void CPngCodec::SetScreenGamma(double fScreenGamma)
{
	m_fScreenGamma = fScreenGamma;
}

void CPngCodec::SetConvertToScreenGamma(bool bConvertToScreenGamma)
{
	m_bConvertToScreenGamma = bConvertToScreenGamma;
}

void CPngCodec::SetConvertGrayscaleToRGB(bool bGrayscaleToRgb)
{
	m_bConvertGrayscaleToRGB = bGrayscaleToRgb;
}

void CPngCodec::SetTransparencyToAlpha(bool bTransparencyToAlpha)
{
	m_bTransparencyToAlpha = bTransparencyToAlpha;
}

void CPngCodec::SetForce8BitGrayscale(bool bForce8bitGrayscale)
{
	m_bForce8BitGrayscale = bForce8bitGrayscale;
}

void CPngCodec::SetConvert16BitTo8(bool bConvert16BitTo8Bit)
{
	m_bConvert16BitTo8 = bConvert16BitTo8Bit;
}

void CPngCodec::SetPaletteToRGB(bool bPaletteToRGB)
{
	m_bPaletteToRGB = bPaletteToRGB;
}


};



