#include "CommonMultiChoiceSelectors.hpp"

#include <cstdio>
#include <new>
#include <vector>

#include "sfz/math/MathHelpers.hpp"

namespace gui {

using std::vector;

MultiChoiceSelector* createLinearFloatMultiChoiceSelector(const string& text, float* valuePtr, float minValue, float intervalSize, size_t numIntervals, float stateAlignOffset, const string& unit, size_t numberOfDecimals) noexcept
{
	// Create the choice strings
	char format[16];
	std::snprintf(format, sizeof(format), "%s%i%s%s", "%.", int(numberOfDecimals), "f", unit.c_str());
	char buffer[256];
	vector<string> choices;
	for (size_t i = 0; i < numIntervals; ++i) {
		std::snprintf(buffer, sizeof(buffer), format, minValue + float(i) * intervalSize);
		choices.emplace_back(buffer);
	}
	
	return new (std::nothrow) MultiChoiceSelector{text, choices, [valuePtr, minValue, intervalSize, numIntervals]() {
		float val = *valuePtr;
		for (size_t i = 0; i < numIntervals; ++i) {
			if (sfz::approxEqual(val, minValue + float(i) * intervalSize)) {
				return int(i);
			}
		}
		return -1;
	}, [valuePtr, minValue, intervalSize](int choice) {
		*valuePtr = minValue + float(choice) * intervalSize;
	}, stateAlignOffset};
}

MultiChoiceSelector* createLinearIntegerMultiChoiceSelector(const string& text, int32_t* valuePtr, int32_t minValue, int32_t intervalSize, size_t numIntervals, float stateAlignOffset, const string& unit) noexcept
{
	// Create the choice strings
	char buffer[256];
	vector<string> choices;
	for (size_t i = 0; i < numIntervals; ++i) {
		std::snprintf(buffer, sizeof(buffer), "%i%s", minValue + int32_t(i) * intervalSize, unit.c_str());
		choices.emplace_back(buffer);
	}
	
	return new (std::nothrow) MultiChoiceSelector{text, choices, [valuePtr, minValue, intervalSize, numIntervals]() {
		int32_t val = *valuePtr;
		for (size_t i = 0; i < numIntervals; ++i) {
			if (val == (minValue + int32_t(i) * intervalSize)) {
				return int(i);
			}
		}
		return -1;
	}, [valuePtr, minValue, intervalSize](int choice) {
		*valuePtr =  minValue + int32_t(choice) * intervalSize;
	}, stateAlignOffset};
}

} // namespace gui