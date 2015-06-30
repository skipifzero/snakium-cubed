#pragma once
#ifndef SFZ_ASSERT_HPP
#define SFZ_ASSERT_HPP

#include <string>
#include <iostream> // std::cerr
#include <exception> // std::terminate()
#include <cassert>

/**
 * @brief Stops program or opens debugger if condition is false.
 * To be used often to catch bugs during debugging progress. Should normally only be enabled in
 * debug builds. Disabled by defining SFZ_NO_DEBUG, but also by defining SFZ_NO_ASSERTIONS.
 */
#define sfz_assert_debug(condition) sfz_assert_debug_impl(condition)

/**
 * @brief Stops program or opens debugger if condition is false and displays message.
 * To be used often to catch bugs during debugging progress. Should normally only be enabled in
 * debug builds. Disabled by defining SFZ_NO_DEBUG, but also by defining SFZ_NO_ASSERTIONS.
 */
#define sfz_assert_debug_m(condition, message) sfz_assert_debug_m_impl(condition, message)

/**
 * @brief Stops program or opens debugger if condition is false.
 * To be used for more serious things that you want to catch quickly even in a release build.
 * Should normally always be enabled, but can be disabled by defining SFZ_NO_ASSERTIONS.
 */
#define sfz_assert_release(condition) sfz_assert_release_impl(condition)

/**
 * @brief Stops program or opens debugger if condition is false and displays message.
 * To be used for more serious things that you want to catch quickly even in a release build.
 * Should normally always be enabled, but can be disabled by defining SFZ_NO_ASSERTIONS.
 */
#define sfz_assert_release_m(condition, message) sfz_assert_release_m_impl(condition, message)

/**
 * @brief Stops execution of program and displays message.
 * This is meant to be used for errors that can't be recovered from and should crash the program.
 * Should always be enabled, but can be disabled (not recommended) by defining SFZ_DISABLE_ERRORS.
 */
#define sfz_error(message) sfz_error_impl(message)

#include "sfz/Assert.inl"
#endif