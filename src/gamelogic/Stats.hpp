#pragma once
#ifndef S3_GAMELOGIC_STATS_HPP
#define S3_GAMELOGIC_STATS_HPP

#include <cstdint>

namespace s3 {

using std::int64_t;

struct Stats final {
	int64_t score = 0;
	int64_t objectsEaten = 0;
	int64_t bonusObjectsEaten = 0;
	int64_t missedBonusObjects = 0;
	int64_t numberOfDives = 0;
};

} // namespace s3
#endif