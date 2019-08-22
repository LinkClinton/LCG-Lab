#pragma once

#include "Factory.hpp"

#ifdef _WIN32

class WindowsFactory : public Factory {
public:
	WindowsFactory(Graphics* graphics);

	virtual auto createVertexBuffer(int size, int stride, const ResourceInfo &info)->VertexBuffer* override;

	virtual auto createIndexBuffer(int size, const ResourceInfo &info)->IndexBuffer* override;

	virtual auto createStructuredBuffer(int element_size, int element_count, const ResourceInfo& info) -> StructuredBuffer* override;

	virtual auto createConstantBuffer(int size, const ResourceInfo &info)->ConstantBuffer* override;

	virtual auto createTexture2D(int width, int height, PixelFormat pixelFormat, const ResourceInfo &info)->Texture2D* override;

	virtual auto createTexture3D(int width, int height, int depth, PixelFormat pixelFormat, const ResourceInfo &info)->Texture3D * override;
	
	virtual auto createInputLayout() -> InputLayout* override;

	virtual auto createRenderTarget(SwapChain* swapChain) -> RenderTarget* override;

	virtual auto createRenderTarget(Texture2D* texture2D, PixelFormat pixelFormat) -> RenderTarget* override;

	virtual auto createDepthStencil(Texture2D* texture2D, PixelFormat pixelFormat) -> DepthStencil* override;

	virtual auto createResourceUsage(StructuredBuffer* structuredBuffer) -> ResourceUsage* override;

	virtual auto createResourceUsage(Texture2D* texture2D, PixelFormat pixelFormat) -> ResourceUsage* override;

	virtual auto createResourceUsage(Texture3D* texture3D, PixelFormat pixelFormat) -> ResourceUsage* override;

	virtual auto createUnorderedAccessUsage(Texture2D* texture2D, PixelFormat pixelFormat) -> UnorderedAccessUsage* override;

	virtual auto createUnorderedAccessUsage(Texture3D* texture3D, PixelFormat pixelFormat)->UnorderedAccessUsage* override;

	virtual auto createVertexShader(const std::vector<codebyte> &codeBytes, bool isCompiled = false) -> VertexShader* override;

	virtual auto createPixelShader(const std::vector<codebyte> &codeBytes, bool isCompiled = false) -> PixelShader* override;

	virtual auto createRasterizerState() -> RasterizerState* override;

	virtual auto createDepthStencilState() -> DepthStencilState* override;

	virtual auto createInput(Framework* framework) -> Input* override;

	virtual void destroyVertexBuffer(VertexBuffer* &vertexBuffer)override;

	virtual void destroyIndexBuffer(IndexBuffer* &indexBuffer)override;

	virtual void destroyStructuredBuffer(StructuredBuffer*& structuredBuffer) override;

	virtual void destroyConstantBuffer(ConstantBuffer* &constantBuffer)override;

	virtual void destroyTexture2D(Texture2D* &texture2D)override;

	virtual void destroyTexture3D(Texture3D* &texture3D)override;

	virtual void destroyInputLayout(InputLayout* &inputLayout)override;

	virtual void destroyRenderTarget(RenderTarget* &renderTarget)override;

	virtual void destroyDepthStencil(DepthStencil* &depthStencil)override;

	virtual void destroyResourceUsage(ResourceUsage* &resourceUsage)override;

	virtual void destroyUnorderedAccessUsage(UnorderedAccessUsage* &unorderedAccessUsage)override;

	virtual void destroyVertexShader(VertexShader* &vertexShader)override;

	virtual void destroyPixelShader(PixelShader* &pixelShader)override;

	virtual void destroyRasterizerState(RasterizerState* &rasterizerState)override;

	virtual void destroyDepthStencilState(DepthStencilState* &depthStencilState)override;

	virtual void destroyInput(Input* &input)override;
};

#endif // _WIN32


