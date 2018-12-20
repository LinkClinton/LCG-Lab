#include "CPUMemoryTestUnit.hpp"


int main() {
	//CPUMemoryTestUnit::writeTestVolumeFile("volume", Size(1000, 1000, 1000));

	CPUMemoryTestUnit testUnit("volume", Size(1000, 1000, 1000));

	testUnit.initialize();
	testUnit.run(10000);

	system("pause");
}