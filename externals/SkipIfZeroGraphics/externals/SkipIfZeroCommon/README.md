# SkipIfZero Common
SkipIfZero Common is a collection of reusable c++11 code used in SkipIfZero projects. It contains math, algorithms, utilities, etc.

## Building
It is advisable to let CMake generate the wanted build solution in a directory called `build` inside the project folder as this folder is ignored by git. The CMake variable `SFZ_COMMON_BUILD_TESTS` determines whether the tests should be built or not.

### Requirements
- CMake 3.0 or newer for generating build solution.
- C++ compiler with good c++11 support: Clang, GCC or MSVC recommended.

### Linux and Mac OS X
 The following commands will generate the standard makefile build solution on Linux and Mac OS X:

	mkdir build

	cd build

	// No tests
	cmake ..

	// Builds tests
	cmake .. -DSFZ_COMMON_BUILD_TESTS:BOOLEAN=TRUE

You then have the following options:

	make      // Builds the project

	make test // Runs the tests (if built)

	make help // Shows available options

#### Xcode
On Mac OS X you might also want to generate an Xcode project, you can do this using the following commands:

	mkdir build

	cd build

	// No tests
	cmake .. -GXcode

	// Builds tests
	cmake .. -GXcode -DSFZ_COMMON_BUILD_TESTS:BOOLEAN=TRUE

### Windows
Windows (Visual Studio) support is somewhat experimental. It seems to mostly work, but some features (such as noexcept and constexpr) are disabled through a macro hack. Be careful.

	Create a directory called build in the root directory

	Open cmd inside the build directory

	cmake ..

## Linking

### CMake
To link SkipIfZero Common in your CMake project the easiest solution is to copy the wanted version into a subdirectory of your project. Then use the following command in your `CMakeLists.txt` to add the `${SFZ_COMMON_INCLUDE_DIRS}` and `${SFZ_COMMON_LIBRARIES}` variables:

	add_subdirectory(path_to_SkipIfZero_Common_dir)

Now you can link SkipIfZero Common with the following commands:

	include_directories(${SFZ_COMMON_INCLUDE_DIRS})

	target_link_libraries(some_executable ${SFZ_COMMON_LIBRARIES})

If you also want to build the tests (but really, why would you in this situation?) you also need to define `SFZ_COMMON_BUILD_TESTS` before adding the directory.


## License
Licensed under zlib, this means that you can basically use the code however you want as long as you give credit and don't claim you wrote it yourself. See LICENSE file for more info.