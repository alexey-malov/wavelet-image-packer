// bwImagePacker.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "IPngCodec.h"
#include "IPicturePacker.h"

using namespace std;
using namespace bwPng;
using namespace bwlpacker;

auto_ptr<IPicturePacker>	pBwlPacker(IPicturePacker::Create(100000));
auto_ptr<IPngCodec>			pPngCodec(IPngCodec::Create());

IPicturePacker::ForcedBitsCount	fbc = IPicturePacker::FBC_NONE;

int PngToBWL(const char* strPngFileName, const char* strBWLFileName)
{
	pPngCodec->SetConvert16BitTo8(true);
	pPngCodec->SetPaletteToRGB(true);
	pPngCodec->SetTransparencyToAlpha(true);

	auto_ptr<IDataStream> pDataStream(IDataStream::CreateFileDataStream(strPngFileName));
	if (!pDataStream.get()) 
	{
		printf("cannot open %s file.", strPngFileName);
		return -1;
	}

	size_t width, height, bitDepth, memorySize;
	IPngCodec::PixelDataFormat pdf;
	if (!pPngCodec->GetImageInfo(pDataStream.get(), &width, &height, &bitDepth, &pdf, &memorySize))
	{
		switch(pPngCodec->GetLastError()) 
		{
		case IPngCodec::EC_END_OF_DATA:
			printf("Error: end of data.");
			break;
		case IPngCodec::EC_WRONG_PNG_SIGNATURE:
			printf("Error: wrong png signature");
			break;
		default:
			printf("Error: unknown error.");
		}
		return -1;
	}

	printf("Loading png file...");
	std::vector<char> buf(memorySize);
	if (!pPngCodec->Decode(pDataStream.get(), &buf[0]))
	{
		switch(pPngCodec->GetLastError()) 
		{
		case IPngCodec::EC_END_OF_DATA:
			printf("Error: end of data.");
			break;
		case IPngCodec::EC_WRONG_PNG_SIGNATURE:
			printf("Error: wrong png signature");
			break;
		default:
			printf("Error: unknown error.");
		}
		return -1;
	}
	printf("Ok\n");

	IPicturePacker::PixelDataFormat bwlFormat;
	switch(pdf) 
	{
	case IPngCodec::PDF_GRAY :
		bwlFormat = IPicturePacker::PDF_GRAY;
		break;
	case IPngCodec::PDF_GRAY_ALPHA :
		bwlFormat = IPicturePacker::PDF_GRAY_ALPHA;
		break;
	case IPngCodec::PDF_RGB :
		bwlFormat = IPicturePacker::PDF_RGB;
		break;
	case IPngCodec::PDF_RGBA :
		bwlFormat = IPicturePacker::PDF_RGBA;
		break;
	default:
		printf("Error: indexed color images are not supported");
		return -1;
	}

	printf("Packing...");

	LARGE_INTEGER freq, cnt0, cnt1;
	QueryPerformanceFrequency(&freq);

	QueryPerformanceCounter(&cnt0);
	if (!pBwlPacker->Pack(&buf[0], width, height, bwlFormat))
	{
		printf("Error: cannot pack image");
		return -1;
	}
	QueryPerformanceCounter(&cnt1);
	LONGLONG diff = cnt1.QuadPart - cnt0.QuadPart;
	double tm = diff * 1.0 / freq.QuadPart;
	printf("Ok\n");

	printf("Saving...");
	if (!pBwlPacker->Save(strBWLFileName)) 
	{
		printf("Error: cannot write to %s", (LPCTSTR)strBWLFileName);
		return -1;
	}
	printf("Ok\n");

	printf("Initial size: %d bytes\n"
		"Packed size: %d bytes (%d%%)\n"
		"Bitrate: %f bits per pixel\n"
		"Pack time: %.4f seconds\n",
		buf.size(),
		pBwlPacker->GetPackedSize(),
		pBwlPacker->GetPackedSize() * 100 / buf.size(),
		pBwlPacker->GetPackedSize() * 8.0f / (width * height),
		tm);

	return 0;
}

