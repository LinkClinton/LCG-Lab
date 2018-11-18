#pragma once

#include "Factory.hpp"

#ifdef _WIN32

class WindowsFactory : public Factory {
public:
	WindowsFactory(Graphics* graphics);

	virtual auto createVertexBuffer(int size, int stride) -> VertexBuffer* override;

	virtual auto createIndexBuffer(int size) -> IndexBuffer* override;

	virtual auto createConstantBuffer(int size) -> ConstantBuffer* override;

	virtual auto createTexture2D(int width, int height, PixelFormat pixelFormat, BindUsage bindUsage = BindUsage::NoneUsage) -> Texture2D* override;

	virtual auto createTexture3D(int width, int height, int depth, PixelFormat pixelFormat, BindUsage bindUsage = BindUsage::NoneUsage) -> Texture3D* override;
	
	virtual auto createInputLayout() -> InputLayout* override;

	virtual auto createRenderTarget(SwapChain* swapChain) -> RenderTarget* override;

	virtual auto createRenderTarget(Texture2D* texture2D, PixelFormat pixelFormat) -> RenderTarget* override;

	virtual auto createDepthStencil(Texture2D* texture2D, PixelFormat pixelFormat) -> DepthStencil* override;

	virtual auto createResourceUsage(Texture2D* texture2D, PixelFormat pixelFormat) -> ResourceUsage* override;

	virtual auto createResourceUsage(Texture3D* texture3D, PixelFormat pixelFormat) -> ResourceUsage* override;

	virtual auto createUnorderedAccessUsage(Texture2D* texture2D, PixelFormat pixelFormat) -> UnorderedAccessUsage* override;

	virtual auto createUnorderedAccessUsage(Texture3D* texture3D, PixelFormat pixelFormat)->UnorderedAccessUsage* override;

	virtual auto createVertexShader(const std::vector<codebyte> &codeBytes, bool isCompiled = false) -> VertexShader* override;

	virtual auto createPixelShader(const std::vector<codebyte> &codeBytes, bool isCompiled = false) -> PixelShader* override;

	virtual auto createRasterizerState() -> RasterizerState* override;

	virtual auto createDepthStencilState() -> DepthStencilState* override;

	virtual void destoryVertexbuffer(VertexBuffer* &vertexBuffer)override;

	virtual void destoryIndexBuffer(IndexBuffer* &indexBuffer)override;

	virtual void destoryConstantBuffer(ConstantBuffer* &constantBuffer)override;

	virtual void destoryTexture2D(Texture2D* &texture2D)override;

	virtual void destoryTexture3D(Texture3D* &texture3D)override;

	virtual void destoryInputLayout(InputLayout* &inputLayout)override;

	virtual void destoryRenderTarget(RenderTarget* &renderTarget)override;

	virtual void destoryDepthStencil(DepthStencil* &depthStencil)override;

	virtual void destoryResourceUsage(ResourceUsage* &resourceUsage)override;

	virtual void destoryUnorderedAccessUsage(UnorderedAccessUsage* &unorderedAccessUsage)override;

	virtual void destoryVertexShader(VertexShader* &vertexShader)override;

	virtual void destoryPixelShader(PixelShader* &pixelShader)override;

	virtual void destoryRasterizerState(RasterizerState* &rasterizerState)override;

	virtual void destoryDepthStencilState(DepthStencilState* &depthStencilState)override;
};

#endif // _WIN32


