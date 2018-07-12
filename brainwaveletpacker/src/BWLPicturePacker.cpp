// BWLPicturePacker.cpp: implementation of the CBWLPicturePacker class.
//
//////////////////////////////////////////////////////////////////////

#include "bwl_includes.h"
#include "BWLPicturePacker.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace bwlpacker
{


CBWLPicturePacker::CBWLPicturePacker(size_t nPackBufferSize)
:m_pPacker(IArbitraryWaveletPacker::Create(nPackBufferSize))
,m_bForceYUVCompression(true)
,m_bForceBGRDecompression(false)
,m_bForceRGBDecompression(false)
,m_nForceDecompressionBits(FBC_NONE)
,m_yq(0.06f)
,m_uq(0.025f)
,m_vq(0.025f)
,m_aq(0.06f)
,m_rq(0.05f)
,m_gq(0.07f)
,m_bq(0.04f)
,m_pPackedDataPointer(NULL)
{

}

CBWLPicturePacker::~CBWLPicturePacker()
{

}

CBWLPicturePacker* CBWLPicturePacker::Create(size_t packBufferSize)
{
	return new CBWLPicturePacker(packBufferSize);
}

bool CBWLPicturePacker::Pack(const void *pImageData, size_t width, size_t height, PixelDataFormat pixelType)
{
	// then we need to load data from byte buffers to float buffers
	ImageHeader imgHdr;
	imgHdr.width = width;
	imgHdr.height = height;
	imgHdr.dataFormat = pixelType;
	imgHdr.dwFlags = m_bForceYUVCompression ? ICF_FORCE_YUV : 0;

	RewritePackBuffer();
	WritePackBuffer(&imgHdr, sizeof(imgHdr));

	LoadDataIntoBuffersAndPack(pixelType, pImageData, width, height);
	return true;
}

void CBWLPicturePacker::ResizeBuffers(PixelDataFormat pf, size_t newSize)
{
	switch(pf) 
	{
	case PDF_R8G8B8A8:
	case PDF_B8G8R8A8:
		ResizeBuffer(BUFFER_3, newSize);
	case PDF_R8G8B8X8:
	case PDF_B8G8R8X8:
	case PDF_R8G8B8:
	case PDF_B8G8R8:
		ResizeBuffer(BUFFER_2, newSize);
		ResizeBuffer(BUFFER_1, newSize);
	case PDF_LUMINANCE8:
		ResizeBuffer(BUFFER_0, newSize);
		break;
	case PDF_LUMINANCE8ALPHA8:
		ResizeBuffer(BUFFER_0, newSize);
		ResizeBuffer(BUFFER_3, newSize);
		break;
	}
}

void CBWLPicturePacker::ResizeBuffer(BufferIndex index, size_t newSize)
{
	if (m_Components[index].size() < newSize) 
	{
		m_Components[index].resize(newSize);
	}
}

void CBWLPicturePacker::LoadDataIntoBuffersAndPack(PixelDataFormat pf, const void *pData, size_t width, size_t height)
{
	size_t dataSize = width * height;
	ResizeBuffers(pf, dataSize);

	switch(pf) 
	{
	case PDF_R8G8B8:		// RGB
		if (m_bForceYUVCompression) 
		{
			RGB2YUV(pData, dataSize);
			PackYUVBuffers(width, height);
		}
		else
		{
			LoadRGBX_BGRX_RGB_BGR((const TypeRGB*)pData, dataSize);
			PackRGBBuffers(width, height);
		}
		break;
	case PDF_B8G8R8:		// BGR
		if (m_bForceYUVCompression) 
		{
			BGR2YUV(pData, dataSize);
			PackYUVBuffers(width, height);
		}
		else
		{
			LoadRGBX_BGRX_RGB_BGR((const TypeBGR*)pData, dataSize);
			PackRGBBuffers(width, height);
		}
		break;
	case PDF_R8G8B8X8:		// RGBX
		if (m_bForceYUVCompression) 
		{
			RGBX2YUV(pData, dataSize);

			PackYUVBuffers(width, height);
		}
		else
		{
			LoadRGBX_BGRX_RGB_BGR((const TypeRGBX*)pData, dataSize);

			PackRGBBuffers(width, height);
		}
		break;
	case PDF_B8G8R8X8:		// BGRX
		if (m_bForceYUVCompression) 
		{
			BGRX2YUV(pData, dataSize);

			PackYUVBuffers(width, height);
		}
		else
		{
			LoadRGBX_BGRX_RGB_BGR((const TypeBGRX*)pData, dataSize);
			PackRGBBuffers(width, height);
		}
		break;
	case PDF_R8G8B8A8:		// RGBA
		if (m_bForceYUVCompression) 
		{
			RGBA2YUVA(pData, dataSize);

			PackYUVABuffers(width, height);
		}
		else
		{
			LoadRGBA_BGRA((const TypeRGBA*)pData, dataSize);

			PackRGBABuffers(width, height);
		}
		break;
	case PDF_B8G8R8A8:		// BGRA
		if (m_bForceYUVCompression) 
		{
			BGRA2YUVA(pData, dataSize);

			PackYUVABuffers(width, height);
		}
		else
		{
			LoadRGBA_BGRA((const TypeBGRA*)pData, dataSize);

			PackRGBABuffers(width, height);
		}
		break;

		// YUV formats
	case PDF_LUMINANCE8:		// Grayscale
		LoadL((const TypeL*)pData, dataSize);
		PackBuffer(Y_BUFFER, width, height, m_yq);
		break;
	case PDF_LUMINANCE8ALPHA8:		// Grayscale+Alpha
		LoadLA((const TypeLA*)pData, dataSize);

		PackBuffer(Y_BUFFER, width, height, m_yq);
		PackBuffer(A_BUFFER, width, height, m_aq);
		break;

		// 16 bit buffers
	case PDF_B5G5R5:
		if (m_bForceYUVCompression) 
		{
			BGR15ToYUV(pData, dataSize);
			PackYUVBuffers(width, height);
		}
		else
		{
			LoadBGR15((const TypeBGRA*)pData, dataSize);
			PackRGBBuffers(width, height);
		}
		break;
	case PDF_B5G6R5:
		if (m_bForceYUVCompression) 
		{
			BGR16ToYUV(pData, dataSize);
			PackYUVBuffers(width, height);
		}
		else
		{
			LoadBGR16((const TypeBGRA*)pData, dataSize);
			PackRGBBuffers(width, height);
		}
		break;
	case PDF_B5G5R5A1:
		if (m_bForceYUVCompression) 
		{
			BGRA16ToYUVA(pData, dataSize);
			PackYUVABuffers(width, height);
		}
		else
		{
			LoadBGRA16((const TypeBGRA*)pData, dataSize);
			PackRGBABuffers(width, height);
		}
		break;

	default:
		break;
	}
}



void CBWLPicturePacker::RGBA2YUVA(const void *pRGBA, size_t num_coeffs)
{
	ToYUVA((const TypeRGBA*)pRGBA, num_coeffs);
}

void CBWLPicturePacker::BGRA2YUVA(const void *pBGRA, size_t num_coeffs)
{
	ToYUVA((const TypeBGRA*)pBGRA, num_coeffs);
}

void CBWLPicturePacker::RGBX2YUV(const void *pRGBX, size_t num_coeffs)
{
	ToYUV((const TypeRGBX*)pRGBX, num_coeffs);
}

void CBWLPicturePacker::RGB2YUV(const void *pRGB, size_t num_coeffs)
{
	ToYUV((const TypeRGB*)pRGB, num_coeffs);
}

void CBWLPicturePacker::BGRX2YUV(const void *pBGRX, size_t num_coeffs)
{
	ToYUV((const TypeBGRX*)pBGRX, num_coeffs);
}

void CBWLPicturePacker::BGR2YUV(const void *pBGR, size_t num_coeffs)
{
	ToYUV((const TypeBGR*)pBGR, num_coeffs);
}

void CBWLPicturePacker::YUVA2RGBA(void *pRGBA, size_t numCoeffs)
{
	ToRGBA_BGRA((TypeRGBA*)pRGBA, numCoeffs);

}

void CBWLPicturePacker::YUVA2BGRA(void *pBGRA, size_t numCoeffs)
{
	ToRGBA_BGRA((TypeBGRA*)pBGRA, numCoeffs);

}

void CBWLPicturePacker::YUV2RGBX(void *pRGBX, size_t numCoeffs)
{
	ToRGBX_BGRX((TypeRGBX*)pRGBX, numCoeffs);

}

void CBWLPicturePacker::YUV2BGRX(void *pBGRX, size_t numCoeffs)
{
	ToRGBX_BGRX((TypeBGRX*)pBGRX, numCoeffs);

}

void CBWLPicturePacker::YUV2RGB(void *pRGB, size_t numCoeffs)
{
	ToRGB_BGR((TypeRGB*)pRGB, numCoeffs);

}

void CBWLPicturePacker::YUV2BGR(void *pBGR, size_t numCoeffs)
{
	ToRGB_BGR((TypeBGR*)pBGR, numCoeffs);

}

void CBWLPicturePacker::LoadL(const TypeL *p, size_t numCoeffs)
{
	data_t *pL = GetComponentBuffer(L_BUFFER);
	data_t *pEndOfLBuffer = pL + numCoeffs;
	do 
	{
		*pL = p->l;
		p++;
	} while(++pL < pEndOfLBuffer);
	
}

void CBWLPicturePacker::LoadLA(const TypeLA *p, size_t numCoeffs)
{
	data_t *pL = GetComponentBuffer(L_BUFFER);
	data_t *pA = GetComponentBuffer(A_BUFFER);

	data_t *pEndOfLBuffer = pL + numCoeffs;
	do 
	{
		*(pL++) = p->l;
		*(pA++) = p->a;
		p++;
	} while(pL < pEndOfLBuffer);

}

void CBWLPicturePacker::SaveL(TypeL *p, size_t numCoeffs)const
{
	const data_t *pL = GetComponentBuffer(L_BUFFER);

	const data_t *pEndOfLBuffer = pL + numCoeffs;
	do 
	{
		p->l = (*pL < 0) ? 0
			: (*pL <= 255) ? (byte_t)*pL
			: 255;
		pL++;
		p++;
	} while(pL < pEndOfLBuffer);

}

void CBWLPicturePacker::SaveLA(TypeLA *p, size_t numCoeffs)const
{
	const data_t *pL = GetComponentBuffer(L_BUFFER);
	const data_t *pA = GetComponentBuffer(A_BUFFER);

	const data_t *pEndOfLBuffer = pL + numCoeffs;
	do 
	{
		p->l = (*pL < 0) ? 0
			: (*pL <= 255) ? (byte_t)*pL
			: 255;
		p->a = (*pA < 0) ? 0
			: (*pA <= 255) ? (byte_t)*pA
			: 255;
		pL++;
		pA++;
		p++;
	} while(pL < pEndOfLBuffer);

}

void CBWLPicturePacker::PackBuffer(BufferIndex index, size_t width, size_t height, data_t quantizer)
{
	BlockHeader hdr;
	m_pPacker->Pack(GetComponentBuffer(index), width, height, quantizer);
	hdr.packedSize = m_pPacker->GetPackedSize();
	WritePackBuffer(&hdr, sizeof(hdr));
	WritePackBuffer(m_pPacker->GetPackedData(), hdr.packedSize);
}

void CBWLPicturePacker::DepackBuffer(BufferIndex index)
{

	const BlockHeader * pHeader = (const BlockHeader *)ReadPackedStream(sizeof(BlockHeader));
	m_pPacker->Depack(ReadPackedStream(pHeader->packedSize), GetComponentBuffer(index));
}


void CBWLPicturePacker::RewritePackBuffer()
{
	m_PackBuffer.Rewrite();
}

size_t CBWLPicturePacker::WritePackBuffer(const void *pData, size_t numBytesToWrite)
{
	return m_PackBuffer.Write(pData, numBytesToWrite);
}

void CBWLPicturePacker::PackYUVBuffers(size_t width, size_t height)
{
	PackBuffer(Y_BUFFER, width, height, m_yq);
	PackBuffer(U_BUFFER, width, height, m_uq);
	PackBuffer(V_BUFFER, width, height, m_vq);
}

void CBWLPicturePacker::PackRGBBuffers(size_t width, size_t height)
{
	PackBuffer(R_BUFFER, width, height, m_rq);
	PackBuffer(G_BUFFER, width, height, m_gq);
	PackBuffer(B_BUFFER, width, height, m_bq);

}

void CBWLPicturePacker::PackRGBABuffers(size_t width, size_t height)
{
	PackBuffer(R_BUFFER, width, height, m_rq);
	PackBuffer(G_BUFFER, width, height, m_gq);
	PackBuffer(B_BUFFER, width, height, m_bq);
	PackBuffer(A_BUFFER, width, height, m_aq);

}

void CBWLPicturePacker::PackYUVABuffers(size_t width, size_t height)
{
	PackBuffer(Y_BUFFER, width, height, m_yq);
	PackBuffer(U_BUFFER, width, height, m_uq);
	PackBuffer(V_BUFFER, width, height, m_vq);
	PackBuffer(A_BUFFER, width, height, m_aq);

}

void CBWLPicturePacker::OpenPackedStream(const void *pPackedStream)
{
	m_pPackedDataPointer = (const byte_t*)pPackedStream;
}

const void* CBWLPicturePacker::ReadPackedStream(size_t numBytesToRead) const
{
	const void* pData = m_pPackedDataPointer;
	m_pPackedDataPointer += numBytesToRead;
	return pData;
}

void CBWLPicturePacker::DepackYUVABuffers()
{
	DepackBuffer(Y_BUFFER);
	DepackBuffer(U_BUFFER);
	DepackBuffer(V_BUFFER);
	DepackBuffer(A_BUFFER);
}
void CBWLPicturePacker::DepackRGBABuffers()
{
	DepackBuffer(R_BUFFER);
	DepackBuffer(G_BUFFER);
	DepackBuffer(B_BUFFER);
	DepackBuffer(A_BUFFER);
}
void CBWLPicturePacker::DepackRGBBuffers()
{
	DepackBuffer(R_BUFFER);
	DepackBuffer(G_BUFFER);
	DepackBuffer(B_BUFFER);
}
void CBWLPicturePacker::DepackYUVBuffers()
{
	DepackBuffer(Y_BUFFER);
	DepackBuffer(U_BUFFER);
	DepackBuffer(V_BUFFER);
}

void CBWLPicturePacker::DepackAndSaveBuffers(PixelDataFormat pf, size_t width, size_t height, void *pOutput, uint_t dwFlags)
{
	size_t dataSize = width * height;
	ResizeBuffers(pf, dataSize);

	BOOL bForceYUV = dwFlags & ICF_FORCE_YUV;

	pf = ChooseDecompressionFormat(pf);



	switch(pf) 
	{
	case PDF_R8G8B8:		// RGB
		if (bForceYUV) 
		{
			DepackYUVBuffers();
			YUV2RGB(pOutput, dataSize);
		}
		else
		{
			DepackRGBBuffers();
			SaveRGB_BGR((TypeRGB*)pOutput, dataSize);
		}
		break;
	case PDF_B8G8R8:		// BGR
		if (bForceYUV) 
		{
			DepackYUVBuffers();
			YUV2BGR(pOutput, dataSize);
		}
		else
		{
			DepackRGBBuffers();
			SaveRGB_BGR((TypeBGR*)pOutput, dataSize);
		}
		break;
	case PDF_R8G8B8X8:		// RGBX
		if (bForceYUV) 
		{
			DepackYUVBuffers();
			YUV2RGBX(pOutput, dataSize);
		}
		else
		{
			DepackRGBBuffers();
			SaveRGBX_BGRX((TypeRGBX*)pOutput, dataSize);
		}
		break;
	case PDF_B8G8R8X8:		// BGRX
		if (bForceYUV) 
		{
			DepackYUVBuffers();
			YUV2BGRX(pOutput, dataSize);
		}
		else
		{
			DepackRGBBuffers();
			SaveRGBX_BGRX((TypeBGRX*)pOutput, dataSize);
		}
		break;
	case PDF_R8G8B8A8:		// RGBA
		if (bForceYUV) 
		{
			DepackYUVABuffers();
			YUVA2RGBA(pOutput, dataSize);
		}
		else
		{
			DepackRGBABuffers();
			SaveRGBA_BGRA((TypeRGBA*)pOutput, dataSize);
		}
		break;
	case PDF_B8G8R8A8:		// BGRA
		if (bForceYUV) 
		{
			DepackYUVABuffers();
			YUVA2BGRA(pOutput, dataSize);
		}
		else
		{
			DepackRGBABuffers();
			SaveRGBA_BGRA((TypeBGRA*)pOutput, dataSize);
		}
		break;

		// YUV formats
	case PDF_LUMINANCE8:		// Grayscale
		DepackBuffer(Y_BUFFER);
		SaveL((TypeL*)pOutput, dataSize);
		break;
	case PDF_LUMINANCE8ALPHA8:		// Grayscale+Alpha
		DepackBuffer(Y_BUFFER);
		DepackBuffer(A_BUFFER);
		SaveLA((TypeLA*)pOutput, dataSize);
		break;

		// 16 bit buffers
	case PDF_B5G5R5:
		if (m_bForceYUVCompression) 
		{
			DepackYUVBuffers();
			YUVToBGR15(pOutput, dataSize);
		}
		else
		{
			DepackRGBBuffers();
			SaveBGR15(pOutput, dataSize);
		}
		break;
	case PDF_B5G6R5:
		if (m_bForceYUVCompression) 
		{
			DepackYUVBuffers();
			YUVToBGR16(pOutput, dataSize);
		}
		else
		{
			DepackRGBBuffers();
			SaveBGR16(pOutput, dataSize);
		}
		break;
	case PDF_B5G5R5A1:
		if (m_bForceYUVCompression) 
		{
			DepackYUVABuffers();
			YUVAToBGRA16(pOutput, dataSize);
		}
		else
		{
			DepackRGBABuffers();
			SaveBGRA16(pOutput, dataSize);
		}
		break;
	default:
		break;
	}

}

bool CBWLPicturePacker::Depack(const void *pPackedImage, void *pOutput)
{
	OpenPackedStream(pPackedImage);
	const ImageHeader* pImageHeader = (const ImageHeader*)ReadPackedStream(sizeof(ImageHeader));
	DepackAndSaveBuffers(pImageHeader->dataFormat, pImageHeader->width, pImageHeader->height, pOutput, pImageHeader->dwFlags);
	return true;
}

const void* CBWLPicturePacker::GetPackedData() const
{
	return m_PackBuffer.GetData();
}

size_t CBWLPicturePacker::GetPackedSize() const
{
	return m_PackBuffer.GetSize();
}

void CBWLPicturePacker::SetYQuantizer(data_t q)
{
	m_yq = q;
}

void CBWLPicturePacker::SetUQuantizer(data_t q)
{
	m_uq = q;
}

void CBWLPicturePacker::SetVQuantizer(data_t q)
{
	m_vq = q;
}

void CBWLPicturePacker::SetAQuantizer(data_t q)
{
	m_aq = q;
}

void CBWLPicturePacker::SetRQuantizer(data_t q)
{
	m_rq = q;
}

void CBWLPicturePacker::SetGQuantizer(data_t q)
{
	m_gq = q;
}

void CBWLPicturePacker::SetBQuantizer(data_t q)
{
	m_bq = q;
}

IPicturePacker::data_t CBWLPicturePacker::GetBQuantizer()const
{
	return m_bq;
}

IPicturePacker::data_t CBWLPicturePacker::GetGQuantizer()const
{
	return m_gq;
}

IPicturePacker::data_t CBWLPicturePacker::GetRQuantizer()const
{
	return m_rq;
}

IPicturePacker::data_t CBWLPicturePacker::GetAQuantizer()const
{
	return m_aq;
}

IPicturePacker::data_t CBWLPicturePacker::GetYQuantizer()const
{
	return m_yq;
}

IPicturePacker::data_t CBWLPicturePacker::GetUQuantizer()const
{
	return m_uq;
}

IPicturePacker::data_t CBWLPicturePacker::GetVQuantizer()const
{
	return m_vq;
}


size_t CBWLPicturePacker::GetImageWidth(const void *pPackedData) const
{
	return ((const ImageHeader*)pPackedData)->width;
}

size_t CBWLPicturePacker::GetImageHeight(const void *pPackedData) const
{
	return ((const ImageHeader*)pPackedData)->height;
}

IPicturePacker::PixelDataFormat CBWLPicturePacker::GetImagePixelDataFormat(const void *pPackedData) const
{
	return ((const ImageHeader*)pPackedData)->dataFormat;
}

size_t CBWLPicturePacker::GetImageBufferSize(size_t width, size_t height, PixelDataFormat pf)const
{
	size_t numBytes = 0;
	
	pf = ChooseDecompressionFormat(pf);

	switch(pf) 
	{
	case PDF_R8G8B8:		// RGB
	case PDF_B8G8R8:		// BGR
		numBytes = 3;
		break;
	case PDF_R8G8B8X8:	// RGBX
	case PDF_B8G8R8X8:	// BGRX
	case PDF_R8G8B8A8:	// RGBA
	case PDF_B8G8R8A8:	// BGRA
		numBytes = 4;
		break;

		// YUV formats
	case PDF_LUMINANCE8:			// Grayscale
		numBytes = 1;
		break;
	case PDF_LUMINANCE8ALPHA8:		// Grayscale+Alpha
	case PDF_B5G5R5:
	case PDF_B5G6R5:
	case PDF_B5G5R5A1:
		numBytes = 2;
		break;
	}

	return numBytes * width * height;
}

size_t CBWLPicturePacker::GetImageBufferSize(const void *pPackedData)const
{
	const ImageHeader * pHeader = (const ImageHeader*)pPackedData;	

	return GetImageBufferSize(pHeader->width, pHeader->height, pHeader->dataFormat);
}

void CBWLPicturePacker::ForceYUVCompression(bool bForceYUV)
{
	m_bForceYUVCompression = bForceYUV;
}

bool CBWLPicturePacker::IsYUVCompressionForced() const
{
	return m_bForceYUVCompression;
}

bool CBWLPicturePacker::Save(FILE *pFile) const
{
	if (!pFile) 
	{
		return false;
	}
	return (fwrite(GetPackedData(), 1, GetPackedSize(), pFile) == GetPackedSize());
}

bool CBWLPicturePacker::Save(const char *fileName) const
{
	FILE *pFile = fopen(fileName, "wb");
	if (!pFile) 
	{
		return false;
	}
	bool bresult = Save(pFile);
	fclose(pFile);
	return bresult;
}

void CBWLPicturePacker::ForceRGBDecompression(bool bForceRGB)
{
	if ((m_bForceRGBDecompression = bForceRGB))
	{
		m_bForceBGRDecompression = false;
	}
}

void CBWLPicturePacker::ForceBGRDecompression(bool bForceBGR)
{
	if ((m_bForceBGRDecompression = bForceBGR)) 
	{
		m_bForceRGBDecompression = false;
	}
}


bool CBWLPicturePacker::IsRGBDecompressionForced()const
{
	return m_bForceRGBDecompression;
}

bool CBWLPicturePacker::IsBGRDecompressionForced()const
{
	return m_bForceBGRDecompression;
}

void CBWLPicturePacker::ForceDecompressionBits(ForcedBitsCount decompressionBits)
{
	m_nForceDecompressionBits = decompressionBits;
}

IPicturePacker::ForcedBitsCount CBWLPicturePacker::GetForcedDecompressionBits()const
{
	return m_nForceDecompressionBits;
}


bool CBWLPicturePacker::CheckSignature(const void *pPackedData) const
{
	return *((size_t*)pPackedData) == 'PLWB';
}

void CBWLPicturePacker::LoadBGR15(const void* p_, size_t numCoeffs)
{
	data_t *pR = GetComponentBuffer(R_BUFFER);
	data_t *pG = GetComponentBuffer(G_BUFFER);
	data_t *pB = GetComponentBuffer(B_BUFFER);
	const TypeBGR15 *p = static_cast<const TypeBGR15*>(p_);

	for (size_t i = 0; i < numCoeffs; i++, p++) 
	{
		pR[i] = ((*p) & (0x1f << 10)) * (255.0f / (0x1f << 10));
		pG[i] = ((*p) & (0x1f << 5)) * (255.0f / (0x1f << 5));
		pB[i] = ((*p) & 0x1f) * (255.0f / 0x1f);
	}

}

void CBWLPicturePacker::LoadBGR16(const void* p_, size_t numCoeffs)
{
	data_t *pR = GetComponentBuffer(R_BUFFER);
	data_t *pG = GetComponentBuffer(G_BUFFER);
	data_t *pB = GetComponentBuffer(B_BUFFER);
	const TypeBGR16 *p = static_cast<const TypeBGR16*>(p_);

	for (size_t i = 0; i < numCoeffs; i++, p++) 
	{
		pR[i] = ((*p) & (0x1f << 11)) * (255.0f / (0x1f << 11));
		pG[i] = ((*p) & (0x3f << 5)) * (255.0f / (0x3f << 5));
		pB[i] = ((*p) & 0x1f) * (255.0f / 0x1f);
	}
}

void CBWLPicturePacker::LoadBGRA16(const void* p_, size_t numCoeffs )
{
	data_t *pR = GetComponentBuffer(R_BUFFER);
	data_t *pG = GetComponentBuffer(G_BUFFER);
	data_t *pB = GetComponentBuffer(B_BUFFER);
	data_t *pA = GetComponentBuffer(A_BUFFER);
	const TypeBGRA16 *p = static_cast<const TypeBGRA16*>(p_);

	for (size_t i = 0; i < numCoeffs; i++, p++) 
	{
		pR[i] = ((*p) & (0x1f << 10)) * (255.0f / (0x1f << 10));
		pG[i] = ((*p) & (0x1f << 5)) * (255.0f / (0x1f << 5));
		pB[i] = ((*p) & 0x1f) * (255.0f / 0x1f);
		pA[i] = ((*p) & 0x8000) ? 255.0f : 0.0f;
	}
}

/*
void CBWLPicturePacker::LoadBGR15(const void* p_, size_t numCoeffs)
{
	data_t *pR = GetComponentBuffer(R_BUFFER);
	data_t *pG = GetComponentBuffer(G_BUFFER);
	data_t *pB = GetComponentBuffer(B_BUFFER);
	const TypeBGR15 *p = static_cast<const TypeBGR15*>(p_);

	for (size_t i = 0; i < numCoeffs; i++, p++) 
	{
		pR[i] = ((*p) & (0x1f << 10)) * (255.0f / (0x1f << 10));
		pG[i] = ((*p) & (0x1f << 5)) * (255.0f / (0x1f << 5));
		pB[i] = ((*p) & 0x1f) * (255.0f / 0x1f);
	}

}

*/
void CBWLPicturePacker::SaveBGR15(void* p_, size_t numCoeffs)
{
	const data_t *pR = GetComponentBuffer(R_BUFFER);
	const data_t *pG = GetComponentBuffer(G_BUFFER);
	const data_t *pB = GetComponentBuffer(B_BUFFER);
	TypeBGR15 *p = static_cast<TypeBGR15*>(p_);

	for (size_t i = 0; i < numCoeffs; i++, p++) 
	{
		int r = (int)pR[i];
		r = (r <= 0) ? 0
			: (r < 255) ? (r * (0x1f << 10) / 255) & (0x1f << 10)
			: (0x1f << 10);

		int g = (int)pG[i];
		g = (g <= 0) ? 0
			: (g < 255) ? (g * (0x1f << 5) / 255) & (0x1f << 5)
			: (0x1f << 5);

		int b = (int)pB[i];
		b = (b <= 0) ? 0
			: (r < 255) ? (b * (0x1f) / 255) & (0x1f)
			: (0x1f << 10);

		*p = r | g | b;
	}

}

void CBWLPicturePacker::SaveBGR16(void* p_, size_t numCoeffs)
{
	const data_t *pR = GetComponentBuffer(R_BUFFER);
	const data_t *pG = GetComponentBuffer(G_BUFFER);
	const data_t *pB = GetComponentBuffer(B_BUFFER);
	TypeBGR16 *p = static_cast<TypeBGR16*>(p_);

	for (size_t i = 0; i < numCoeffs; i++, p++) 
	{
		int r = (int)pR[i];
		r = (r <= 0) ? 0
			: (r < 255) ? (r * (0x1f << 11) / 255) & (0x1f << 11)
			: (0x1f << 11);

		int g = (int)pG[i];
		g = (g <= 0) ? 0
			: (g < 255) ? (g * (0x3f << 5) / 255) & (0x3f << 5)
			: (0x3f << 5);

		int b = (int)pB[i];
		b = (b <= 0) ? 0
			: (r < 255) ? (b * (0x1f) / 255) & (0x1f)
			: (0x1f << 10);

		*p = r | g | b;
	}
}

void CBWLPicturePacker::SaveBGRA16(void* p_, size_t numCoeffs )
{
	const data_t *pR = GetComponentBuffer(R_BUFFER);
	const data_t *pG = GetComponentBuffer(G_BUFFER);
	const data_t *pB = GetComponentBuffer(B_BUFFER);
	const data_t *pA = GetComponentBuffer(A_BUFFER);
	TypeBGRA16 *p = static_cast<TypeBGRA16*>(p_);

	for (size_t i = 0; i < numCoeffs; i++, p++) 
	{
		int r = (int)pR[i];
		r = (r <= 0) ? 0
			: (r < 255) ? (r * (0x1f << 10) / 255) & (0x1f << 10)
			: (0x1f << 10);

		int g = (int)pG[i];
		g = (g <= 0) ? 0
			: (g < 255) ? (g * (0x1f << 5) / 255) & (0x1f << 5)
			: (0x1f << 5);

		int b = (int)pB[i];
		b = (b <= 0) ? 0
			: (r < 255) ? (b * (0x1f) / 255) & (0x1f)
			: (0x1f << 10);

		*p = r | g | b | ((pA[i] < 128.0f) ? 0 : 0x8000);
	}
}



void CBWLPicturePacker::BGR15ToYUV(const void* p_, size_t numCoeffs )
{
	data_t *pY = GetComponentBuffer(Y_BUFFER);
	data_t *pU = GetComponentBuffer(U_BUFFER);
	data_t *pV = GetComponentBuffer(V_BUFFER);
	const TypeBGR15 *p = static_cast<const TypeBGR15*>(p_);

	for (size_t i = 0; i < numCoeffs; i++, p++) 
	{
		data_t r = ((*p) & (0x1f << 10)) * (255.0f / (0x1f << 10));
		data_t g = ((*p) & (0x1f << 5)) * (255.0f / (0x1f << 5));
		data_t b = ((*p) & 0x1f) * (255.0f / 0x1f);
		pY[i] = ((data_t)0.299) * r + ((data_t)0.587) * g + ((data_t)0.114) * b;
		pU[i] = r - pY[i];
		pV[i] = b - pY[i];
	}

}

void CBWLPicturePacker::BGR16ToYUV(const void* p_, size_t numCoeffs )
{
	data_t *pY = GetComponentBuffer(Y_BUFFER);
	data_t *pU = GetComponentBuffer(U_BUFFER);
	data_t *pV = GetComponentBuffer(V_BUFFER);
	const TypeBGR16 *p = static_cast<const TypeBGR16*>(p_);

	for (size_t i = 0; i < numCoeffs; i++, p++) 
	{
		data_t r = ((*p) & (0x1f << 11)) * (255.0f / (0x1f << 11));
		data_t g = ((*p) & (0x3f << 5)) * (255.0f / (0x3f << 5));
		data_t b = ((*p) & 0x1f) * (255.0f / 0x1f);
		pY[i] = ((data_t)0.299) * r + ((data_t)0.587) * g + ((data_t)0.114) * b;
		pU[i] = r - pY[i];
		pV[i] = b - pY[i];
	}
}

void CBWLPicturePacker::BGRA16ToYUVA(const void* p_, size_t numCoeffs )
{
	data_t *pY = GetComponentBuffer(Y_BUFFER);
	data_t *pU = GetComponentBuffer(U_BUFFER);
	data_t *pV = GetComponentBuffer(V_BUFFER);
	data_t *pA = GetComponentBuffer(A_BUFFER);
	const TypeBGRA16 *p = static_cast<const TypeBGRA16*>(p_);

	for (size_t i = 0; i < numCoeffs; i++, p++) 
	{
		data_t r = ((*p) & (0x1f << 10)) * (255.0f / (0x1f << 10));
		data_t g = ((*p) & (0x1f << 5)) * (255.0f / (0x1f << 5));
		data_t b = ((*p) & 0x1f) * (255.0f / 0x1f);
		pY[i] = ((data_t)0.299) * r + ((data_t)0.587) * g + ((data_t)0.114) * b;
		pU[i] = r - pY[i];
		pV[i] = b - pY[i];
		pA[i] = ((*p) & 0x8000) ? 255.0f : 0.0f;
	}
}

// convert YUV float to BGR15 word 
void CBWLPicturePacker::YUVToBGR15(void* p_, size_t numCoeffs)
{
	const data_t *pY = GetComponentBuffer(Y_BUFFER);
	const data_t *pU = GetComponentBuffer(U_BUFFER);
	const data_t *pV = GetComponentBuffer(V_BUFFER);

	TypeBGR15 *p = static_cast<TypeBGR15*>(p_);

	for (size_t i = 0; i < numCoeffs; i++, p++) 
	{
		data_t y = pY[i];
		data_t u = pU[i];
		data_t v = pV[i];

		int r = (int)(y + u);
		r = (r <= 0) ? 0
			: (r < 255) ? ((r * (0x1f << 10)) / 255) & (0x1f << 10)
			: (0x1f << 10);

		int g = (int)(y - ((data_t)0.509) * u - ((data_t)0.194) * v);
		g = (g <= 0) ? 0
			: (g < 255) ? ((g * (0x1f << 5)) / 255) & (0x1f << 5)
			: (0x1f << 5);

		int b = (int)(y + v);
		b = (b <= 0) ? 0
			: (b < 255) ? ((b * 0x1f) / 255) & (0x1f)
			: 0x1f;

		*p = r | g | b;
	}
}

// convert YUV float to BGR16 word
void CBWLPicturePacker::YUVToBGR16(void* p_, size_t numCoeffs)
{
	const data_t *pY = GetComponentBuffer(Y_BUFFER);
	const data_t *pU = GetComponentBuffer(U_BUFFER);
	const data_t *pV = GetComponentBuffer(V_BUFFER);

	TypeBGR16 *p = static_cast<TypeBGR16*>(p_);

	for (size_t i = 0; i < numCoeffs; i++, p++) 
	{
		data_t y = pY[i];
		data_t u = pU[i];
		data_t v = pV[i];

		int r = (int)(y + u);
		r = (r <= 0) ? 0
			: (r < 255) ? (r * (0x1f << 11) / 255) & (0x1f << 11)
			: (0x1f << 11);

		int g = (int)(y - ((data_t)0.509) * u - ((data_t)0.194) * v);
		g = (g <= 0) ? 0
			: (g < 255) ? (g * (0x3f << 5) / 255) & (0x3f << 5)
			: (0x3f << 5);

		int b = (int)(y + v);
		b = (b <= 0) ? 0
			: (b < 255) ? (b * (0x1f) / 255) & (0x1f)
			: 0x1f;

		*p = r | g | b;
	}
}

// convert YUV float to BGRA16
void CBWLPicturePacker::YUVAToBGRA16(void* p_, size_t numCoeffs)
{
	const data_t *pY = GetComponentBuffer(Y_BUFFER);
	const data_t *pU = GetComponentBuffer(U_BUFFER);
	const data_t *pV = GetComponentBuffer(V_BUFFER);
	const data_t *pA = GetComponentBuffer(A_BUFFER);

	TypeBGRA16 *p = static_cast<TypeBGRA16*>(p_);

	for (size_t i = 0; i < numCoeffs; i++, p++) 
	{
		data_t y = pY[i];
		data_t u = pU[i];
		data_t v = pV[i];

		int r = (int)(y + u);
		r = (r <= 0) ? 0
			: (r < 255) ? (r * (0x1f << 10) / 255) & (0x1f << 10)
			: (0x1f << 10);

		int g = (int)(y - ((data_t)0.509) * u - ((data_t)0.194) * v);
		g = (g <= 0) ? 0
			: (g < 255) ? (g * (0x1f << 5) / 255) & (0x1f << 5)
			: (0x1f << 5);

		int b = (int)(y + v);
		b = (b <= 0) ? 0
			: (b < 255) ? (b * (0x1f) / 255) & (0x1f)
			: 0x1f;

		*p = r | g | b | ((pA[i] < 128.0f) ? 0 : 0x8000);
	}
}

IPicturePacker::PixelDataFormat CBWLPicturePacker::ChooseDecompressionFormat(PixelDataFormat pf) const
{
	if (m_bForceBGRDecompression) 
	{
		switch(pf) 
		{
		case PDF_R8G8B8 :
			pf = PDF_B8G8R8;
			break;
		case PDF_R8G8B8X8 :
			pf = PDF_B8G8R8X8;
			break;
		case PDF_R8G8B8A8 :
			pf = PDF_B8G8R8A8;
			break;
		}
	}
	else if (m_bForceRGBDecompression) 
	{
		switch(pf) 
		{
		case PDF_B8G8R8:
			pf = PDF_R8G8B8;
			break;
		case PDF_B8G8R8X8:
			pf = PDF_R8G8B8X8;
			break;
		case PDF_B8G8R8A8:
			pf = PDF_R8G8B8A8;
			break;
		}
	}

	switch(m_nForceDecompressionBits) 
	{
	case FBC_24:
		{
			switch(pf) 
			{
			case PDF_B8G8R8:
			case PDF_B8G8R8X8:
			case PDF_B8G8R8A8:
			case PDF_B5G5R5:
			case PDF_B5G6R5:
			case PDF_B5G5R5A1:
				pf = m_bForceRGBDecompression ? PDF_R8G8B8
					: m_bForceBGRDecompression ? PDF_B8G8R8
					: PDF_B8G8R8;
				break;
			case PDF_R8G8B8:
			case PDF_R8G8B8X8:
			case PDF_R8G8B8A8:
				pf = m_bForceRGBDecompression ? PDF_R8G8B8
					: m_bForceBGRDecompression ? PDF_B8G8R8
					: PDF_R8G8B8;
				break;
			}
		}
		break;
	case FBC_32:
		{
			switch(pf) 
			{
			case PDF_B8G8R8:
			case PDF_B8G8R8X8:
			case PDF_B8G8R8A8:
			case PDF_B5G5R5:
			case PDF_B5G6R5:
			case PDF_B5G5R5A1:
				pf = m_bForceRGBDecompression ? PDF_R8G8B8X8
					: m_bForceBGRDecompression ? PDF_B8G8R8X8
					: PDF_B8G8R8X8;
				break;
			case PDF_R8G8B8:
			case PDF_R8G8B8X8:
				pf = m_bForceRGBDecompression ? PDF_R8G8B8X8
					: m_bForceBGRDecompression ? PDF_B8G8R8X8
					: PDF_R8G8B8X8;
				break;
			case PDF_R8G8B8A8:
				pf = m_bForceRGBDecompression ? PDF_R8G8B8A8
					: m_bForceBGRDecompression ? PDF_B8G8R8A8
					: PDF_R8G8B8A8;
				break;
			}
		}
		break;
	case FBC_16:
		{
			switch(pf) 
			{
			case PDF_R8G8B8:
			case PDF_R8G8B8X8:
			case PDF_B8G8R8:
			case PDF_B8G8R8X8:
			case PDF_B5G6R5:
			case PDF_B5G5R5:
				pf = PDF_B5G6R5;
				break;
			case PDF_R8G8B8A8:
			case PDF_B8G8R8A8:
				pf = PDF_B5G5R5A1;
				break;
			}
		}
		break;
	case FBC_15:
		{
			switch(pf) 
			{
			case PDF_R8G8B8:
			case PDF_R8G8B8X8:
			case PDF_B8G8R8:
			case PDF_B8G8R8X8:
			case PDF_R8G8B8A8:
			case PDF_B8G8R8A8:
			case PDF_B5G6R5:
			case PDF_B5G5R5:
			case PDF_B5G5R5A1:
				pf = PDF_B5G5R5;
				break;
			}
		}
		break;
	}

	return pf;
}

IPicturePacker::PixelDataFormat CBWLPicturePacker::GetDecompressedFormat(const void* pPictureData)const
{
	return ChooseDecompressionFormat(((const ImageHeader*)pPictureData)->dataFormat);
}

void CBWLPicturePacker::EnableTiling(size_t tileSize)
{
	m_pPacker->EnableTiling(tileSize);
}

bool CBWLPicturePacker::IsTilingEnabled() const
{
	return m_pPacker->IsTilingEnabled();
}

size_t CBWLPicturePacker::GetTileSize() const
{
	return m_pPacker->GetTileSize();
}

void CBWLPicturePacker::DisableTiling()
{
	m_pPacker->DisableTiling();
}

};
		


