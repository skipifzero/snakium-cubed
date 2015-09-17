#pragma once
#ifndef S3_GAMELOGIC_CONFIG_HPP
#define S3_GAMELOGIC_CONFIG_HPP

#include <cstdint>

namespace s3 {

using std::int32_t;

/**
 * @brief Struct used to create an s3::Model.
 */
struct ModelConfig final {
	int32_t gridWidth = 6;
	float tilesPerSecond = 2.5f;
	float speedIncreasePerObject = 0.025f;
	int32_t objectValue = 8;
	bool hasBonus = true;
	int32_t bonusFrequency = 8;
	int32_t bonusDuration = 16;
	int32_t bonusObjectValue = 32;
};

} // namespace s3

#endif