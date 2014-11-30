#include "sfz/util/StopWatch.hpp"

namespace sfz {

StopWatch::StopWatch() noexcept
{
	start();
}

void StopWatch::start() noexcept
{
	mHasTime = false;
	mStartTime = std::chrono::high_resolution_clock::now();
}

void StopWatch::stop() noexcept
{
	mHasTime = true;
	mStopTime = std::chrono::high_resolution_clock::now();
}

std::chrono::seconds StopWatch::getTimeSeconds() noexcept
{
	if (!mHasTime) {
		mStopTime = std::chrono::high_resolution_clock::now();
	}
	return std::chrono::duration_cast<std::chrono::seconds>(mStopTime - mStartTime);
}

std::chrono::milliseconds StopWatch::getTimeMilliSeconds() noexcept
{
	if (!mHasTime) {
		mStopTime = std::chrono::high_resolution_clock::now();
	}
	return std::chrono::duration_cast<std::chrono::milliseconds>(mStopTime - mStartTime);
}

std::chrono::nanoseconds StopWatch::getTimeNanoSeconds() noexcept
{
	if (!mHasTime) {
		mStopTime = std::chrono::high_resolution_clock::now();
	}
	return std::chrono::duration_cast<std::chrono::nanoseconds>(mStopTime - mStartTime);
}

} // namespace sfz