int BWL2BMP(const char *strBWLFileName, const char *strBMPFileName)
{
	FILE *pFile = fopen(strBWLFileName, "rb");
	if (!pFile) 
	{
		printf("Error: cannot open file %s", strBWLFileName);
		return false;
	}

	fseek(pFile, 0, SEEK_END);
	size_t nFileSize = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);

	printf("Reading BWL file...");
	vector<char> buf(nFileSize);
	if (fread(&buf[0], 1, nFileSize, pFile) != nFileSize)
	{
		fclose(pFile);
		return -1;
	}
	printf("Ok\n");
	
	if (!pBwlPacker->CheckSignature(&buf[0])) 
	{
		printf("Error: Cannot find BWLP signature.");
		return -1;
	}


	pBwlPacker->ForceBGRDecompression();
	pBwlPacker->ForceDecompressionBits(fbc);

	size_t width, height, depackedSize;
	IPicturePacker::PixelDataFormat pdf;

	width = pBwlPacker->GetImageWidth(&buf[0]);
	height = pBwlPacker->GetImageHeight(&buf[0]);
	depackedSize = pBwlPacker->GetImageBufferSize(&buf[0]);
	pdf = pBwlPacker->GetDecompressedFormat(&buf[0]);

	vector<char> depackBuf(depackedSize);


	printf("Decompressing...");

	LARGE_INTEGER freq, cnt0, cnt1;
	QueryPerformanceFrequency(&freq);

	QueryPerformanceCounter(&cnt0);

	pBwlPacker->Depack(&buf[0], &depackBuf[0]);
	
	QueryPerformanceCounter(&cnt1);
	LONGLONG diff = cnt1.QuadPart - cnt0.QuadPart;
	double tm = diff * 1.0 / freq.QuadPart;
	printf("Ok\n");

	printf("Depack time: %.4f seconds\n", tm);


	BITMAPFILEHEADER bfh;
	BITMAPINFOHEADER bih;
	bfh.bfType = 'MB';
	bfh.bfReserved1 = 0;
	bfh.bfReserved2 = 0;
	bfh.bfOffBits = sizeof(bfh) + sizeof(bih);

	bih.biCompression = BI_RGB;
	bih.biHeight = -(int)height;
	bih.biWidth = width;
	bih.biPlanes = 1;
	bih.biSize = sizeof(bih);
	bih.biSizeImage = 0;
	bih.biXPelsPerMeter = 1;
	bih.biYPelsPerMeter = 1;
	bih.biClrImportant = 0;
	bih.biClrUsed = 0;

	printf("Writing BMP file...");

	pFile = fopen(strBMPFileName, "wb");
	if (!pFile) 
	{
		printf("Error: cannot create output file : %s" , strBMPFileName);
		return -1;
	}
	
	
	switch(pdf) 
	{
	case IPicturePacker::PDF_B5G5R5:
	case IPicturePacker::PDF_B5G5R5A1:
		{
			size_t rowBytes = width * 2;
			size_t rowPitch = (rowBytes + 3) & 0xfffffffc;
			bih.biBitCount = 16;

			fwrite(&bfh, 1, sizeof(bfh), pFile);
			fwrite(&bih, 1, sizeof(bih), pFile);
			
			for (size_t y = 0; y < height; y++) 
			{
				int dummy = 0;
				fwrite(&depackBuf[0] + rowBytes * y, 1, rowBytes, pFile);
				fwrite(&dummy, 1, rowPitch - rowBytes, pFile);
			}
			break;
		}
	case IPicturePacker::PDF_BGR :
	case IPicturePacker::PDF_RGB :
		{
			size_t rowBytes = width * 3;
			size_t rowPitch = (rowBytes + 3) & 0xfffffffc;
			bih.biBitCount = 24;

			fwrite(&bfh, 1, sizeof(bfh), pFile);
			fwrite(&bih, 1, sizeof(bih), pFile);
			
			for (size_t y = 0; y < height; y++) 
			{
				int dummy = 0;
				fwrite(&depackBuf[0] + rowBytes * y, 1, rowBytes, pFile);
				fwrite(&dummy, 1, rowPitch - rowBytes, pFile);
			}
		}
		break;
	case IPicturePacker::PDF_BGRX :
	case IPicturePacker::PDF_RGBX :
	case IPicturePacker::PDF_BGRA :
	case IPicturePacker::PDF_RGBA :
		{
			size_t rowBytes = width * 4;
			bih.biBitCount = 32;

			fwrite(&bfh, 1, sizeof(bfh), pFile);
			fwrite(&bih, 1, sizeof(bih), pFile);
			fwrite(&depackBuf[0], 1, depackBuf.size(), pFile);
		}
		break;
	case IPicturePacker::PDF_GRAY :
	case IPicturePacker::PDF_GRAY_ALPHA :
		{
			bih.biBitCount = 8;
			bfh.bfOffBits += 256 * 4;
			fwrite(&bfh, 1, sizeof(bfh), pFile);
			fwrite(&bih, 1, sizeof(bih), pFile);

			// write palette
			for (int i = 0; i < 256; i++) 
			{
				int pal = i + (i << 8) + (i << 16);
				fwrite(&pal, sizeof(pal), 1, pFile);
			}

			if (pdf == IPicturePacker::PDF_GRAY) 
			{
				size_t rowBytes = width;
				size_t rowPitch = (rowBytes + 3) & 0xfffffffc;
			
				for (size_t y = 0; y < height; y++) 
				{
					int dummy = 0;
					fwrite(&depackBuf[0] + rowBytes * y, 1, rowBytes, pFile);
					fwrite(&dummy, 1, rowPitch - rowBytes, pFile);
				}
				
			}
			else
			{
				size_t rowBytes = width;
				size_t rowPitch = (rowBytes + 3) & 0xfffffffc;
			
				for (size_t y = 0; y < height; y++) 
				{
					const short *pRow = (const short*)&depackBuf[y * rowBytes * 2];
					for (size_t x = 0; x < width; x++) 
					{
						fwrite(&pRow[x], 1, 1, pFile);
					}
					int dummy = 0;
					fwrite(&dummy, 1, rowPitch - rowBytes, pFile);
				}
			}
		}
		break;
	}

	fclose(pFile);

	printf("Ok\n");
	return 0;
}

