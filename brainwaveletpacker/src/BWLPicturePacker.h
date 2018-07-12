// BWLPicturePacker.h: interface for the CBWLPicturePacker class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BWLPICTUREPACKER_H__5696816B_6F67_41C9_9806_9C1408644E5D__INCLUDED_)
#define AFX_BWLPICTUREPACKER_H__5696816B_6F67_41C9_9806_9C1408644E5D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\INCLUDE\IPicturePacker.h"
#include "../include/IArbitraryWaveletPacker.h"
#include "PackBuffer.h"

namespace bwlpacker
{



class CBWLPicturePacker : public IPicturePacker  
{
private:
	enum 
	{
		MAX_COMPONENTS = 4
	};
	enum BufferIndex
	{
		BUFFER_0 = 0,
		BUFFER_1 = 1,
		BUFFER_2 = 2,
		BUFFER_3 = 3,
		R_BUFFER = BUFFER_0,
		G_BUFFER = BUFFER_1,
		B_BUFFER = BUFFER_2,
		A_BUFFER = BUFFER_3,
		Y_BUFFER = BUFFER_0,
		U_BUFFER = BUFFER_1,
		V_BUFFER = BUFFER_2,
		L_BUFFER = Y_BUFFER,
	};
public:
	typedef std::vector<byte_t>				byte_vector_t;
	typedef std::vector<data_t>				data_vector_t;
	typedef unsigned int					uint_t;


	struct ImageHeader
	{
		enum signature_type
		{
			IMAGE_HEADER = 'PLWB',
		};
		ImageHeader()
			:signature(IMAGE_HEADER)
		{
		}
		signature_type	signature;	// signature
		size_t			width;		// image width
		size_t			height;		// image height
		PixelDataFormat	dataFormat;	// output image format
		uint_t			dwFlags;	// flags
	};

	struct BlockHeader
	{
		size_t			packedSize;	// packed size
	};

public:

	virtual void DisableTiling();
	virtual size_t GetTileSize()const;
	virtual bool IsTilingEnabled()const;
	virtual void EnableTiling(size_t tileSize=128);
	virtual PixelDataFormat GetDecompressedFormat(const void* pPictureData)const;
	virtual void ForceDecompressionBits(ForcedBitsCount decompressionBits);
	virtual ForcedBitsCount GetForcedDecompressionBits()const;
	virtual bool CheckSignature(const void* pPackedData)const;
	virtual void ForceBGRDecompression(bool bForceBGR=true);
	virtual void ForceRGBDecompression(bool bForceRGB=true);
	virtual bool IsRGBDecompressionForced()const;
	virtual bool IsBGRDecompressionForced()const;
	virtual bool Save(const char* fileName)const;
	virtual bool Save(FILE *pFile)const;
	virtual bool IsYUVCompressionForced()const;
	virtual void ForceYUVCompression(bool bForceYUV=TRUE);
	virtual size_t GetImageBufferSize(const void *pPackedData)const;
	virtual size_t GetImageBufferSize(size_t width, size_t height, PixelDataFormat pf)const;
	virtual PixelDataFormat GetImagePixelDataFormat(const void *pPackedData) const;
	virtual size_t GetImageHeight(const void *pPackedData)const;
	virtual size_t GetImageWidth(const void* pPackedData)const;
	virtual void SetYQuantizer(data_t q);
	virtual void SetUQuantizer(data_t q);
	virtual void SetVQuantizer(data_t q);
	virtual void SetAQuantizer(data_t q);
	virtual void SetRQuantizer(data_t q);
	virtual void SetGQuantizer(data_t q);
	virtual void SetBQuantizer(data_t q);

	virtual data_t GetYQuantizer()const;
	virtual data_t GetUQuantizer()const;
	virtual data_t GetVQuantizer()const;
	virtual data_t GetAQuantizer()const;
	virtual data_t GetRQuantizer()const;
	virtual data_t GetGQuantizer()const;
	virtual data_t GetBQuantizer()const;

	virtual size_t GetPackedSize()const;
	virtual const void* GetPackedData()const;
	virtual bool Depack(const void *pPackedImage, void*pOutput);
	virtual bool Pack(const void* pImageData, size_t width, size_t height,  PixelDataFormat pixelType);

	static CBWLPicturePacker* Create(size_t packBufferSize = 0);
protected:
	CBWLPicturePacker(size_t nPackBufferSize);
	virtual ~CBWLPicturePacker();
private:
	std::auto_ptr<IArbitraryWaveletPacker> m_pPacker;
private:

