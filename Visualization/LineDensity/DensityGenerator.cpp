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
	mInputLayout->addElement(InputLayoutElement("POSITION", 12, 0));

	mTransformBuffer = mFactory->createConstantBuffer(sizeof(mat4) * 2, ResourceInfo::ConstantBuffer());
	mVertexBuffer = mFactory->createVertexBuffer(sizeof(vec3) * 4, sizeof(vec3), ResourceInfo::VertexBuffer());
	mIndexBuffer = mFactory->createIndexBuffer(sizeof(unsigned) * 6, ResourceInfo::IndexBuffer());

	mCommonVertexShader = mFactory->createVertexShader(ShaderFile::read("./Shader/DensityGeneratorVertex.cso"), true);
	
	mMergePixelShader = mFactory->createPixelShader(ShaderFile::read("./Shader/DensityGeneratorMergePixel.cso"), true);
	mDrawPixelShader = mFactory->createPixelShader(ShaderFile::read("./Shader/DensityGeneratorDrawPixel.cso"), true);

	mRasterizerState = mFactory->createRasterizerState();
	mRasterizerState->enableDepth(false);

	mInstanceBuffer = nullptr;
	mInstanceUsage = nullptr;

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

DensityGenerator::~DensityGenerator() {
	mFactory->destroyTexture2D(mTarget);
	mFactory->destroyTexture2D(mHeatMap);
	mFactory->destroyTexture2D(mBuffer);
	mFactory->destroyTexture2D(mCount);

	mFactory->destroyInputLayout(mInputLayout);

	mFactory->destroyRenderTarget(mRenderTarget);

	mFactory->destroyStructuredBuffer(mInstanceBuffer);
	mFactory->destroyConstantBuffer(mTransformBuffer);
	mFactory->destroyVertexBuffer(mVertexBuffer);
	mFactory->destroyIndexBuffer(mIndexBuffer);

	mFactory->destroyVertexShader(mCommonVertexShader);
	
	mFactory->destroyPixelShader(mDrawPixelShader);
	mFactory->destroyPixelShader(mMergePixelShader);

	mFactory->destroyUnorderedAccessUsage(mHeatMapRWUsage);
	mFactory->destroyUnorderedAccessUsage(mBufferRWUsage);
	mFactory->destroyUnorderedAccessUsage(mCountRWUsage);

	mFactory->destroyResourceUsage(mInstanceUsage);

	mFactory->destroyRasterizerState(mRasterizerState);
}

void DensityGenerator::run(real width) {
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
	graphics->setVertexBuffer(mVertexBuffer);
	graphics->setIndexBuffer(mIndexBuffer);

	graphics->setVertexShader(mCommonVertexShader);

	graphics->setPrimitiveType(PrimitiveType::TriangleList);
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
		max_size = std::max(max_size, static_cast<int>(lines.size()));

	std::vector<mat4> instance_data(max_size);

	//when the size of buffer is less than lines
	//we need to expand the buffer
	if (mInstanceBuffer == nullptr ||
		mInstanceBuffer->element_count() < max_size) {

		//destroy old buffer
		mFactory->destroyStructuredBuffer(mInstanceBuffer);
		mFactory->destroyResourceUsage(mInstanceUsage);

		//create new buffer
		mInstanceBuffer = mFactory->createStructuredBuffer(sizeof(mat4),
			max_size, ResourceInfo::ShaderResource());
		mInstanceUsage = mFactory->createResourceUsage(mInstanceBuffer);
	}

	graphics->setResourceUsage(mInstanceUsage, 0);

	//for each line series
	for (auto &lines : mLineSeries) {
		graphics->clearUnorderedAccessUsageFloat(mBufferRWUsage, uav_float_clear);
		graphics->clearUnorderedAccessUsageUint(mCountRWUsage, uav_uint_clear);

		graphics->setPixelShader(mDrawPixelShader);

		assert(lines.size() >= 2);

		const auto line_count = static_cast<int>(lines.size());

		for (size_t index = 0; index < lines.size(); index++) {
			instance_data[index] = lines.line_transform(index, width);
		}

		mInstanceBuffer->update(instance_data.data());
		
		graphics->drawIndexedInstanced(6, line_count, 0, 0);

		//merge line series
		graphics->setPixelShader(mMergePixelShader);

		graphics->drawIndexedInstanced(6, line_count, 0, 0);
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
