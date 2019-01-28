#include "WindowsVertexShader.hpp"
#include "WindowsGraphics.hpp"

#ifdef _WIN32

#include <d3dcompiler.h>

#pragma comment(lib, "d3dcompiler.lib")

WindowsVertexShader::WindowsVertexShader(Graphics * graphics, const std::vector<codebyte>& codeBytes, bool isCompiled)
	:VertexShader(codeBytes)
{

#ifdef _DEBUG
	UINT flag = D3DCOMPILE_DEBUG;
#else
	UINT flag = D3DCOMPILE_OPTIMIZATION_LEVEL2;
#endif // DEBUG

	if (isCompiled == false) {
		ID3DBlob* errorBlob = nullptr;
		ID3DBlob* resultBlob = nullptr;

		D3DCompile(&mCodeBytes[0], mCodeBytes.size(), nullptr, nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_0", flag, 0,
			&resultBlob, &errorBlob);

		static_cast<WindowsGraphics*>(graphics)->mDevice->CreateVertexShader(resultBlob->GetBufferPointer(), resultBlob->GetBufferSize(), nullptr,
			&mVertexShader);

		for (size_t i = 0; i < resultBlob->GetBufferSize(); i++)
			mCompiledCodeBytes.push_back(((codebyte*)resultBlob->GetBufferPointer())[i]);

		Utility::dispose(resultBlob);
		Utility::dispose(errorBlob);
	}
	else {
		static_cast<WindowsGraphics*>(graphics)->mDevice->CreateVertexShader(&codeBytes[0], codeBytes.size(), nullptr,
			&mVertexShader);

		mCompiledCodeBytes = mCodeBytes;
	}
}

WindowsVertexShader::~WindowsVertexShader()
{
	Utility::dispose(mVertexShader);
}

#endif // _WIN32

