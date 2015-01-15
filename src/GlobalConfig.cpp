#include "GlobalConfig.hpp"

namespace s3 {

namespace {

const std::string DEFAULT_INI_PATH{"assets/snakium-cubed-default.ini"};
const std::string USER_INI_PATH{"snakium-cubed.ini"};


/*bool file_exists(const std::string& path) noexcept
{
	FILE* filePtr = std::fopen(path.c_str(), "r");
	if (filePtr == NULL) return false;
	std::fclose(filePtr);
	return true;
}*/

} // anonymous namespace

void GlobalConfig::load() noexcept
{
	using namespace boost::filesystem;
	using namespace boost::property_tree;
	using std::uint16_t;

	// Copy default ini to user ini location if user ini doesn't exist
	if (!exists(USER_INI_PATH)) {
		if (!exists(DEFAULT_INI_PATH)) {
			std::cerr << "Missing default ini at path: " << DEFAULT_INI_PATH <<  std::endl;
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