#pragma once
#include "../Vector2.h"

struct HWND__;

class Window
{
public:
	// コンストラクタ
	Window(const Vec2& size);
	// デストラクタ
	~Window();

	// ウィンドウハンドルの取得
	HWND__* Win(void)const;

	// ウィンドウサイズの取得
	Vec2 WinSize(void) const;

	// メッセージ確認
	static bool CheckMsg(void);

private:
	// ウィンドウ生成
	void CreateWin(const Vec2& size);

	// ウィンドウハンドル
	void* handle;
};
