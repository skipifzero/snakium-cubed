#pragma once
#ifndef SFZ_UTIL_IO_HPP
#define SFZ_UTIL_IO_HPP

#include <string>

namespace sfz {

/** @brief Returns path to MyDocuments on Windows, user root (~) on Unix. */
const std::string& myDocumentsPath() noexcept;

/** @brief Returns path to where game folders with saves should be placed. */
const std::string& gameBaseFolderPath() noexcept;

/** @brief Returns whether a given directory exists or not. */
bool directoryExists(const char* path) noexcept;

/** @brief Attempts to create a directory and returns whether successful or not. */
bool createDirectory(const char* path) noexcept;

/** @brief Attempts to copy file from source to destination. */ 
bool copyFile(const char* srcPath, const char* dstPath) noexcept;

} // namespace sfz

#endif