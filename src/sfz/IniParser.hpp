#pragma once
#ifndef SFZ_INI_PARSER_HPP
#define SFZ_INI_PARSER_HPP

#include <cstdint>
#include <limits>
#include <map>
#include <string>

namespace sfz {

using std::int32_t;
using std::numeric_limits;
using std::string;
using std::map;

// IniParser class
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

class IniParser final {
public:

	// Constructors & destructors
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	IniParser() = default;
	IniParser(const IniParser&) = default;
	IniParser& operator= (const IniParser&) = default;
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

	// Sanitizers
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

	string sanitizeString(const string& section, const string& key,
	                      const string& defaultValue = "") noexcept;

	bool sanitizeBool(const string& section, const string& key,
	                  bool defaultValue = false) noexcept;

	int32_t sanitizeInt(const string& section, const string& key,
	                    int32_t defaultValue = 0,
	                    int32_t minValue = numeric_limits<int32_t>::min(),
	                    int32_t maxValue = numeric_limits<int32_t>::max()) noexcept;

	float sanitizeFloat(const string& section, const string& key,
	                    float defaultValue = 0.0f,
	                    float minValue = numeric_limits<float>::min(),
	                    float maxValue = numeric_limits<float>::max()) noexcept;

private:
	string mPath;
	map<string,map<string,string>> mIniTree;
};

} // namespace sfz
#endif