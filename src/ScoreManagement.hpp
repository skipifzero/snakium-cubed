#pragma once
#ifndef S3_SCORE_MANAGEMENT_HPP
#define S3_SCORE_MANAGEMENT_HPP

#include <cstddef>

#include "gamelogic/Stats.hpp"

namespace s3 {

using std::size_t;

// Constants
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

constexpr size_t NUM_SCORES_SAVED = 3;
constexpr size_t SCORE_NAME_LENGTH = 20;

enum class ScoreConfigType {
	STANDARD,
	LARGE,
	GIANT
};

// Bundle struct
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

struct ScoreBundle final {
	Stats standardResults[NUM_SCORES_SAVED];
	char standardNames[NUM_SCORES_SAVED][SCORE_NAME_LENGTH + 1];
	size_t numStandardResults = 0;
	
	Stats largeResults[NUM_SCORES_SAVED];
	char largeNames[NUM_SCORES_SAVED][SCORE_NAME_LENGTH + 1];
	size_t numLargeResults = 0;

	Stats giantResults[NUM_SCORES_SAVED];
	char giantNames[NUM_SCORES_SAVED][SCORE_NAME_LENGTH + 1];
	size_t numGiantResults = 0;
};

// General functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

ScoreBundle createEmptyScoreBundle() noexcept;

/**
 * @brief Attempts to add new high score to the current ones
 * @param currentHighScores the current scores bundle
 * @param configType the config the new score belongs to
 * @param newScore the new score
 * @param scoreName the name of the new score (must be at least SCORE_NAME_LENGTH large buffer)
 * @return the ranking of the new score, -1 if not ranked
 */
int tryAddScoreToBundle(ScoreBundle& currentHighScores, ScoreConfigType configType,
                        Stats newScore, const char* scoreName) noexcept;

int checkScoreRank(const ScoreBundle& currentHighScores, ScoreConfigType configType, Stats newScore) noexcept;

// IO functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

bool writeScores(const ScoreBundle& scores) noexcept;

bool loadScores(ScoreBundle& scoresOut) noexcept;

bool deleteScores() noexcept;

} // namespace s3
#endif