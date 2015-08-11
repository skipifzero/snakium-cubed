#include "IniParser.hpp"

#include <cctype> // std::tolower()
#include <cstdio>
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
	std::FILE* file = std::fopen(path.c_str(), "r");
	if (file == NULL) return false;

	// TODO: Implement

	return false;
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
	vector<string> lines;
	if (!readFile(mPath, lines)) return false;

	// TODO: Implement

	return false;
}

bool IniParser::save() noexcept
{
	// TODO: Implement
	return false;
}

// IniParser: Getters
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

bool IniParser::itemExists(const string& section, const string& key) const noexcept
{
	auto sectionItr = mIniTree.find(section);
	if (sectionItr == mIniTree.end()) return false;
	auto keyItr = sectionItr->second.find(key);
	if (keyItr == sectionItr->second.end()) return false;
	return true;
}
	
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

} // namespace sfz