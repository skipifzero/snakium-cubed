#pragma once
#ifndef SFZ_GL_ALIGNMENT_HPP
#define SFZ_GL_ALIGNMENT_HPP

#include <cstdint> // int8_t
#include <iostream> // std::ostream



namespace gl {

using std::int8_t;

enum class HorizontalAlign : int8_t {
	LEFT = -1, 
	CENTER = 0, 
	RIGHT = 1
};

enum class VerticalAlign : int8_t {
	BOTTOM = -1, 
	MIDDLE = 0, 
	TOP = 1
};

inline const char* to_string(HorizontalAlign align) noexcept
{
	switch (align) {
	case HorizontalAlign::LEFT: return "LEFT";
	case HorizontalAlign::CENTER: return "CENTER";
	case HorizontalAlign::RIGHT: return "RIGHT";
	}
}

inline const char* to_string(VerticalAlign align) noexcept
{
	switch (align) {
	case VerticalAlign::BOTTOM: return "BOTTOM";
	case VerticalAlign::MIDDLE: return "MIDDLE";
	case VerticalAlign::TOP: return "TOP";
	}
}

inline std::ostream& operator<< (std::ostream& ostream, HorizontalAlign align) noexcept
{
	return ostream << to_string(align);
}

inline std::ostream& operator<< (std::ostream& ostream, VerticalAlign align) noexcept
{
	return ostream << to_string(align);
}

/**
 * @brief Calculates the distance between two HorizontalAlign enums.
 * Calculates and returns the distance between two enum values as defined by the enums. The answer
 * will be signed and may have a positive or negative value depending on the 'direction'.
 */
inline int8_t distance(HorizontalAlign origin, HorizontalAlign destination) noexcept
{
	return static_cast<int8_t>(destination) - static_cast<int8_t>(origin);
}

/**
 * @brief Calculates the distance between two VerticalAlign enums.
 * Calculates and returns the distance between two enum values as defined by the enums. The answer
 * will be signed and may have a positive or negative value depending on the 'direction'.
 */
inline int8_t distance(VerticalAlign origin, VerticalAlign destination) noexcept
{
	return static_cast<int8_t>(destination) - static_cast<int8_t>(origin);
}

/**
 * @brief Calculates the new position when changing alignment.
 *
 * Beware that this function will probably cause problems in a coordinate system with inverted
 * y-axis (i.e. (0,0) in upper left corner).
 * 
 * @param oldPosition the old position
 * @param size the size of the object in the desired axis
 * @param oldAlignment the old alignment
 * @param newAlignment the new alignment
 * @return the new position
 */
template<typename T, typename Align>
T calculateNewPosition(T oldPosition, T size, Align oldAlignment, Align newAlignment) noexcept
{
	// Calculate the denominator. If the align distance is 2 we don't want to do anything to the
	// size, i.e. divide by 1. If the distance is 1 we want to adjust by half the size, so we 
	// divide by 2. 
	int8_t denominator;
	switch (distance(oldAlignment, newAlignment)) {
	case 0:
		// origin == destination, so do nothing.
		return oldPosition;
	case -2:
		denominator = -1;
		break;
	case -1:
		denominator = -2;
		break;
	case 1:
		denominator = 2;
		break;
	case 2:
		denominator = 1;
		break;
	}
	// Adjust the position with object size divided by the previously calculated denominator.
	return oldPosition + (size / static_cast<T>(denominator));
}
 
} // namespace sfz


#endif