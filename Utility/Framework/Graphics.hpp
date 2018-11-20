#pragma once

class InputLayout;
class VertexShader;
class PixelShader;
class VertexBuffer;
class IndexBuffer;
class ConstantBuffer;
class RenderTarget;
class DepthStencil;
class ResourceUsage;
class UnorderedAccessUsage;
class Texture2D;
class Texture3D;
class RasterizerState;
class DepthStencilState;

enum class PrimitiveType;

#include "Utility.hpp"

#include <vector>

class Graphics {
protected:
	InputLayout* mInputLayout;
	
	VertexShader* mVertexShader;
	PixelShader* mPixelShader;

	RenderTarget* mRenderTarget;
	DepthStencil* mDepthStencil;

	std::vector<UnorderedAccessUsage*> mUnorderedAccessUsage;
	
	Graphics() :
		mInputLayout(nullptr), mVertexShader(nullptr), 
		mPixelShader(nullptr), mRenderTarget(nullptr),
		mDepthStencil(nullptr) {}
public:
	virtual void clearState() = 0;

	virtual void clearRenderTarget(RenderTarget* renderTarget, const float rgba[4]) = 0;

	virtual void clearDepthStencil(DepthStencil* depthStencil, float depth, codebyte stencil) = 0;

	virtual void clearUnorderedAccessUsageUint(UnorderedAccessUsage* unorderedAccessUsage, const unsigned int value[4]) = 0;

	virtual void setViewPort(float left, float top, float right, float bototm) = 0;

	virtual void setVertexBuffer(VertexBuffer* buffer) = 0;

	virtual void setIndexBuffer(IndexBuffer* buffer) = 0;

	virtual void setConstantBuffer(ConstantBuffer* buffer, int registerID) = 0;

	virtual void setResourceUsage(ResourceUsage* resourceUsage, int registerID) = 0;

	virtual void setUnorderedAccessUsage(const std::vector<UnorderedAccessUsage*> &unorderedAccessUsages) = 0;

	virtual void setRenderTarget(RenderTarget* renderTarget) = 0;

	virtual void setDepthStencil(DepthStencil* depthStencil) = 0;

	virtual void setInputLayout(InputLayout* inputLayout) = 0;

	virtual void setVertexShader(VertexShader* vertexShader) = 0;

	virtual void setPixelShader(PixelShader* pixelShader) = 0;

	virtual void setRasterizerState(RasterizerState* rasterizerState) = 0;

	virtual void setDepthStencilState(DepthStencilState* depthStencilState) = 0;
	
	virtual void setPrimitiveType(PrimitiveType primitiveType) = 0;

	virtual void drawIndexed(int indexCount, int startIndexLocation, int baseVertexLocation) = 0;

	virtual void drawIndexedInstanced(int indexCount, int instanceCount, int startIndexLocation, int baseVertexLocation) = 0;
};