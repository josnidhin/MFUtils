// MFUtils.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

// MF related libs
#pragma comment(lib, "mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")

#include <iostream>
#include <wrl.h>

// MF relates header files
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <mferror.h>

using namespace std;
using namespace Microsoft::WRL;

template <class T> void SafeRelease(T **ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = nullptr;
	}
}

void OutputVideoProcessorDetails();

int _tmain(int argc, _TCHAR* argv[])
{
	int exitCode = 0;

	if (FAILED(CoInitializeEx(0, COINIT_MULTITHREADED)))
	{
		cout << "COM Init failed." << endl;
		exitCode = -1;
		goto OnExit;
	}

	if (FAILED(MFStartup(MF_VERSION)))
	{
		cout << "MFStartup failed." << endl;
		exitCode = -1;
		goto OnExit;
	}

	OutputVideoProcessorDetails();

OnExit:
	CoUninitialize();
	MFShutdown();

	std::cout << "Press enter to exit..." << std::endl;
	getchar();

	return exitCode;
}

void OutputVideoProcessorDetails()
{
	HRESULT hr = S_OK;
	UINT32 count = 0, attrValue;
	UINT32 unFlags = MFT_ENUM_FLAG_SYNCMFT | MFT_ENUM_FLAG_LOCALMFT | MFT_ENUM_FLAG_SORTANDFILTER;
	IMFActivate **imfActivate = nullptr;
	IMFTransform *imfTransform = nullptr;
	IMFAttributes *imfAttributes = nullptr;

	MFT_REGISTER_TYPE_INFO inputType = { MFMediaType_Video, MFVideoFormat_YUY2 };
	MFT_REGISTER_TYPE_INFO outputType = { MFMediaType_Video, MFVideoFormat_ARGB32 };

	hr = MFTEnumEx(MFT_CATEGORY_VIDEO_PROCESSOR, unFlags, &inputType, &outputType, &imfActivate, &count);

	if (FAILED(hr) || count < 1)
	{
		cout << "No video processors found." <<endl;
		return;
	}

	cout << "Found " << count << " video processor(s)." << endl;

	for (UINT32 i = 0; i < count; i++)
	{
		hr = imfActivate[i]->ActivateObject(IID_PPV_ARGS(&imfTransform));

		cout << "Transform " << i+1 << endl;
		cout << "===========" << endl;

		imfTransform->GetAttributes(&imfAttributes);

		cout << "GPU-accelerated video processing - ";
		if (SUCCEEDED(imfAttributes->GetUINT32(MF_SA_D3D11_AWARE, &attrValue)))
			cout << "Supported" << endl;
		else
			cout << "Not Supported" << endl;

		SafeRelease(&imfAttributes);
		SafeRelease(&imfTransform);
		SafeRelease(&imfActivate[i]);
	}
	CoTaskMemFree(imfActivate);
}