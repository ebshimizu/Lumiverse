# About
Lumiverse is a framework that provides intuitive control over lighting devices.
Lumiverse can support any interface if there's a driver for it, and can output to
multiple interfaces at the same time.

## Build instructions
Please note that the current master version is not stable.
The latest stable version is [v1.1](https://github.com/ebshimizu/Lumiverse/tree/v1.1)

* Install CMake (>=2.8 suggested, >=2.6 required)
* Install build dependencies for specific modules:
	* If you are running Lumiverse on OS X/Linux, [OLA](http://www.openlighting.org/) is highly recommended.
    * DMX USB PRO MK2 driver: [ftd2xx](http://www.ftdichip.com/Drivers/D2XX.htm)
    * Bindings for other languages: [SWIG](http://www.swig.org/) (>= 3.0.0)
    	* Python: 2.x and 3.x both work
    	* C#: .NET (any version that works with SWIG)
    * Arnold Renderer (Experimental):
    	* zlib (included in source/LumiverseCore/lib)
    	* libpng (included in source/LumiverseCore/lib)
    * [Doxygen](http://www.doxygen.org/) (if you wish to generate the documentation files)
* Make a folder to contain all of your build files
* Inside of the folder you made, run `cmake ../source` or use the CMake GUI to configure the build
* Build

There are a many options for including or excluding Lumiverse features in the CMake build settings.
See the CMakeLists file in LumiverseCore for details or take a look at the options in the
CMake GUI.

### Notes for building with the Arnold Renderer
This feature is currently experimental. You will need a license from [SolidAngle](https://www.solidangle.com/) to render images
without a watermark and must specify the path to the renderer library. Documentation for this feature may not be available due to
the experimental nature of the feature.

## Documentation
Documentation for the project is generated from source with [Doxygen](http://www.stack.nl/~dimitri/doxygen/)
You may choose to not build documentation by setting the `LUMIVERSE_DOCS`
cmake variable accordingly during project generation.
Documentation will be output to  `Lumiverse/docs` if generated. 


## Platform and Interface Support
Lumiverse supports Windows and Linux. OS X support is being tested.

At the moment, Lumiverse has drivers for the OLA framework (Linux/OS X only), ENTTEC USB DMX PRO Mk 2, KiNet, and Art-Net.
It is highly recommended to use the OLA framework if you are on Linux or OS X. Windows users may
use the built in Lumiverse drivers for communication with particular lighting protocols. The ENTTEC driver
may need additional configuration to work on non-Windows platforms.

## Acknowledgements
This project was supported in part by funding from the [Carnegie Mellon
University Frank-Ratchye Fund for Art @ the Frontier](http://studioforcreativeinquiry.org/)