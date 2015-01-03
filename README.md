# snakium³

The much anticipated sequel to snakium, the summer hit of 2013. snakium³ (or snakium cubed) is making the move to 3D! The goal is to keep the tight and simple gameplay of snake while taking it one step beyond.

## Building

### Dependencies

- CMake (for generating build solution)
- SDL2
- SDL2_image
- GLEW (packaged)

### Mac OS X

#### Dependencies

If homebrew is installed the dependencies can be installed with the following commands:

	brew install sdl2

	brew install sdl2_image

#### Make

Open a terminal in the snakium-cubed root directory.

	mdkir build

	cd build

	cmake ..

This generates the makefile. You can the build and run with:

	make install // builds

	./bin/snakium-cubed // runs

The generated binary will be placed in the `bin` directory inside the `build` directory.

#### Xcode

Open a terminal in the snakium-cubed root directory.

	mkdir build

	cd build

	cmake .. -GXcode

The assets folder needs to be copied to wherever Xcode places the compiled binary.

### Windows (Visual Studio)

#### Dependencies
To build you need at least Visual Studio 2015 (preview) or newer. The first step is to download the SDL2 and SDL2_image development builds for Windows. Then create a path variable named `SDL2` that contains the path to both the SDL2 and SDL2_image builds you just downloaded.

#### Generating Visual Studio solution
Create a directory called `build` in the snakium-cubed root directory. Open cmd inside this directory and enter `cmake ..`, this should generate a Visual Studio solution.

#### Copy some files
To actually run the compiled `.exe` it needs to be in the same directory as the `assets` folder and the required runtime `.dll`s (SDL2.dll, SDL2_image.dll and all dlls these may depend on, all of these are part of the runtime binaries available for download). If you're just going to run through Visual Studio it should be enough to copy the assets directory and the .dlls into the build directory.