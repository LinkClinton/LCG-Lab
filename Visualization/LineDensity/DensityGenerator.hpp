#pragma once

#include "Utility.hpp"
#include "LineSeries.hpp"

class DensityGenerator {
public:
	DensityGenerator(
		Factory* factory,
		const std::vector<LineSeries>& line_series,
		size_t heatmap_width, size_t heatmap_height);

	~DensityGenerator();

	void run(real width = 2.0f);
private:
	Factory* mFactory;
	
	std::vector<LineSeries> mLineSeries;

	size_t mWidth;
	size_t mHeight;

	auto lineTransform(const vec2& start, const vec2& end, real width = 2.0f) const -> mat4;

	friend class ImageGenerator;
private:
	Texture2D* mTarget;
	Texture2D* mHeatMap;
	Texture2D* mBuffer;
	Texture2D* mCount;

	InputLayout* mInputLayout;

	RenderTarget* mRenderTarget;

	ConstantBuffer* mTransformBuffer;
	VertexBuffer* mVertexBuffer;
	IndexBuffer* mIndexBuffer;

	VertexShader* mCommonVertexShader;
	
	RasterizerState* mRasterizerState;
	
	PixelShader* mDrawPixelShader;
	PixelShader* mMergePixelShader;

	ResourceUsage* mHeatMapResourceUsage;

	UnorderedAccessUsage* mHeatMapRWUsage;
	UnorderedAccessUsage* mBufferRWUsage;
	UnorderedAccessUsage* mCountRWUsage;
};