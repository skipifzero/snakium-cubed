#pragma once
#ifndef S3_GAMELOGIC_STATS_HPP
#define S3_GAMELOGIC_STATS_HPP

#include <cstdint>

namespace s3 {

using std::int64_t;

struct Stats final {

	int64_t score = 0;
};

} // namespace s3
#endif