#pragma once

struct IDXGIFactory7;
struct ID3D12Device5;

class Device
{
public:
	// ファクトリーの取得
	IDXGIFactory7* Factory(void) const;

	// デバイスの取得
	ID3D12Device5* Dev(void) const;

	// インスタンス変数の取得
	static Device& Get(void);

private:
	Device(const Device&) = delete;
	void operator=(const Device&) = delete;

	// コンストラクタ
	Device();
	// デストラクタ
	~Device();

	// ファクトリーの生成
	void CreateFactory(void);

	// デバイスの生成
	void CreateDevice(void);


	// ファクトリー
	IDXGIFactory7* factory;

	// デバイス
	ID3D12Device5* device;
};
