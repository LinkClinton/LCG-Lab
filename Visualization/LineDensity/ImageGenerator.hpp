#pragma once

#include "Utility.hpp"

#include <DirectXTex.h>

class DensityGenerator;
class ColorMapped;

class ImageGenerator {
public:
	ImageGenerator(
		const std::shared_ptr<DensityGenerator>& densityGenerator, 
		const std::shared_ptr<ColorMapped> &colorMapped);

	ImageGenerator(
		const std::shared_ptr<DensityGenerator>& densityGenerator,
		const std::shared_ptr<ColorMapped>& colorMapped,
		SwapChain* swapChain);

	~ImageGenerator();

	void run();

	void save(const std::wstring& name);
private:
	Factory* mFactory;

	std::shared_ptr<DensityGenerator> mDensityGenerator;
	std::shared_ptr<ColorMapped> mColorMapped;

	size_t mWidth;
	size_t mHeight;
private:
	Texture2D* mTarget;

	InputLayout* mInputLayout;

	RenderTarget* mRenderTarget;

	RasterizerState* mRasterizerState;

	ConstantBuffer* mUploadBuffer;
	ConstantBuffer* mColorMappedBuffer;
	VertexBuffer* mVertexBuffer;
	IndexBuffer* mIndexBuffer;

	VertexShader* mVertexShader;

	PixelShader* mPixelShader;
};
