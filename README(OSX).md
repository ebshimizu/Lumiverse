# Build instructions (OSX)

## Prerequisites
You will need CMake (version 3.0 and above) at minimum to build Lumiverse.
You will also need Doxygen for generating the documentation files for
Lumiverse, however generating the documentation is optional. Additional
prerequisites for various features are listed below.

* OLA Driver: OLA (highly recommended for Linux / OS X users).
* Bindings for other languages: SWIG (>= 3.0.0)
* Python: 2.x and 3.x both work
* C#: .NET (any version that works with SWIG)
* DMX USB PRO MK2 driver: ftd2xx
* Arnold Renderer (Experimental)
* Arnold development library
* zlib (included in source/LumiverseCore/lib)
* libpng (included in source/LumiverseCore/lib)

For OSX, you may install some of the dependencies via [macports](https://www.macports.org/) or [homebrew](http://brew.sh/). Note that you will need to add the installation directory of the tool to your `DYLD_LIBRARY_PATH` so the linker uses them. If you are using macports and wish to create python bindings, you will need the `swig-python` port in addition to `swig`.

## Build Steps
After installing necessary prerequisites, use CMake to generate the build
files for your platform of choice.

Make a folder to contain all of your build files. Inside of the folder you
made, run cmake [path to Lumiverse]/source or use the CMake GUI Build.

Note: If you are building bindings for Python with SWIG, typically Python
does not come with a debug library and the build will fail in the Debug
configuration.

There are a many options for including or excluding Lumiverse features in
the CMake build settings. See the CMakeLists file in the source directory
for details or take a look at the options in the CMake GUI.
