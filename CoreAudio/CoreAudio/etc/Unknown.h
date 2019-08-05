#pragma once
#include <Unknwn.h>

template <typename T>
class MyUnknown : IUnknown
{
public:
	// 参照カウント加算
	unsigned long __stdcall AddRef(void)
	{
		return InterlockedIncrement(&ref);
	}

	// 参照カウント減少
	unsigned long __stdcall Release(void)
	{
		unsigned long hr = InterlockedDecrement(&ref);
		if (hr == 0)
		{
			delete this;
		}

		return hr;
	}

	// サポートチェック
	long __stdcall QueryInterface(const GUID& id, void** obj)
	{
		if (obj == nullptr)
		{
			return E_INVALIDARG;
		}

		*obj = nullptr;
		if (id == IID_IUnknown || id == __uuidof(T))
		{
			*obj = (void*)this;
			AddRef();
			return S_OK;
		}

		return E_NOINTERFACE;
	}

protected:
	// コンストラクタ
	MyUnknown() : ref(1) {}
	// デストラクタ
	virtual ~MyUnknown() {}

private:
	// 参照カウント
	unsigned long ref;
};
