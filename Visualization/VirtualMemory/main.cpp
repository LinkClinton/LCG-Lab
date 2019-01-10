#include "VMRenderFramework.hpp"
#include "CPUMemoryTestUnit.hpp"

//VMRenderFramework renderFramework = VMRenderFramework("VirtualMemory", 1920, 1080);

CPUMemoryTestUnit testUnit("volume", 1000);

int main() {
	testUnit.initialize();
	testUnit.run(100);
	system("pause");
	//renderFramework.initialize();
	//renderFramework.showWindow();
	//renderFramework.runLoop();
}