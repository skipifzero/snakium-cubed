# SkipIfZero Graphics

SkipIfZero Graphics is a collection of variuous graphics related utilities and wrappers. One of its main purposes is to handle common dependencies in C++ OpenGL applications. It links to SDL and OpenGL and also exposes these libraries, no need to link them individually. It also comes bundled with SkipIfZero Common and GLEW, which are also exposed.

## Building

It is advisable to let CMake generate the wanted build solution in a directory called build inside the project root directory as this directory is ignored by git.

### Dependencies

- __SDL2__
- __SkipIfZero Common__ (packaged)
- __STB libraries__ (packaged)
- __GLEW__ (packaged)
- __OpenGL 3.3__
- Mac OS X: __Xcode 6__ or newer
- Windows: __Visual Studio 2015__ or newer

### Linux and Mac OS X

 The following commands will generate the standard makefile build solution on Linux and Mac OS X:

	mkdir build

	cd build

	// No tests
	cmake ..

	// Builds tests
	cmake .. -DSFZ_GRAPHICS_BUILD_TESTS:BOOLEAN=TRUE

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
	cmake .. -GXcode -DSFZ_GRAPHICS_BUILD_TESTS:BOOLEAN=TRUE

### Windows

#### Installing dependencies

##### CMake
Install latest version of CMake and add it to your `Path` variable. Warning: Backup your `Path` before installing. The installer is buggy and has on occasion deleted all the contents of my `Path` variable.

##### SDL2
Download and install the SDL2 development libraries for Visual C++ from the official website. Then create an environment variable called `SDL2` and point it to the root of the SDL2 installation.

##### OpenGL
Up to date OpenGL drivers are by default not shipped with Windows. As long as you have installed the latest drivers for your graphics card you should be okay.

#### Generating Visual Studio solution
Create a directory called `build` inside the project root directory and then open `cmd` inside this `build` directory. Run the following command:

	cmake .. -G "Visual Studio 14 2015 Win64"
    
    // With tests
    cmake .. -G "Visual Studio 14 2015 Win64" -DSFZ_GRAPHICS_BUILD_TESTS:BOOLEAN=TRUE
## Linking

### CMake
To link SkipIfZero Graphics in your CMake project the easiest solution is to copy the wanted version into a subdirectory of your project. Then use the following command in your `CMakeLists.txt` to add the `${SFZ_GRAPHICS_INCLUDE_DIRS}` and `${SFZ_GRAPHICS_LIBRARIES}` variables:

	add_subdirectory(path_to_SkipIfZero_Graphics_dir)

Now you can link SkipIfZero Graphics and related libraries with the following commands:

	include_directories(${SFZ_GRAPHICS_INCLUDE_DIRS})

	target_link_libraries(some_executable ${SFZ_GRAPHICS_LIBRARIES})

This will also link and expose SDL2, OpenGL, GLEW and SkipIfZero Common to the including project.

If you also want to build the tests (but really, why would you in this situation?) you also need to define `SFZ_GRAPHICS_BUILD_TESTS` before adding the directory.

## License
License to be decided, but will likely be zlib. Contact me if you want to use the library and I will fix it.
