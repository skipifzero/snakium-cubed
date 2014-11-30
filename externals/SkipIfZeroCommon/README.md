# SkipIfZero Common
SkipIfZero Common is a collection of reusable c++11 code used in SkipIfZero projects. It contains math, algorithms, utilities, etc.

## Building
It is advisable to let CMake generate the wanted build solution in a directory called `build` inside the project folder as this folder is ignored by git.

### Requirements
- CMake 3.0 or newer for generating build solution.
- C++ compiler with good c++11 support: Clang, GCC or MSVC recommended.

### Linux and Mac OS X
 The following commands will generate the standard makefile build solution on Linux and Mac OS X:

	mkdir build

	cd build

	cmake ..

You then have the following options:

	
	make      // Builds the project
	
	make test // Runs the tests

	make help // Shows available options

#### Xcode
On Mac OS X you might also want to generate an Xcode project, you can do this using the following commands:

	mkdir build

	cd build

	cmake .. -GXcode

### Windows
CMake will generate a Visual Studio solution by default, but will not currently build. The reason for this is that Visual Studio doesn't support noexcept yet, which is heavily used in this project.

## Linking

### CMake
To link SkipIfZero Common in your CMake project the easiest solution is to copy the wanted version into a subdirectory of your project. Then use the following command in your `CMakeLists.txt` to add the `${SFZ_COMMON_INCLUDE_DIRS}` and `${SFZ_COMMON_LIBRARIES}` variables:

	add_subdirectory(path_to_SkipIfZero_Common_dir)

Now you can link SkipIfZero Common with the following commands:

	include_directories(${SFZ_COMMON_INCLUDE_DIRS})

	target_link_libraries(some_executable ${SFZ_COMMON_LIBRARIES})


## License
Licensed under zlib, this means that you can basically use the code however you want as long as you give credit and don't claim you wrote it yourself. See LICENSE file for more info.