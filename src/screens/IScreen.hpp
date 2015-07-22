#pragma once
#ifndef S3_I_SCREEN_HPP
#define S3_I_SCREEN_HPP

#include <vector>
#include <sfz/SDL.hpp>

namespace s3 {

/** @brief Common interface for each screen. */
class IScreen {
public:
	/** @brief Called once every frame, update logic should happen here. */
	virtual void update(const std::vector<SDL_Event>& events, float delta) = 0;

	/** @brief Called once very frame after update(), return nullptr to keep current screen. */
	virtual IScreen* changeScreen() = 0;

	/** @brief Called once every frame after changeScreen(), return true to quit program. */
	virtual bool quit() = 0;

	/** @brief Called once every frame after quit(), rendering should happen here. */
	virtual void render(float delta) = 0;
};

} // namespace s3
#endif