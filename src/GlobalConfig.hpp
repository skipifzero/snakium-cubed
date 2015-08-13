#pragma once
#ifndef S3_CONFIG_HPP
#define S3_CONFIG_HPP

#include <cstdint> // uint16_t

#include "IniParser.hpp"

#include "gamelogic/ModelConfig.hpp"

namespace s3 {

using std::uint16_t;

// GlobalConfig class
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

class GlobalConfig {
public:
	// Public members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	ModelConfig modelConfig;
	bool fullscreen;
	uint16_t windowResolutionX, windowResolutionY;
	bool vsync;
	uint16_t msaa;
	bool transparentCube;
	
	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	GlobalConfig(const GlobalConfig&) = default;
	GlobalConfig& operator= (const GlobalConfig&) = default;
	~GlobalConfig() noexcept = default;

	GlobalConfig() noexcept;

	// Loading and saving to file
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	void load() noexcept;
	void save() noexcept;

private:
	sfz::IniParser mIniParser;
};

} // namespace s3
#endif