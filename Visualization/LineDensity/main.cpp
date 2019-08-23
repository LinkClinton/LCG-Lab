#include "DensityGenerator.hpp"
#include "ImageGenerator.hpp"
#include "SharpGenerator.hpp"
#include "ColorMapped.hpp"
#include "CommandList.hpp"

#include <WindowsFactory.hpp>
#include <WindowsGraphics.hpp>
#include <WindowsFramework.hpp>

#include <chrono>

using time_point = std::chrono::high_resolution_clock;

class DensityContext {
public:
	DensityContext() {
		mGraphics = std::make_shared<WindowsGraphics>();
		mFactory = std::make_shared<WindowsFactory>(mGraphics.get());

		mColorMapped = std::make_shared<ColorMapped>(
			std::vector<vec4>({
				vec4(0.0f,0.0f,0.0f,1.0f),
				vec4(1.0f,0.0f,0.0f,1.0f)
				}), 15.0f);
	}

	void run() {
		input_line_data();
		input_color_mapped();
		random_line_data();
		output_heat_map();
		output_line_image();
	}
private:
	void input_line_data() const {
		if (mInputLineName.empty() == true) return;

		std::cout << "start read data from file[" << mInputLineName << "]." << std::endl;

		std::cout << "end read data from file." << std::endl;
	}

	void input_color_mapped() const {
		if (mInputColorMappedName.empty() == true) return;

		std::cout << "start read color mapped from file[" << mInputColorMappedName << "]." << std::endl;

		std::cout << "end read color mapped from file." << std::endl;
	}

	void random_line_data() {
		if (mInputLineName.empty() == false) return;

		std::cout << "start build data randomly." << std::endl;
		std::cout << "build " << mRandomLineSeriesCount << " line series with " << mRandomLineCount << " lines." << std::endl;

		const auto start_time = time_point::now();

		std::vector<LineSeries> data;

		const auto width = static_cast<real>(mHeatMapWidth);
		const auto height = static_cast<real>(mHeatMapHeight);

		for (size_t index = 0; index < mRandomLineSeriesCount;index++) 
			data.push_back(LineSeries::random_make(mRandomLineCount + 1, width, height));

		mDensityGenerator = std::make_shared<DensityGenerator>(mFactory.get(), data, mHeatMapWidth, mHeatMapHeight);

		const auto end_time = time_point::now();

		std::cout << "end build data randomly, cost " <<
			std::chrono::duration_cast<std::chrono::duration<float>>(end_time - start_time).count() << "s." << std::endl;
	}

	void output_heat_map() {
		if (mOutputHeatMapName.empty() == true) return;

		std::cout << "start build heat map with line width " << mLineWidth << "." << std::endl;
		std::cout << "heat map width : " << mHeatMapWidth << ", heat map height : " << mHeatMapHeight << "." << std::endl;
		
		const auto start_time = time_point::now();

		mImageGenerator = std::make_shared<ImageGenerator>(mDensityGenerator, mColorMapped);		
		
		mDensityGenerator->run(mLineWidth);
		mImageGenerator->run();

		const auto end_time = time_point::now();

		std::cout << "end build heat map, cost " <<
			std::chrono::duration_cast<std::chrono::duration<float>>(end_time - start_time).count() << "s." << std::endl;
		std::cout << "output heat map to file[" << mOutputHeatMapName << "]." << std::endl;
		
		mImageGenerator->save(force_to_wstring(mOutputHeatMapName));
	}

	void output_line_image() {
		if (mOutputImageName.empty() == true) return;

		std::cout << "start build image with line width " << mLineWidth << "." << std::endl;
		std::cout << "image width : " << mImageWidth << ", image height : " << mImageHeight << "." << std::endl;

		const auto start_time = time_point::now();

		mSharpGenerator = std::make_shared<SharpGenerator>(mFactory.get(), mDensityGenerator, mImageWidth, mImageHeight);
		mSharpGenerator->run(mLineWidth);

		const auto end_time = time_point::now();

		std::cout << "end build image, cost " << 
			std::chrono::duration_cast<std::chrono::duration<float>>(end_time - start_time).count() << "s." << std::endl;
		std::cout << "output image to file[" << mOutputImageName << "]." << std::endl;

		mSharpGenerator->save(force_to_wstring(mOutputImageName));
	}

