# About
Lumiverse is a framework that provides intuitive control over lighting devices.
Lumiverse can support any interface if there's a driver for it, and can output to
multiple interfaces at the same time.

## Build instructions
* Install CMake (> 2.8 suggested)
* Install build dependencies for specific modules:
    * DMX USB PRO MK2 driver: ftd2xx
    * Bindings for other languages: SWIG (>= 3.0.0)
    	* Python: Python
    	* C#: .NET (any version that works with SWIG)
* Make a folder to contain all of your build files
* Inside of the folder you made, run `cmake ../source`
* Build

## Platform and Interface Support
Lumiverse supports Windows, OS X, and iOS. Linux has some small issues at the
moment preventing the project from compiling. I had some issues with the
FTD2XX driver on OS X, so your mileage with OS X may vary.

At the moment, Lumiverse only has drivers for ENTTEC's DMX PRO MkII devices,
and has only been tested using one connected DMX PRO device at a time. iOS
supports none of the USB interfaces. It should support Art-Net and other
DMX over IP protocols at some point in the future, but I've run it with
a Socket.IO interface just fine.

## Project Roadmap
* Write up documentation for current code base
* Linux Support
* Move USB drivers from FTD2XX to libftdi
* Clean up build output locations for other language bindings
* Write Interfaces
	* Art-Net
	* KiNet
	* ENTTEC DMX PRO (Mk 1)
	* Generic FTDI device
	* Philips Hue
	* ACN