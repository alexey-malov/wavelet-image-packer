// IPicturePacker.h: interface for the IPicturePacker class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPICTUREPACKER_H__7F5ABBC2_BC88_4E5F_BE34_ED7C16615015__INCLUDED_)
#define AFX_IPICTUREPACKER_H__7F5ABBC2_BC88_4E5F_BE34_ED7C16615015__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <STDIO.H>
#include "IWaveletPacker.h"

namespace bwlpacker
{


class IPicturePacker  
{
public:
	typedef IWaveletPacker::data_t data_t;
	enum ImageCompressionFlags
	{
		ICF_FORCE_YUV = 1,	// force RGB to YUV conversion
	};

	enum ForcedBitsCount
	{
		FBC_NONE = 0,
		FBC_15 = 15,
		FBC_16 = 16,
		FBC_24 = 24,
		FBC_32 = 32,
	};


	enum PixelDataFormat
	{
		// rgb formats
		PDF_R8G8B8 = 0,		// RGB
		PDF_R8G8B8X8 = 1,	// RGBX
		PDF_R8G8B8A8 = 2,	// RGBA

		// bgr formats
		PDF_B8G8R8 = 3,		// BGR
		PDF_B8G8R8X8 = 4,	// BGRX
		PDF_B8G8R8A8 = 5,	// BGRA

		// YUV formats
		PDF_LUMINANCE8 = 6,			// Grayscale
		PDF_LUMINANCE8ALPHA8 = 7,		// Grayscale+Alpha

		// 16 bit rgb formats
		PDF_B5G5R5 = 8,		//	xRRRRRGGGGGBBBBB
		PDF_B5G6R5 = 9,		//	RRRRRGGGGGGBBBBB
		PDF_B5G5R5A1 = 10,	//	ARRRRRGGGGGBBBBB

		// aliases
		PDF_RGB = PDF_R8G8B8,						// alias for R8G8B9
		PDF_BGR = PDF_B8G8R8,						// alias for B8G8R8
		PDF_RGBX = PDF_R8G8B8X8,					// alias for PDF_R8G8B8X8
		PDF_BGRX = PDF_B8G8R8X8,					// alias for PDF_B8G8R8X8
		PDF_RGBA = PDF_R8G8B8A8,					// alias for PDF_R8G8B8A8
		PDF_BGRA = PDF_B8G8R8A8,					// alias for PDF_B8G8R8A8
		PDF_LUMINANCE = PDF_LUMINANCE8,				// alias for PDF_LUMINANCE8
		PDF_LUMINANCE_ALPHA = PDF_LUMINANCE8ALPHA8,	// alias for PDF_LUMINANCE8ALPHA8
		PDF_GRAY = PDF_LUMINANCE,					// alias for PDF_LUMINANCE8
		PDF_GRAY_ALPHA = PDF_LUMINANCE_ALPHA,		// alias for PDF_LUMINANCE8ALPHA8
		PDF_RGB15 = PDF_B5G5R5,						// alias for PDF_B5G5R5
		PDF_RGB16 = PDF_B5G6R5,						// alias for PDF_B5G6R5
		PDF_RGBA16 = PDF_B5G5R5A1,					// alias for PDF_B5G5R5A1
	};
	typedef unsigned char	byte_t;
	typedef unsigned short	word_t;

	struct TypeRGB	{	byte_t r,g,b;	};
	struct TypeRGBA	{	byte_t r,g,b,a;	};
	struct TypeRGBX	{	byte_t r,g,b,x;	};

	struct TypeBGR	{	byte_t b,g,r;	};
	struct TypeBGRA	{	byte_t b,g,r,a;	};
	struct TypeBGRX	{	byte_t b,g,r,x;	};

	struct TypeL	{	byte_t l;		};
	struct TypeLA	{	byte_t l,a;		};

	typedef word_t	TypeBGR15;
	typedef word_t	TypeBGR16;
	typedef word_t	TypeBGRA16;

public:
	virtual PixelDataFormat GetDecompressedFormat(const void* pPictureData)const = 0;

	// force decompression bits (default - FBC_NONE)
	virtual void ForceDecompressionBits(ForcedBitsCount decompressionBits) = 0;

	// returns force decompression bits (default - FBC_NONE)
	virtual ForcedBitsCount GetForcedDecompressionBits()const = 0;

