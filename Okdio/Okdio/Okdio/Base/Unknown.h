#pragma once
#include <Unknwn.h>

namespace okmonn
{
	template <typename T>
	class Unknown:
		public IUnknown
	{
	public:
		// 参照カウント加算
		unsigned long __stdcall AddRef(void) {
			return InterlockedIncrement(&cnt);
		}

		// 参照カウント減少
		unsigned long __stdcall Release(void) {
			unsigned long hr = InterlockedDecrement(&cnt);
			if (hr == 0)
			{
				delete this;
			}

			return hr;
		}

		// サポートチェック
		long __stdcall QueryInterface(const GUID& id, void** obj) {
			if (obj == nullptr)
			{
				return E_INVALIDARG;
			}

			*obj = nullptr;
			if (id == __uuidof(T))
			{
				*obj = (void*)this;
				AddRef();
				return S_OK;
			}

			return E_NOINTERFACE;
		}

	private:
		// 参照カウント
		long cnt;

	protected:
		// コンストラクタ
		Unknown() : cnt(1) {}
		// デストラクタ
		virtual ~Unknown() {}
	};
}
