#ifndef DIRECTX_SHADER_LOADER
#define DIRECTX_SHADER_LOADER

#include "BaseClass.h"
#include <string>

struct ShaderLoader :
	public BaseClass<ID3DBlob> {
public:
	/*コンストラクタ
	.ファイルパス
	.エントリー関数名
	.シェーダモデル*/
	ShaderLoader(const std::string& file_path, 
		const std::string& entry_func_name, const std::string& shader_model);

private:
	/*シェーダのコンパイル
	.ファイルパス
	.エントリー関数名
	.シェーダモデル*/
	void ShaderCompile(const std::string& file_path,
		const std::string& entry_func_name, const std::string& shader_model);
};

#endif
