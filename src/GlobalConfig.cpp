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

// ConfigData struct
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

const GraphicsConfig TOASTER_GRAPHICS_CONFIG = {
	1, // int32_t vsync; // 0 = off, 1 = on, 2 = swap control tear
	false, // bool nativeInternalRes;
	480, // int32_t internalResolutionY;
	0.15f, // float blurResScaling;
	1.0f, // float spotlightResScaling;
	0.15f, // float lightShaftsResScaling;
	1 // int32_t scalingAlgorithm;
};

const GraphicsConfig LAPTOP_W_INTEL_GRAPHICS_CONFIG = {
	1, // int32_t vsync; // 0 = off, 1 = on, 2 = swap control tear
	false, // bool nativeInternalRes;
	720, // int32_t internalResolutionY;
	0.15f, // float blurResScaling;
	1.0f, // float spotlightResScaling;
	0.3f, // float lightShaftsResScaling;
	1 // int32_t scalingAlgorithm;
};

const GraphicsConfig LAPTOP_W_NVIDIA_GRAPHICS_CONFIG = {
	1, // int32_t vsync; // 0 = off, 1 = on, 2 = swap control tear
	false, // bool nativeInternalRes;
	1080, // int32_t internalResolutionY;
	0.2f, // float blurResScaling;
	1.0f, // float spotlightResScaling;
	0.4f, // float lightShaftsResScaling;
	1 // int32_t scalingAlgorithm;
};

const GraphicsConfig GAMING_COMPUTER_GRAPHICS_CONFIG = {
	1, // int32_t vsync; // 0 = off, 1 = on, 2 = swap control tear
	false, // bool nativeInternalRes;
	1440, // int32_t internalResolutionY;
	0.25f, // float blurResScaling;
	1.0f, // float spotlightResScaling;
	0.5f, // float lightShaftsResScaling;
	3 // int32_t scalingAlgorithm;
};

const GraphicsConfig FUTURE_SUPERCOMPUTER_GRAPHICS_CONFIG = {
	1, // int32_t vsync; // 0 = off, 1 = on, 2 = swap control tear
	false, // bool nativeInternalRes;
	2160, // int32_t internalResolutionY;
	0.4f, // float blurResScaling;
	1.0f, // float spotlightResScaling;
	0.5f, // float lightShaftsResScaling;
	3 // int32_t scalingAlgorithm;
};

bool operator== (const GraphicsConfig& lhs, const GraphicsConfig& rhs) noexcept
{
	return lhs.vsync == rhs.vsync &&
	       lhs.nativeInternalRes == rhs.nativeInternalRes &&
	       lhs.internalResolutionY == rhs.internalResolutionY &&
	       lhs.blurResScaling == rhs.blurResScaling &&
	       lhs.spotlightResScaling == rhs.spotlightResScaling &&
	       lhs.lightShaftsResScaling == rhs.lightShaftsResScaling &&
	       lhs.scalingAlgorithm == rhs.scalingAlgorithm;
}

bool operator!= (const GraphicsConfig& lhs, const GraphicsConfig& rhs) noexcept
{
	return !(lhs == rhs);
}

bool operator== (const ConfigData& lhs, const ConfigData& rhs) noexcept
{
	return

	// Debug
	lhs.continuousShaderReload == rhs.continuousShaderReload &&
	lhs.printFrametimes == rhs.printFrametimes &&

	// Graphics
	lhs.displayIndex == rhs.displayIndex &&
	lhs.fullscreenMode == rhs.fullscreenMode &&
	lhs.gc == rhs.gc &&

	// Audio
	lhs.musicVolume == rhs.musicVolume &&
	lhs.sfxVolume == rhs.sfxVolume &&

	// Game Settings
	lhs.inputBufferSize == rhs.inputBufferSize &&

	// Custom Model
	lhs.modelConfig == rhs.modelConfig;
}

