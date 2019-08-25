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

	void run();

	auto data() const -> const std::vector<LineSeries>&;

	auto width()const -> size_t;

	auto height()const -> size_t;
private:
	Factory* mFactory;
	
	std::vector<LineSeries> mLineSeries;

	size_t mWidth;
	size_t mHeight;

	friend class ImageGenerator;
	friend class SharpGenerator;
private:
	Texture2D* mTarget;
	Texture2D* mHeatMap;
	Texture2D* mBuffer;
	Texture2D* mCount;

	InputLayout* mInputLayout;

	RenderTarget* mRenderTarget;

	ConstantBuffer* mTransformBuffer;
	VertexBuffer* mVertexBuffer;

	VertexShader* mCommonVertexShader;
	
	RasterizerState* mRasterizerState;
	
	PixelShader* mDrawPixelShader;
	PixelShader* mMergePixelShader;

	UnorderedAccessUsage* mHeatMapRWUsage;
	UnorderedAccessUsage* mBufferRWUsage;
	UnorderedAccessUsage* mCountRWUsage;
};