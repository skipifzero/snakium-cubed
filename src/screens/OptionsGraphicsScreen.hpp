#pragma once
#ifndef S3_SCREENS_OPTIONS_GRAPHICS_SCREEN_HPP
#define S3_SCREENS_OPTIONS_GRAPHICS_SCREEN_HPP

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

class OptionsGraphicsScreen : public sfz::BaseScreen {
public:
	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	OptionsGraphicsScreen(const OptionsGraphicsScreen&) = delete;
	OptionsGraphicsScreen& operator= (const OptionsGraphicsScreen&) = delete;
	~OptionsGraphicsScreen() noexcept = default;

	OptionsGraphicsScreen() noexcept;

	// Overriden screen methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	virtual UpdateOp update(UpdateState& state) override final;
	virtual void render(UpdateState& state) override final;
	virtual void onResize(vec2 windowDimensions, vec2 drawableDimensions);

	// Public members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	ConfigData cfgData;
	vec2 mDrawableDim{-1.0f}; // Public access is slightly hack

private:
	// Private methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	void applyConfig() noexcept;
	void updateResolutionFactors() noexcept;
	void updateResolutionStrings() noexcept;

	// Private members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	sdl::Window* mWindowPtr = nullptr;
	gui::System mGuiSystem;
	UpdateOp mUpdateOp = sfz::SCREEN_NO_OP;

	shared_ptr<BaseItem> mInternalResMultiChoicePtr, mBlurResMultiChoicePtr,
	                     mSpotlightResMultiChoicePtr/*, mLightShaftResMultiChoicePtr*/;

	shared_ptr<BaseItem> mCancelApplyCon, mCancelButton, mApplyButton;

	vector<int> mYResolutions;
	vector<string> mInternalResStrs;
	vector<string> mSecondaryResFactorStrs;
};

} // namespace s3
#endif