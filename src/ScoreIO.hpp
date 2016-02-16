#pragma once
#ifndef S3_SCORE_IO_HPP
#define S3_SCORE_IO_HPP

#include "gamelogic/Stats.hpp"

namespace s3 {

// Bundle struct
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

struct ScoreBundle final {
	Stats standardResults[5];
	bool standardExists[5];

	Stats largeResults[5];
	bool largeExists[5];
	
	Stats giantResults[5];
	bool giantExists[5];
};

// IO functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

bool writeScores(const ScoreBundle& scores) noexcept;

bool loadScores(ScoreBundle& scoresOut) noexcept;

bool deleteScores() noexcept;

} // namespace s3
#endif