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
	Config() noexcept = default;
	Config(const Config&) noexcept = default;

	int16_t cubeSizeX, cubeSizeY, cubeSizeZ;
	float tilesPerSecond;
	bool hasSpeedIncrease;
	float speedIncreasePerObject;
	int16_t pointsPerObject;
	bool hasBonus;
	int16_t bonusFrequency, bonusDuration, pointsPerBonusObject;
};

} // namespace s3

#endif