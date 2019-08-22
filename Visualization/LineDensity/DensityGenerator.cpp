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

	mCommonVertexShader = mFactory->createVertexShader(ShaderFile::read("DensityGeneratorVertex.cso"), true);
	mInstanceVertexShader = mFactory->createVertexShader(ShaderFile::read("DensityGeneratorInstanceVertex.cso"), true);
	
	mMergePixelShader = mFactory->createPixelShader(ShaderFile::read("DensityGeneratorMergePixel.cso"), true);
	mDrawPixelShader = mFactory->createPixelShader(ShaderFile::read("DensityGeneratorDrawPixel.cso"), true);

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

	mFactory->destroyConstantBuffer(mTransformBuffer);
	mFactory->destroyVertexBuffer(mVertexBuffer);
	mFactory->destroyIndexBuffer(mIndexBuffer);

	mFactory->destroyVertexShader(mCommonVertexShader);
	mFactory->destroyVertexShader(mInstanceVertexShader);

	mFactory->destroyPixelShader(mDrawPixelShader);
	mFactory->destroyPixelShader(mMergePixelShader);

	mFactory->destroyUnorderedAccessUsage(mHeatMapRWUsage);
	mFactory->destroyUnorderedAccessUsage(mBufferRWUsage);
	mFactory->destroyUnorderedAccessUsage(mCountRWUsage);

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

	const auto graphics = mFactory->graphics();
	
	graphics->clearState();
	graphics->clearUnorderedAccessUsageFloat(mHeatMapRWUsage, uav_float_clear);
	
	graphics->setRenderTarget(mRenderTarget);
	
	graphics->setInputLayout(mInputLayout);
	graphics->setVertexBuffer(mVertexBuffer);
	graphics->setIndexBuffer(mIndexBuffer);

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

		graphics->setVertexShader(mInstanceVertexShader);
		graphics->setPixelShader(mDrawPixelShader);

		assert(lines.size() >= 2);

		const auto line_count = static_cast<int>(lines.size());

		for (size_t index = 0; index < lines.size(); index++) {
			const auto line = lines.line(index);

			instance_data[index] = lineTransform(line.first, line.second, width);
		}

		mInstanceBuffer->update(instance_data.data());
		mTransformBuffer->update(matrix);

		graphics->drawIndexedInstanced(6, line_count, 0, 0);

		//merge line series
		graphics->setVertexShader(mCommonVertexShader);
		graphics->setPixelShader(mMergePixelShader);

		matrix[0] = glm::scale(mat4(1), vec3(mTarget->getWidth(), mTarget->getHeight(), 1));

		mTransformBuffer->update(matrix);

		graphics->drawIndexed(6, 0, 0);
	}
}

auto DensityGenerator::lineTransform(const vec2& start, const vec2& end, real width) const -> mat4 {

	auto matrix = mat4(1);
	const auto vector = end - start;

	matrix = glm::translate(matrix, vec3(start.x, start.y, 0));
	matrix = glm::rotate(matrix, glm::atan(vector.y, vector.x), vec3(0, 0, 1));
	matrix = glm::translate(matrix, vec3(0, -width * 0.5f, 0));
	matrix = glm::scale(matrix, vec3(glm::length(vector), width, 1));
	
	return matrix;
}
