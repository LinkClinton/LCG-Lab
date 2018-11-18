#pragma once

#include "DepthStencilState.hpp"

#ifdef _WIN32

#include <d3d11.h>

class WindowsDepthStencilState : public DepthStencilState {
private:
	D3D11_DEPTH_STENCIL_DESC mDepthStencilDesc;
	ID3D11DepthStencilState* mDepthStencilState;

	friend class WindowsGraphics;
public:
	WindowsDepthStencilState();

	~WindowsDepthStencilState();

	virtual void setDepthEnable(bool enable)override;

	virtual void setComparisonMode(ComparisonMode compairsonMode)override;
};

#endif // _WIN32
