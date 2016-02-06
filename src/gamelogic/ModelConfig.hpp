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
	int32_t gridWidth = 3;

	float tilesPerSecond = 2.5f;
	float speedIncreasePerObject = 0.025f;
	
	int32_t bonusFrequency = 8;
	int32_t bonusDuration = 8;
	int32_t numberOfBonusObjects = 1;
	int32_t earlyDuration = 8;
	int32_t shiftBonusDuration = 3;

	int32_t objectValue = 8;
	int32_t objectEarlyBonus = 8;
	int32_t objectShiftBonus = 8;
	int32_t bonusObjectValue = 32;
	int32_t bonusObjectShiftBonus = 16;
};

bool operator== (const ModelConfig& lhs, const ModelConfig& rhs) noexcept;
bool operator!= (const ModelConfig& lhs, const ModelConfig& rhs) noexcept;

const ModelConfig STANDARD_CONFIG = []() {
	ModelConfig tmp;

	tmp.gridWidth = 3;

	tmp.tilesPerSecond = 2.5f;
	tmp.speedIncreasePerObject = 0.025f;
	
	tmp.bonusFrequency = 8;
	tmp.bonusDuration = 8;
	tmp.numberOfBonusObjects = 1;
	tmp.earlyDuration = 8;
	tmp.shiftBonusDuration = 3;

	return tmp;
}();

const ModelConfig LARGE_CONFIG = []() {
	ModelConfig tmp;

	tmp.gridWidth = 4;

	tmp.tilesPerSecond = 2.5f;
	tmp.speedIncreasePerObject = 0.025f;

	tmp.bonusFrequency = 8;
	tmp.bonusDuration = 12;
	tmp.numberOfBonusObjects = 2;
	tmp.earlyDuration = 8;
	tmp.shiftBonusDuration = 4;

	return tmp;
}();

const ModelConfig GIANT_CONFIG = []() {
	ModelConfig tmp;

	tmp.gridWidth = 5;

	tmp.tilesPerSecond = 2.5f;
	tmp.speedIncreasePerObject = 0.025f;

	tmp.bonusFrequency = 8;
	tmp.bonusDuration = 16;
	tmp.numberOfBonusObjects = 3;
	tmp.earlyDuration = 8;
	tmp.shiftBonusDuration = 5;

	return tmp;
}();

} // namespace s3
#endif