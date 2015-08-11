#pragma once
#ifndef SFZ_INI_PARSER_HPP
#define SFZ_INI_PARSER_HPP

#include <cstdint>
#include <string>
#include <unordered_map>

namespace sfz {

using std::int32_t;
using std::string;
using std::unordered_map;

// IniParser class
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

class IniParser final {
public:

	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	IniParser() = delete;
	IniParser(const IniParser&) = delete;
	IniParser& operator= (const IniParser&) = delete;
	~IniParser() noexcept = default;

	IniParser(const string& path) noexcept;
	
	// Loading and saving to file functions
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	bool load() noexcept;
	bool save() noexcept;

	// Info about a specific item
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	bool itemExists(const string& section, const string& key) const noexcept;
	bool itemIsBool(const string& section, const string& key) const noexcept;
	bool itemIsInt(const string& section, const string& key) const noexcept;
	bool itemIsFloat(const string& section, const string& key) const noexcept;

	// Getters
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	string getString(const string& section, const string& key,
	                 const string& defaultValue = "") const noexcept;

	bool getBool(const string& section, const string& key,
	             bool defaultValue = false) const noexcept;

	int32_t getInt(const string& section, const string& key,
	               int32_t defaultValue = 0) const noexcept;

	float getFloat(const string& section, const string& key,
	               float defaultValue = 0.0f) const noexcept;

	// Setters
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	void setString(const string& section, const string& key, const string& value) noexcept;
	void setBool(const string& section, const string& key, bool value) noexcept;
	void setInt(const string& section, const string& key, int32_t value) noexcept;
	void setFloat(const string& section, const string& key, float value) noexcept;

private:
	string mPath;
	unordered_map<string,unordered_map<string,string>> mIniTree;
};

} // namespace sfz
#endif