	// check packed data signature
	virtual bool CheckSignature(const void* pPackedData)const = 0;

	// Force BGR decompression of RGB images
	virtual void ForceBGRDecompression(bool bForceBGR=true) = 0;

	// Force RGB decompression of BGR images
	virtual void ForceRGBDecompression(bool bForceRGB=true) = 0;

	// is RGB decompression of BGR images forced
	virtual bool IsRGBDecompressionForced()const = 0;

	// is BGR decompression of RGB images forced
	virtual bool IsBGRDecompressionForced()const = 0;

	// Save to file
	virtual bool Save(const char* fileName)const = 0;

	// save to file stream
	virtual bool Save(FILE *pFile)const = 0;

	// Check if YUV compression forced (true)
	virtual bool IsYUVCompressionForced()const = 0;

	// forces RGB data to YUV format before compression
	virtual void ForceYUVCompression(bool bForceYUV=true) = 0;

	// get buffer size (in bytes) to fit depacked image
	virtual size_t GetImageBufferSize(const void *pPackedData)const = 0;

	// get buffer size (in bytes) to fit depacked image
	virtual size_t GetImageBufferSize(size_t width, size_t height, PixelDataFormat pf)const = 0;

	// Get image pixel format
	virtual PixelDataFormat GetImagePixelDataFormat(const void *pPackedData) const = 0;

	// Get packed Image Height
	virtual size_t GetImageHeight(const void *pPackedData)const = 0;

	// Get packed image width
	virtual size_t GetImageWidth(const void* pPackedData)const = 0;
	
	virtual void SetYQuantizer(data_t q)=0;	// Sets Y quantizer (0.08 is default)
	virtual void SetUQuantizer(data_t q)=0;	// Sets U quantizer (0.03 is default)
	virtual void SetVQuantizer(data_t q)=0;	// Sets V quantizer (0.03 is default)
	virtual void SetAQuantizer(data_t q)=0;	// Sets A quantizer (0.07 is default)
	virtual void SetRQuantizer(data_t q)=0;	// Sets R quantizer (0.07 is default)
	virtual void SetGQuantizer(data_t q)=0;	// Sets G quantizer (0.08 is default)
	virtual void SetBQuantizer(data_t q)=0;	// Sets B quantizer (0.05 is default)

	virtual data_t GetYQuantizer()const=0;	// Retrieves Y quantizer value
	virtual data_t GetUQuantizer()const=0;	// Retrieves U quantizer value
	virtual data_t GetVQuantizer()const=0;	// Retrieves V quantizer value
	virtual data_t GetAQuantizer()const=0;	// Retrieves A quantizer value
	virtual data_t GetRQuantizer()const=0;	// Retrieves R quantizer value
	virtual data_t GetGQuantizer()const=0;	// Retrieves G quantizer value
	virtual data_t GetBQuantizer()const=0;	// Retrieves B quantizer value

	// Retrieves packed size (call it after Pack())
	virtual size_t GetPackedSize()const=0;	

	// Get packed data address
	virtual const void* GetPackedData()const=0;	

	/*
	 *	Depack image pPackedImage to pOutput
	 *	you must provide pOutput with buffer address of enough size (see GetImageBufferSize())
	 */
	virtual bool Depack(const void *pPackedImage, void *pOutput) = 0;

	/*
	 *	Pack image with given width, height, and pixel format 
	 *	(packed data is stored inside packer - you must not free it)
	 *	use GetPackedSize() and GetPackedData() to get packed data pointer
	 */
	virtual bool Pack(const void* pImageData, size_t width, size_t height, PixelDataFormat pixelType) = 0;

	/*
	 *	Use this function to create Image packer
	 */
	static IPicturePacker* Create(size_t nPackBufferSize = 0);

	virtual void DisableTiling() = 0;
	virtual size_t GetTileSize()const = 0;
	virtual bool IsTilingEnabled()const = 0;
	virtual void EnableTiling(size_t tileSize=128) = 0;
	
	virtual ~IPicturePacker();
protected:
	IPicturePacker();
private:
};


};

#endif // !defined(AFX_IPICTUREPACKER_H__7F5ABBC2_BC88_4E5F_BE34_ED7C16615015__INCLUDED_)
