#include "Okdio/Okdio.h"
#include <iostream>

int main(void)
{
	auto hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
	_ASSERT(hr == S_OK);

	{
		Microsoft::WRL::ComPtr<OkdioEngine>engine = nullptr;
		hr = okmonn::CreateOkdioEngine(okmonn::AudioDeviceType::Render, okmonn::AudioType::SHARED, IID_PPV_ARGS(&engine));
		engine->Start();
		while (!(GetKeyState(VK_ESCAPE) & 0x80))
		{

		}

		engine->Stop();
	}
	
	CoUninitialize();

	return 0;
}