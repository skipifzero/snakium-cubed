#include "gamelogic/Stats.hpp"

#include <cmath>

namespace s3 {

int32_t totalScore(const Stats& stats, const ModelConfig& cfg) noexcept
{
	return stats.objectsEaten * cfg.objectValue + 
	       stats.objectsEarly * cfg.objectEarlyBonus +
	       stats.objectsShift * cfg.objectShiftBonus +

	       stats.bonusObjectsEaten * cfg.bonusObjectValue +
	       stats.bonusObjectsShift * cfg.bonusObjectShiftBonus;
}

} // namespace s3