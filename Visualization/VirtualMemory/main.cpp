#include "RenderFramework.hpp"

RenderFramework renderFramework = RenderFramework("VirtualMemory", 1920, 1080);

int main() {
	renderFramework.showWindow();
	renderFramework.runLoop();
}