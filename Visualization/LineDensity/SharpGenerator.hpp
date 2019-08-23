#pragma once

#include "Utility.hpp"

class LineSeries;
class DensityGenerator;

class SharpGenerator {
public:
	SharpGenerator(
		Factory* factory,
		const std::shared_ptr<DensityGenerator>& densityGenerator,
		size_t width, size_t height);

	SharpGenerator(
		Factory* factory,
		const std::shared_ptr<DensityGenerator>& densityGenerator,
		SwapChain* swapChain);

	~SharpGenerator();

	void run(real width = 2.0f);

	void save(const std::wstring& fileName);
private:
	Factory* mFactory;

	std::shared_ptr<DensityGenerator> mDensityGenerator;

	size_t mWidth;
	size_t mHeight;
private:
	Texture2D* mTarget;
	
	InputLayout* mInputLayout;

	RenderTarget* mRenderTarget;

	StructuredBuffer* mInstanceBuffer;

	ConstantBuffer* mTransformBuffer;
	VertexBuffer* mVertexBuffer;
	IndexBuffer* mIndexBuffer;

	VertexShader* mVertexShader;

	RasterizerState* mRasterizerState;

	PixelShader* mPixelShader;
	
	ResourceUsage* mInstanceUsage;
};