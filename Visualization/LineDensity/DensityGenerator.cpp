#include "DensityGenerator.hpp"

DensityGenerator::DensityGenerator(
	Factory* factory, 
	const std::vector<LineSeries>& line_series, 
	size_t heatmap_width, size_t heatmap_height) :
	mFactory(factory), mLineSeries(line_series), 
	mWidth(heatmap_width), mHeight(heatmap_height){

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

	mMergeVertexShader = mFactory->createVertexShader(ShaderFile::read("MergeVertexShader.cso"), true);
	mLineVertexShader = mFactory->createVertexShader(ShaderFile::read("LineVertexShader.cso"), true);

	mMergePixelShader = mFactory->createPixelShader(ShaderFile::read("MergePixelShader.cso"), true);
	mLinePixelShader = mFactory->createPixelShader(ShaderFile::read("LinePixelShader.cso"), true);
	
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
	mFactory->destroyVertexbuffer(mVertexBuffer);
	mFactory->destroyIndexBuffer(mIndexBuffer);

	mFactory->destroyVertexShader(mLineVertexShader);
	mFactory->destroyVertexShader(mMergeVertexShader);

	mFactory->destroyPixelShader(mLinePixelShader);
	mFactory->destroyPixelShader(mMergePixelShader);

	mFactory->destroyUnorderedAccessUsage(mHeatMapRWUsage);
	mFactory->destroyUnorderedAccessUsage(mBufferRWUsage);
	mFactory->destroyUnorderedAccessUsage(mCountRWUsage);
}

void DensityGenerator::run(real width) {
	unsigned int uavClear[4] = { 0, 0, 0, 0 };

	mat4 matrix[] = {
		mat4(1.0f),
		glm::orthoLH_ZO(0.0f, 
		static_cast<float>(mTarget->getWidth()),
		static_cast<float>(mTarget->getHeight()), 0.0f,
		0.0f, 1.0f) 
	};

	auto graphics = mFactory->graphics();

	graphics->clearState();
	graphics->clearUnorderedAccessUsageUint(mHeatMapRWUsage, uavClear);
	
	graphics->setRenderTarget(mRenderTarget);
	
	graphics->setInputLayout(mInputLayout);
	graphics->setVertexBuffer(mVertexBuffer);
	graphics->setIndexBuffer(mIndexBuffer);

	graphics->setPrimitiveType(PrimitiveType::TriangleList);
	graphics->setViewPort(0, 0, 
		static_cast<float>(mWidth), 
		static_cast<float>(mHeight));
	
	graphics->setConstantBuffer(mTransformBuffer, 0);
	graphics->setUnorderedAccessUsage({
		mBufferRWUsage,
		mCountRWUsage,
		mHeatMapRWUsage,
		});

	//for each line series
	for (auto &lines : mLineSeries) {
		graphics->clearUnorderedAccessUsageUint(mBufferRWUsage, uavClear);

		graphics->setVertexShader(mLineVertexShader);
		graphics->setPixelShader(mLinePixelShader);

		for (size_t index = 0; index < lines.size(); index++) {
			const auto line = lines.line(index);

			matrix[0] = lineTransform(line.first, line.second, width);

			mTransformBuffer->update(matrix);

			graphics->drawIndexed(6, 0, 0);
		}

		graphics->setVertexShader(mMergeVertexShader);
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