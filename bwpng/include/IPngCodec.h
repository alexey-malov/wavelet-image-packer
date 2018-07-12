// IPngCodec.h: interface for the IPngCodec class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPNGCODEC_H__7E602D6C_18EE_4751_9485_62579E801DB7__INCLUDED_)
#define AFX_IPNGCODEC_H__7E602D6C_18EE_4751_9485_62579E801DB7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IDataStream.h"

namespace bwPng
{


class IPngCodec  
{
public:
	enum PixelDataFormat
	{
		PDF_GRAY,
		PDF_PALETTE,
		PDF_RGB,
		PDF_RGBA,
		PDF_GRAY_ALPHA,
	};
	enum ErrorCode
	{
		EC_NOERROR = 0,
		EC_END_OF_DATA,
		EC_WRONG_PNG_SIGNATURE,

	};
public:

	// Change screen gamma (default 2.2)
	virtual void SetScreenGamma(double fScreenGamma) = 0;

	// Enable/Disable convert to screen gamma (default: true)
	virtual void SetConvertToScreenGamma(bool bConvertToScreenGamma) = 0;

	// Enable / disable convert grayscale to RGB (default - false)
	virtual void SetConvertGrayscaleToRGB(bool bGrayscaleToRgb) = 0;

	// Enable / disable convert transparency to alpha (default - true)
	virtual void SetTransparencyToAlpha(bool bTransparencyToAlpha) = 0;

	// enable / disable forcing convert 1,2,4 Grayscale to 8 bit (default true)
	virtual void SetForce8BitGrayscale(bool bForce8bitGrayscale) = 0;

	// enable / disable Convert 16 bit components to 16 bit (default true)
	virtual void SetConvert16BitTo8(bool bConvert16BitTo8Bit) = 0;

	// enable / disable convert palette to RGB (default false)
	virtual void SetPaletteToRGB(bool bPaletteToRGB) = 0;

	// get Screen gamma (true by default)
	virtual double GetScreenGamma()const = 0;

	// get convert to screen gamma (true by default)
	virtual bool GetConvertToScreenGamma()const = 0;

	// get convert grayscale to RGB (false by default)
	virtual bool GetConvertGrayscaleToRGB()const = 0;

	// get convert transparency to alpha (true by default)
	virtual bool GetTransparencyToAlpha()const = 0;

	// get forcing convert 1,2,4 Grayscale to 8 bit (default true)
	virtual bool GetForce8BitGrayscale()const = 0;

	// Get convert 16 bit components to 8 bit (true by default)
	virtual bool GetConvert16BitTo8()const = 0;

	// get convert palette to RGB (false by default)
	virtual bool GetPaletteToRGB()const = 0;

	// returns last error
	virtual ErrorCode GetLastError()const = 0;

	// creates Instance of png codec (you need delete it when you finish work with it)
	static IPngCodec* Create();
	
	// returns image info (set null to properties of image if you don't want to use it)
	virtual bool GetImageInfo(IDataStream *pInputStream, size_t *pWidth = NULL, size_t *pHeight = NULL, size_t *pBitDepth = NULL,  PixelDataFormat *pPixelDataFormat = NULL, size_t *pMemorySize = NULL) = 0;

	// Decode image to specified buffer
	virtual bool Decode(IDataStream *pInputStream, void* pOutputData) = 0;

	// destructor
	virtual ~IPngCodec();
protected:
	// constructor (you cannot create instance of IPngCodec directly - use Create() method)
	IPngCodec();

};



};


#endif // !defined(AFX_IPNGCODEC_H__7E602D6C_18EE_4751_9485_62579E801DB7__INCLUDED_)
