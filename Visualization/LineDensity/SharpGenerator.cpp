#include "SharpGenerator.hpp"
#include "DensityGenerator.hpp"
#include "LineSeries.hpp"

#include <algorithm>
#include <random>

#undef max

SharpGenerator::SharpGenerator(
	Factory* factory, 
	const std::shared_ptr<DensityGenerator> &densityGenerator, 
	size_t w, size_t h) : 
	mFactory(factory), mDensityGenerator(densityGenerator),
	mWidth(w), mHeight(h) {

	const auto width = static_cast<int>(mWidth);
	const auto height = static_cast<int>(mHeight);

	mTarget = mFactory->createTexture2D(width, height, PixelFormat::R8G8B8A8Unknown,
		ResourceInfo(BindUsage::RenderTargetUsage));
	
	mRenderTarget = mFactory->createRenderTarget(mTarget, mTarget->getPixelFormat());

	mInputLayout = mFactory->createInputLayout();
	mInputLayout->addElement(InputLayoutElement("POSITION", 12, 0));

	mTransformBuffer = mFactory->createConstantBuffer(sizeof(mat4) * 2, ResourceInfo::ConstantBuffer());
	mVertexBuffer = mFactory->createVertexBuffer(sizeof(vec3) * 4, sizeof(vec3), ResourceInfo::VertexBuffer());
	mIndexBuffer = mFactory->createIndexBuffer(sizeof(unsigned) * 6, ResourceInfo::IndexBuffer());

	mVertexShader = mFactory->createVertexShader(ShaderFile::read("./Shader/SharpGeneratorVertex.cso"), true);
	mPixelShader = mFactory->createPixelShader(ShaderFile::read("./Shader/SharpGeneratorPixel.cso"), true);

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

SharpGenerator::SharpGenerator(
	Factory* factory, 
	const std::shared_ptr<DensityGenerator>& densityGenerator, 
	SwapChain* swapChain) :
	mFactory(factory), mDensityGenerator(densityGenerator),
	mWidth(swapChain->getWidth()), mHeight(swapChain->getHeight())
{
	const auto width = static_cast<int>(mWidth);
	const auto height = static_cast<int>(mHeight);

	mTarget = nullptr;

	mRenderTarget = mFactory->createRenderTarget(swapChain);

	mInputLayout = mFactory->createInputLayout();
	mInputLayout->addElement(InputLayoutElement("POSITION", 12, 0));

	mTransformBuffer = mFactory->createConstantBuffer(sizeof(mat4) * 2, ResourceInfo::ConstantBuffer());
	mVertexBuffer = mFactory->createVertexBuffer(sizeof(vec3) * 4, sizeof(vec3), ResourceInfo::VertexBuffer());
	mIndexBuffer = mFactory->createIndexBuffer(sizeof(unsigned) * 6, ResourceInfo::IndexBuffer());

	mVertexShader = mFactory->createVertexShader(ShaderFile::read("SharpGeneratorVertex.cso"), true);
	mPixelShader = mFactory->createPixelShader(ShaderFile::read("SharpGeneratorPixel.cso"), true);

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

SharpGenerator::~SharpGenerator() {
	mFactory->destroyTexture2D(mTarget);

	mFactory->destroyInputLayout(mInputLayout);

	mFactory->destroyRenderTarget(mRenderTarget);

	mFactory->destroyStructuredBuffer(mInstanceBuffer);
	mFactory->destroyConstantBuffer(mTransformBuffer);
	mFactory->destroyVertexBuffer(mVertexBuffer);
	mFactory->destroyIndexBuffer(mIndexBuffer);

	mFactory->destroyVertexShader(mVertexShader);
	mFactory->destroyPixelShader(mPixelShader);

	mFactory->destroyResourceUsage(mInstanceUsage);

	mFactory->destroyRasterizerState(mRasterizerState);
}

void SharpGenerator::run(real width) {
	float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

	mat4 matrix[] = {
		mat4(1.0f),
		glm::orthoLH_ZO(0.0f,
		static_cast<float>(mWidth),
		static_cast<float>(mHeight), 0.0f,
		0.0f, 1.0f)
	};

	mTransformBuffer->update(matrix);

	const auto graphics = mFactory->graphics();

	graphics->clearState();
	graphics->clearRenderTarget(mRenderTarget, color);

	graphics->setRenderTarget(mRenderTarget);

	graphics->setInputLayout(mInputLayout);
	graphics->setVertexBuffer(mVertexBuffer);
	graphics->setIndexBuffer(mIndexBuffer);

	graphics->setVertexShader(mVertexShader);
	graphics->setPixelShader(mPixelShader);

	graphics->setPrimitiveType(PrimitiveType::TriangleList);
	graphics->setViewPort(0, 0,
		static_cast<float>(mWidth),
		static_cast<float>(mHeight));

	graphics->setRasterizerState(mRasterizerState);

	graphics->setConstantBuffer(mTransformBuffer, 0);
	

	// get the max size of line series
	auto max_size = 0;

	for (auto& lines : mDensityGenerator->mLineSeries)
		max_size = std::max(max_size, static_cast<int>(lines.size()));

	struct sharp_instance_buffer {
		mat4 world;
		vec4 color;
	};

	std::vector<sharp_instance_buffer> instance_data(max_size);

	//when the size of buffer is less than lines
	//we need to expand the buffer
	if (mInstanceBuffer == nullptr ||
		mInstanceBuffer->element_count() < max_size) {

		//destroy old buffer
		mFactory->destroyStructuredBuffer(mInstanceBuffer);
		mFactory->destroyResourceUsage(mInstanceUsage);

		//create new buffer
		mInstanceBuffer = mFactory->createStructuredBuffer(sizeof(sharp_instance_buffer),
			max_size, ResourceInfo::ShaderResource());
		mInstanceUsage = mFactory->createResourceUsage(mInstanceBuffer);
	}

	graphics->setResourceUsage(mInstanceUsage, 0);

	const auto scale_width = mWidth / static_cast<float>(mDensityGenerator->mWidth);
	const auto scale_height = mHeight / static_cast<float>(mDensityGenerator->mHeight);
	const auto scale_vector = vec2(scale_width, scale_height);

	//for each line series
	for (auto& lines : mDensityGenerator->mLineSeries) {
		assert(lines.size() >= 2);

		const auto line_count = static_cast<int>(lines.size());

		for (size_t index = 0; index < lines.size(); index++) {
			instance_data[index].color = lines.color();
			instance_data[index].world = lines.line_transform_with_scale(index, scale_vector, width);
		}

		mInstanceBuffer->update(instance_data.data());

		graphics->drawIndexedInstanced(6, line_count, 0, 0);
	}
}

void SharpGenerator::save(const std::wstring& fileName) {
	mTarget->save(fileName);
}
