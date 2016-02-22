#include "ScoreManagement.hpp"

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include <sfz/util/IO.hpp>

namespace s3 {

using std::string;
using std::uint8_t;

// statics
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

static const char* scoresPath() noexcept
{
	static const string PATH = sfz::gameBaseFolderPath() + "/snakium-cubed/highscores.bin";
	return PATH.c_str();
}

// General functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

ScoreBundle createEmptyScoreBundle() noexcept
{
	ScoreBundle tmp;
	for (size_t i = 0; i < NUM_SCORES_SAVED; ++i) {
		for (size_t n = 0; n < SCORE_NAME_LENGTH + 1; ++n) {
			tmp.standardNames[i][n] = '\0';
			tmp.largeNames[i][n] = '\0';
			tmp.giantNames[i][n] = '\0';
		}
	}
	return tmp;
}

int tryAddScoreToBundle(ScoreBundle& currentHighScores, ScoreConfigType configType,
                        Stats newScore, const char* scoreName) noexcept
{
	int32_t score = 0;
	Stats* scores = nullptr;
	char (*names)[SCORE_NAME_LENGTH + 1] = nullptr;
	size_t* numScores = nullptr;
	const ModelConfig* mc = nullptr;
	switch (configType) {
	case ScoreConfigType::STANDARD:
		score = totalScore(newScore, STANDARD_CONFIG);
		scores = currentHighScores.standardResults;
		names = currentHighScores.standardNames;
		numScores = &currentHighScores.numStandardResults;
		mc = &STANDARD_CONFIG;
		break;
	case ScoreConfigType::LARGE:
		score = totalScore(newScore, LARGE_CONFIG);
		scores = currentHighScores.largeResults;
		names = currentHighScores.largeNames;
		numScores = &currentHighScores.numLargeResults;
		mc = &LARGE_CONFIG;
		break;
	case ScoreConfigType::GIANT:
		score = totalScore(newScore, GIANT_CONFIG);
		scores = currentHighScores.giantResults;
		names = currentHighScores.giantNames;
		numScores = &currentHighScores.numGiantResults;
		mc = &GIANT_CONFIG;
		break;
	}

	// Insert score
	for (size_t i = 0; i < *numScores; ++i) {
		int32_t ithScore = totalScore(scores[i], *mc);
		if (ithScore < score) {
			for (size_t j = NUM_SCORES_SAVED - 1; i >= j; --j) {
				scores[j+1] = scores[j];
				std::memcpy(names[j+1], names[j], SCORE_NAME_LENGTH);
			}
			scores[i] = newScore;
			std::memcpy(names[i], scoreName, SCORE_NAME_LENGTH);
			*numScores = std::min(*numScores + 1, NUM_SCORES_SAVED);
			return (int)i+1;
		}
	}

	// Just add score if list is not filled
	if (*numScores < NUM_SCORES_SAVED) {
		scores[*numScores] = newScore;
		std::memcpy(names[*numScores], scoreName, SCORE_NAME_LENGTH);
		*numScores += 1;
		return (int)*numScores;
	}

	return -1;
}

// IO functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

bool writeScores(const ScoreBundle& scores) noexcept
{
	return sfz::writeBinaryFile(scoresPath(), (const uint8_t*)&scores, sizeof(ScoreBundle));
}

bool loadScores(ScoreBundle& scoresOut) noexcept
{
	ScoreBundle tmp;
	int32_t res = sfz::readBinaryFile(scoresPath(), (uint8_t*)&tmp, sizeof(ScoreBundle));
	if (res == -2) {
		std::cerr << "Corrupt score file." << std::endl;
		return false;
	}
	if (res == -1) {
		return false;
	}
	scoresOut = tmp;
	return true;
}

bool deleteScores() noexcept
{ 
	return sfz::deleteFile(scoresPath());
}

} // namespace s3