void Test()
{
	std::auto_ptr<bwlpacker::IWaveletPacker> pWaveletPacker(bwlpacker::IWaveletPacker::Create());
	const float coeffs[] = 
	{
		1,	2,	3,	4,	5,	6,	7,	8,
		9,	10,	11,	12,	13,	14,	15,	16,
		17,	18,	19,	20,	21,	22,	23,	24,
		25,	26,	27,	28,	29,	30,	31,	32,
		25,	26,	27,	28,	29,	30,	31,	32,
		17,	18,	19,	20,	21,	22,	23,	24,
		9,	10,	11,	12,	13,	14,	15,	16,
		1,	2,	3,	4,	5,	6,	7,	8,
	};
	float coeffs1[] = 
	{
		0,	0,	0,	0,	0,	0,	0,	0,
		0,	0,	0,	0,	0,	0,	0,	0,
		0,	0,	0,	0,	0,	0,	0,	0,
		0,	0,	0,	0,	0,	0,	0,	0,
		0,	0,	0,	0,	0,	0,	0,	0,
		0,	0,	0,	0,	0,	0,	0,	0,
		0,	0,	0,	0,	0,	0,	0,	0,
		0,	0,	0,	0,	0,	0,	0,	0,
	};
	pWaveletPacker->Pack(coeffs, 8, 4);
	pWaveletPacker->Depack(pWaveletPacker->GetPackedData(), coeffs1);
}

