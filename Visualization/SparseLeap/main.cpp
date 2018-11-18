#include "RenderFramework.hpp"

int main() {
	RenderFramework renderFramework = RenderFramework("SparseLeap", 1920, 1080);

	renderFramework.build();
	renderFramework.showWindow();
	renderFramework.runLoop();
}