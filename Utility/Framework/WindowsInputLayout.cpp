#include "WindowsInputLayout.hpp"
#include "WindowsGraphics.hpp"
#include "WindowsVertexShader.hpp"

#include "Utility.hpp"

#ifdef _WIN32

void WindowsInputLayout::resetInstance(WindowsGraphics * graphics, WindowsVertexShader * vertexShader)
{
	mElementDesc.resize(mElements.size());

	for (size_t i = 0; i < mElements.size(); i++) {
		D3D11_INPUT_ELEMENT_DESC inputDesc;

		inputDesc.SemanticName = &mElements[i].Tag[0];
		inputDesc.SemanticIndex = mElements[i].Index;
		inputDesc.InputSlot = 0;
		inputDesc.InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
		inputDesc.InstanceDataStepRate = 0;

		if (i == 0) inputDesc.AlignedByteOffset = 0;
		else inputDesc.AlignedByteOffset = mElementDesc[i - 1].AlignedByteOffset + mElements[i - 1].Size;

		switch (mElements[i].Size)
		{
		case 4:
			inputDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT;
			break;
		case 8:
			inputDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT;
			break;
		case 12:
			inputDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT;
			break;
		case 16:
			inputDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;
			break;

		default:
			std::runtime_error("The input layout element size is invalid.");
			break;
		}

		mElementDesc[i] = inputDesc;
	}

	Utility::Dispose(mInputLayout);

	auto shaderCompiledCode = vertexShader->getCompiledCodeBytes();

	graphics->mDevice->CreateInputLayout(&mElementDesc[0], mElementDesc.size(),
		&shaderCompiledCode[0], shaderCompiledCode.size(), &mInputLayout);
}

WindowsInputLayout::WindowsInputLayout(const std::vector<InputLayoutElement>& elements) :
	InputLayout()
{
	for (size_t i = 0; i < elements.size(); i++)
		addElement(elements[i]);

	mInputLayout = nullptr;
}

WindowsInputLayout::~WindowsInputLayout()
{
	Utility::Dispose(mInputLayout);
}

void WindowsInputLayout::addElement(const InputLayoutElement & element)
{
	mElements.push_back(element);
}

#endif // _WIN32
