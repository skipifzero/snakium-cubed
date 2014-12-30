#pragma once
#ifndef S3_GAMELOGIC_CONFIG_HPP
#define S3_GAMELOGIC_CONFIG_HPP

#include <cstdint> // int16_t

namespace s3 {

using std::int16_t;

/**
 * @brief Struct used to create an s3::Model.
 */
struct Config final {
	int16_t size = 6;
	float tilesPerSecond = 2.5f;
	bool hasSpeedIncrease = true;
	float speedIncreasePerObject = 0.025f;
	int16_t pointsPerObject = 8;
	bool hasBonus = true;
	int16_t bonusFrequency = 8;
	int16_t bonusDuration = 16;
	int16_t pointsPerBonusObject = 32;
};

} // namespace s3

#endif