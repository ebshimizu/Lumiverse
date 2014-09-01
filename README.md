# About
Lumiverse is a framework that provides intuitive control over lighting devices.
Lumiverse can support any interface if there's a driver for it, and can output to
multiple interfaces at the same time.

## Build instructions
Note that the latest stable version is [v1.1](https://github.com/ebshimizu/Lumiverse/tree/v1.1)

* Install CMake (>=2.8 suggested, >=2.6 required)
* Install build dependencies for specific modules:
    * DMX USB PRO MK2 driver: [ftd2xx](http://www.ftdichip.com/Drivers/D2XX.htm)
    * Bindings for other languages: [SWIG](http://www.swig.org/) (>= 3.0.0)
    	* Python: Python
    	* C#: .NET (any version that works with SWIG)
    * Arnold Renderer:
    	* zlib (included in source/LumiverseCore/lib)
    	* libpng (included in source/LumiverseCore/lib)
* Make a folder to contain all of your build files
* Inside of the folder you made, run `cmake ../source`
* Build

There are a few options for including or excluding features in the CMake build settings.
See the CMakeLists file in LumiverseCore for details or take a look at the options in the
CMake GUI.

### Notes for building with the Arnold Renderer
This feature is currently experimental. You will need a license from [SolidAngle](https://www.solidangle.com/) to render images
without a watermark.

## Documentation
Documentation for the project is generated from source with [Doxygen](http://www.stack.nl/~dimitri/doxygen/)
You may choose to not build documentation by setting the `LUMIVERSE_DOCS`
cmake variable accordingly during project generation.
Documentation will be output to  `Lumiverse/docs` if generated. 


## Platform and Interface Support
Lumiverse supports Windows, OS X, and iOS. Linux has some small issues at the
moment preventing the project from compiling. I had some issues with the
FTD2XX driver on OS X, so your mileage with OS X may vary.

At the moment, Lumiverse has drivers for the ENTTEC USB DMX PRO Mk 2
and for KiNet.

This project was supported in part by funding from the [Carnegie Mellon
University Frank-Ratchye Fund for Art @ the Frontier](http://studioforcreativeinquiry.org/)