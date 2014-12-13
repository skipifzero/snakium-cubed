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

void checkGLErrorsMessage(const std::string& msg) noexcept
{
	if (gl::checkAllGLErrors()) {
		std::cerr << msg << std::endl;
	}
}

} // anonymous namespace

MainLoop::MainLoop(std::function<bool(float)> update,
                   std::function<bool(sdl::Window&,float)> render) noexcept
:
	sdlSession{{sdl::InitFlags::EVERYTHING}, {sdl::ImgInitFlags::PNG}},
	window{"snakium³", 400, 400,
	     {sdl::WindowFlags::OPENGL, sdl::WindowFlags::RESIZABLE, sdl::WindowFlags::ALLOW_HIGHDPI}},
	glContext{window.mPtr, 3, 3, gl::GLContextProfile::CORE},
	updateFunc{update},
	renderFunc{render}
{
	// Initializes GLEW, must happen after GL context is created.
	glewExperimental = GL_TRUE;
	GLenum glewError = glewInit();
	if (glewError != GLEW_OK) {
		std::cerr << "GLEW initialization failure:\n" << glewGetErrorString(glewError) << std::endl;
		std::terminate();
	}
	checkGLErrorsMessage("^^^ Above errors caused by glewInit().");
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
		if(renderFunc(window, delta)) return;
	}
}

} // namespace s3