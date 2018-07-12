// bmp2bwl.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "include/bwlpacker.h"


void BMP2BWL(const char *strFileName, const char *strOutputName)
{
	FILE *pFile = fopen(strFileName, "rb");
	if (!pFile) 
	{
		printf("cannot open file %s\n", strFileName);
		exit(1);
	}

	printf("Reading...");
	BITMAPFILEHEADER bfh;
	fread(&bfh, sizeof(bfh), 1, pFile);

	BITMAPINFOHEADER bih;
	fread(&bih, sizeof(bih), 1, pFile);

	int absH = bih.biHeight;
	if (absH < 0) 
	{
		absH = -absH;
	}

	std::vector<char> buf(absH * bih.biWidth * bih.biBitCount / 8);

	// load bitmap
	int y;
	int rowBytes = bih.biBitCount / 8 * bih.biWidth;
	int rowPitch = (rowBytes + 3) & 0xfffffffc;

	if (bih.biHeight > 0) 
	{
		for (y = absH-1; y >= 0; y--) 
		{
			fread(&buf[0] + rowBytes * y, 1, rowBytes, pFile);
			fseek(pFile, rowPitch - rowBytes, SEEK_CUR);
		}
	}
	else
	{
		for (y = 0; y < absH; y++) 
		{
			fread(&buf[0] + rowBytes * y, 1, rowBytes, pFile);
			fseek(pFile, rowPitch - rowBytes, SEEK_CUR);
		}
	}

	fclose(pFile);
	printf("Ok\n");

	if (bih.biBitCount != 24 && bih.biBitCount != 32) 
	{
		printf("Only 24 and 32 bit bitmaps are supported\n");
		exit(-1);
	}

	bwlpacker::IPicturePacker::PixelDataFormat pixelFormat = 
		(bih.biBitCount == 24) ? bwlpacker::IPicturePacker::PDF_BGR : bwlpacker::IPicturePacker::PDF_BGRX;

	std::auto_ptr<bwlpacker::IPicturePacker> pPacker(bwlpacker::IPicturePacker::Create());

	printf("Packing...");
	pPacker->Pack(&buf[0], bih.biWidth, absH, pixelFormat);
	printf("Ok\n");
	printf("Packed size: %d bytes (bitrate: %f bits per pixel)\n", 
		pPacker->GetPackedSize(), 
		pPacker->GetPackedSize() * 8.0 / (bih.biWidth * (-bih.biHeight)));

	pFile = fopen(strOutputName, "wb");
	if (!pFile) 
	{
		printf("cannot open file %s for writing\n", strFileName);
		exit(1);
	}
	fwrite(pPacker->GetPackedData(), pPacker->GetPackedSize(), 1, pFile);
	fclose(pFile);

}

using namespace bwlpacker;


void BWL2BMP(const char* strInputFileName, const char* strOutputFileName)
{
	std::auto_ptr<bwlpacker::IPicturePacker> pPacker(bwlpacker::IPicturePacker::Create());

	pPacker->ForceDecompressionBits(bwlpacker::IPicturePacker::FBC_15);
	
}

int main(int argc, char* argv[])
{
	if (argc == 3) 
	{
		BMP2BWL(argv[1], argv[2]);
	}
	else
	{
		printf("Usage: bmp2bwl.exe input_bmp_file output_bwl_file");
	}
	return 0;
}