	static auto force_to_wstring(const std::string &str) -> std::wstring {
		std::wstring result;

		//only support english name
		for (auto& c : str) result.push_back(static_cast<wchar_t>(c));

		return result;
	}
public:
	std::string mInputLineName;
	std::string mInputColorMappedName;
	std::string mOutputHeatMapName;
	std::string mOutputImageName;

	float mLineWidth = 1.0f;

	size_t mHeatMapWidth = 128;
	size_t mHeatMapHeight = 72;

	size_t mImageWidth = 1280;
	size_t mImageHeight = 720;

	size_t mRandomLineCount = 20;
	size_t mRandomLineSeriesCount = 1000;
private:
	std::shared_ptr<WindowsGraphics> mGraphics;
	std::shared_ptr<WindowsFactory> mFactory;

	std::shared_ptr<ColorMapped> mColorMapped;
	std::shared_ptr<DensityGenerator> mDensityGenerator;
	std::shared_ptr<ImageGenerator> mImageGenerator;
	std::shared_ptr<SharpGenerator> mSharpGenerator;
};

/*
 * Density Command List
 * -il fileName : input the line file.
 * -wl width : set the line width.
 * -ic fileName : input the color mapped file.
 * -mw width : set the heat map width.
 * -mh height : set the heat map height.
 * -lw width : set the line image width.
 * -lh height : set the line image height.
 * -rl count : set the random line count.
 * -rs count : set the random line series count.
 * -om fileName : output the heat map.
 * -ol fileName : output the line image.
 */
int main(int argc, char** argv) {
	CommandList commandList;
	DensityContext context;

	commandList.setCommand("-il", [](void* ctx, const std::string& fileName)
		{
			if (fileName.size() == 0) {
				std::cout << "error: input Line file is invalid." << std::endl;
				return false;
			}

			static_cast<DensityContext*>(ctx)->mInputLineName = fileName;
			
			return true;
		});
	commandList.setCommand("-wl", [](void* ctx, const std::string& width)
		{
			static_cast<DensityContext*>(ctx)->mLineWidth = std::stof(width);

			return true;
		});
	commandList.setCommand("-ic", [](void* ctx, const std::string& fileName)
		{
			if (fileName.size() == 0) {
				std::cout << "error: input color mapped file is invalid." << std::endl;
				return false;
			}

			static_cast<DensityContext*>(ctx)->mInputColorMappedName = fileName;

			return true;
		});
	commandList.setCommand("-mw", [](void* ctx, const std::string& width)
	{
			static_cast<DensityContext*>(ctx)->mHeatMapWidth = std::stoull(width);
			return true;
	});
	commandList.setCommand("-mh", [](void* ctx, const std::string& height)
		{
			static_cast<DensityContext*>(ctx)->mHeatMapHeight = std::stoull(height);
			return true;
		});
	commandList.setCommand("-lw", [](void* ctx, const std::string& width)
		{
			static_cast<DensityContext*>(ctx)->mImageWidth = std::stoull(width);
			return true;
		});
	commandList.setCommand("-lh", [](void* ctx, const std::string& height)
		{
			static_cast<DensityContext*>(ctx)->mImageHeight = std::stoull(height);
			return true;
		});
	commandList.setCommand("-rl", [](void* ctx, const std::string& count)
		{
			static_cast<DensityContext*>(ctx)->mRandomLineCount = std::stoull(count);

			return true;
		});
	commandList.setCommand("-rs", [](void* ctx, const std::string& count)
		{
			static_cast<DensityContext*>(ctx)->mRandomLineSeriesCount = std::stoull(count);
			return true;
		});
	commandList.setCommand("-om", [](void* ctx, const std::string& fileName)
		{
			if (fileName.size() == 0) {
				std::cout << "error : output heat map file is invalid." << std::endl;
				return false;
			}

			static_cast<DensityContext*>(ctx)->mOutputHeatMapName = fileName;

			return true;
		});
	commandList.setCommand("-ol", [](void* ctx, const std::string& fileName)
		{
			if (fileName.size() == 0) {
				std::cout << "error : output image file is invalid." << std::endl;
				return false;
			}

			static_cast<DensityContext*>(ctx)->mOutputImageName = fileName;

			return true;
		});

	if (commandList.execute(&context, CommandList::read_from_argv(argc, argv)) == false) return -1;

	context.run();
	/*DGFramework dg = DGFramework("Density-Generator", 1280, 720);
	dg.showWindow();
	dg.runLoop();*/
}