bool operator!= (const ConfigData& lhs, const ConfigData& rhs) noexcept
{
	return !(lhs == rhs);
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
	
	// [Audio]
	static const string aStr = "Audio";
	musicVolume = ip.sanitizeInt(aStr, "iMusicVolume", 10, 0, 10);
	sfxVolume =   ip.sanitizeInt(aStr, "iSfxVolume", 10, 0, 10);

	// [CustomModel]
	static const string cmStr = "CustomModel";
	ModelConfig& mc = modelConfig;
	mc.gridWidth =              ip.sanitizeInt(cmStr, "iGridWidth", 3, 2, 128);

	mc.tilesPerSecond =         ip.sanitizeFloat(cmStr, "fTilesPerSecond", 2.25f, 0.05f, 60.0f);
	mc.speedIncreasePerObject = ip.sanitizeFloat(cmStr, "fSpeedIncreasePerObject", 0.025f, 0.001f, 60.0f);
	
	mc.bonusFrequency =         ip.sanitizeInt(cmStr, "iBonusFrequency", 8, 1, 1024);
	mc.bonusDuration =          ip.sanitizeInt(cmStr, "iBonusDuration", 32, 0, 4096);
	mc.numberOfBonusObjects =   ip.sanitizeInt(cmStr, "iNumberOfBonusObjects", 1, 0, 32);
	mc.earlyDuration =          ip.sanitizeInt(cmStr, "iEarlyDuration", 8, 0, 1024);
	mc.shiftBonusDuration =     ip.sanitizeInt(cmStr, "iShiftBonusDuration", 8, 0, 1024);

	mc.objectValue =            ip.sanitizeInt(cmStr, "iObjectValue", 8, 0, 4096);
	mc.objectEarlyBonus =       ip.sanitizeInt(cmStr, "iObjectEarlyBonus", 8, 0, 4096);
	mc.objectShiftBonus =       ip.sanitizeInt(cmStr, "iObjectShiftBonus", 8, 0, 4096);
	mc.bonusObjectValue =       ip.sanitizeInt(cmStr, "iBonusObjectValue", 32, 0, 4096);
	mc.bonusObjectShiftBonus =  ip.sanitizeInt(cmStr, "iBonusObjectShiftBonus", 16, 0, 4096);

	// [Debug]
	static const string dStr = "Debug";
	continuousShaderReload = ip.sanitizeBool(dStr, "bContinuousShaderReload", false);
	printFrametimes =        ip.sanitizeBool(dStr, "bPrintFrametimes", false);

	// [GameSettings]
	static const string gsStr = "GameSettings";
	inputBufferSize = ip.sanitizeInt(gsStr, "iInputBufferSize", 2, 1, 5);
	
	// [Graphics]
	static const string grStr = "Graphics";
	gc.nativeInternalRes =     ip.sanitizeBool(grStr, "bNativeInternalRes", false);
	gc.blurResScaling =        ip.sanitizeFloat(grStr, "fBlurResScaling", 0.4f, 0.01f, 2.0f);
	gc.internalResolutionY =   ip.sanitizeInt(grStr, "iInternalResolutionY", 1080, 120, 8192);
	gc.lightShaftsResScaling = ip.sanitizeFloat(grStr, "fLightShaftsResScaling", 0.5f, 0.01f, 10.0f);
	gc.spotlightResScaling =   ip.sanitizeFloat(grStr, "fSpotlightResScaling", 1.0f, 0.01f, 10.0f);
	displayIndex =          ip.sanitizeInt(grStr, "iDisplayIndex", -1, -1, 8);
	fullscreenMode =        ip.sanitizeInt(grStr, "iFullscreenMode", 1, 0, 2);
	gc.scalingAlgorithm =      ip.sanitizeInt(grStr, "iScalingAlgorithm", 3, 0, 8);
	gc.vsync =                 ip.sanitizeInt(grStr, "iVSync", 1, 0, 2);
}

