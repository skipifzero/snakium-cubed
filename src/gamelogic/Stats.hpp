#pragma once
#ifndef S3_GAMELOGIC_STATS_HPP
#define S3_GAMELOGIC_STATS_HPP

#include <cstdint>

#include "gamelogic/ModelConfig.hpp"

namespace s3 {

using std::int64_t;

struct Stats final {
	int64_t objectsEaten = 0;
	int64_t objectsEarly = 0;
	int64_t objectsShift = 0;

	int64_t bonusObjectsEaten = 0;
	int64_t bonusObjectsShift = 0;
	int64_t bonusObjectsMissed = 0;

	int64_t tilesTraversed = 0;
	int64_t numberOfShifts = 0;
};

int32_t totalScore(const Stats& stats, const ModelConfig& cfg) noexcept;

} // namespace s3
#endif