static void Help()
{
	printf(
		"%s",
		"BWImagePacker v1.0 (c) Vivid^Brainwave team (vivid@bwteam.org)\n"
		"Usage\n"
		"bwimagepacker.exe [options] inputFile.png outputFile.bwl\n"
		"or\n"
		"bwimagepacker.exe -depack [-force-hi-color] inputFile.bwl outputFile.bmp\n\n"
		"options:\n"
		"\t-help                    this help\n"
		"\t-force-yuv               force yuv mode (by default)\n"
		"\t-force-rgb               force rgb mode\n"
		"\t-no-gamma                do not convert source gamma to screen gamma\n"
		"\t-yq value                Y-quantizer value (0.06 by default)\n"
		"\t-uq value                U-quantizer value (0.025 by default)\n"
		"\t-vq value                V-quantizer value (0.025 by default)\n"
		"\t-aq value                A-quantizer value (0.06 by default)\n"
		"\t-rq value                R-quantizer value (0.05 by default)\n"
		"\t-gq value                G-quantizer value (0.07 by default)\n"
		"\t-bq value                B-quantizer value (0.04 by default)\n"
		"\t-enable-tiling tileSize  Enable tiling. <tileSize> is a power of two (128 is optimal).\n"
		"\t                         Tiling can increase depacking speed (less compression)\n"
		"\t-disable-tiling          Disable tiling (default)\n"
		);
}

int main(int argc, char* argv[])
{
//	Test();
//	return 0;
	bool bDepacking = false;

	if (argc < 3) 
	{
		printf("Not enough parameters\n");
		Help();
		return -1;
	}


	int nParam = 1;
	while (nParam < argc - 2) 
	{
		CString param = argv[nParam];
		param.MakeLower();
		if (param == "-force-yuv") 
		{
			pBwlPacker->ForceYUVCompression(true);
		}
		else if (param == "-force-rgb")
		{
			pBwlPacker->ForceYUVCompression(false);
		}
		else if (param == "-yq") 
		{
			char *stopst;
			nParam++;
			pBwlPacker->SetYQuantizer((float)strtod(argv[nParam], &stopst));
		}
		else if (param == "-uq") 
		{
			char *stopst;
			nParam++;
			pBwlPacker->SetUQuantizer((float)strtod(argv[nParam], &stopst));
		}
		else if (param == "-vq") 
		{
			char *stopst;
			nParam++;
			pBwlPacker->SetVQuantizer((float)strtod(argv[nParam], &stopst));
		}
		else if (param == "-aq") 
		{
			char *stopst;
			nParam++;
			pBwlPacker->SetAQuantizer((float)strtod(argv[nParam], &stopst));
		}
		else if (param == "-rq") 
		{
			char *stopst;
			nParam++;
			pBwlPacker->SetRQuantizer((float)strtod(argv[nParam], &stopst));
		}
		else if (param == "-gq") 
		{
			char *stopst;
			nParam++;
			pBwlPacker->SetGQuantizer((float)strtod(argv[nParam], &stopst));
		}
		else if (param == "-bq") 
		{
			char *stopst;
			nParam++;
			pBwlPacker->SetBQuantizer((float)strtod(argv[nParam], &stopst));
		}
		else if (param == "-help")
		{
			Help();
			return 0;
		}
		else if (param == "-depack")
		{
			bDepacking = true;
		}
		else if (param == "-force-hi-color")
		{
			fbc = IPicturePacker::FBC_15;
		}
		else if (param == "-no-gamma")
		{
			pPngCodec->SetConvertToScreenGamma(false);
		}
		else if (param == "-enable-tiling") 
		{
			char *stopst;
			nParam++;
			int tileSize = (int)strtol(argv[nParam], &stopst, 10);
			if ((*stopst != '\0') ||				// must be valid number
				(tileSize <= 0) ||					// must be positive
				((tileSize & (tileSize - 1)) != 0)	// must be the power of two
				)
			{
				puts("tile size must be the power-of-two value\n");
				return -1;
			}
			pBwlPacker->EnableTiling(tileSize);
		}
		else if (param == "-disable-tiling") 
		{
			pBwlPacker->DisableTiling();
		}
		else
		{
			printf("Error: wrong parameter %s\n\n", param);
			Help();
			return -1;
		}

		nParam++;
	}

	if (nParam != argc - 2) 
	{
		printf("Error: wrong command line parameters.\n\n");
		Help();
		return -1;
	}

	if (bDepacking) 
	{
		return BWL2BMP(argv[nParam], argv[nParam+1]);
	}
	else
	{
		return PngToBWL(argv[nParam], argv[nParam+1]);
	}
	

	return 0;
}

