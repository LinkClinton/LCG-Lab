#include "WindowsDepthStencilState.hpp"

#include "Utility.hpp"

#ifdef _WIN32

WindowsDepthStencilState::WindowsDepthStencilState()
{
	mDepthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	mDepthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	mDepthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	mDepthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS;

	mDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	mDepthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	mDepthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	mDepthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS;

	mDepthStencilDesc.DepthEnable = mDepthEnable;
	mDepthStencilDesc.DepthFunc = Utility::ConvertComparisonMode(mComparisonMode);
	mDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
	mDepthStencilDesc.StencilEnable = false;
	mDepthStencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	mDepthStencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	
	mDepthStencilState = nullptr;
}

WindowsDepthStencilState::~WindowsDepthStencilState()
{
	Utility::Dispose(mDepthStencilState);
}

void WindowsDepthStencilState::setDepthEnable(bool enable)
{
	mDepthEnable = enable;

	mDepthStencilDesc.DepthEnable = mDepthEnable;
}

void WindowsDepthStencilState::setComparisonMode(ComparisonMode compairsonMode)
{
	mComparisonMode = compairsonMode;

	mDepthStencilDesc.DepthFunc = Utility::ConvertComparisonMode(mComparisonMode);
}

#endif // _WIN32


