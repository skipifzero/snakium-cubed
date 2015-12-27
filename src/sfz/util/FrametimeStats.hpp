#pragma once
#ifndef SFZ_UTIL_FRAMETIMES_STATS_HPP
#define SFZ_UTIL_FRAMETIMES_STATS_HPP

#include <cstdint>
#include <memory>

namespace sfz {

using std::size_t;
using std::unique_ptr;

/**
 * @brief Class used to calculate useful frametime statistics
 * All frametimes entered and received are in seconds, except for the string representation which
 * will be in milliseconds.
 */
class FrametimeStats final {
public:
	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	FrametimeStats(const FrametimeStats&) = delete;
	FrametimeStats& operator= (const FrametimeStats&) = delete;
	FrametimeStats(FrametimeStats&&) noexcept = default;
	FrametimeStats& operator= (FrametimeStats&&) noexcept = default;

	FrametimeStats() noexcept;
	FrametimeStats(size_t maxNumSamples) noexcept;
	
	// Public methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	void addSample(float sampleInSeconds) noexcept;
	void reset() noexcept;
	
	inline size_t maxNumSamples() const noexcept { return mMaxNumSamples; }
	inline size_t currentNumSamples() const noexcept { return mCurrentNumSamples; }
	inline float min() const noexcept { return mMin; }
	inline float max() const noexcept { return mMax; }
	inline float avg() const noexcept { return mAvg; }
	inline float sd() const noexcept { return mSD; }
	inline const char* to_string() const noexcept { return &mString[0]; }

private:
	// Private members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	unique_ptr<float[]> mSamples;
	size_t mMaxNumSamples, mCurrentNumSamples;
	float mMin, mMax, mAvg, mSD;
	unique_ptr<char[]> mString;
};

} // namespace sfz
#endif