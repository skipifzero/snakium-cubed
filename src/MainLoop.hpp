#pragma once
#ifndef S3_MAIN_LOOP_HPP
#define S3_MAIN_LOOP_HPP

#include <sfz/SDL.hpp>
#include <sfz/GL.hpp>
#include <functional> // std::function
#include <chrono> // std::chrono::high_resolution_clock

namespace s3 {

class MainLoop final {
public:
	MainLoop() = delete;
	MainLoop(const MainLoop&) = delete;
	MainLoop& operator= (const MainLoop&) = delete;

	MainLoop(std::function<bool(float)> update, std::function<bool(float)> render) noexcept;
	~MainLoop() noexcept;
	void run();

private:
	std::function<bool(float)> updateFunc;
	std::function<bool(float)> renderFunc;
};

} // namespace s3

#endif