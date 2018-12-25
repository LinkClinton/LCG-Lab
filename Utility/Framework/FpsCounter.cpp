#include "FpsCounter.hpp"

FpsCounter::FpsCounter()
{
	mNowTime = 0;
	mNowFps = 0;
	mFps = 0;
}

void FpsCounter::update(float deltaTime)
{
	mNowFps++;

	mNowTime = mNowTime + deltaTime;

	if (mNowTime > 1.0f) {
		mFps = mNowFps;

		mNowTime = mNowTime - 1.0f;
		mNowFps = 0;
	}
}

auto FpsCounter::getFramePerSecond() -> int
{
	return mFps;
}


