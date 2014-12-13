#include "MainLoop.hpp"

namespace s3 {

namespace {

float calculateDelta() noexcept
{
	static std::chrono::high_resolution_clock::time_point previousTime, currentTime;

	previousTime = currentTime;
	currentTime = std::chrono::high_resolution_clock::now();

	using FloatSecondDuration = std::chrono::duration<float>;
	return std::chrono::duration_cast<FloatSecondDuration>(currentTime - previousTime).count();
}

} // anonymous namespace

MainLoop::MainLoop(std::function<bool(float)> update, std::function<bool(float)> render) noexcept
:
	updateFunc(update),
	renderFunc(render)
{
	// Initialization done.
}

MainLoop::~MainLoop() noexcept
{

}

void MainLoop::run()
{
	float delta = calculateDelta(); // Call calculateDelta() here to initialize counting.
	
	while (true) {
		delta = calculateDelta();

		if(updateFunc(delta)) return;
		if(renderFunc(delta)) return;
	}
}

} // namespace s3