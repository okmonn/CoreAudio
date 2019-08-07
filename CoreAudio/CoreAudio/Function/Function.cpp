#include "Function.h"
#include "../Manager/Manager.h"
#include <mmdeviceapi.h>
#include <propvarutil.h>
#include <functiondiscoverykeys_devpkey.h>
#include <wrl.h>

// ユニコード文字をマルチバイト文字に変換
std::string okmonn::ChangeCode(const std::wstring& wstr)
{
	std::vector<char> buffer(WideCharToMultiByte(CP_OEMCP, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr));
	WideCharToMultiByte(CP_OEMCP, 0, wstr.c_str(), -1, &buffer.front(), buffer.size(), nullptr, nullptr);
	return std::string(buffer.begin(), buffer.end());
}

// オーディオデバイス列挙
std::vector<okmonn::DeviceProp> okmonn::GetDeviceProp(const okmonn::DeviceType& type)
{
	Microsoft::WRL::ComPtr<IMMDeviceEnumerator>enumerator = nullptr;
	auto hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&enumerator));
	_ASSERT(hr == S_OK);

	Microsoft::WRL::ComPtr<IMMDeviceCollection>collection = nullptr;
	hr = enumerator->EnumAudioEndpoints(EDataFlow(type), DEVICE_STATE_ACTIVE, &collection);
	_ASSERT(hr == S_OK);

	unsigned int num = 0;
	hr = collection->GetCount(&num);
	_ASSERT(hr == S_OK);

	std::vector<okmonn::DeviceProp>deviceProp(num);
	
	Microsoft::WRL::ComPtr<IMMDevice>device = nullptr;
	Microsoft::WRL::ComPtr<IPropertyStore>prop = nullptr;
	LPWSTR buf = nullptr;
	PROPVARIANT var{};
	for (unsigned int i = 0; i < num; ++i)
	{
		hr = collection->Item(i, &device);
		_ASSERT(hr == S_OK);

		//ID取得
		hr = device->GetId(&buf);
		_ASSERT(hr == S_OK);
		deviceProp[i].id = ChangeCode(std::wstring(buf));

		//ステータス取得
		unsigned long state = 0;
		hr = device->GetState(&state);
		_ASSERT(hr == S_OK);
		switch (state)
		{
		case DEVICE_STATE_ACTIVE:
			deviceProp[i].state = "ACTIVE";
			break;
		case DEVICE_STATE_DISABLED:
			deviceProp[i].state = "DISABLED";
			break;
		case DEVICE_STATE_NOTPRESENT:
			deviceProp[i].state = "NOTPRESENT";
			break;
		case DEVICE_STATE_UNPLUGGED:
			deviceProp[i].state = "UNPLUGGED";
			break;
		default:
			deviceProp[i].state.clear();
			break;
		}

		hr = device->OpenPropertyStore(STGM_READ, &prop);
		_ASSERT(hr == S_OK);

		//フルネーム取得
		hr = prop->GetValue(PKEY_Device_FriendlyName, &var);
		_ASSERT(hr == S_OK);
		hr = PropVariantToStringAlloc(var, &buf);
		_ASSERT(hr == S_OK);
		deviceProp[i].fullName = ChangeCode(std::wstring(buf));

		//ショートネーム取得
		hr = prop->GetValue(PKEY_Device_DeviceDesc, &var);
		_ASSERT(hr == S_OK);
		hr = PropVariantToStringAlloc(var, &buf);
		_ASSERT(hr == S_OK);
		deviceProp[i].shortName = ChangeCode(std::wstring(buf));

		//物理デバイス名取得
		hr = prop->GetValue(PKEY_DeviceInterface_FriendlyName, &var);
		_ASSERT(hr == S_OK);
		hr = PropVariantToStringAlloc(var, &buf);
		_ASSERT(hr == S_OK);
		deviceProp[i].shortName = ChangeCode(std::wstring(buf));
	}

	CoTaskMemFree(buf);

	return deviceProp;
}

// オーディオエンジンの生成
long okmonn::CreateAudioEngin(const unsigned int& index, const okmonn::DeviceType& type, const GUID& id, void** obj)
{
	return Manager::Get().CreateObj(index, type, id, obj);
}
