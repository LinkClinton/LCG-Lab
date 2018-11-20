#include "WindowsGraphics.hpp"
#include "WindowsInputLayout.hpp"
#include "WindowsVertexShader.hpp"
#include "WindowsPixelShader.hpp"
#include "WindowsVertexBuffer.hpp"
#include "WindowsIndexBuffer.hpp"
#include "WindowsConstantBuffer.hpp"
#include "WindowsRenderTarget.hpp"
#include "WindowsDepthStencil.hpp"
#include "WindowsResourceUsage.hpp"
#include "WindowsTexture2D.hpp"
#include "WindowsTexture3D.hpp"
#include "WindowsRasterizerState.hpp"
#include "WindowsDepthStencilState.hpp"
#include "WindowsUnorderedAccessUsage.hpp"

#include "Utility.hpp"

#ifdef _WIN32

#pragma comment(lib, "d3d11.lib")

void WindowsGraphics::setRenderTarget(RenderTarget * renderTarget, DepthStencil * depthStencil,
	const std::vector<UnorderedAccessUsage*>& unorderedAccessUsage)
{
	auto renderTargetInstance = mRenderTarget != nullptr ?
		static_cast<WindowsRenderTarget*>(mRenderTarget)->mRenderTarget : nullptr;

	auto depthStencilInstance = mDepthStencil != nullptr ?
		static_cast<WindowsDepthStencil*>(mDepthStencil)->mDepthStencil : nullptr;

	std::vector<ID3D11UnorderedAccessView*> views(unorderedAccessUsage.size());

	for (size_t i = 0; i < views.size(); i++)
		views[i] = static_cast<WindowsUnorderedAccessUsage*>(unorderedAccessUsage[i])->mUnorderedAccessView;
	
	mDeviceContext->OMSetRenderTargetsAndUnorderedAccessViews(1, &renderTargetInstance,
		depthStencilInstance, 1, views.size(), views.size() != 0 ? &views[0] : nullptr, nullptr);
}

WindowsGraphics::WindowsGraphics()
{
	D3D_FEATURE_LEVEL features[3] = {
		D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0
	};
	
#ifdef _DEBUG
	auto createFlag = D3D11_CREATE_DEVICE_FLAG::D3D11_CREATE_DEVICE_DEBUG;
#else
	auto createFlag = 0;
#endif

	D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE,
		0, createFlag, features, 3, D3D11_SDK_VERSION, &mDevice, &mFeature, &mDeviceContext);
}

WindowsGraphics::~WindowsGraphics()
{
	Utility::Dispose(mDevice);
	Utility::Dispose(mDeviceContext);
}

void WindowsGraphics::clearState()
{
	mDeviceContext->ClearState();

	mUnorderedAccessUsage.clear();

	mRenderTarget = nullptr;
	mDepthStencil = nullptr;
	mInputLayout = nullptr;
	mVertexShader = nullptr;
	mPixelShader = nullptr;
}

void WindowsGraphics::clearRenderTarget(RenderTarget * renderTarget, const float rgba[4])
{
	mDeviceContext->ClearRenderTargetView(static_cast<WindowsRenderTarget*>(renderTarget)->mRenderTarget, rgba);
}

void WindowsGraphics::clearDepthStencil(DepthStencil * depthStencil, float depth, codebyte stencil)
{
	auto windowsDepthStencil = static_cast<WindowsDepthStencil*>(depthStencil);

	mDeviceContext->ClearDepthStencilView(windowsDepthStencil->mDepthStencil,
		D3D11_CLEAR_FLAG::D3D11_CLEAR_DEPTH | D3D11_CLEAR_FLAG::D3D11_CLEAR_STENCIL, depth, stencil);
}

void WindowsGraphics::clearUnorderedAccessUsageUint(UnorderedAccessUsage * unorderedAccessUsage, const unsigned int value[4])
{
	mDeviceContext->ClearUnorderedAccessViewUint(static_cast<WindowsUnorderedAccessUsage*>(unorderedAccessUsage)->mUnorderedAccessView,
		value);
}

void WindowsGraphics::setViewPort(float left, float top, float right, float bottom)
{
	D3D11_VIEWPORT viewPort;

	viewPort.TopLeftX = left;
	viewPort.TopLeftY = top;
	viewPort.Width = right - left;
	viewPort.Height = bottom - top;
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;

	mDeviceContext->RSSetViewports(1, &viewPort);
}

void WindowsGraphics::setVertexBuffer(VertexBuffer * buffer)
{
	UINT stride = buffer->getStride();
	UINT offset = 0;

	mDeviceContext->IASetVertexBuffers(0, 1,
		&static_cast<WindowsVertexBuffer*>(buffer)->mBuffer, &stride, &offset);
}

