#include "ImageGenerator.hpp"
#include "DensityGenerator.hpp"
#include "ColorMapped.hpp"

ImageGenerator::ImageGenerator(
	const std::shared_ptr<DensityGenerator>& densityGenerator, 
	const std::shared_ptr<ColorMapped> &colorMapped) :
	mFactory(densityGenerator->mFactory), mDensityGenerator(densityGenerator),
	mColorMapped(colorMapped),
	mWidth(densityGenerator->mWidth), mHeight(densityGenerator->mHeight) {

	mTarget = mFactory->createTexture2D(
		static_cast<int>(mWidth),
		static_cast<int>(mHeight),
		PixelFormat::R8G8B8A8Unknown,
		ResourceInfo(BindUsage::RenderTargetUsage));

	mInputLayout = mFactory->createInputLayout();
	mInputLayout->addElement(InputLayoutElement("POSITION", 12, 0));

	mRenderTarget = mFactory->createRenderTarget(mTarget, mTarget->getPixelFormat());

	mUploadBuffer = mFactory->createConstantBuffer(sizeof(mat4) * 3, ResourceInfo::ConstantBuffer());
	mColorMappedBuffer = mFactory->createConstantBuffer(sizeof(vec4) * MAX_COLOR_POINTS, ResourceInfo::ConstantBuffer());
	mVertexBuffer = mFactory->createVertexBuffer(sizeof(vec3) * 4, sizeof(vec3), ResourceInfo::VertexBuffer());
	mIndexBuffer = mFactory->createIndexBuffer(sizeof(unsigned) * 6, ResourceInfo::IndexBuffer());

	mVertexShader = mFactory->createVertexShader(ShaderFile::read("./Shader/ImageGeneratorVertex.cso"), true);

	mPixelShader = mFactory->createPixelShader(ShaderFile::read("./Shader/ImageGeneratorPixel.cso"), true);

	mRasterizerState = mFactory->createRasterizerState();
	mRasterizerState->enableDepth(false);

	vec3 position[] = {
		vec3(0, 0, 0),
		vec3(0, 1, 0),
		vec3(1, 1, 0),
		vec3(1, 0, 0)
	};

	unsigned indices[] = { 0, 2, 1, 0, 3, 2 };

	mVertexBuffer->update(&position);
	mIndexBuffer->update(&indices);
}

ImageGenerator::ImageGenerator(const std::shared_ptr<DensityGenerator>& densityGenerator,
	const std::shared_ptr<ColorMapped>& colorMapped, SwapChain* swapChain) :
	mFactory(densityGenerator->mFactory), mDensityGenerator(densityGenerator),
	mColorMapped(colorMapped),
	mWidth(swapChain->getWidth()), mHeight(swapChain->getHeight()) {

	mTarget = nullptr;

	mInputLayout = mFactory->createInputLayout();
	mInputLayout->addElement(InputLayoutElement("POSITION", 12, 0));

	mRenderTarget = mFactory->createRenderTarget(swapChain);

	mUploadBuffer = mFactory->createConstantBuffer(sizeof(mat4) * 3, ResourceInfo::ConstantBuffer());
	mColorMappedBuffer = mFactory->createConstantBuffer(sizeof(vec4) * MAX_COLOR_POINTS, ResourceInfo::ConstantBuffer());
	mVertexBuffer = mFactory->createVertexBuffer(sizeof(vec3) * 4, sizeof(vec3), ResourceInfo::VertexBuffer());
	mIndexBuffer = mFactory->createIndexBuffer(sizeof(unsigned) * 6, ResourceInfo::IndexBuffer());

	mVertexShader = mFactory->createVertexShader(ShaderFile::read("./Shader/ImageGeneratorVertex.cso"), true);

	mPixelShader = mFactory->createPixelShader(ShaderFile::read("./Shader/ImageGeneratorPixel.cso"), true);

	mRasterizerState = mFactory->createRasterizerState();
	mRasterizerState->enableDepth(false);

	vec3 position[] = {
		vec3(0, 0, 0),
		vec3(0, 1, 0),
		vec3(1, 1, 0),
		vec3(1, 0, 0)
	};

	unsigned indices[] = { 0, 2, 1, 0, 3, 2 };

	mVertexBuffer->update(&position);
	mIndexBuffer->update(&indices);
}

ImageGenerator::~ImageGenerator() {
	mFactory->destroyTexture2D(mTarget);
	mFactory->destroyInputLayout(mInputLayout);
	mFactory->destroyRenderTarget(mRenderTarget);
	mFactory->destroyConstantBuffer(mUploadBuffer);
	mFactory->destroyConstantBuffer(mColorMappedBuffer);
	mFactory->destroyVertexBuffer(mVertexBuffer);
	mFactory->destroyIndexBuffer(mIndexBuffer);
	mFactory->destroyVertexShader(mVertexShader);
	mFactory->destroyPixelShader(mPixelShader);
	mFactory->destroyRasterizerState(mRasterizerState);
}

void ImageGenerator::run() {
	const float color[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	const auto width = static_cast<float>(mWidth);
	const auto height = static_cast<float>(mHeight);

	const auto graphics = mFactory->graphics();

	auto colorMapped = mColorMapped->colors();

	assert(colorMapped.size() <= MAX_COLOR_POINTS);

	mat4 matrix[] = {
		mat4(1.0f),
		glm::orthoLH_ZO(0.0f, width, height, 0.0f,
		0.0f, 1.0f),
		mat4(0)
	};

	graphics->clearState();

	graphics->setRenderTarget(mRenderTarget);
	graphics->clearRenderTarget(mRenderTarget, color);

	graphics->setInputLayout(mInputLayout);
	graphics->setVertexBuffer(mVertexBuffer);
	graphics->setIndexBuffer(mIndexBuffer);

	graphics->setPrimitiveType(PrimitiveType::TriangleList);
	graphics->setViewPort(0, 0, width, height);

	graphics->setRasterizerState(mRasterizerState);

	graphics->setConstantBuffer(mUploadBuffer, 0);
	graphics->setConstantBuffer(mColorMappedBuffer, 1);
	graphics->setUnorderedAccessUsage({ mDensityGenerator->mHeatMapRWUsage });

	graphics->setVertexShader(mVertexShader);
	graphics->setPixelShader(mPixelShader);

	matrix[0] = scale(matrix[0], vec3(width, height, 1.0f));
	matrix[2][0] = vec4(
		mDensityGenerator->mWidth / width,
		mDensityGenerator->mHeight / height,
		static_cast<float>(colorMapped.size()),
		mColorMapped->space());

	mUploadBuffer->update(matrix);
	mColorMappedBuffer->update(colorMapped.data());

	graphics->drawIndexed(6, 0, 0);
}

void ImageGenerator::save(const std::wstring& name) {
	assert(mTarget != nullptr);

	mTarget->save(name);
}
