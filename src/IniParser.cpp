#include "IniParser.hpp"

#include "sfz/Assert.hpp"

#include <cctype> // std::tolower()
#include <fstream>
#include <vector>

namespace sfz {

using std::vector;

// Static functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

static void toLowerCase(string& str) noexcept
{
	for (size_t i = 0; i < str.size(); ++i) {
		str[i] = std::tolower(str[i]);
	}
}

static bool readFile(const string& path, vector<string>& lines) noexcept
{
	std::ifstream file{path};
	if (!file.is_open()) return false;
	std::string str;

	while (std::getline(file, str)) {
		if (str == "") continue;
		if (str[0] == ';') continue; // Remove comments
		lines.push_back(str);
	}
	return true;
}

// IniParser: Constructors & destructors
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

IniParser::IniParser(const string& path) noexcept
:
	mPath{path}
{ }
	
// IniParser: Loading and saving to file functions
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

bool IniParser::load() noexcept
{
	mIniTree.clear();

	vector<string> lines;
	if (!readFile(mPath, lines)) return false;

	string currentSection = "";
	for (size_t i = 0; i < lines.size(); ++i) {

		// Check if new section
		size_t sectLoc = lines[i].find_first_of("[");
		if (sectLoc != lines[i].npos) {
			size_t endLoc = lines[i].find_first_of("]");
			if (endLoc == lines[i].npos) return false;
			if (sectLoc >= endLoc) return false;
			currentSection = lines[i].substr(sectLoc+1, endLoc-sectLoc-1);
			mIniTree.emplace(std::make_pair(currentSection, unordered_map<string, string>{}));
			continue;
		}

		// Add item to tree
		size_t delimLoc = lines[i].find_first_of("=");
		if (delimLoc == lines[i].npos) return false;
		if (delimLoc == 0) return false;
		if (delimLoc+1 == lines[i].size()) return false;
		mIniTree[currentSection][lines[i].substr(0, delimLoc)] =
		     lines[i].substr(delimLoc+1, lines[i].size()-delimLoc+1);
	}

	return true;
}

bool IniParser::save() noexcept
{
	// TODO: Implement
	return false;
}

// IniParser: Info about a specific item
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

bool IniParser::itemExists(const string& section, const string& key) const noexcept
{
	auto sectionItr = mIniTree.find(section);
	if (sectionItr == mIniTree.end()) return false;
	auto keyItr = sectionItr->second.find(key);
	if (keyItr == sectionItr->second.end()) return false;
	return true;
}

bool IniParser::itemIsBool(const string& section, const string& key) const noexcept
{
	auto sectionItr = mIniTree.find(section);
	if (sectionItr == mIniTree.end()) return false;
	auto keyItr = sectionItr->second.find(key);
	if (keyItr == sectionItr->second.end()) return false;

	string val = keyItr->second;
	toLowerCase(val);
	return val == "true" || val == "false"
	    || val == "on" || val == "off"
	    || val == "1" || val == "0";
}

bool IniParser::itemIsInt(const string& section, const string& key) const noexcept
{
	auto sectionItr = mIniTree.find(section);
	if (sectionItr == mIniTree.end()) return false;
	auto keyItr = sectionItr->second.find(key);
	if (keyItr == sectionItr->second.end()) return false;

	string val = keyItr->second;
	try {
		std::stoi(val);
		return true;
	} catch (...) {
		return false;
	}
}

bool IniParser::itemIsFloat(const string& section, const string& key) const noexcept
{
	auto sectionItr = mIniTree.find(section);
	if (sectionItr == mIniTree.end()) return false;
	auto keyItr = sectionItr->second.find(key);
	if (keyItr == sectionItr->second.end()) return false;

	string val = keyItr->second;
	try {
		std::stof(val);
		return true;
	} catch (...) {
		return false;
	}
}

// IniParser: Getters
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

string IniParser::getString(const string& section, const string& key,
                            const string& defaultValue) const noexcept
{
	auto sectionItr = mIniTree.find(section);
	if (sectionItr == mIniTree.end()) return defaultValue;
	auto keyItr = sectionItr->second.find(key);
	if (keyItr == sectionItr->second.end()) return defaultValue;
	return keyItr->second;
}

bool IniParser::getBool(const string& section, const string& key,
                        bool defaultValue) const noexcept
{
	string val = this->getString(section, key, defaultValue ? "true" : "false");
	toLowerCase(val);
	if (val == "true") return true;
	if (val == "on") return true;
	if (val == "1") return true;
	return false;

}

int32_t IniParser::getInt(const string& section, const string& key,
                          int32_t defaultValue) const noexcept
{
	string val = this->getString(section, key, std::to_string(defaultValue));
	int32_t value;
	try {
		value = std::stoi(val);
	} catch (...) {
		value = defaultValue;
	}
	return value;
}

float IniParser::getFloat(const string& section, const string& key,
                          float defaultValue) const noexcept
{
	string val = this->getString(section, key, std::to_string(defaultValue));
	float value;
	try {
		value = std::stof(val);
	}
	catch (...) {
		value = defaultValue;
	}
	return value;
}

// IniParser: Setters
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void IniParser::setString(const string& section, const string& key, const string& value) noexcept
{
	mIniTree[section][key] = value;
}

void IniParser::setBool(const string& section, const string& key, bool value) noexcept
{
	this->setString(section, key, value ? "true" : "false");
}

void IniParser::setInt(const string& section, const string& key, int32_t value) noexcept
{
	this->setString(section, key, std::to_string(value));
}

void IniParser::setFloat(const string& section, const string& key, float value) noexcept
{
	this->setString(section, key, std::to_string(value));
}

// Sanitizers
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void IniParser::sanitizeString(const string& section, const string& key,
                               const string& defaultValue) noexcept
{
	if (!this->itemExists(section, key)) this->setString(section, key, defaultValue);
}

void IniParser::sanitizeBool(const string& section, const string& key,
                             bool defaultValue) noexcept
{
	if (!this->itemIsBool(section, key)) this->setBool(section, key, defaultValue);
}

void IniParser::sanitizeInt(const string& section, const string& key,
                            int32_t defaultValue, int32_t minValue, int32_t maxValue) noexcept
{
	sfz_assert_debug(minValue <= maxValue);
	if (!this->itemIsInt(section, key)) {
		this->setInt(section, key, defaultValue);
		return;
	}
	int32_t value = this->getInt(section, key);
	if (value > maxValue) this->setInt(section, key, maxValue);
	else if (value < minValue) this->setInt(section, key, minValue);
}

void IniParser::sanitizeFloat(const string& section, const string& key,
                              float defaultValue, float minValue, float maxValue) noexcept
{
	sfz_assert_debug(minValue <= maxValue);
	if (!this->itemIsFloat(section, key)) {
		this->setFloat(section, key, defaultValue);
		return;
	}
	float value = this->getFloat(section, key);
	if (value > maxValue) this->setFloat(section, key, maxValue);
	else if (value < minValue) this->setFloat(section, key, minValue);
}

} // namespace sfz