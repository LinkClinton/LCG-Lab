#pragma once

class FpsCounter {
private:
	float mNowTime;
	int mNowFps;
	int mFps;
public:
	FpsCounter();

	void update(float deltaTime);

	auto getFramePerSecond() -> int;
};
