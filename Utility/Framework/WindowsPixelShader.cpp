#include "WindowsPixelShader.hpp"
#include "WindowsGraphics.hpp"

#ifdef _WIN32

#include <d3dcompiler.h>

WindowsPixelShader::WindowsPixelShader(Graphics * graphics, const std::vector<codebyte>& codeBytes, bool isCompiled)
	: PixelShader(codeBytes)
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
			D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ps_5_0", flag, 0,
			&resultBlob, &errorBlob);

		static_cast<WindowsGraphics*>(graphics)->mDevice->CreatePixelShader(resultBlob->GetBufferPointer(), resultBlob->GetBufferSize(), nullptr,
			&mPixelShader);

		for (size_t i = 0; i < resultBlob->GetBufferSize(); i++)
			mCompiledCodeBytes.push_back(((codebyte*)resultBlob->GetBufferPointer())[i]);

		Utility::dispose(errorBlob);
		Utility::dispose(resultBlob);
	}
	else {
		static_cast<WindowsGraphics*>(graphics)->mDevice->CreatePixelShader(&mCodeBytes[0], mCodeBytes.size(), nullptr,
			&mPixelShader);

		mCompiledCodeBytes = mCodeBytes;
	}
}

WindowsPixelShader::~WindowsPixelShader()
{
	Utility::dispose(mPixelShader);
}

#endif // _WIN32


