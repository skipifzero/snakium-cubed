#include "ScoreIO.hpp"

#include <cstdint>
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