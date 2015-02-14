#include "GlobalConfig.hpp"

#include "sfz/MSVC12HackON.hpp"

namespace s3 {

namespace {

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
	// [Graphics]
	sanitizePTItemBool(pt, "Graphics.fullscreen", false);
	sanitizePTItem<uint16_t>(pt, "Graphics.windowResolutionX", 64, 8192, 800);
	sanitizePTItem<uint16_t>(pt, "Graphics.windowResolutionY", 64, 8192, 800);
	sanitizePTItem<uint16_t>(pt, "Graphics.MSAA", 0, 32, 8);
	sanitizePTItemBool(pt, "Graphics.transparentCube", false);

	// [GameSettings]
	sanitizePTItem<uint16_t>(pt, "GameSettings.gridWidth", 2, 128, 4);
	sanitizePTItem<float>(pt, "GameSettings.tilesPerSecond", 0.05f, 60.0f, 2.3f);
	sanitizePTItemBool(pt, "GameSettings.hasSpeedIncrease", true);
	sanitizePTItem<float>(pt, "GameSettings.speedIncreasePerObject", 0.001f, 60.0f, 0.025f);
	sanitizePTItem<uint16_t>(pt, "GameSettings.pointsPerObject", 0, 4096, 8);
	sanitizePTItemBool(pt, "GameSettings.hasBonus", true);
	sanitizePTItem<uint16_t>(pt, "GameSettings.bonusFrequency", 1, 1024, 8);
	sanitizePTItem<uint16_t>(pt, "GameSettings.bonusDuration", 0, 4096, 16);
	sanitizePTItem<uint16_t>(pt, "GameSettings.pointsPerBonusObject", 0, 4096, 32);
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
}

} // namespace s3

#include "sfz/MSVC12HackOFF.hpp"