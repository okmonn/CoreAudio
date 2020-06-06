#include "../include/Window.h"
#include "../include/resource.h"
#include <Windows.h>
#include <cassert>

/*ウィンドウコールバック*/
static std::int64_t __stdcall WindowCallback(void* handle, std::uint32_t message, std::uint64_t wparam, std::int64_t lparam) {
	switch (message)
	{
	case WM_CLOSE:
		DestroyWindow(HWND(handle));
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		break;
	}
	return DefWindowProc(HWND(handle), message, wparam, lparam);
}

Window::Window(const okmonn::Vec2& window_size, const okmonn::Vec2& screen_pos) : 
	window_size(window_size), screen_pos(screen_pos)
{
	CreateWindowHandle();
	ShowWindow(Get(), SW_SHOW);
}

Window::~Window()
{
	if (handle != nullptr) {
		handle = nullptr;
	}
}

void Window::CreateWindowHandle(void)
{
	WNDCLASSEX desc{};
	desc.cbClsExtra    = 0;
	desc.cbSize        = sizeof(WNDCLASSEX);
	desc.cbWndExtra    = 0;
	desc.hbrBackground = CreateSolidBrush(COLOR_APPWORKSPACE);
	desc.hInstance     = GetModuleHandle(0);
	desc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
	desc.hIcon         = LoadIcon(desc.hInstance, MAKEINTRESOURCE(MYLOGO_ICO));
	desc.hIconSm       = desc.hIcon;
	desc.lpfnWndProc   = WNDPROC(WindowCallback);
	desc.lpszClassName = L"おかもん";
	desc.lpszMenuName  = desc.lpszClassName;
	desc.style         = CS_HREDRAW | CS_VREDRAW;
	RegisterClassEx(&desc);

	RECT rect{};
	rect.bottom = window_size.y;
	rect.left   = 0;
	rect.right  = window_size.x;
	rect.top    = 0;

	handle = CreateWindowEx(WS_EX_ACCEPTFILES, desc.lpszClassName, L"おかもん", WS_OVERLAPPEDWINDOW, screen_pos.x, screen_pos.y,
		window_size.x, window_size.y, nullptr, nullptr, desc.hInstance, nullptr);
	assert(handle != nullptr);
}

HWND__* Window::Get(void) const
{
	return (HWND__*)handle;
}

okmonn::Vec2 Window::GetScreenPos(void) const
{
	return screen_pos;
}

okmonn::Vec2 Window::GetWindowSize(void) const
{
	return window_size;
}

HWND__* Window::operator->(void) const
{
	return Get();
}

HWND__** Window::operator&(void)
{
	return (HWND__**)&handle;
}
