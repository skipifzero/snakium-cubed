#include "GlobalConfig.hpp"

namespace s3 {

namespace {

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

} // anonymous namespace

void GlobalConfig::load() noexcept
{
	using namespace boost::filesystem;
	using namespace boost::property_tree;
	using std::uint16_t;

	static const std::string& DEFAULT_INI_PATH = defaultIniPath();
	static const std::string& USER_INI_PATH = userIniPath();

	// Copy default ini to user ini location if user ini doesn't exist
	if (!exists(USER_INI_PATH)) {
		if (!exists(DEFAULT_INI_PATH)) {
			std::cerr << "Missing default ini at path: " << DEFAULT_INI_PATH << std::endl;
			std::terminate();
		}

		boost::system::error_code errVal;
		copy_file(DEFAULT_INI_PATH, USER_INI_PATH, errVal);
		if (errVal.value() != boost::system::errc::success) {
			std::cerr << "Failed to copy default ini (" << DEFAULT_INI_PATH << ") to user ini ("
			          << USER_INI_PATH << ") location, message: " << errVal.message() << std::endl;
			std::terminate();
		}
	}
	
	mPtree.clear();
	ini_parser::read_ini(USER_INI_PATH, mPtree);

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

}

} // namespace s3