	template <class T>
	void ToYUVA(const T* p, size_t numCoeffs)
	{
		data_t *pY = GetComponentBuffer(Y_BUFFER);
		data_t *pU = GetComponentBuffer(U_BUFFER);
		data_t *pV = GetComponentBuffer(V_BUFFER);
		data_t *pA = GetComponentBuffer(A_BUFFER);
		
		for (size_t i = 0; i < numCoeffs; ++i, ++p) 
		{
			data_t r = p->r;
			data_t g = p->g;
			data_t b = p->b;
			pY[i] = ((data_t)0.299) * r + ((data_t)0.587) * g + ((data_t)0.114) * b;
			pU[i] = r - pY[i];
			pV[i] = b - pY[i];
			pA[i] = p->a;
		}
	}

	template <class T>
	void ToYUV(const T* p, size_t numCoeffs)
	{
		data_t *pY = GetComponentBuffer(Y_BUFFER);
		data_t *pU = GetComponentBuffer(U_BUFFER);
		data_t *pV = GetComponentBuffer(V_BUFFER);

		for (size_t i = 0; i < numCoeffs; ++i, ++p) 
		{
			data_t r = p->r;
			data_t g = p->g;
			data_t b = p->b;
			pY[i] = ((data_t)0.299) * r + ((data_t)0.587) * g + ((data_t)0.114) * b;
			pU[i] = r - pY[i];
			pV[i] = b - pY[i];
		}
	}

	template <class T>
	void ToRGBA_BGRA(T* p, size_t numCoeffs)
	{
		const data_t *pY = GetComponentBuffer(Y_BUFFER);
		const data_t *pU = GetComponentBuffer(U_BUFFER);
		const data_t *pV = GetComponentBuffer(V_BUFFER);
		const data_t *pA = GetComponentBuffer(A_BUFFER);

		for (size_t i = 0; i < numCoeffs; ++i, ++p) 
		{
			data_t y = pY[i];
			data_t u = pU[i];
			data_t v = pV[i];

			int t = (int)(y + u);
			p->r = (t >= 0) ? ((t <= 255) ? (byte_t)t : 255) : 0;

			t = (int)(y - ((data_t)0.509) * u - ((data_t)0.194) * v);
			p->g = (t >= 0) ? ((t <= 255) ? (byte_t)t : 255) : 0;

			t = (int)(y + v);
			p->b = (t >= 0) ? ((t <= 255) ? (byte_t)t : 255) : 0;
			
			t = (int)pA[i];
			p->a = (t >= 0) ? ((t <= 255) ? (byte_t)t : 255) : 0;
		}
	}

	template <class T>
	void ToRGB_BGR(T* p, size_t numCoeffs)
	{
		const data_t *pY = GetComponentBuffer(Y_BUFFER);
		const data_t *pU = GetComponentBuffer(U_BUFFER);
		const data_t *pV = GetComponentBuffer(V_BUFFER);

		for (size_t i = 0; i < numCoeffs; ++i, ++p) 
		{
			data_t y = pY[i];
			data_t u = pU[i];
			data_t v = pV[i];

			int t = (int)(y + u);
			p->r = (t >= 0) ? ((t <= 255) ? (byte_t)t : 255) : 0;

			t = (int)(y - ((data_t)0.509) * u - ((data_t)0.194) * v);
			p->g = (t >= 0) ? ((t <= 255) ? (byte_t)t : 255) : 0;

			t = (int)(y + v);
			p->b = (t >= 0) ? ((t <= 255) ? (byte_t)t : 255) : 0;

		}
	}

	template <class T>
	void ToRGBX_BGRX(T* p, size_t numCoeffs)
	{
		const data_t *pY = GetComponentBuffer(Y_BUFFER);
		const data_t *pU = GetComponentBuffer(U_BUFFER);
		const data_t *pV = GetComponentBuffer(V_BUFFER);
		for (size_t i = 0; i < numCoeffs; ++i, ++p) 
		{
			data_t y = pY[i];
			data_t u = pU[i];
			data_t v = pV[i];

			int t = (int)(y + u);
			p->r = (t >= 0) ? ((t <= 255) ? (byte_t)t : 255) : 0;

			t = (int)(y - ((data_t)0.509) * u - ((data_t)0.194) * v);
			p->g = (t >= 0) ? ((t <= 255) ? (byte_t)t : 255) : 0;

			t = (int)(y + v);
			p->b = (t >= 0) ? ((t <= 255) ? (byte_t)t : 255) : 0;

			p->x = 0;
		}
	}

