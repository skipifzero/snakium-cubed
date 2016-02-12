#pragma once
#ifndef S3_SCREENS_OPTIONS_CUSTOM_SCREEN_HPP
#define S3_SCREENS_OPTIONS_CUSTOM_SCREEN_HPP

#include <memory>

#include <sfz/GUI.hpp>
#include <sfz/Screens.hpp>

#include "GlobalConfig.hpp"

namespace s3 {

using sfz::UpdateOp;
using sfz::UpdateState;

using gui::BaseItem;

using std::shared_ptr;

class OptionsCustomScreen : public sfz::BaseScreen {
public:
	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	OptionsCustomScreen(const OptionsCustomScreen&) = delete;
	OptionsCustomScreen& operator= (const OptionsCustomScreen&) = delete;
	~OptionsCustomScreen() noexcept = default;

	OptionsCustomScreen() noexcept;

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