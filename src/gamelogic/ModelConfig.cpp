#include "gamelogic/ModelConfig.hpp"

namespace s3 {

bool operator== (const ModelConfig& lhs, const ModelConfig& rhs) noexcept
{
	return lhs.gridWidth == rhs.gridWidth &&

	lhs.tilesPerSecond == rhs.tilesPerSecond &&
	lhs.speedIncreasePerObject == rhs.speedIncreasePerObject &&
	
	lhs.bonusFrequency == rhs.bonusFrequency &&
	lhs.bonusDuration == rhs.bonusDuration &&
	lhs.numberOfBonusObjects == rhs.numberOfBonusObjects &&
	lhs.earlyDuration == rhs.earlyDuration &&
	lhs.shiftBonusDuration == rhs.shiftBonusDuration &&

	lhs.objectValue == rhs.objectValue &&
	lhs.objectEarlyBonus == rhs.objectEarlyBonus &&
	lhs.objectShiftBonus == rhs.objectShiftBonus &&

	lhs.bonusObjectValue == rhs.bonusObjectValue &&
	lhs.bonusObjectShiftBonus == rhs.bonusObjectShiftBonus;
}

bool operator!= (const ModelConfig& lhs, const ModelConfig& rhs) noexcept
{
	return !(lhs == rhs);
}

} // namespace s3