	template <class T>
	void LoadRGBA_BGRA(const T* p, size_t numCoeffs)
	{
		data_t *pR = GetComponentBuffer(R_BUFFER);
		data_t *pG = GetComponentBuffer(G_BUFFER);
		data_t *pB = GetComponentBuffer(B_BUFFER);
		data_t *pA = GetComponentBuffer(A_BUFFER);
		for (size_t i = 0; i < numCoeffs; ++i, ++p) 
		{
			pR[i] = p->r;
			pG[i] = p->g;
			pB[i] = p->b;
			pA[i] = p->a;
		}
	}

	template <class T>
	void LoadRGBX_BGRX_RGB_BGR(const T* p, size_t numCoeffs)
	{
		data_t *pR = GetComponentBuffer(R_BUFFER);
		data_t *pG = GetComponentBuffer(G_BUFFER);
		data_t *pB = GetComponentBuffer(B_BUFFER);
		for (size_t i = 0; i < numCoeffs; ++i, ++p) 
		{
			pR[i] = p->r;
			pG[i] = p->g;
			pB[i] = p->b;
		}
	}

	template <class T>
	void SaveRGBA_BGRA(T* p, size_t numCoeffs)const
	{
		const data_t *pR = GetComponentBuffer(R_BUFFER);
		const data_t *pG = GetComponentBuffer(G_BUFFER);
		const data_t *pB = GetComponentBuffer(B_BUFFER);
		const data_t *pA = GetComponentBuffer(A_BUFFER);
		for (size_t i = 0; i < numCoeffs; ++i, ++p) 
		{

			int t = (int)pR[i];
			p->r = (t >= 0) ? ((t <= 255) ? (byte_t)t : 255) : 0;

			t = (int)pG[i];
			p->g = (t >= 0) ? ((t <= 255) ? (byte_t)t : 255) : 0;

			t = (int)pB[i];
			p->b = (t >= 0) ? ((t <= 255) ? (byte_t)t : 255) : 0;

			t = (int)pA[i];
			p->a = (t >= 0) ? ((t <= 255) ? (byte_t)t : 255) : 0;
		}
	}

	template <class T>
	void SaveRGB_BGR(T* p, size_t numCoeffs)const
	{
		const data_t *pR = GetComponentBuffer(R_BUFFER);
		const data_t *pG = GetComponentBuffer(G_BUFFER);
		const data_t *pB = GetComponentBuffer(B_BUFFER);
		for (size_t i = 0; i < numCoeffs; ++i, ++p) 
		{

			int t = (int)pR[i];
			p->r = (t >= 0) ? ((t <= 255) ? (byte_t)t : 255) : 0;

			t = (int)pG[i];
			p->g = (t >= 0) ? ((t <= 255) ? (byte_t)t : 255) : 0;

			t = (int)pB[i];
			p->b = (t >= 0) ? ((t <= 255) ? (byte_t)t : 255) : 0;

		}
	}

	template <class T>
	void SaveRGBX_BGRX(T* p, size_t numCoeffs)
	{
		const data_t *pR = GetComponentBuffer(R_BUFFER);
		const data_t *pG = GetComponentBuffer(G_BUFFER);
		const data_t *pB = GetComponentBuffer(B_BUFFER);
		for (size_t i = 0; i < numCoeffs; ++i, ++p) 
		{			
			int t = (int)pR[i];
			p->r = (t >= 0) ? ((t <= 255) ? (byte_t)t : 255) : 0;

			t = (int)pG[i];
			p->g = (t >= 0) ? ((t <= 255) ? (byte_t)t : 255) : 0;

			t = (int)pB[i];
			p->b = (t >= 0) ? ((t <= 255) ? (byte_t)t : 255) : 0;

			p->x = 0;
		}
	}

	void LoadBGR15(const void* p, size_t numCoeffs);
	void LoadBGR16(const void* p, size_t numCoeffs);
	void LoadBGRA16(const void* p, size_t numCoeffs);


	//////////////////////////////////////////////////////////////////////////
	
	// convert YUVA float data to RGBA byte data
	void YUVA2RGBA(void *pRGBA, size_t numCoeffs);
	// convert YUVA float data to RGBA byte data
	void YUVA2BGRA(void *pBGRA, size_t numCoeffs);

	// convert YUVA float data to RGBA byte data
	void YUV2RGBX(void *pRGBX, size_t numCoeffs);
	// convert YUVA float data to RGBA byte data
	void YUV2BGRX(void *pBGRX, size_t numCoeffs);

