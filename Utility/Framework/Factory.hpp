#pragma once

#include "Utility.hpp"
#include "ResourceInfo.hpp"

#include <vector>

class Framework;
class Graphics;
class VertexBuffer;
class IndexBuffer;
class StructuredBuffer;
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
class Input;

enum class PixelFormat;

class Factory {
protected:
	Graphics* mGraphics;

	Factory(Graphics* graphics) :
		mGraphics(graphics) {}

public:
	virtual auto createVertexBuffer(int size, int stride, const ResourceInfo &info) -> VertexBuffer* = 0;

	virtual auto createIndexBuffer(int size, const ResourceInfo &info) -> IndexBuffer* = 0;

	virtual auto createStructuredBuffer(int element_size, int element_count, const ResourceInfo& info) -> StructuredBuffer* = 0;

	virtual auto createConstantBuffer(int size, const ResourceInfo &info) -> ConstantBuffer* = 0;

	virtual auto createTexture2D(int width, int height, PixelFormat pixelFormat, const ResourceInfo &info) -> Texture2D* = 0;

	virtual auto createTexture3D(int width, int height, int depth, PixelFormat pixelFormat, const ResourceInfo &info) -> Texture3D * = 0;

	virtual auto createInputLayout() -> InputLayout* = 0;

	virtual auto createRenderTarget(SwapChain* swapChain) -> RenderTarget* = 0;

	virtual auto createRenderTarget(Texture2D* texture2D, PixelFormat pixelFormat) -> RenderTarget* = 0;

	virtual auto createDepthStencil(Texture2D* texture2D, PixelFormat pixelFormat) -> DepthStencil* = 0;

	virtual auto createResourceUsage(Texture2D* texture2D, PixelFormat pixelFormat) -> ResourceUsage* = 0;

	virtual auto createResourceUsage(Texture3D* texture3D, PixelFormat pixelFormat) -> ResourceUsage* = 0;

	virtual auto createResourceUsage(StructuredBuffer* structuredBuffer) -> ResourceUsage* = 0;

	virtual auto createUnorderedAccessUsage(Texture2D* texture2D, PixelFormat pixelFormat) -> UnorderedAccessUsage* = 0;

	virtual auto createUnorderedAccessUsage(Texture3D* texture3D, PixelFormat pixelFormat) -> UnorderedAccessUsage* = 0;

	virtual auto createVertexShader(const std::vector<codebyte> &codeBytes, bool isCompiled = false) -> VertexShader* = 0;

	virtual auto createPixelShader(const std::vector<codebyte> &codeBytes, bool isCompiled = false) -> PixelShader* = 0;

	virtual auto createRasterizerState() -> RasterizerState* = 0;

	virtual auto createDepthStencilState() -> DepthStencilState* = 0;

	virtual auto createInput(Framework* framework) -> Input* = 0;

	virtual void destroyVertexBuffer(VertexBuffer* &vertexBuffer) = 0;

	virtual void destroyIndexBuffer(IndexBuffer* &indexBuffer) = 0;

	virtual void destroyStructuredBuffer(StructuredBuffer*& structuredBuffer) = 0;

	virtual void destroyConstantBuffer(ConstantBuffer* &constantBuffer) = 0;

	virtual void destroyTexture2D(Texture2D* &texture2D) = 0;

	virtual void destroyTexture3D(Texture3D* &texture3D) = 0;

	virtual void destroyInputLayout(InputLayout* &inputLayout) = 0;

	virtual void destroyRenderTarget(RenderTarget* &renderTarget) = 0;

	virtual void destroyDepthStencil(DepthStencil* &depthStencil) = 0;

	virtual void destroyResourceUsage(ResourceUsage* &resourceUsage) = 0;
	
	virtual void destroyUnorderedAccessUsage(UnorderedAccessUsage* &unorderedAccessUsage) = 0;

	virtual void destroyVertexShader(VertexShader* &vertexShader) = 0;

	virtual void destroyPixelShader(PixelShader* &pixelShader) = 0;

	virtual void destroyRasterizerState(RasterizerState* &rasterState) = 0;

	virtual void destroyDepthStencilState(DepthStencilState* &depthStencilState) = 0;

	virtual void destroyInput(Input* &input) = 0;

	auto graphics() const -> Graphics* { return mGraphics; }
};