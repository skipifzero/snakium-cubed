#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <string>

#include <SDL.h> // SDL_GetBasePath();

#include "sfz/util/IO.hpp"

using std::string;

const string& basePath()
{
	static const string path{SDL_GetBasePath()}; // Leaks by design. :P
	return path;
}

const string& stupidFileName()
{
	static const string stupid{"jfioaejfaiojefaiojfeaojf.fajefaoejfa"};
	return stupid;
}

TEST_CASE("createFile() & fileExists() & deleteFile()", "[sfz::IO]")
{
	const string filePath = basePath() + stupidFileName();
	const char* fpath = filePath.c_str();

	bool resExists1 = sfz::fileExists(fpath);
	if (resExists1) {
		REQUIRE(sfz::deleteFile(fpath));
		resExists1 = sfz::fileExists(fpath);
	}
	REQUIRE(!resExists1);

	REQUIRE(sfz::createFile(fpath));
	REQUIRE(sfz::fileExists(fpath));
	REQUIRE(sfz::deleteFile(fpath));
	REQUIRE(!sfz::fileExists(fpath));
}

TEST_CASE("createDirectory() & directoryExists() & deleteDirectory()", "[sfz::IO]")
{
	const string dirPath = basePath() + stupidFileName();
	const char* dpath = dirPath.c_str();

	bool resExists1 = sfz::directoryExists(dpath);
	if (resExists1) {
		REQUIRE(sfz::deleteDirectory(dpath));
		resExists1 = sfz::directoryExists(dpath);
	}
	REQUIRE(!resExists1);

	REQUIRE(sfz::createDirectory(dpath));
	REQUIRE(sfz::directoryExists(dpath));
	REQUIRE(sfz::deleteDirectory(dpath));
	REQUIRE(!sfz::directoryExists(dpath));
}

TEST_CASE("writeBinaryFile() & readBinaryFile() & sizeofFile(), ", "[sfz::IO]")
{
	const string filePath = basePath() + stupidFileName();
	const char* fpath = filePath.c_str();
	const uint8_t data[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
	uint8_t data2[sizeof(data)];

	bool fileExists = sfz::fileExists(fpath);
	if (fileExists) {
		REQUIRE(sfz::deleteFile(fpath));
		fileExists = sfz::fileExists(fpath);
	}
	REQUIRE(!fileExists);

	REQUIRE(sfz::writeBinaryFile(fpath, data, sizeof(data)));
	REQUIRE(sfz::readBinaryFile(fpath, data2, sizeof(data2)) == 0);
	auto data3 = sfz::readBinaryFile(fpath);
	REQUIRE(data3.size() == sizeof(data));
	REQUIRE(sizeof(data) == (size_t)sfz::sizeofFile(fpath));

	for (size_t i = 0; i < sizeof(data); ++i) {
		REQUIRE(data[i] == data2[i]);
		REQUIRE(data[i] == data3[i]);
	}

	REQUIRE(sfz::deleteFile(fpath));
	REQUIRE(!sfz::fileExists(fpath));
}

TEST_CASE("readTextFile()", "[sfz::IO]")
{
	const string filePath = basePath() + stupidFileName();
	const char* fpath = filePath.c_str();
	const char* strToWrite = "Hello World!\nHello World 2!\nHello World 3!";

	bool fileExists = sfz::fileExists(fpath);
	if (fileExists) {
		REQUIRE(sfz::deleteFile(fpath));
		fileExists = sfz::fileExists(fpath);
	}
	REQUIRE(!fileExists);

	REQUIRE(sfz::writeBinaryFile(fpath, (const uint8_t*)strToWrite, sizeof(strToWrite)));
	REQUIRE(sfz::fileExists(fpath));

	string fileStr = sfz::readTextFile(fpath);
	REQUIRE(fileStr.size() >= sizeof(strToWrite));
	for (size_t i = 0; i < sizeof(strToWrite); ++i) {
		REQUIRE(fileStr[i] == strToWrite[i]);
	}

	REQUIRE(sfz::deleteFile(fpath));
}