	// convert YUVA float data to RGBA byte data
	void YUV2RGB(void *pRGB, size_t numCoeffs);
	// convert YUVA float data to RGBA byte data
	void YUV2BGR(void *pBGRX, size_t numCoeffs);


	// convert YUV float to BGR15 word 
	void YUVToBGR15(void* p, size_t numCoeffs);
	// convert YUV float to BGR16 word
	void YUVToBGR16(void* p, size_t numCoeffs);
	// convert YUV float to BGRA16
	void YUVAToBGRA16(void* p, size_t numCoeffs);


	//////////////////////////////////////////////////////////////////////////
	
	// convert BGR byte data to YUV float data
	void BGR2YUV(const void *pBGR, size_t num_coeffs);
	// convert RGB byte data to YUV float
	void RGB2YUV(const void *pRGB, size_t num_coeffs);

	// convert BGRX data to YUV float data
	void BGRX2YUV(const void *pBGRX, size_t num_coeffs);
	// convert RGBX byte data to YUV float
	void RGBX2YUV(const void *pRGBX, size_t num_coeffs);

	// convert BGRA byte data to YUVA float
	void BGRA2YUVA(const void *pBGRA, size_t num_coeffs);
	// convert RGBA byte data to YUVA float
	void RGBA2YUVA(const void* pRGBA, size_t num_coeffs);

	// convert BGR15 word to YUV float
	void BGR15ToYUV(const void* p, size_t numCoeffs );
	// convert BGR16 word to YUV float
	void BGR16ToYUV(const void* p, size_t numCoeffs );
	// convert BGRA16 word to YUV float
	void BGRA16ToYUVA(const void* p, size_t numCoeffs );

	void LoadDataIntoBuffersAndPack(PixelDataFormat pf, const void* pData, size_t width, size_t height);
	void ResizeBuffer(BufferIndex index, size_t newSize);
	void ResizeBuffers(PixelDataFormat pf, size_t newSize);


	void SaveBGR15(void* p, size_t numCoeffs);
	void SaveBGRA16(void* p, size_t numCoeffs);
	void SaveBGR16(void* p, size_t numCoeffs);

	inline data_t* GetComponentBuffer(BufferIndex index)
	{
		return &m_Components[index][0];
	}
	inline const data_t* GetComponentBuffer(BufferIndex index)const
	{
		return &m_Components[index][0];
	}
private:
	PixelDataFormat ChooseDecompressionFormat(PixelDataFormat pf)const;
	void DepackAndSaveBuffers(PixelDataFormat pf, size_t width, size_t height, void* pOutput, uint_t dwFlags);
	const void* ReadPackedStream(size_t numBytesToRead)const;
	void OpenPackedStream(const void* pPackedStream);
	void DepackBuffer(BufferIndex index);
	
	void PackYUVABuffers(size_t width, size_t height);
	void PackRGBABuffers(size_t width, size_t height);
	void PackRGBBuffers(size_t width, size_t height);
	void PackYUVBuffers(size_t width, size_t height);

	void DepackYUVABuffers();
	void DepackRGBABuffers();
	void DepackRGBBuffers();
	void DepackYUVBuffers();

	size_t WritePackBuffer(const void* pData, size_t numBytesToWrite);
	void RewritePackBuffer();
	void PackBuffer(BufferIndex index, size_t width, size_t height, data_t quantizer);
	void LoadLA(const TypeLA *p, size_t numCoeffs);
	void LoadL(const TypeL *p, size_t numCoeffs);

	void SaveLA(TypeLA *p, size_t numCoeffs)const;
	void SaveL(TypeL *p, size_t numCoeffs)const;

	/*
	 *	[r/y][g/u][b/v][a]
	 */
	data_vector_t	m_Components[MAX_COMPONENTS];

	bool	m_bForceYUVCompression;

	bool	m_bForceBGRDecompression;	// mutually
	bool	m_bForceRGBDecompression;	// exclusive

	//bool	m_bForce15BitDecompression;		// mutually
	//bool	m_bForce16BitDecompression;		// exclusive
	//bool	m_bForce32BitDecompression;	// exlusive
	//bool	m_bForce24BitDecompression;	// exlusive

	ForcedBitsCount	m_nForceDecompressionBits;

	float	m_yq;
	float	m_uq;
	float	m_vq;
	float	m_aq;
	float	m_rq;
	float	m_gq;
	float	m_bq;

	CPackBuffer m_PackBuffer;
	mutable const byte_t	*m_pPackedDataPointer;
};


};

#endif // !defined(AFX_BWLPICTUREPACKER_H__5696816B_6F67_41C9_9806_9C1408644E5D__INCLUDED_)
