#ifndef DIRECTX_BASE_CLASS
#define DIRECTX_BASE_CLASS

#include "ComPtr.h"

template <typename T>
struct BaseClass {
public:
	/*コンストラクタ*/
	BaseClass() {}
	/*デストラクタ*/
	virtual ~BaseClass() {}

public:
	/*オブジェクトの取得
	return オブジェクト*/
	T* Get(void) const {
		return obj.Get();
	}
	/*オブジェクトアドレスの取得
	return オブジェクトアドレス*/
	T** GetAddress(void) {
		return &obj;
	}

private:
	/*オブジェクト*/
	ComPtr<T>obj;
};

#endif
