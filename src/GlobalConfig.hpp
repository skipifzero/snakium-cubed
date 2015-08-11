#pragma once
#ifndef S3_CONFIG_HPP
#define S3_CONFIG_HPP

#include <cstdint> // uint16_t

#include "gamelogic/ModelConfig.hpp"

namespace s3 {

using std::uint16_t;

struct GlobalConfig {
public:
	ModelConfig mModelConfig;
	bool mFullscreen;
	uint16_t mWindowResolutionX, mWindowResolutionY;
	bool mVSync;
	uint16_t mMSAA;
	bool mTransparentCube;

	void load() noexcept;
	void save() noexcept;
};

} // namespace s3
#endif