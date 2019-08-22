#pragma once

#include "Graphics.hpp"

#ifdef _WIN32

#include <d3d11.h>

class WindowsGraphics : public Graphics {
private:
	ID3D11Device* mDevice;
	ID3D11DeviceContext* mDeviceContext;

	D3D_FEATURE_LEVEL mFeature;

	friend class WindowsFramework;
	friend class WindowsSwapChain;
	friend class WindowsVertexShader;
	friend class WindowsPixelShader;
	friend class WindowsInputLayout;
	friend class WindowsConstantBuffer;
	friend class WindowsVertexBuffer;
	friend class WindowsIndexBuffer;
	friend class WindowsRenderTarget;
	friend class WindowsDepthStencil;
	friend class WindowsTexture2D;
	friend class WindowsTexture3D;
	friend class WindowsResourceUsage;
	friend class WindowsUnorderedAccessUsage;

	void setRenderTarget(RenderTarget* renderTarget, DepthStencil* depthStencil,
		const std::vector<UnorderedAccessUsage*> &unorderedAccessUsage);
public:
	WindowsGraphics();

	~WindowsGraphics();

	virtual void clearState()override;

	virtual void clearRenderTarget(RenderTarget* renderTarget, const float rgba[4])override;

	virtual void clearDepthStencil(DepthStencil* depthStencil, float depth, codebyte stencil)override;

	virtual void clearUnorderedAccessUsageUint(UnorderedAccessUsage* unorderedAccessUsage, const unsigned int value[4])override;

	virtual void clearUnorderedAccessUsageFloat(UnorderedAccessUsage* unorderedAccessUsage, const float value[4]) override;

	virtual void setViewPort(float left, float top, float right, float bototm)override;

	virtual void setVertexBuffer(VertexBuffer* buffer)override;

	virtual void setIndexBuffer(IndexBuffer* buffer)override;

	virtual void setConstantBuffer(ConstantBuffer* buffer, int registerID)override;

	virtual void setResourceUsage(ResourceUsage* resourceUsage, int registerID)override;
	
	virtual void setUnorderedAccessUsage(const std::vector<UnorderedAccessUsage*> &unorderedAccessUsage)override;

	virtual void setRenderTarget(RenderTarget* renderTarget)override;

	virtual void setDepthStencil(DepthStencil* depthStencil)override;

	virtual void setInputLayout(InputLayout* inputLayout)override;

	virtual void setVertexShader(VertexShader* vertexShader)override;

	virtual void setPixelShader(PixelShader* pixelShader)override;

	virtual void setRasterizerState(RasterizerState* rasterizerState)override;

	virtual void setDepthStencilState(DepthStencilState* depthStencilState)override;

	virtual void setPrimitiveType(PrimitiveType primitiveType)override;

	virtual void drawIndexed(int indexCount, int startIndexLocation, int baseVertexLocation)override;

	virtual void drawIndexedInstanced(int indexCount, int instanceCount, int startIndexLocation, int baseVertexLocation)override;
};

#endif // _WIN32
