# About
Lumiverse is a framework that provides intuitive control over lighting devices.
Lumiverse can support any interface if there's a driver for it, and can output to
multiple interfaces at the same time.

# Build instructions
* Install CMake (> 2.8 suggested)
* Install build dependencies for specific modules:
  * DMX USB PRO MK2 driver: ftd2xx
  * Python Bindings: Python, SWIG (>= 3.0.0)
* Make a folder to contain all of your build files
* Inside of the folder you made, run `cmake ../source`
