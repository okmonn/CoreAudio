#include "DXR/Window/Window.h"
#include "DXR/Queue/Queue.h"
#include "DXR/List/List.h"
#include "DXR/Allocator/Allocator.h"
#include "DXR/Fence/Fence.h"
#include "DXR/Swap/Swap.h"
#include "DXR/Render/Render.h"
#include <d3d12.h>
#include <memory>

float color[] = {
	1.0f,
	1.0f,
	1.0f,
	1.0f
};

int main()
{
	std::shared_ptr<Window>win     = std::make_shared<Window>(Vec2(640, 480));
	std::shared_ptr<Queue>queue    = std::make_shared<Queue>(D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT);
	std::shared_ptr<List>list      = std::make_shared<List>(D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT);
	std::shared_ptr<Allocator>allo = std::make_shared<Allocator>(D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT);
	std::unique_ptr<Fence>fence    = std::make_unique<Fence>(queue);
	std::shared_ptr<Swap>swap      = std::make_shared<Swap>(win, queue);
	std::unique_ptr<Render>render  = std::make_unique<Render>(swap);

	while (Window::CheckMsg())
	{
		allo->Reset();
		list->Reset(allo);


		list->Close();
		ID3D12CommandList* tmp[] = {
			(ID3D12CommandList*)list->Lis()
		};
		queue->Execution(tmp);
		fence->Wait();
		swap->Present();
	}

	return 0;
}