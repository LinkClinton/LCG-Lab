#pragma once

#include <chrono>

typedef std::chrono::high_resolution_clock Time;

class Timer {
private:
	Time::time_point mLastTime;
public:
	Timer();

	void start();

	float getTime();
};