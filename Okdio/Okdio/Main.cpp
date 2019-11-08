#include "Okdio/Okdio.h"

// エントリーポイント
int main(void)
{
	auto hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	_ASSERT(hr == S_OK);

	OkdioEngine* engine = nullptr;
	bool flag = okmonn::CreateEngine(okmonn::AudioDevType::output, okmonn::AudioType::shared, IID_PPV_ARGS(&engine));
	_ASSERT(flag == true);
	flag = engine->Start();
	_ASSERT(flag == true);

	Okdio* okdio = nullptr;
	flag = engine->CreateOkdio("SOS.wav", IID_PPV_ARGS(&okdio));
	_ASSERT(flag == true);
	
	bool key = false;
	bool play = false;
	while (!(GetKeyState(VK_ESCAPE) & 0x80))
	{
		if (GetKeyState(VK_SPACE) & 0x80)
		{
			if (key == false)
			{
				key  = true;
				if (play == false)
				{
					play = true;
					okdio->Play(false);
				}
				else
				{
					play = false;
					okdio->Stop();
				}
			}
		}
		else
		{
			key = false;
		}
	}

	okdio->Release();
	engine->Release();
	CoUninitialize();
	return 0;
}