#pragma once
#ifndef S3_CONFIG_HPP
#define S3_CONFIG_HPP

#include <cstdint>

#include <sfz/util/IniParser.hpp>

#include "gamelogic/ModelConfig.hpp"

namespace s3 {

using std::int32_t;

// ConfigData struct
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

struct ConfigData {
	ModelConfig modelConfig;
	bool fullscreen;
	int32_t windowResolutionX, windowResolutionY;
	bool vsync;
	int32_t msaa;
	bool transparentCube;
};

// GlobalConfig class
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

class GlobalConfig final : public ConfigData {
public:
	// Singleton instance
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	static GlobalConfig& INSTANCE() noexcept;
	
	// Loading and saving to file
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	void load() noexcept;
	void save() noexcept;

	// ConfigData getters & setters
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	inline ConfigData data() const noexcept { return *this; }
	void data(const ConfigData& configData) noexcept;

private:
	// Private constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	GlobalConfig(const GlobalConfig&) = default;
	GlobalConfig& operator= (const GlobalConfig&) = default;
	~GlobalConfig() noexcept = default;

	GlobalConfig() noexcept;

	// Private members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	sfz::IniParser mIniParser;
};

} // namespace s3
#endif