void GlobalConfig::save() noexcept
{
	// [Audio]
	static const string aStr = "Audio";
	mIniParser.setInt(aStr, "iMusicVolume", musicVolume);
	mIniParser.setInt(aStr, "iSfxVolume", sfxVolume);

	// [CustomModel]
	static const string cmStr = "CustomModel";
	mIniParser.setInt(cmStr, "iGridWidth", modelConfig.gridWidth);

	mIniParser.setFloat(cmStr, "fTilesPerSecond", modelConfig.tilesPerSecond);
	mIniParser.setFloat(cmStr, "fSpeedIncreasePerObject", modelConfig.speedIncreasePerObject);
	
	mIniParser.setInt(cmStr, "iBonusFrequency", modelConfig.bonusFrequency);
	mIniParser.setInt(cmStr, "iBonusDuration", modelConfig.bonusDuration);
	mIniParser.setInt(cmStr, "iNumberOfBonusObjects", modelConfig.numberOfBonusObjects);
	mIniParser.setInt(cmStr, "iEarlyDuration", modelConfig.earlyDuration);
	mIniParser.setInt(cmStr, "iShiftBonusDuration", modelConfig.shiftBonusDuration);

	mIniParser.setInt(cmStr, "iObjectValue", modelConfig.objectValue);
	mIniParser.setInt(cmStr, "iObjectEarlyBonus", modelConfig.objectEarlyBonus);
	mIniParser.setInt(cmStr, "iObjectShiftBonus", modelConfig.objectShiftBonus);
	mIniParser.setInt(cmStr, "iBonusObjectValue", modelConfig.bonusObjectValue);
	mIniParser.setInt(cmStr, "iBonusObjectShiftBonus", modelConfig.bonusObjectShiftBonus);

	// [Debug]
	static const string dStr = "Debug";
	mIniParser.setBool(dStr, "bContinuousShaderReload", continuousShaderReload);
	mIniParser.setBool(dStr, "bPrintFrametimes", printFrametimes);

	// [GameSettings]
	static const string gsStr = "GameSettings";
	mIniParser.setInt(gsStr, "iInputBufferSize", inputBufferSize);

	// [Graphics]
	static const string grStr = "Graphics";
	mIniParser.setBool(grStr, "bNativeInternalRes", gc.nativeInternalRes);
	mIniParser.setFloat(grStr, "fBlurResScaling", gc.blurResScaling);
	mIniParser.setInt(grStr, "iInternalResolutionY", gc.internalResolutionY);
	mIniParser.setFloat(grStr, "fLightShaftsResScaling", gc.lightShaftsResScaling);
	mIniParser.setFloat(grStr, "fSpotlightResScaling", gc.spotlightResScaling);
	mIniParser.setInt(grStr, "iDisplayIndex", displayIndex);
	mIniParser.setInt(grStr, "iFullscreenMode", fullscreenMode);
	mIniParser.setInt(grStr, "iScalingAlgorithm", gc.scalingAlgorithm);
	mIniParser.setInt(grStr, "iVSync", gc.vsync);

	if (!mIniParser.save()) {
		std::cerr << "Couldn't save config.ini at: " << userIniPath() << std::endl;
	}
}

// GlobalConfig: ConfigData getters & setters
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void GlobalConfig::data(const ConfigData& configData) noexcept
{
	// Debug
	this->continuousShaderReload = configData.continuousShaderReload;
	this->printFrametimes = configData.printFrametimes;

	// Graphics
	this->displayIndex = configData.displayIndex;
	this->fullscreenMode = configData.fullscreenMode;
	this->gc = configData.gc;

	// Audio
	this->musicVolume = configData.musicVolume;
	this->sfxVolume = configData.sfxVolume;

	// Game Settings
	this->inputBufferSize = configData.inputBufferSize;

	// Custom Model
	this->modelConfig = configData.modelConfig;
}

// GlobalConfig: Private constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

GlobalConfig::GlobalConfig() noexcept
:
	mIniParser{userIniPath()}
{ }

} // namespace s3