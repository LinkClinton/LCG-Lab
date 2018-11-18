#pragma once

#include "Utility.hpp"

#include <vector>

class Graphics;
class VertexBuffer;
class IndexBuffer;
class ConstantBuffer;
class Texture2D;
class Texture3D;
class SwapChain;
class InputLayout;
class RenderTarget;
class DepthStencil;
class ResourceUsage;
class UnorderedAccessUsage;
class VertexShader;
class PixelShader;
class RasterizerState;
class DepthStencilState;

enum class PixelFormat;

class Factory {
protected:
	Graphics* mGraphics;

	Factory(Graphics* graphics) :
		mGraphics(graphics) {}

public:
	virtual auto createVertexBuffer(int size, int stride) -> VertexBuffer* = 0;

	virtual auto createIndexBuffer(int size) -> IndexBuffer* = 0;

	virtual auto createConstantBuffer(int size) -> ConstantBuffer* = 0;

	virtual auto createTexture2D(int width, int height, PixelFormat pixelFormat, BindUsage bindUsage = BindUsage::NoneUsage) -> Texture2D* = 0;

	virtual auto createTexture3D(int width, int height, int depth, PixelFormat pixelFormat, BindUsage bindUsage = BindUsage::NoneUsage) -> Texture3D * = 0;

	virtual auto createInputLayout() -> InputLayout* = 0;

	virtual auto createRenderTarget(SwapChain* swapChain) -> RenderTarget* = 0;

	virtual auto createRenderTarget(Texture2D* texture2D, PixelFormat pixelFormat) -> RenderTarget* = 0;

	virtual auto createDepthStencil(Texture2D* texture2D, PixelFormat pixelFormat) -> DepthStencil* = 0;

	virtual auto createResourceUsage(Texture2D* texture2D, PixelFormat pixelFormat) -> ResourceUsage* = 0;

	virtual auto createResourceUsage(Texture3D* texture3D, PixelFormat pixelFormat) -> ResourceUsage* = 0;

	virtual auto createUnorderedAccessUsage(Texture2D* texture2D, PixelFormat pixelFormat) -> UnorderedAccessUsage* = 0;

	virtual auto createUnorderedAccessUsage(Texture3D* texture3D, PixelFormat pixelFormat) -> UnorderedAccessUsage* = 0;

	virtual auto createVertexShader(const std::vector<codebyte> &codeBytes, bool isCompiled = false) -> VertexShader* = 0;

	virtual auto createPixelShader(const std::vector<codebyte> &codeBytes, bool isCompiled = false) -> PixelShader* = 0;

	virtual auto createRasterizerState() -> RasterizerState* = 0;

	virtual auto createDepthStencilState() -> DepthStencilState* = 0;

	virtual void destoryVertexbuffer(VertexBuffer* &vertexBuffer) = 0;

	virtual void destoryIndexBuffer(IndexBuffer* &indexBuffer) = 0;

	virtual void destoryConstantBuffer(ConstantBuffer* &constantBuffer) = 0;

	virtual void destoryTexture2D(Texture2D* &texture2D) = 0;

	virtual void destoryTexture3D(Texture3D* &texture3D) = 0;

	virtual void destoryInputLayout(InputLayout* &inputLayout) = 0;

	virtual void destoryRenderTarget(RenderTarget* &renderTarget) = 0;

	virtual void destoryDepthStencil(DepthStencil* &depthStencil) = 0;

	virtual void destoryResourceUsage(ResourceUsage* &resourceUsage) = 0;
	
	virtual void destoryUnorderedAccessUsage(UnorderedAccessUsage* &unorderedAccessUsage) = 0;

	virtual void destoryVertexShader(VertexShader* &vertexShader) = 0;

	virtual void destoryPixelShader(PixelShader* &pixelShader) = 0;

	virtual void destoryRasterizerState(RasterizerState* &rasterState) = 0;

	virtual void destoryDepthStencilState(DepthStencilState* &depthStencilState) = 0;
};