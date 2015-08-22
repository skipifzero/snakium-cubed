#pragma once
#ifndef SFZ_GUI_BUTTON_HPP
#define SFZ_GUI_BUTTON_HPP

#include <string>

#include "sfz/geometry/Rectangle.hpp"

namespace sfz {

using std::string;

// Button
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

class Button {
public:
	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	Button() noexcept = default;
	Button(const Button&) = default;
	Button& operator= (const Button&) = default;

	Button(const Rectangle& rect, void(*funcPtr)(Button&) = nullptr) noexcept;
	Button(const Rectangle& rect, const string& text, void(*funcPtr)(Button&) = nullptr) noexcept;

	// Public methods
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	// TOOD: Needs better API. Returns whether activated or not
	bool update(vec2 mousePos, bool clicked) noexcept;

	// Getters
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	inline const Rectangle& rect() const noexcept{ return mRect; }
	inline const string& text() const noexcept { return mText; }

	inline bool isEnabled() const noexcept { return mEnabled; }
	inline bool isSelected() const noexcept { return mSelected; }
	inline bool isActivated() const noexcept { return mActivated; }

	// Setters
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	inline void rect(const Rectangle& rect) noexcept { mRect = rect; }
	inline void text(const string& text) noexcept { mText = text; }
	inline void funcPtr(void(*funcPtr)(Button& ref)) noexcept { mFuncPtr = funcPtr; }

	inline void enable() noexcept { mEnabled = true; }
	inline void disable() noexcept { mEnabled = false; mSelected = false; mActivated = false; }

private:
	Rectangle mRect;
	string mText;
	void(*mFuncPtr)(Button& ref) = nullptr;
	bool mEnabled = true;
	bool mSelected = false;
	bool mActivated = false;
	
};

} // namespace sfz
#endif