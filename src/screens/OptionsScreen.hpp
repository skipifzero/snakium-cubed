#pragma once
#ifndef S3_SCREENS_OPTIONS_SCREEN_HPP
#define S3_SCREENS_OPTIONS_SCREEN_HPP

#include <string>
#include <vector>

#include <sfz/GUI.hpp>
#include <sfz/math/Vector.hpp>
#include <sfz/Screens.hpp>
#include <sfz/sdl/Window.hpp>

#include "GlobalConfig.hpp"

namespace s3 {

using sfz::UpdateOp;
using sfz::UpdateState;
using sfz::vec2;
using sfz::vec2i;
using sfz::vec3;

using gui::BaseItem;

using std::shared_ptr;
using std::string;
using std::vector;

class OptionsScreen : public sfz::BaseScreen {
public:
	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	OptionsScreen(const OptionsScreen&) = delete;
	OptionsScreen& operator= (const OptionsScreen&) = delete;
	~OptionsScreen() noexcept = default;

	OptionsScreen() noexcept;

	// Overriden screen methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	virtual UpdateOp update(UpdateState& state) override final;
	virtual void render(UpdateState& state) override final;

	// Public members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	ConfigData cfgData;

private:
	// Private methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	void applyConfig() noexcept;

	// Private members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	sdl::Window* mWindowPtr = nullptr;
	gui::System mGuiSystem;
	UpdateOp mUpdateOp = sfz::SCREEN_NO_OP;

	shared_ptr<BaseItem> mCancelApplyCon, mCancelButton, mApplyButton;
};

} // namespace s3

#endif