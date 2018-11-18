#pragma once

#include "RasterizerState.hpp"

#ifdef _WIN32

#include <d3d11.h>

class WindowsRasterizerState : public RasterizerState {
private:
	D3D11_RASTERIZER_DESC mRasterizerDesc;
	ID3D11RasterizerState* mRasterizerState;

	friend class WindowsGraphics;
public:
	WindowsRasterizerState();

	~WindowsRasterizerState();

	virtual void setFillMode(FillMode fillMode)override;

	virtual void setCullMode(CullMode cullMode)override;
};

#endif // _WIN32
