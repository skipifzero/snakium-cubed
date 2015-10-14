# SkipIfZero Common
SkipIfZero Common is a collection of code intended for games and graphical application development, it is the basis for all SkipIfZero C++ OpenGL projects. It contains a linear algebra math library, a geometry library with intersection tests, OpenGL utilities, SDL wrappers (including a gameloop wrapper which handles various forms of input), other utilities (such as an ini parser) and more.

SkipIfZero Common handles common dependencies in C++ OpenGL applications. It links to SDL and OpenGL and exposes these libraries through CMake, no need to link them individually. It also comes bundled with a few libraries (among them GLEW) in order to minimize external dependency requirements.

## Building
It is advisable to let CMake generate the wanted build solution in a directory called `build` inside the project folder as this folder is ignored by git. The CMake variable `SFZ_COMMON_BUILD_TESTS` determines whether the tests should be built or not.

### Requirements
- __SDL2__
- __OpenGL 3.3__ or newer
- Windows: __Visual Studio 2015__ or newer
- Mac OS X: __Xcode 7__ or newer

### Windows

#### Installing dependencies

##### SDL2
Download and install the SDL2 development libraries for Visual C++ from the official website. Then create an environment variable called `SDL2` and point it to the root of the SDL2 installation.

##### OpenGL
Up to date OpenGL drivers are by default not shipped with Windows. As long as you have installed the latest drivers for your graphics card you should be okay.

#### Generating Visual Studio solution

Use the following commands to generate a Visual Studio solution:

	Create a directory called build in the root directory

	Open cmd inside the build directory

	// With tests
	cmake .. -G "Visual Studio 14 2015 Win64" -DSFZ_COMMON_BUILD_TESTS:BOOLEAN=TRUE

	// Without tests
	cmake .. -G "Visual Studio 14 2015 Win64"

### Mac OS X

#### Installing dependencies

##### SDL2

Either visit the offical [website](https://www.libsdl.org/) and follow the installation instructions there or install using homebrew with the following command: `brew install sdl2`.

#### Generating Xcode project

The following commands will generate an Xcode project:

	mkdir build

	cd build

	// With tests
	cmake .. -GXcode -DSFZ_COMMON_BUILD_TESTS:BOOLEAN=TRUE

	// Without tests
	cmake .. -GXcode

### Unix (including Linux & Mac OS X)

The following commands will generate the standard makefile build solution:

	mkdir build

	cd build

	// With tests
	cmake .. -DSFZ_COMMON_BUILD_TESTS:BOOLEAN=TRUE

	// Without tests
	cmake ..

You then have the following options:

	make      // Builds the project

	make test // Runs the tests (if built)

	make help // Shows available options


## Linking

### CMake
To link SkipIfZero Common in your CMake project the easiest solution is to copy the wanted version into a subdirectory of your project. Then use the following command in your `CMakeLists.txt` to add the `${SFZ_COMMON_INCLUDE_DIRS}` and `${SFZ_COMMON_LIBRARIES}` variables:

	add_subdirectory(path_to_SkipIfZero_Common_dir)

Now you can link SkipIfZero Common with the following commands:

	include_directories(${SFZ_COMMON_INCLUDE_DIRS})

	target_link_libraries(some_executable ${SFZ_COMMON_LIBRARIES})

If you also want to build the tests (but really, why would you in this situation?) you also need to define `SFZ_COMMON_BUILD_TESTS` before adding the directory.

In addition to SkipIfZero Common, the `${SFZ_COMMON_INCLUDE_DIRS}` and `${SFZ_COMMON_LIBRARIES` variables expose SDL2, OpenGL and GLEW to the including project.

## License
Licensed under zlib, this means that you can basically use the code however you want as long as you give credit and don't claim you wrote it yourself. See LICENSE file for more info.

Libraries used by SkipIfZero Common fall under various licenses, see their respective LICENSE file for more info.