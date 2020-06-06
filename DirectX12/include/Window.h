#ifndef DIRECTX_WINDOW
#define DIRECTX_WINDOW

#include <Vector.h>

struct HWND__;

class Window {
public:
	/*コンストラクタ
	.ウィンドウサイズ
	.スクリーン座標*/
	Window(const okmonn::Vec2& window_size, const okmonn::Vec2& screen_pos = 0x80000000);
	/*デストラクタ*/
	~Window();

public:
	/*ウィンドウハンドルの取得
	return ウィンドウハンドル*/
	HWND__* Get(void) const;
	/*スクリーン座標の取得
	return スクリーン座標*/
	okmonn::Vec2 GetScreenPos(void) const;
	/*ウィンドウサイズの取得
	return ウィンドウサイズ*/
	okmonn::Vec2 GetWindowSize(void) const;

public:
	HWND__* operator->(void) const;
	HWND__** operator&(void);

private:
	/*ウィンドウハンドルの生成*/
	void CreateWindowHandle(void);

private:
	/*ウィンドウハンドル*/
	void* handle;
	/*スクリーン座標*/
	okmonn::Vec2 screen_pos;
	/*ウィンドウサイズ*/
	okmonn::Vec2 window_size;
};

#endif
