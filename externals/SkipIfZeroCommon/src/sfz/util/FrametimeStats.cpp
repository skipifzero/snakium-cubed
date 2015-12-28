#include "sfz/util/FrametimeStats.hpp"

#include <algorithm>
#include <cstdio>
#include <new>

#include <sfz/Assert.hpp>

namespace sfz {

// FrametimeStats: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

FrametimeStats::FrametimeStats() noexcept
:
	mSamples{},
	
	mMaxNumSamples{0},
	mCurrentNumSamples{0},

	mMin{-1.0f}, 
	mMax{-1.0f}, 
	mAvg{-1.0f}, 
	mSD{-1.0f},

	mString{}
{ }

FrametimeStats::FrametimeStats(size_t maxNumSamples) noexcept
:
	mSamples{new (std::nothrow) float[maxNumSamples]},
	
	mMaxNumSamples{maxNumSamples},
	mCurrentNumSamples{0},

	mMin{-1.0f}, 
	mMax{-1.0f}, 
	mAvg{-1.0f}, 
	mSD{-1.0f},

	mString{new (std::nothrow) char[128]}
{ }

// FrametimeStats: Public methods
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void FrametimeStats::addSample(float sampleInSeconds) noexcept
{
	sfz_assert_debug(mMaxNumSamples > 0);
	sfz_assert_debug(mCurrentNumSamples <= mMaxNumSamples);

	if (mMaxNumSamples == mCurrentNumSamples) {
		for (size_t i = 0; i < (mMaxNumSamples-1); ++i) {
			mSamples[i] = mSamples[i + 1];
		}
		mSamples[mMaxNumSamples - 1] = sampleInSeconds;
	}
	else {
		mSamples[mCurrentNumSamples] = sampleInSeconds;
		mCurrentNumSamples++;
	}

	float sum = 0.0f;
	mMin = 1000000000.0f;
	mMax = -1000000000.0f;
	for (size_t i = 0; i < mCurrentNumSamples; ++i) {
		sum += mSamples[i];
		mMin = std::min(mMin, mSamples[i]);
		mMax = std::max(mMax, mSamples[i]);
	}
	mAvg = sum / float(mCurrentNumSamples);

	float varianceSum = 0.0f;
	for (size_t i = 0; i < mCurrentNumSamples; ++i) {
		varianceSum += std::pow((mSamples[i] - mAvg), 2);
	}
	mSD = std::sqrt(varianceSum / float(mCurrentNumSamples));

	std::snprintf(&mString[0], 128, "Avg: %.1fms, SD: %.1fms, Min: %.1fms, Max: %.1fms",
	              mAvg * 1000.0f, mSD * 1000.0f, mMin * 1000.0f, mMax * 1000.0f);
}

void FrametimeStats::reset() noexcept
{
	mCurrentNumSamples = 0;
}

} // namespace sfz