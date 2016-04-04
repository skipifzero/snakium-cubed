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

struct GraphicsConfig {
	int32_t vsync; // 0 = off, 1 = on, 2 = swap control tear
	bool nativeInternalRes;
	int32_t internalResolutionY;
	float blurResScaling;
	float spotlightResScaling;
	float lightShaftsResScaling;
	int32_t scalingAlgorithm;
};

extern const GraphicsConfig TOASTER_GRAPHICS_CONFIG;
extern const GraphicsConfig LAPTOP_W_INTEL_GRAPHICS_CONFIG;
extern const GraphicsConfig LAPTOP_W_NVIDIA_GRAPHICS_CONFIG;
extern const GraphicsConfig GAMING_COMPUTER_GRAPHICS_CONFIG;
extern const GraphicsConfig FUTURE_SUPERCOMPUTER_GRAPHICS_CONFIG;

bool operator== (const GraphicsConfig& lhs, const GraphicsConfig& rhs) noexcept;
bool operator!= (const GraphicsConfig& lhs, const GraphicsConfig& rhs) noexcept;

struct ConfigData {
	// Debug
	bool continuousShaderReload;
	bool printFrametimes;

	// Graphics
	int32_t displayIndex;
	int32_t fullscreenMode; // 0 = off, 1 = windowed, 2 = exclusive
	GraphicsConfig gc;

	// Audio
	int32_t musicVolume; // [0, 10]
	int32_t sfxVolume; // [0, 10]

	// Game Settings
	int32_t inputBufferSize;

	// Custom Model
	ModelConfig modelConfig;
};

bool operator== (const ConfigData& lhs, const ConfigData& rhs) noexcept;
bool operator!= (const ConfigData& lhs, const ConfigData& rhs) noexcept;

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