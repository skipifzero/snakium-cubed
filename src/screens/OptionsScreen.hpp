#pragma once
#ifndef S3_SCREENS_OPTIONS_SCREEN_HPP
#define S3_SCREENS_OPTIONS_SCREEN_HPP

#include <string>
#include <vector>

#include <sfz/math/Vector.hpp>
#include <sfz/Screens.hpp>
#include <sfz/GUI.hpp>

#include "GlobalConfig.hpp"

namespace s3 {

using sfz::UpdateOp;
using sfz::UpdateState;
using sfz::vec2;
using sfz::vec3;

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
	// Private members
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	gui::System mGuiSystem;
	vector<sfz::vec2i> mResolutions;
	UpdateOp mUpdateOp = sfz::SCREEN_NO_OP;
};

} // namespace s3

#endif