void WindowsGraphics::setIndexBuffer(IndexBuffer * buffer)
{
	mDeviceContext->IASetIndexBuffer(static_cast<WindowsIndexBuffer*>(buffer)->mBuffer, DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
}


void WindowsGraphics::setConstantBuffer(ConstantBuffer * buffer, int registerID)
{
	mDeviceContext->VSSetConstantBuffers(registerID, 1, &static_cast<WindowsConstantBuffer*>(buffer)->mBuffer);
	mDeviceContext->PSSetConstantBuffers(registerID, 1, &static_cast<WindowsConstantBuffer*>(buffer)->mBuffer);
}

void WindowsGraphics::setResourceUsage(ResourceUsage * resourceUsage, int registerID)
{
	mDeviceContext->VSSetShaderResources(registerID, 1, &static_cast<WindowsResourceUsage*>(resourceUsage)->mShaderResourceView);
	mDeviceContext->PSSetShaderResources(registerID, 1, &static_cast<WindowsResourceUsage*>(resourceUsage)->mShaderResourceView);
}

void WindowsGraphics::setUnorderedAccessUsage(const std::vector<UnorderedAccessUsage*> &unorderedAccessUsage)
{
	mUnorderedAccessUsage = unorderedAccessUsage;
	
	setRenderTarget(mRenderTarget, mDepthStencil, mUnorderedAccessUsage);
}

void WindowsGraphics::setRenderTarget(RenderTarget * renderTarget)
{
	mRenderTarget = renderTarget;

	setRenderTarget(mRenderTarget, mDepthStencil, mUnorderedAccessUsage);
}

void WindowsGraphics::setDepthStencil(DepthStencil * depthStencil)
{
	mDepthStencil = depthStencil;

	setRenderTarget(mRenderTarget, mDepthStencil, mUnorderedAccessUsage);
}

void WindowsGraphics::setInputLayout(InputLayout * inputLayout)
{
	mInputLayout = inputLayout;

	if (mVertexShader != nullptr) {
		auto tempInputLayout = static_cast<WindowsInputLayout*>(mInputLayout);

		tempInputLayout->resetInstance(this, static_cast<WindowsVertexShader*>(mVertexShader));

		mDeviceContext->IASetInputLayout(tempInputLayout->mInputLayout);
	}
}

void WindowsGraphics::setVertexShader(VertexShader * vertexShader)
{
	mVertexShader = vertexShader;

	auto tempVertexShader = static_cast<WindowsVertexShader*>(mVertexShader);

	mDeviceContext->VSSetShader(tempVertexShader->mVertexShader, nullptr, 0);

	if (mInputLayout != nullptr) {
		auto tempInputLayout = static_cast<WindowsInputLayout*>(mInputLayout);

		tempInputLayout->resetInstance(this, tempVertexShader);

		mDeviceContext->IASetInputLayout(tempInputLayout->mInputLayout);
	}
}

void WindowsGraphics::setPixelShader(PixelShader * pixelShader)
{
	mDeviceContext->PSSetShader(static_cast<WindowsPixelShader*>(pixelShader)->mPixelShader, nullptr, 0);
}

void WindowsGraphics::setRasterizerState(RasterizerState * rasterizerState)
{
	auto windowsRasterizerState = static_cast<WindowsRasterizerState*>(rasterizerState);

	Utility::Dispose(windowsRasterizerState->mRasterizerState);

	mDevice->CreateRasterizerState(&windowsRasterizerState->mRasterizerDesc, &windowsRasterizerState->mRasterizerState);

	mDeviceContext->RSSetState(windowsRasterizerState->mRasterizerState);
}

void WindowsGraphics::setDepthStencilState(DepthStencilState * depthStencilState)
{
	auto windowsDepthStencilState = static_cast<WindowsDepthStencilState*>(depthStencilState);

	Utility::Dispose(windowsDepthStencilState->mDepthStencilState);

	mDevice->CreateDepthStencilState(&windowsDepthStencilState->mDepthStencilDesc, &windowsDepthStencilState->mDepthStencilState);
	
	mDeviceContext->OMSetDepthStencilState(windowsDepthStencilState->mDepthStencilState, 0);
}

void WindowsGraphics::setPrimitiveType(PrimitiveType primitiveType)
{
	mDeviceContext->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)Utility::ConvertPrimitiveType(primitiveType));
}

void WindowsGraphics::drawIndexed(int indexCount, int startIndexLocation, int baseVertexLocation)
{
	mDeviceContext->DrawIndexed(indexCount, startIndexLocation, baseVertexLocation);
}

void WindowsGraphics::drawIndexedInstanced(int indexCount, int instanceCount, int startIndexLocation, int baseVertexLocation)
{
	mDeviceContext->DrawIndexedInstanced(indexCount, instanceCount, startIndexLocation, baseVertexLocation, 0);
}

#endif // _WIN32
