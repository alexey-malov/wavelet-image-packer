// bwlpack.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "bwlpacker.h"
#include "bwlpack.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// The one and only application object

CWinApp theApp;

using namespace std;

using namespace bwlpacker;

void Test1()
{
	std::auto_ptr<IWaveletPacker> pPacker(IWaveletPacker::Create());

	static const float input[] =
	{
		5, 7, 6, 4,
		8, 9, 3, 0,
		5, 2, 1, 7,
		8, 9, 4, 7,
	};

	float output[sizeof(input) / sizeof(float)];

	pPacker->Pack(input, 4, 4);
	pPacker->Depack(pPacker->GetPackedData(), output);

}

void TestRect()
{
	std::auto_ptr<IArbitraryWaveletPacker> pPacker(IArbitraryWaveletPacker::Create());

	const float input[] =
	{
		5, 7, 7,
		6, 4, 8,
		1, 4, 9,
		5, 7, 7,
		6, 4, 8,
	};

	const float input1[] =
	{
		5, 7, 2,
		5, 7, 2,
	};

	float output[sizeof(input) / sizeof(float)];

	pPacker->Pack(input, 3, 5, 4);
	pPacker->Depack(pPacker->GetPackedData(), output);
	pPacker->Pack(input1, 3, 2, 4);
	pPacker->Depack(pPacker->GetPackedData(), output);
}

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		cerr << _T("Fatal Error: MFC initialization failed") << endl;
		return 1;
	}

	TestRect();

	return 0;
}


