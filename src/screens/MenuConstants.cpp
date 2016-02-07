#include "screens/MenuConstants.hpp"

#include <memory>

#include <sfz/Assert.hpp>

namespace s3 {

using namespace gui;
using std::shared_ptr;

float calcRestPadding(float numH1s, float numH2s, float numH3s, float numStandardPaddings) noexcept
{
	float restPadding =  SPACE_AVAILABLE - numH1s * (H1_HEIGHT + H1_PADDING)
	                                     - numH2s * (H2_HEIGHT + H2_PADDING)
	                                     - numH3s * (H3_HEIGHT + H3_PADDING)
	                                     - numStandardPaddings * STANDARD_PADDING;
	sfz_assert_debug(restPadding >= 0.0f);
	return restPadding;
}

void addTitle(System& system, BaseItem* titleItem) noexcept
{
	system.addSpacing(MENU_TOP_PADDING);
	system.addItem(shared_ptr<BaseItem>{titleItem}, TITLE_HEIGHT);
	system.addSpacing(TITLE_PADDING);
}


void addNavbar(System& system, BaseItem* navbarItem, float restPadding, float width) noexcept
{
	addNavbar(system, shared_ptr<BaseItem>{navbarItem}, restPadding, width);
}

void addNavbar(System& system, shared_ptr<BaseItem> navbarItem, float restPadding, float width) noexcept
{
	system.addSpacing(restPadding);
	system.addSpacing(NAVBAR_PADDING);
	system.addItem(navbarItem, vec2{width, NAVBAR_HEIGHT});
	system.addSpacing(MENU_BOTTOM_PADDING);
}

void addHeading1(System& system, BaseItem* item, float width, gl::HorizontalAlign hAlign) noexcept
{
	system.addItem(shared_ptr<BaseItem>{item}, vec2{width, H1_HEIGHT}, hAlign);
	system.addSpacing(H1_PADDING);
}

void addHeading2(System& system, BaseItem* item, float width, gl::HorizontalAlign hAlign) noexcept
{
	system.addItem(shared_ptr<BaseItem>{item}, vec2{width, H2_HEIGHT}, hAlign);
	system.addSpacing(H2_PADDING);
}

void addHeading3(System& system, BaseItem* item, float width, gl::HorizontalAlign hAlign) noexcept
{
	system.addItem(shared_ptr<BaseItem>{item}, vec2{width, H3_HEIGHT}, hAlign);
	system.addSpacing(H3_PADDING);
}

void addStandardPadding(System& system) noexcept
{
	system.addSpacing(STANDARD_PADDING);
}

void addHeading1(ScrollListContainer& scrollList, shared_ptr<BaseItem> item, float width, gl::HorizontalAlign hAlign) noexcept
{
	scrollList.addItem(item, vec2{width, H1_HEIGHT}, hAlign);
	scrollList.addSpacing(H1_PADDING);
}

void addHeading2(ScrollListContainer& scrollList, shared_ptr<BaseItem> item, float width, gl::HorizontalAlign hAlign) noexcept
{
	scrollList.addItem(item, vec2{width, H2_HEIGHT}, hAlign);
	scrollList.addSpacing(H2_PADDING);
}

void addHeading3(ScrollListContainer& scrollList, shared_ptr<BaseItem> item, float width, gl::HorizontalAlign hAlign) noexcept
{
	scrollList.addItem(item, vec2{width, H3_HEIGHT}, hAlign);
	scrollList.addSpacing(H3_PADDING);
}

void addStandardPadding(ScrollListContainer& scrollList) noexcept
{
	scrollList.addSpacing(STANDARD_PADDING);
}

} // namespace s3