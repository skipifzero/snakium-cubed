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
	static GlobalConfig cfg = []() {
		GlobalConfig temp;
		temp.load();
		return temp;
	}();
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
	mc.speedIncreasePerObject = ip.sanitizeFloat(gsStr, "fSpeedIncreasePerObject", 0.025f, 0.001f, 60.0f);
	mc.tilesPerSecond =         ip.sanitizeFloat(gsStr, "fTilesPerSecond", 2.25f, 0.05f, 60.0f);
	mc.gridWidth =              ip.sanitizeInt(gsStr, "iGridWidth", 3, 2, 128);;
	mc.bonusDuration =          ip.sanitizeInt(gsStr, "iBonusDuration", 32, 0, 4096);
	mc.bonusFrequency =         ip.sanitizeInt(gsStr, "iBonusFrequency", 8, 1, 1024);
	mc.bonusObjectValue =       ip.sanitizeInt(gsStr, "iBonusObjectValue", 32, 32, 4096);
	mc.objectValue =            ip.sanitizeInt(gsStr, "iObjectValue", 8, 0, 4096);
	
	// [Graphics]
	static const string grStr = "Graphics";
	transparentCube =   ip.sanitizeBool(grStr, "bTransparentCube", true);
	displayIndex =      ip.sanitizeInt(grStr, "iDisplayIndex", 0, 0, 32);
	fullscreenMode =    ip.sanitizeInt(grStr, "iFullscreenMode", 0, 0, 2);
	msaa =              ip.sanitizeInt(grStr, "iMSAA", 4, 0, 32);
	vsync =             ip.sanitizeInt(grStr, "iVSync", 1, 0, 2);
	windowResolutionX = ip.sanitizeInt(grStr, "iWindowResolutionX", 800, 64, 8192);
	windowResolutionY = ip.sanitizeInt(grStr, "iWindowResolutionY", 800, 64, 8192);
}

void GlobalConfig::save() noexcept
{
	// [GameSettings]
	static const string gsStr = "GameSettings";
	mIniParser.setBool(gsStr, "bHasBonus", modelConfig.hasBonus);
	mIniParser.setInt(gsStr, "iGridWidth", modelConfig.gridWidth);
	mIniParser.setFloat(gsStr, "fSpeedIncreasePerObject", modelConfig.speedIncreasePerObject);
	mIniParser.setFloat(gsStr, "fTilesPerSecond", modelConfig.tilesPerSecond);
	mIniParser.setInt(gsStr, "iBonusDuration", modelConfig.bonusDuration);
	mIniParser.setInt(gsStr, "iBonusFrequency", modelConfig.bonusFrequency);
	mIniParser.setInt(gsStr, "iBonusObjectValue", modelConfig.bonusObjectValue);
	mIniParser.setInt(gsStr, "iObjectValue", modelConfig.objectValue);

	// [Graphics]
	static const string grStr = "Graphics";
	mIniParser.setBool(grStr, "bTransparentCube", transparentCube);
	mIniParser.setInt(grStr, "iDisplayIndex", displayIndex);
	mIniParser.setInt(grStr, "iFullscreenMode", fullscreenMode);
	mIniParser.setInt(grStr, "iMSAA", msaa);
	mIniParser.setInt(grStr, "iVSync", vsync);
	mIniParser.setInt(grStr, "iWindowResolutionX", windowResolutionX);
	mIniParser.setInt(grStr, "iWindowResolutionY", windowResolutionY);

	if (!mIniParser.save()) {
		std::cerr << "Couldn't save config.ini at: " << userIniPath() << std::endl;
	}
}

// GlobalConfig: ConfigData getters & setters
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void GlobalConfig::data(const ConfigData& configData) noexcept
{
	this->modelConfig = configData.modelConfig;
	this->displayIndex = configData.displayIndex;
	this->fullscreenMode = configData.fullscreenMode;
	this->windowResolutionX = configData.windowResolutionX;
	this->windowResolutionY = configData.windowResolutionY;
	this->vsync = configData.vsync;
	this->msaa = configData.msaa;
	this->transparentCube = configData.transparentCube;
}

// GlobalConfig: Private constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

GlobalConfig::GlobalConfig() noexcept
:
	mIniParser{ userIniPath() }
{ }

} // namespace s3