#include "DensityGenerator.hpp"
#include "ImageGenerator.hpp"

#include <WindowsFactory.hpp>
#include <WindowsGraphics.hpp>
#include <WindowsFramework.hpp>

class DGFramework : public WindowsFramework {
public:
	DGFramework(const std::string& name, int width, int height)
		: WindowsFramework(name, width, height) {
		
		mDensityGenerator = 
			new DensityGenerator(mFactory,
				{
					LineSeries(
						{
							vec2(0,0),
							vec2(20, 20),
							vec2(30, 40),
							vec2(40, 80)
						})
				}, 100, 100);
	}

	~DGFramework() {
		delete mDensityGenerator;
	}

	void render(void* sender, float deltaTime)override {
		mDensityGenerator->run();

		mSwapChain->present();
	}
private:
	DensityGenerator* mDensityGenerator;
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
