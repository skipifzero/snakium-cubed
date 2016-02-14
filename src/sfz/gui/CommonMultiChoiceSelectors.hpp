#pragma once
#ifndef SFZ_GUI_COMMON_MULTI_CHOICE_SELECTORS_HPP
#define SFZ_GUI_COMMON_MULTI_CHOICE_SELECTORS_HPP

#include <cstdint>
#include <string>

#include "sfz/gui/MultiChoiceSelector.hpp"

namespace gui {

using std::int32_t;
using std::size_t;
using std::string;

// MultiChoiceSelector creator functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

/**
 * @brief Creates a MultiChoiceSelector for a float value with evenly placed intervals
 * @param text the name of the value
 * @param valuePtr the pointer to the value (for both reading and writing)
 * @param minValue the minimum value
 * @param intervalSize the interval between values
 * @param stateAlignOffset see MultiChoiceSelector constructor
 * @param unit the unit for the value
 * @param numberOfDecimals the number of float decimals to print
 */
MultiChoiceSelector* createLinearFloatMultiChoiceSelector(const string& text, float* valuePtr,
                          float minValue, float intervalSize, size_t numIntervals, 
                          float stateAlignOffset = 0.0f, const string& unit = "", 
                          size_t numberOfDecimals = 2) noexcept;

/**
 * @brief Creates a MultiChoiceSelector for a integer value with evenly placed intervals
 * @param text the name of the value
 * @param valuePtr the pointer to the value (for both reading and writing)
 * @param minValue the minimum value
 * @param intervalSize the interval between values
 * @param stateAlignOffset see MultiChoiceSelector constructor
 * @param unit the unit for the value
 */
MultiChoiceSelector* createLinearIntegerMultiChoiceSelector(const string& text, int32_t* valuePtr,
                          int32_t minValue, int32_t intervalSize, size_t numIntervals,
                          float stateAlignOffset = 0.0f, const string& unit = "") noexcept;

} // namespace gui
#endif