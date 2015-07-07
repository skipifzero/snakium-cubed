#include "sfz/util/StopWatch.hpp"

#include <ratio> // std::milli, std::nano

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

float StopWatch::getTimeSeconds() noexcept
{
	if (!mHasTime) {
		mStopTime = std::chrono::high_resolution_clock::now();
	}
	using FloatSecondDuration = std::chrono::duration<float>;
	return std::chrono::duration_cast<FloatSecondDuration>(mStopTime - mStartTime).count();
}

float StopWatch::getTimeMilliSeconds() noexcept
{
	if (!mHasTime) {
		mStopTime = std::chrono::high_resolution_clock::now();
	}
	using FloatMilliSecondDuration = std::chrono::duration<float, std::milli>;
	return std::chrono::duration_cast<FloatMilliSecondDuration>(mStopTime - mStartTime).count();
}

float StopWatch::getTimeNanoSeconds() noexcept
{
	if (!mHasTime) {
		mStopTime = std::chrono::high_resolution_clock::now();
	}
	using FloatNanoSecondDuration = std::chrono::duration<float, std::nano>;
	return std::chrono::duration_cast<FloatNanoSecondDuration>(mStopTime - mStartTime).count();
}

} // namespace sfz