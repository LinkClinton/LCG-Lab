#include "DensityGenerator.hpp"
#include "ImageGenerator.hpp"
#include "ColorMapped.hpp"

#include <WindowsFactory.hpp>
#include <WindowsGraphics.hpp>
#include <WindowsFramework.hpp>

class DGFramework : public WindowsFramework {
public:
	DGFramework(const std::string& name, int width, int height)
		: WindowsFramework(name, width, height) {
		
		const auto image_width = 128;
		const auto image_height = 72;

		std::vector<LineSeries> data;

		for (size_t i = 0; i < 1000; i++)
			data.push_back(LineSeries::random_make(10, 
				static_cast<real>(image_width), 
				static_cast<real>(image_height)));

		mDensityGenerator = std::make_shared<DensityGenerator>
			(mFactory, data, image_width, image_height);

		mColorMapped = std::make_shared<ColorMapped>(
			std::vector<vec4>({
				vec4(0.0f,1.0f,0.0f,1.0f),
				vec4(1.0f,0.0f,0.0f,1.0f)
				}), 15.f);
		
		mImageGenerator = std::make_shared<ImageGenerator>(
			mDensityGenerator,
			mColorMapped,
			mSwapChain);

		
	}

	~DGFramework() {

	}

	void render(void* sender, float deltaTime)override {

		mDensityGenerator->run(1.0f);
		mImageGenerator->run();
		mSwapChain->present(true);
	}
private:
	std::shared_ptr<DensityGenerator> mDensityGenerator;
	std::shared_ptr<ImageGenerator> mImageGenerator;
	std::shared_ptr<ColorMapped> mColorMapped;
};

int main() {
	DGFramework dg = DGFramework("Density-Generator", 1280, 720);
	dg.showWindow();
	dg.runLoop();

	/*const auto graphics = std::make_shared<WindowsGraphics>();
	const auto factory = std::make_shared<WindowsFactory>(graphics.get());

	auto dg = DensityGenerator(factory.get(),
		{
			LineSeries(
				{
					vec2(0,0),
					vec2(20, 20),
					vec2(30, 40),
					vec2(40, 80)
				})
		}, 100, 100);

	dg.run();

	ImageGenerator::save(L"Test.png", dg);*/
}
