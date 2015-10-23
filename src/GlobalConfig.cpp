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
	
	// [CustomModel]
	static const string cmStr = "CustomModel";
	ModelConfig& mc = modelConfig;
	mc.hasBonus =               ip.sanitizeBool(cmStr, "bHasBonus", true);
	mc.speedIncreasePerObject = ip.sanitizeFloat(cmStr, "fSpeedIncreasePerObject", 0.025f, 0.001f, 60.0f);
	mc.tilesPerSecond =         ip.sanitizeFloat(cmStr, "fTilesPerSecond", 2.25f, 0.05f, 60.0f);
	mc.gridWidth =              ip.sanitizeInt(cmStr, "iGridWidth", 3, 2, 128);;
	mc.bonusDuration =          ip.sanitizeInt(cmStr, "iBonusDuration", 32, 0, 4096);
	mc.bonusFrequency =         ip.sanitizeInt(cmStr, "iBonusFrequency", 8, 1, 1024);
	mc.bonusObjectValue =       ip.sanitizeInt(cmStr, "iBonusObjectValue", 32, 32, 4096);
	mc.objectValue =            ip.sanitizeInt(cmStr, "iObjectValue", 8, 0, 4096);
	
	// [Debug]
	static const string dStr = "Debug";
	continuousShaderReload = ip.sanitizeBool(dStr, "bContinuousShaderReload", false);

	// [GameSettings]
	static const string gsStr = "GameSettings";
	inputBufferSize = ip.sanitizeInt(gsStr, "iInputBufferSize", 2, 1, 5);
	
	// [Graphics]
	static const string grStr = "Graphics";
	internalResScaling = ip.sanitizeFloat(grStr, "fInternalResScaling", 1.0f, 0.01f, 10.0f);
	displayIndex =      ip.sanitizeInt(grStr, "iDisplayIndex", 0, 0, 32);
	fullscreenMode =    ip.sanitizeInt(grStr, "iFullscreenMode", 0, 0, 2);
	msaa =              ip.sanitizeInt(grStr, "iMSAA", 4, 0, 32);
	refreshRate =       ip.sanitizeInt(grStr, "iRefreshRate", 60, 15, 240);
	resolutionX =       ip.sanitizeInt(grStr, "iResolutionX", 1920, 200, 30720);
	resolutionY =       ip.sanitizeInt(grStr, "iResolutionY", 1080, 200, 17280);
	scalingAlgorithm =  ip.sanitizeInt(grStr, "iScalingAlgorithm", 0, 0, 1337); // TODO: Fix
	vsync =             ip.sanitizeInt(grStr, "iVSync", 1, 0, 2);
	windowHeight =      ip.sanitizeInt(grStr, "iWindowHeight", 800, 200, 10000);
	windowWidth =       ip.sanitizeInt(grStr, "iWindowWidth", 800, 200, 10000);
}

void GlobalConfig::save() noexcept
{
	// [CustomModel]
	static const string cmStr = "CustomModel";
	mIniParser.setBool(cmStr, "bHasBonus", modelConfig.hasBonus);
	mIniParser.setInt(cmStr, "iGridWidth", modelConfig.gridWidth);
	mIniParser.setFloat(cmStr, "fSpeedIncreasePerObject", modelConfig.speedIncreasePerObject);
	mIniParser.setFloat(cmStr, "fTilesPerSecond", modelConfig.tilesPerSecond);
	mIniParser.setInt(cmStr, "iBonusDuration", modelConfig.bonusDuration);
	mIniParser.setInt(cmStr, "iBonusFrequency", modelConfig.bonusFrequency);
	mIniParser.setInt(cmStr, "iBonusObjectValue", modelConfig.bonusObjectValue);
	mIniParser.setInt(cmStr, "iObjectValue", modelConfig.objectValue);

	// [Debug]
	static const string dStr = "Debug";
	mIniParser.setBool(dStr, "bContinuousShaderReload", continuousShaderReload);

	// [GameSettings]
	static const string gsStr = "GameSettings";
	mIniParser.setInt(gsStr, "iInputBufferSize", inputBufferSize);

	// [Graphics]
	static const string grStr = "Graphics";
	mIniParser.setFloat(grStr, "fInternalResScaling", internalResScaling);
	mIniParser.setInt(grStr, "iDisplayIndex", displayIndex);
	mIniParser.setInt(grStr, "iFullscreenMode", fullscreenMode);
	mIniParser.setInt(grStr, "iMSAA", msaa);
	mIniParser.setInt(grStr, "iRefreshRate", refreshRate);
	mIniParser.setInt(grStr, "iResolutionX", resolutionX);
	mIniParser.setInt(grStr, "iResolutionY", resolutionY);
	mIniParser.setInt(grStr, "iScalingAlgorithm", scalingAlgorithm);
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
	this->internalResScaling = configData.internalResScaling;
	this->scalingAlgorithm = configData.scalingAlgorithm;

	this->inputBufferSize = configData.inputBufferSize;
	this->modelConfig = configData.modelConfig;
}

// GlobalConfig: Private constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

GlobalConfig::GlobalConfig() noexcept
:
	mIniParser{ userIniPath() }
{ }

} // namespace s3