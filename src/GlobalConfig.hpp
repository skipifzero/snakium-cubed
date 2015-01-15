#pragma once
#ifndef S3_CONFIG_HPP
#define S3_CONFIG_HPP

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/filesystem.hpp>

#include <iostream>
#include <string>
#include <exception> // std::terminate()
#include <cstdint> // int16_t

#include "gamelogic/ModelConfig.hpp"

namespace s3 {

struct GlobalConfig {
	ModelConfig mModelConfig;


	boost::property_tree::ptree mPtree;
	void load() noexcept;
	void save() noexcept;
};

} // namespace s3

#endif