#pragma once

#include "DensityGenerator.hpp"
#include "ImageGenerator.hpp"
#include "SharpGenerator.hpp"
#include "ColorMapped.hpp"

#include <WindowsFactory.hpp>
#include <WindowsGraphics.hpp>

class DGFramework : public WindowsFramework {
public:
	DGFramework(const std::string& name, int width, int height) :
		WindowsFramework(name, width, height) {
		
		const auto image_width = 128;
		const auto image_height = 72;

		std::vector<LineSeries> data;

		for (size_t i = 0; i < 4; i++)
			data.push_back(LineSeries::random_make(20,
				static_cast<real>(image_width),
				static_cast<real>(image_height)));

		mDensityGenerator = std::make_shared<DensityGenerator>
			(mFactory, data, image_width, image_height);

		mColorMapped = std::make_shared<ColorMapped>(
			std::vector<vec4>({
				vec4(0.0f,1.0f,0.0f,1.0f),
				vec4(1.0f,0.0f,0.0f,1.0f),
				vec4(0.0f,0.0f,1.0f,1.0f)
				}), 0.5f);

		mImageGenerator = std::make_shared<ImageGenerator>(
			mDensityGenerator,
			mColorMapped,
			mSwapChain);
	}

	virtual void render(void* sender, float deltaTime) override {
		mDensityGenerator->run();
		mImageGenerator->run();
		mSwapChain->present(true);
	}

private:
	std::shared_ptr<DensityGenerator> mDensityGenerator;
	std::shared_ptr<ImageGenerator> mImageGenerator;
	std::shared_ptr<SharpGenerator> mSharpGenerator;
	std::shared_ptr<ColorMapped> mColorMapped;
};