#pragma once
#include "../Descriptor/Descriptor.h"
#include <memory>

class Window;

class Output : 
	public Descriptor
{
public:
	// コンストラクタ
	Output(std::weak_ptr<Window>win);
	// デストラクタ
	~Output();

	// リソースの取得
	ID3D12Resource* Rsc(void) const;

private:
	// リソース生成
	void CreateRsc(std::weak_ptr<Window>win);
};
