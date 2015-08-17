#include "GlobalConfig.hpp"

#include <iostream>
#include <string>
#include <exception> // std::terminate()

#include <sfz/SDL.hpp>
#include <sfz/util/IO.hpp>

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

// GlobalConfig: Singleton instance
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

GlobalConfig& GlobalConfig::INSTANCE() noexcept
{
	static GlobalConfig cfg;
	return cfg;
}

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

	sfz::IniParser& ip = mIniParser;
	
	// [GameSettings]
	ModelConfig& mc = modelConfig;
	static const string gsStr = "GameSettings";
	mc.hasBonus =               ip.sanitizeBool(gsStr, "bHasBonus", true);
	mc.hasSpeedIncrease =       ip.sanitizeBool(gsStr, "bHasSpeedIncrease", true);
	mc.speedIncreasePerObject = ip.sanitizeFloat(gsStr, "fSpeedIncreasePerObject", 0.025f, 0.001f, 60.0f);
	mc.tilesPerSecond =         ip.sanitizeFloat(gsStr, "fTilesPerSecond", 2.25f, 0.05f, 60.0f);
	mc.gridWidth =              ip.sanitizeInt(gsStr, "iGridWidth", 3, 2, 128);;
	mc.bonusDuration =          ip.sanitizeInt(gsStr, "iBonusDuration", 32, 0, 4096);
	mc.bonusFrequency =         ip.sanitizeInt(gsStr, "iBonusFrequency", 8, 1, 1024);
	mc.pointsPerBonusObject =   ip.sanitizeInt(gsStr, "iPointsPerBonusObject", 32, 32, 4096);
	mc.pointsPerObject =        ip.sanitizeInt(gsStr, "iPointsPerObject", 8, 0, 4096);
	
	// [Graphics]
	static const string grStr = "Graphics";
	fullscreen =        ip.sanitizeBool(grStr, "bFullscreen", false);
	transparentCube =   ip.sanitizeBool(grStr, "bTransparentCube", true);
	vsync =             ip.sanitizeBool(grStr, "bVSync", true);
	msaa =              ip.sanitizeInt(grStr, "iMSAA", 4, 0, 32);
	windowResolutionX = ip.sanitizeInt(grStr, "iWindowResolutionX", 800, 64, 8192);
	windowResolutionY = ip.sanitizeInt(grStr, "iWindowResolutionY", 800, 64, 8192);
}

void GlobalConfig::save() noexcept
{
	// [GameSettings]
	static const string gsStr = "GameSettings";
	mIniParser.setBool(gsStr, "bHasBonus", modelConfig.hasBonus);
	mIniParser.setBool(gsStr, "bHasSpeedIncrease", modelConfig.hasSpeedIncrease);
	mIniParser.setInt(gsStr, "iGridWidth", modelConfig.gridWidth);
	mIniParser.setFloat(gsStr, "fSpeedIncreasePerObject", modelConfig.speedIncreasePerObject);
	mIniParser.setFloat(gsStr, "fTilesPerSecond", modelConfig.tilesPerSecond);
	mIniParser.setInt(gsStr, "iBonusDuration", modelConfig.bonusDuration);
	mIniParser.setInt(gsStr, "iBonusFrequency", modelConfig.bonusFrequency);
	mIniParser.setInt(gsStr, "iPointsPerBonusObject", modelConfig.pointsPerBonusObject);
	mIniParser.setInt(gsStr, "iPointsPerObject", modelConfig.pointsPerObject);

	// [Graphics]
	static const string grStr = "Graphics";
	mIniParser.setBool(grStr, "bFullscreen", fullscreen);
	mIniParser.setBool(grStr, "bTransparentCube", transparentCube);
	mIniParser.setBool(grStr, "bVSync", vsync);
	mIniParser.setInt(grStr, "iMSAA", msaa);
	mIniParser.setInt(grStr, "iWindowResolutionX", windowResolutionX);
	mIniParser.setInt(grStr, "iWindowResolutionY", windowResolutionY);

	if (!mIniParser.save()) {
		std::cerr << "Couldn't save config.ini at: " << userIniPath() << std::endl;
	}
}

// GlobalConfig: Private constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

GlobalConfig::GlobalConfig() noexcept
:
	mIniParser{ userIniPath() }
{ }

} // namespace s3