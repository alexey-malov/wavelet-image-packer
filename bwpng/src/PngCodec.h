// PngCodec.h: interface for the CPngCodec class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PNGCODEC_H__96142EA4_55F9_4C97_AFAC_334F109BDDA0__INCLUDED_)
#define AFX_PNGCODEC_H__96142EA4_55F9_4C97_AFAC_334F109BDDA0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <png.h>
#include <vector>
#include "..\INCLUDE\IPngCodec.h"
#include "../include/IDataStream.h"

namespace bwPng
{


class CPngCodec : public IPngCodec  
{
private:
	
	class CPngReadStruct
	{
	public:
		CPngReadStruct()
		{
			m_pPng = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
		}
		~CPngReadStruct()
		{
			png_destroy_read_struct(&m_pPng,0,0);
		}
		operator png_structp()
		{
			return m_pPng;
		}
		void SetReadFn(void *pIoPtr, png_rw_ptr read_data_fn)
		{
			png_set_read_fn(m_pPng, pIoPtr, read_data_fn);
		}
		void SetSigBytes(int numBytes)
		{
			png_set_sig_bytes(m_pPng, numBytes);
		}
		void SetStrip16()
		{
			png_set_strip_16(m_pPng);
		}
		void SetPaletteToRGB()
		{
			png_set_palette_to_rgb(m_pPng);
		}
		void UpgradeGrayscaleTo8()
		{
			png_set_gray_1_2_4_to_8(m_pPng);
		}
		void SettRNSToAlpha()
		{
			png_set_tRNS_to_alpha(m_pPng);
		}
		void SetGamma(double screen_gamma, double gamma)
		{
			png_set_gamma(m_pPng, screen_gamma, gamma);
		}
		void SetGrayscaleToRGB()
		{
			png_set_gray_to_rgb(m_pPng);
		}
		void ReadImage(png_bytepp rowPtrs)
		{
			png_read_image(m_pPng, rowPtrs);
		}
	private:
		png_structp m_pPng;
	};

	class CPngInfoStruct
	{
	public:
		CPngInfoStruct(CPngReadStruct &pngReadStruct)
		{
			m_pPng = pngReadStruct;
			m_pPngInfo = png_create_info_struct(m_pPng);
		}
		~CPngInfoStruct()
		{
			png_destroy_info_struct(m_pPng, &m_pPngInfo);
		}
		void ReadInfo()
		{
			png_read_info(m_pPng, m_pPngInfo);
		}
		png_uint_32 GetValid(int chunk)
		{
			return png_get_valid(m_pPng, m_pPngInfo, chunk);
		}
		png_uint_32 GetgAMA(double &fGamma)
		{
			return png_get_gAMA(m_pPng, m_pPngInfo, &fGamma);
		}
		void ReadUpdateInfo()
		{
			png_read_update_info(m_pPng, m_pPngInfo);
		}
		png_uint_32 GetRowBytes()
		{
			return png_get_rowbytes(m_pPng, m_pPngInfo);
		}
		void GetIHDR(
			png_uint_32 &nWidth, 
			png_uint_32 &nHeight,
			int &nBitDepth,
			int &nColorType,
			int &nInterlaceMethod,
			int &nCompression,
			int &nFilterMethod)
		{
			png_get_IHDR(
				m_pPng, 
				m_pPngInfo, 
				&nWidth, 
				&nHeight, 
				&nBitDepth, 
				&nColorType, 
				&nInterlaceMethod, 
				&nCompression,
				&nFilterMethod);
		}
	private:
		png_structp m_pPng;
		png_infop m_pPngInfo;
	};

public:
	virtual void SetScreenGamma(double fScreenGamma);
	virtual void SetConvertToScreenGamma(bool bConvertToScreenGamma);
	virtual void SetConvertGrayscaleToRGB(bool bGrayscaleToRgb);
	virtual void SetTransparencyToAlpha(bool bTransparencyToAlpha);
	virtual void SetForce8BitGrayscale(bool bForce8bitGrayscale);
	virtual void SetConvert16BitTo8(bool bConvert16BitTo8Bit);
	virtual void SetPaletteToRGB(bool bPaletteToRGB);

	virtual double GetScreenGamma()const;
	virtual bool GetConvertToScreenGamma()const;
	virtual bool GetConvertGrayscaleToRGB()const;
	virtual bool GetTransparencyToAlpha()const;
	virtual bool GetForce8BitGrayscale()const;
	virtual bool GetConvert16BitTo8()const;
	virtual bool GetPaletteToRGB()const;

	virtual ErrorCode GetLastError()const;
	virtual bool GetImageInfo(IDataStream *pInputStream, size_t *pWidth, size_t *pHeight, size_t *pBitDepth,  PixelDataFormat *pPixelDataFormat, size_t *pMemorySize);
	virtual bool Decode(IDataStream *pInputStream, void* pOutputData);
	CPngCodec();
	virtual ~CPngCodec();

private:
	void GetOutputImageInfo(CPngInfoStruct &pngInfo, size_t *pWidth = NULL, size_t *pHeight = NULL, size_t *pBitDepth = NULL,  PixelDataFormat *pPixelDataFormat = NULL, size_t *pMemorySize=NULL);
	bool ReadImage(CPngReadStruct &png, CPngInfoStruct &pngInfo, void *pOutputData);
	bool ReadPngSignature(IDataStream *pInputStream, CPngReadStruct &png);
	void UpdatePngInfo(CPngReadStruct &png, CPngInfoStruct &pngInfo);
	inline bool SignError(ErrorCode err)
	{
		m_nErrorCode = err;
		return err == EC_NOERROR;
	}
	static void PNGAPI PngReadFromFile(png_structp pngnfo, png_bytep address, png_size_t readsize);

	ErrorCode	m_nErrorCode;
	bool		m_bPaletteToRGB;
	bool		m_bConvert16BitTo8;
	bool		m_bForce8BitGrayscale;
	bool		m_bTransparencyToAlpha;
	bool		m_bConvertGrayscaleToRGB;
	bool		m_bConvertToScreenGamma;
	double		m_fScreenGamma;

	std::vector<png_byte*> m_rowPtrs;
};



};

#endif // !defined(AFX_PNGCODEC_H__96142EA4_55F9_4C97_AFAC_334F109BDDA0__INCLUDED_)
