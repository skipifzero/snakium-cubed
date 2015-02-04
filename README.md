# snakium³

The much anticipated sequel to snakium, the summer hit of 2013. snakium³ (or snakium cubed) is making the move to 3D! The goal is to keep the tight and simple gameplay of snake while taking it one step beyond.

## Building

It is advisable to let CMake generate the wanted build solution in a directory called build inside the project root directory as this directory is ignored by git.

### Dependencies

- __CMake__ (v3.0 or newer) for generating build solution
- __Boost__ (v1.57)
- __SDL2__
- __SDL2 image__
- __GLEW__ (packaged)
- __OpenGL 3.3__
- Mac OS X: __Xcode 6__ or newer
- Windows: __Visual Studio 2013__ or newer

### Mac OS X

#### Installing dependencies
- Install Homebrew
- CMake: `brew install cmake`
- Boost: `brew install boost`
- SDL2: `brew install sdl2`
- SDL2_image: `brew install sdl2_image`
 
#### Generating Xcode project and building
Open a terminal in the root directory of the project. Then run the following commands:

	mkdir build
	cd build
	cmake .. -GXcode

Note that the assets are only copied when CMakeLists.txt is invoked, i.e. not when simply building. If assets are changed they need to be manually copied into the `build` directory (three places, in build root, in `Debug` and in `Release` directories). Alternatively simply delete the `build` directory completely and regenerate the Xcode project.

#### (alt) Building with make
Open a terminal in the root directory of the project. Then run the following commands:

	mkdir build
	cd build
	cmake ..

You can then build with `make install`. The generated binary will be placed in the `bin` directory inside the `build` directory.

### Windows

#### Installing dependencies

##### CMake
Install latest version of CMake and add it to your `Path` variable. Warning: Backup your `Path` before installing. The installer is buggy and has on occasion deleted all the contents of my `Path` variable.

##### Boost
Download and install the pre-built Boost binaries (as of writing you want the file called `boost_1_57_0-msvc-12.0-64.exe`). Then you need create an environment variable called `BOOST_ROOT` pointing to the root of the Boost installation. You also need to create an environment variable called `BOOST_LIBRARYDIR` which points to the directory containing the compiled libraries (`.lib` and `.dll`), this will by default be a directory called `lib64-msvc-12.0` inside the Boost root directory.

##### SDL2
Download and install the SDL2 development libraries for Visual C++ from the official website. Then create an environment variable called `SDL2` and point it to the root of the SDL2 installation.

##### SDL2 image
Download and install the SDL2_image development libraries for Visual C++ from the official website. Then add an additional entry in the previously created `SDL2` environment variable pointing to the root of the SDL2 image installation.

##### OpenGL
Up to date OpenGL drivers are by default not shipped with Windows. As long as you have installed the latest drivers for your graphics card you should be okay.

#### Generating Visual Studio solution
Create a directory called `build` inside the project root directory and then open `cmd` inside this `build` directory. Run the following command:

	cmake .. -G "Visual Studio 12 2013 Win64"

Note that the assets are only copied when CMakeLists.txt is invoked, i.e. not when simply building. If assets are changed they need to be manually copied into the `build` directory (three places, in build root, in `Debug` and in `Release` directories). Alternatively simply delete the `build` directory completely and regenerate the Visual Studio solution.

## License
TBD