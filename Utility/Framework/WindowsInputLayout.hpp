#pragma once

#include "InputLayout.hpp"

#ifdef _WIN32

#include <d3d11.h>

class WindowsGraphics;
class WindowsVertexShader;

class WindowsInputLayout : public InputLayout {
private:
	ID3D11InputLayout* mInputLayout;

	std::vector<D3D11_INPUT_ELEMENT_DESC> mElementDesc;

	void resetInstance(WindowsGraphics* graphics, WindowsVertexShader* vertexShader);

	friend class WindowsGraphics;
public:
	WindowsInputLayout() = default;

	WindowsInputLayout(const std::vector<InputLayoutElement> &elements);

	~WindowsInputLayout();

	virtual void addElement(const InputLayoutElement &element)override;
};

#endif // _WIN32
