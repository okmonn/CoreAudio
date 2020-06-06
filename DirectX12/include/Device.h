#ifndef DIRECTX_DEVICE
#define DIRECTX_DEVICE

#include "BaseClass.h"

struct Device : 
	public BaseClass<ID3D12Device6> {
public:
	/*コンストラクタ*/
	Device();

private:
	/*デバイスの生成*/
	void CreateDevice(void);
};

#endif
