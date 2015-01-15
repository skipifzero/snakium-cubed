#pragma once
#ifndef S3_GAMELOGIC_CONFIG_HPP
#define S3_GAMELOGIC_CONFIG_HPP

#include <cstdint> // int16_t

namespace s3 {

using std::uint16_t;

/**
 * @brief Struct used to create an s3::Model.
 */
struct ModelConfig final {
	uint16_t gridWidth = 6;
	float tilesPerSecond = 2.5f;
	bool hasSpeedIncrease = true;
	float speedIncreasePerObject = 0.025f;
	uint16_t pointsPerObject = 8;
	bool hasBonus = true;
	uint16_t bonusFrequency = 8;
	uint16_t bonusDuration = 16;
	uint16_t pointsPerBonusObject = 32;
};

} // namespace s3

#endif