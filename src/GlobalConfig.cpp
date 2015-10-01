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
	

	// [Debug]
	static const string dStr = "Debug";
	continuousShaderReload = ip.sanitizeBool(dStr, "bContinuousShaderReload", false);

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
	displayIndex =      ip.sanitizeInt(grStr, "iDisplayIndex", 0, 0, 32);
	fullscreenMode =    ip.sanitizeInt(grStr, "iFullscreenMode", 0, 0, 2);
	msaa =              ip.sanitizeInt(grStr, "iMSAA", 4, 0, 32);
	refreshRate =       ip.sanitizeInt(grStr, "iRefreshRate", 60, 15, 240);
	resolutionX =       ip.sanitizeInt(grStr, "iResolutionX", 1920, 200, 30720);
	resolutionY =       ip.sanitizeInt(grStr, "iResolutionY", 1080, 200, 17280);
	vsync =             ip.sanitizeInt(grStr, "iVSync", 1, 0, 2);
	windowHeight =      ip.sanitizeInt(grStr, "iWindowHeight", 800, 200, 10000);
	windowWidth =       ip.sanitizeInt(grStr, "iWindowWidth", 800, 200, 10000);
}

void GlobalConfig::save() noexcept
{
	// [Debug]
	static const string dStr = "Debug";
	mIniParser.setBool(dStr, "bContinuousShaderReload", continuousShaderReload);

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
	mIniParser.setInt(grStr, "iDisplayIndex", displayIndex);
	mIniParser.setInt(grStr, "iFullscreenMode", fullscreenMode);
	mIniParser.setInt(grStr, "iMSAA", msaa);
	mIniParser.setInt(grStr, "iRefreshRate", refreshRate);
	mIniParser.setInt(grStr, "iResolutionX", resolutionX);
	mIniParser.setInt(grStr, "iResolutionY", resolutionY);
	mIniParser.setInt(grStr, "iVSync", vsync);
	mIniParser.setInt(grStr, "iWindowHeight", windowHeight);
	mIniParser.setInt(grStr, "iWindowWidth", windowWidth);

	if (!mIniParser.save()) {
		std::cerr << "Couldn't save config.ini at: " << userIniPath() << std::endl;
	}
}

// GlobalConfig: ConfigData getters & setters
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void GlobalConfig::data(const ConfigData& configData) noexcept
{
	this->displayIndex = configData.displayIndex;
	this->fullscreenMode = configData.fullscreenMode;
	this->refreshRate = configData.refreshRate;
	this->resolutionX = configData.resolutionX;
	this->resolutionY = configData.resolutionY;
	this->windowWidth = configData.windowWidth;
	this->windowHeight = configData.windowHeight;
	this->vsync = configData.vsync;
	this->msaa = configData.msaa;

	this->modelConfig = configData.modelConfig;
}

// GlobalConfig: Private constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

GlobalConfig::GlobalConfig() noexcept
:
	mIniParser{ userIniPath() }
{ }

} // namespace s3