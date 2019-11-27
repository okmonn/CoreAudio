#include "DXR/Window/Window.h"
#include "DXR/Queue/Queue.h"
#include "DXR/List/List.h"
#include "DXR/Allocator/Allocator.h"
#include "DXR/Fence/Fence.h"
#include "DXR/Swap/Swap.h"
#include "DXR/Render/Render.h"
#include "DXR/Primitive/Primitive.h"
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

	const Vec3f tri[] = {
			Vec3f(0,          1,  0),
			Vec3f(0.866f,  -0.5f, 0),
			Vec3f(-0.866f, -0.5f, 0),
	};
	std::shared_ptr<Primitive>triangle = std::make_shared<Primitive>(tri, _countof(tri));
	const Vec3f pln[] = {
			Vec3f(-100.0f, -1.0f, -2.0f),
			Vec3f(100.0f, -1.0f, 100.0f),
			Vec3f(-100.0f, -1.0f, 100.0f),

			Vec3f(-100.0f, -1.0f, -2.0f),
			Vec3f(100.0f, -1.0f, -2.0f),
			Vec3f(100.0f, -1.0f, 100.0f)
	};
	std::shared_ptr<Primitive>plane = std::make_shared<Primitive>(pln, _countof(pln));

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