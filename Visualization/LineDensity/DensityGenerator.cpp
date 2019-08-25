#include "DensityGenerator.hpp"

#include <algorithm>

#undef max

DensityGenerator::DensityGenerator(
	Factory* factory, 
	const std::vector<LineSeries>& line_series, 
	size_t heatmap_width, size_t heatmap_height) :
	mFactory(factory), mLineSeries(line_series), 
	mWidth(heatmap_width), mHeight(heatmap_height) {

	const auto width = static_cast<int>(mWidth);
	const auto height = static_cast<int>(mHeight);

	mTarget = mFactory->createTexture2D(width, height, PixelFormat::R8G8B8A8Unknown,
		ResourceInfo(BindUsage::RenderTargetUsage));
	mHeatMap = mFactory->createTexture2D(width, height, PixelFormat::R32Float,
		ResourceInfo::UnorderedAccess());
	mBuffer = mFactory->createTexture2D(width, height, PixelFormat::R32Float,
		ResourceInfo::UnorderedAccess());
	mCount = mFactory->createTexture2D(width, 1, PixelFormat::R32Uint,
		ResourceInfo::UnorderedAccess());

	mRenderTarget = mFactory->createRenderTarget(mTarget, mTarget->getPixelFormat());

	mHeatMapRWUsage = mFactory->createUnorderedAccessUsage(mHeatMap, mHeatMap->getPixelFormat());
	mBufferRWUsage = mFactory->createUnorderedAccessUsage(mBuffer, mBuffer->getPixelFormat());
	mCountRWUsage = mFactory->createUnorderedAccessUsage(mCount, mCount->getPixelFormat());

	mInputLayout = mFactory->createInputLayout();
	mInputLayout->addElement(InputLayoutElement("POSITION", sizeof(vec2), 0));

	mTransformBuffer = mFactory->createConstantBuffer(sizeof(mat4) * 2, ResourceInfo::ConstantBuffer());
	mVertexBuffer = mFactory->createVertexBuffer(sizeof(vec3) * 4, sizeof(vec3), ResourceInfo::VertexBuffer());
	
	mCommonVertexShader = mFactory->createVertexShader(ShaderFile::read("./Shader/DensityGeneratorVertex.cso"), true);
	
	mMergePixelShader = mFactory->createPixelShader(ShaderFile::read("./Shader/DensityGeneratorMergePixel.cso"), true);
	mDrawPixelShader = mFactory->createPixelShader(ShaderFile::read("./Shader/DensityGeneratorDrawPixel.cso"), true);

	mRasterizerState = mFactory->createRasterizerState();
	mRasterizerState->enableDepth(false);

	mVertexBuffer = nullptr;
}

DensityGenerator::~DensityGenerator() {
	mFactory->destroyTexture2D(mTarget);
	mFactory->destroyTexture2D(mHeatMap);
	mFactory->destroyTexture2D(mBuffer);
	mFactory->destroyTexture2D(mCount);

	mFactory->destroyInputLayout(mInputLayout);

	mFactory->destroyRenderTarget(mRenderTarget);

	mFactory->destroyConstantBuffer(mTransformBuffer);
	mFactory->destroyVertexBuffer(mVertexBuffer);

	mFactory->destroyVertexShader(mCommonVertexShader);
	
	mFactory->destroyPixelShader(mDrawPixelShader);
	mFactory->destroyPixelShader(mMergePixelShader);

	mFactory->destroyUnorderedAccessUsage(mHeatMapRWUsage);
	mFactory->destroyUnorderedAccessUsage(mBufferRWUsage);
	mFactory->destroyUnorderedAccessUsage(mCountRWUsage);

	mFactory->destroyRasterizerState(mRasterizerState);
}

void DensityGenerator::run() {
	unsigned int uav_uint_clear[4] = { 0, 0, 0, 0 };
	float uav_float_clear[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

	mat4 matrix[] = {
		mat4(1.0f),
		glm::orthoLH_ZO(0.0f, 
		static_cast<float>(mTarget->getWidth()),
		static_cast<float>(mTarget->getHeight()), 0.0f,
		0.0f, 1.0f) 
	};

	mTransformBuffer->update(matrix);

	const auto graphics = mFactory->graphics();
	
	graphics->clearState();
	graphics->clearUnorderedAccessUsageFloat(mHeatMapRWUsage, uav_float_clear);
	
	graphics->setRenderTarget(mRenderTarget);

	graphics->setInputLayout(mInputLayout);
	
	graphics->setVertexShader(mCommonVertexShader);

	graphics->setPrimitiveType(PrimitiveType::LineStrip);
	graphics->setViewPort(0, 0, 
		static_cast<float>(mWidth), 
		static_cast<float>(mHeight));

	graphics->setRasterizerState(mRasterizerState);
	
	graphics->setConstantBuffer(mTransformBuffer, 0);
	graphics->setUnorderedAccessUsage({
		mBufferRWUsage,
		mCountRWUsage,
		mHeatMapRWUsage,
		});


	// get the max size of line series
	auto max_size = 0;

	for (auto& lines : mLineSeries) 
		max_size = std::max(max_size, static_cast<int>(lines.size() + 1));

	std::vector<vec2> lines_points(max_size);

	//when the size of buffer is less than lines
	//we need to expand the buffer
	if (mVertexBuffer == nullptr ||
		mVertexBuffer->count() < max_size) {

		mFactory->destroyVertexBuffer(mVertexBuffer);

		mVertexBuffer = mFactory->createVertexBuffer(
			static_cast<int>(sizeof(vec2)) * max_size, sizeof(vec2), ResourceInfo::VertexBuffer());
	}

	graphics->setVertexBuffer(mVertexBuffer);

	//for each line series
	for (auto &lines : mLineSeries) {
		graphics->clearUnorderedAccessUsageFloat(mBufferRWUsage, uav_float_clear);
		graphics->clearUnorderedAccessUsageUint(mCountRWUsage, uav_uint_clear);

		graphics->setPixelShader(mDrawPixelShader);

		assert(lines.size() >= 2);

		const auto line_count = static_cast<int>(lines.size());

		std::copy(lines.data(), lines.data() + line_count + 1, lines_points.data());

		mVertexBuffer->update(lines_points.data());
		
		graphics->draw(line_count + 1, 0);

		//merge line series
		graphics->setPixelShader(mMergePixelShader);

		graphics->draw(line_count + 1, 0);
	}
}

auto DensityGenerator::data() const -> const std::vector<LineSeries>& {
	return mLineSeries;
}

auto DensityGenerator::width() const -> size_t {
	return mWidth;
}

auto DensityGenerator::height() const -> size_t {
	return mHeight;
}
