#include "WindowsRasterizerState.hpp"

#include "Utility.hpp"

#ifdef _WIN32

WindowsRasterizerState::WindowsRasterizerState()
{
	mRasterizerDesc.FillMode = (D3D11_FILL_MODE)Utility::convertFillMode(mFillMode);
	mRasterizerDesc.CullMode = (D3D11_CULL_MODE)Utility::convertCullMode(mCullMode);
	mRasterizerDesc.FrontCounterClockwise = false;
	mRasterizerDesc.DepthBias = 0;
	mRasterizerDesc.SlopeScaledDepthBias = 0.0f;
	mRasterizerDesc.DepthClipEnable = true;
	mRasterizerDesc.ScissorEnable = false;
	mRasterizerDesc.MultisampleEnable = false;
	mRasterizerDesc.AntialiasedLineEnable = false;

	mRasterizerState = nullptr;
}

WindowsRasterizerState::~WindowsRasterizerState()
{
	Utility::dispose(mRasterizerState);
}

void WindowsRasterizerState::setFillMode(FillMode fillMode)
{
	mFillMode = fillMode;

	mRasterizerDesc.FillMode = (D3D11_FILL_MODE)Utility::convertFillMode(mFillMode);
}

void WindowsRasterizerState::setCullMode(CullMode cullMode)
{
	mCullMode = cullMode;

	mRasterizerDesc.CullMode = (D3D11_CULL_MODE)Utility::convertCullMode(mCullMode);
}

#endif // _WIN32