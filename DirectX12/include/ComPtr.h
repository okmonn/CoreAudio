#ifndef DIRECTX_COM_PTR
#define DIRECTX_COM_PTR

#include <d3d12.h>
#include <dxgi1_6.h>

template <typename T>
class ComPtr {
public:
	/*コンストラクタ*/
	ComPtr() : obj(nullptr) {}
	/*デストラクタ*/
	~ComPtr() {
		if (obj != nullptr) {
			obj->Release();
			obj = nullptr;
		}
	}

public:
	T* Get(void) const {
		return obj;
	}

public:
	T* operator->(void) const {
		return Get();
	}
	T** operator&(void) {
		return &obj;
	}

private:
	/*オブジェクトのポインタ*/
	T* obj;
};

#endif
