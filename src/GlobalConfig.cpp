#include "GlobalConfig.hpp"

#include <iostream>
#include <string>
#include <exception> // std::terminate()

#include <sfz/SDL.hpp>
#include <sfz/util/IO.hpp>

#include "IniParser.hpp"

namespace s3 {

/*namespace {

using namespace boost::property_tree;

const std::string& basePath() noexcept
{
	static const std::string BASE_PATH{SDL_GetBasePath()};
	return BASE_PATH;
}

const std::string& defaultIniPath() noexcept
{
	static const std::string DEFAULT_INI_PATH = basePath() + "assets/snakium-cubed-default.ini";
	return DEFAULT_INI_PATH;
}

const std::string& userIniPath() noexcept
{
	static const std::string USER_INI_PATH = basePath() + "snakium-cubed.ini";
	return USER_INI_PATH;
}

template<typename T>
void sanitizePTItem(ptree& pt, const std::string& key, T min, T max, T def) noexcept
{
	T temp;
	try {
		temp = pt.get<T>(key);
	} catch (ptree_bad_path) {
		pt.put(key, def);
	} catch (ptree_bad_data) {
		pt.put(key, def); // Somewhat superflous, but we might want to do smarter things.
	}
	if (temp < min) {
		pt.put(key, temp);
	} else if (max < temp) {
		pt.put(key, max);
	}
}

void sanitizePTItemBool(ptree& pt, const std::string& key, bool def) noexcept
{
	try {
		pt.get<bool>(key);
	} catch (ptree_bad_path) {
		pt.put(key, def);
	} catch (ptree_bad_data) {
		pt.put(key, def); // Somewhat superflous, but we might want to do smarter things.
	}
}

void sanitizePropertyTree(ptree& pt) noexcept
{
	
}

} // anonymous namespace

void GlobalConfig::load() noexcept
{
	using namespace boost::filesystem;

	// Copy default ini to user ini location if user ini doesn't exist
	if (!exists(userIniPath())) {
		if (!exists(defaultIniPath())) {
			std::cerr << "Missing default ini at path: " << defaultIniPath() << std::endl;
			std::terminate();
		}

		boost::system::error_code errVal;
		copy_file(defaultIniPath(), userIniPath(), errVal);
		if (errVal.value() != boost::system::errc::success) {
			std::cerr << "Failed to copy default ini (" << defaultIniPath() << ") to user ini ("
			          << userIniPath() << ") location, message: " << errVal.message() << std::endl;
			std::terminate();
		}
	}

	// Read user .ini
	mPtree.clear();
	try {
		ini_parser::read_ini(userIniPath(), mPtree);
	} catch (...) {
		std::cerr << "Failed to read user .ini at " << userIniPath() << std::endl;
		std::cerr << "User ini will likely be overwritten with default values." << std::endl;
	}
	sanitizePropertyTree(mPtree);

	// [Graphics]
	mFullscreen = mPtree.get<bool>("Graphics.fullscreen");
	mWindowResolutionX = mPtree.get<uint16_t>("Graphics.windowResolutionX");
	mWindowResolutionY = mPtree.get<uint16_t>("Graphics.windowResolutionY");
	mVSync = mPtree.get<bool>("Graphics.VSync");
	mMSAA = mPtree.get<uint16_t>("Graphics.MSAA");
	mTransparentCube = mPtree.get<bool>("Graphics.transparentCube");

	// [GameSettings]
	mModelConfig.gridWidth = mPtree.get<uint16_t>("GameSettings.gridWidth");
	mModelConfig.tilesPerSecond = mPtree.get<float>("GameSettings.tilesPerSecond");
	mModelConfig.hasSpeedIncrease = mPtree.get<bool>("GameSettings.hasSpeedIncrease");
	mModelConfig.speedIncreasePerObject = mPtree.get<float>("GameSettings.speedIncreasePerObject");
	mModelConfig.pointsPerObject = mPtree.get<uint16_t>("GameSettings.pointsPerObject");
	mModelConfig.hasBonus = mPtree.get<bool>("GameSettings.hasBonus");
	mModelConfig.bonusFrequency = mPtree.get<uint16_t>("GameSettings.bonusFrequency");
	mModelConfig.bonusDuration = mPtree.get<uint16_t>("GameSettings.bonusDuration");
	mModelConfig.pointsPerBonusObject = mPtree.get<uint16_t>("GameSettings.pointsPerBonusObject");
}

void GlobalConfig::save() noexcept
{
	// [Graphics]
	mPtree.put("Graphics.fullscreen", mFullscreen);
	mPtree.put("Graphics.windowResolutionX", mWindowResolutionX);
	mPtree.put("Graphics.windowResolutionY", mWindowResolutionY);
	mPtree.put("Graphics.MSAA", mMSAA);
	mPtree.put("Graphics.VSync", mVSync);
	mPtree.put("Graphics.transparentCube", mTransparentCube);

	// [GameSettings]
	mPtree.put("GameSettings.gridWidth", mModelConfig.gridWidth);
	mPtree.put("GameSettings.tilesPerSecond", mModelConfig.tilesPerSecond);
	mPtree.put("GameSettings.hasSpeedIncrease", mModelConfig.hasSpeedIncrease);
	mPtree.put("GameSettings.speedIncreasePerObject", mModelConfig.speedIncreasePerObject);
	mPtree.put("GameSettings.pointsPerObject", mModelConfig.pointsPerObject);
	mPtree.put("GameSettings.hasBonus", mModelConfig.hasBonus);
	mPtree.put("GameSettings.bonusFrequency", mModelConfig.bonusFrequency);
	mPtree.put("GameSettings.bonusDuration", mModelConfig.bonusDuration);
	mPtree.put("GameSettings.pointsPerBonusObject", mModelConfig.pointsPerBonusObject);

	// Write to ini.
	try {
		ini_parser::write_ini(userIniPath(), mPtree);
	} catch (...) {
		std::cerr << "Failed to write user .ini at " << userIniPath() << std::endl;
	}
}*/



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

	// TODO: Make sure config file exists
	
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