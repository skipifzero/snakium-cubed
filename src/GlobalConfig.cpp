#include "GlobalConfig.hpp"

#include <iostream>
#include <string>
#include <exception> // std::terminate()

#include <sfz/SDL.hpp>
#include <sfz/util/IO.hpp>

#include "IniParser.hpp"

namespace s3 {

using std::string;

// Static functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

static const string& basePath() noexcept
{
	static const string BASE_PATH = sfz::gameBaseFolderPath() + "/snakium-cubed";
	return BASE_PATH;
}

static const string& userIniPath() noexcept
{
	static const string USER_INI_PATH = basePath() + "/config.ini";
	return USER_INI_PATH;
}

static void sanitizeIniParser(sfz::IniParser& ip) noexcept
{
	// [Graphics]
	ip.sanitizeBool("Graphics", "bFullscreen", false);
	ip.sanitizeInt("Graphics", "iWindowResolutionX", 800, 64, 8192);
	ip.sanitizeInt("Graphics", "iWindowResolutionY", 800, 64, 8192);
	ip.sanitizeBool("Graphics", "bVSync", true);
	ip.sanitizeInt("Graphics", "iMSAA", 4, 0, 32);
	ip.sanitizeBool("Graphics", "bTransparentCube", true);

	// [GameSettings]
	ip.sanitizeInt("GameSettings", "iGridWidth", 3, 2, 128);;
	ip.sanitizeFloat("GameSettings", "fTilesPerSecond", 2.25f, 0.05f, 60.0f);
	ip.sanitizeBool("GameSettings", "bHasSpeedIncrease", true);
	ip.sanitizeFloat("GameSettings", "fSpeedIncreasePerObject", 0.025f, 0.001f, 60.0f);
	ip.sanitizeInt("GameSettings", "iPointsPerObject", 8, 0, 4096);
	ip.sanitizeBool("GameSettings", "bHasBonus", true);
	ip.sanitizeInt("GameSettings", "iBonusFrequency", 8, 1, 1024);
	ip.sanitizeInt("GameSettings", "iBonusDuration", 32, 0, 4096);
	ip.sanitizeInt("GameSettings", "iPointsPerBonusObject", 32, 32, 4096);
}

// GlobalConfig: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

GlobalConfig::GlobalConfig() noexcept
:
	mIniParser{userIniPath()}
{ }

// GlobalConfig: Loading and saving to file
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void GlobalConfig::load() noexcept
{
	if (!sfz::directoryExists(sfz::gameBaseFolderPath().c_str())) {
		sfz::createDirectory(sfz::gameBaseFolderPath().c_str());
	}
	if (!sfz::directoryExists(basePath().c_str())) {
		sfz::createDirectory(basePath().c_str());
	}
	if (!sfz::fileExists(userIniPath().c_str())) {
		sfz::createFile(userIniPath().c_str());
	}

	if (!mIniParser.load()) {
		std::cerr << "Couldn't load config.ini at: " << userIniPath() << std::endl;
		std::terminate();
	}

	sanitizeIniParser(mIniParser);

	// [Graphics]
	fullscreen = mIniParser.getBool("Graphics", "bFullscreen");
	windowResolutionX = mIniParser.getInt("Graphics", "iWindowResolutionX");
	windowResolutionY = mIniParser.getInt("Graphics", "iWindowResolutionY");
	vsync = mIniParser.getBool("Graphics", "bVSync");
	msaa = mIniParser.getInt("Graphics", "iMSAA");
	transparentCube = mIniParser.getBool("Graphics", "bTransparentCube");

	// [GameSettings]
	modelConfig.gridWidth = mIniParser.getInt("GameSettings", "iGridWidth");
	modelConfig.tilesPerSecond = mIniParser.getFloat("GameSettings", "fTilesPerSecond");
	modelConfig.hasSpeedIncrease = mIniParser.getBool("GameSettings", "bHasSpeedIncrease");
	modelConfig.speedIncreasePerObject = mIniParser.getFloat("GameSettings", "fSpeedIncreasePerObject");
	modelConfig.pointsPerObject = mIniParser.getInt("GameSettings", "iPointsPerObject");
	modelConfig.hasBonus = mIniParser.getBool("GameSettings", "bHasBonus");
	modelConfig.bonusFrequency = mIniParser.getInt("GameSettings", "iBonusFrequency");
	modelConfig.bonusDuration = mIniParser.getInt("GameSettings", "iBonusDuration");
	modelConfig.pointsPerBonusObject = mIniParser.getInt("GameSettings", "iPointsPerBonusObject");
}

void GlobalConfig::save() noexcept
{
	// [Graphics]
	mIniParser.setBool("Graphics", "bFullscreen", fullscreen);
	mIniParser.setInt("Graphics", "iWindowResolutionX", windowResolutionX);
	mIniParser.setInt("Graphics", "iWindowResolutionY", windowResolutionY);
	mIniParser.setBool("Graphics", "bVSync", vsync);
	mIniParser.setInt("Graphics", "iMSAA", msaa);
	mIniParser.setBool("Graphics", "bTransparentCube", transparentCube);

	// [GameSettings]
	mIniParser.setInt("GameSettings", "iGridWidth", modelConfig.gridWidth);
	mIniParser.setFloat("GameSettings", "fTilesPerSecond", modelConfig.tilesPerSecond);
	mIniParser.setBool("GameSettings", "bHasSpeedIncrease", modelConfig.hasSpeedIncrease);
	mIniParser.setFloat("GameSettings", "fSpeedIncreasePerObject", modelConfig.speedIncreasePerObject);
	mIniParser.setInt("GameSettings", "iPointsPerObject", modelConfig.pointsPerObject);
	mIniParser.setBool("GameSettings", "bHasBonus", modelConfig.hasBonus);
	mIniParser.setInt("GameSettings", "iBonusFrequency", modelConfig.bonusFrequency);
	mIniParser.setInt("GameSettings", "iBonusDuration", modelConfig.bonusDuration);
	mIniParser.setInt("GameSettings", "iPointsPerBonusObject", modelConfig.pointsPerBonusObject);

	if (!mIniParser.save()) {
		std::cerr << "Couldn't save config.ini at: " << userIniPath() << std::endl;
	}
}

} // namespace s3