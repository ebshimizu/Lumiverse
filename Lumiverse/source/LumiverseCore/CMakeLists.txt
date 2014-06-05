include_directories("${CMAKE_CURRENT_LIST_DIR}/types"
  "${CMAKE_CURRENT_LIST_DIR}/DMX"
  "${CMAKE_CURRENT_LIST_DIR}/lib/libjson"
  "${CMAKE_CURRENT_LIST_DIR}/lib/ftd2xx")

# Lumiverse Core Version Number, in case its needed
set (LumiverseCore_VERSION_MAJOR 1)
set (LumiverseCore_VERSION_MINOR 0)
set (LumiverseCore_INCLUDE_DMXPRO2INTERFACE ON CACHE BOOL "Build LumiverseCore with Enttec USB DMX Pro Mk II Driver")
set (LumiverseCore_INCLUDE_KINET ON CACHE BOOL "Build LumiverseCore with KiNet Driver")
set (LumiverseCore_PYTHON_BINDINGS ON CACHE BOOL "Build LumiverseCore bindings for Python")
set (LumiverseCore_CSHARP_BINDINGS OFF CACHE BOOL "Build LumiverseCore bindings for C#")

# Config interface options
IF (LumiverseCore_INCLUDE_DMXPRO2INTERFACE)
    SET (LumiverseCore_USE_DMXPRO2 "#define USE_DMXPRO2")
ENDIF (LumiverseCore_INCLUDE_DMXPRO2INTERFACE)

IF (LumiverseCore_INCLUDE_KINET)
    SET (LumiverseCore_USE_KINET "#define USE_KINET")
ENDIF (LumiverseCore_INCLUDE_KINET)

configure_file (
  "${CMAKE_CURRENT_LIST_DIR}/LumiverseCoreConfig.h.in"
  "${CMAKE_CURRENT_LIST_DIR}/LumiverseCoreConfig.h"
)

# Compiler options
IF(APPLE)
    SET(CLANG_FLAGS "-std=c++11 -stdlib=libc++")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CLANG_FLAGS}")
ENDIF(APPLE)

IF(UNIX)
    SET(GCC_FLAGS "-std=c++11 -pthread -fPIC")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_FLAGS}")
    MESSAGE("Adding -std=c++11 to g++ flags")
ENDIF(UNIX)

# Set source files
set (LUMIVERSE_CORE_SOURCE LumiverseCoreConfig.h
    LumiverseCore.h
    Logger.h
    Logger.cpp
    Device.h
    Device.cpp
    Rig.h
    Rig.cpp
    DeviceSet.h
    DeviceSet.cpp
    LumiverseType.h
    Patch.h
    types/LumiverseFloat.h
    types/LumiverseFloat.cpp
	types/LumiverseEnum.h
	types/LumiverseEnum.cpp
	types/LumiverseTypeUtils.h
	types/LumiverseTypeUtils.cpp
    DMX/DMXPatch.h
    DMX/DMXPatch.cpp
    DMX/DMXDevicePatch.h
    DMX/DMXDevicePatch.cpp
    DMX/DMXInterface.h
	DMX/KiNetInterface.h
	DMX/KiNetInterface.cpp)

# CMake is a bit weird and wants this defined here instead of inside the ftd2xx folder
# Add relevant files to build if using DMXPRO2
IF (LumiverseCore_INCLUDE_DMXPRO2INTERFACE)
    SET (LumiverseCore_USE_DMXPRO2 "#define USE_DMXPRO2" STRING)
	SET (LUMIVERSE_CORE_SOURCE ${LUMIVERSE_CORE_SOURCE} DMX/DMXPro2Interface.h DMX/pro_driver.h DMX/DMXPro2Interface.cpp)
ENDIF(LumiverseCore_INCLUDE_DMXPRO2INTERFACE)

# Build the libraries
add_library(LumiverseCore ${LUMIVERSE_CORE_SOURCE})
target_link_libraries(LumiverseCore libjson)
add_subdirectory(lib)

# DMXPRO2 Interface build settings
IF (LumiverseCore_INCLUDE_DMXPRO2INTERFACE)
    IF(WIN32)
        add_library(ftd2xx STATIC IMPORTED)
        set_property(TARGET ftd2xx PROPERTY IMPORTED_LOCATION
            ${PROJECT_SOURCE_DIR}/LumiverseCore/lib/ftd2xx/ftd2xx.lib)
        target_link_libraries(LumiverseCore ftd2xx)
    ENDIF(WIN32)

    IF(APPLE)
        add_library(ftd2xx SHARED IMPORTED)
        set_property(TARGET ftd2xx PROPERTY IMPORTED_LOCATION
            ${PROJECT_SOURCE_DIR}/LumiverseCore/lib/ftd2xx/libftd2xx.1.2.2.dylib)
        target_link_libraries(LumiverseCore ftd2xx)

        # Note that this does copy over the libftd2xx library to the proper location to run on OS X
        # It's really not that much fun to do so I'm looking for a better way of doing it.
        # For some reason it didn't like linking to libusb and ftd2xx normally.
        execute_process(COMMAND cp ${PROJECT_SOURCE_DIR}/LumiverseCore/lib/ftd2xx/libftd2xx.1.2.2.dylib 
            /usr/local/lib/libftd2xx.1.2.2.dylib)
    ENDIF(APPLE)
ENDIF(LumiverseCore_INCLUDE_DMXPRO2INTERFACE)

# Time for fun other library binding generation time
IF (LumiverseCore_PYTHON_BINDINGS OR LumiverseCore_CSHARP_BINDINGS)
	FIND_PACKAGE(SWIG REQUIRED)
	INCLUDE(${SWIG_USE_FILE})

	INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

	SET(CMAKE_SWIG_FLAGS "")

	SET_SOURCE_FILES_PROPERTIES(LumiverseCore.i PROPERTIES CPLUSPLUS ON)
	SET_SOURCE_FILES_PROPERTIES(LumiverseCore.i PROPERTIES SWIG_FLAGS "-includeall")
	
	IF (LumiverseCore_PYTHON_BINDINGS)
		MESSAGE("Including python bindings...")
		FIND_PACKAGE(PythonLibs)
		INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})

		SWIG_ADD_MODULE(lumiversepython python ${LUMIVERSE_CORE_SOURCE} LumiverseCore.i)

		SWIG_LINK_LIBRARIES(lumiversepython ${PYTHON_LIBRARY} libjson)
	ENDIF (LumiverseCore_PYTHON_BINDINGS)

	IF (LumiverseCore_CSHARP_BINDINGS)
		MESSAGE("Including C# bindings...")
		SWIG_ADD_MODULE(lumiversecsharp csharp ${LUMIVERSE_CORE_SOURCE} LumiverseCore.i)

		SWIG_LINK_LIBRARIES(lumiversecsharp ftd2xx libjson)
	ENDIF (LumiverseCore_CSHARP_BINDINGS)
ENDIF (LumiverseCore_PYTHON_BINDINGS OR LumiverseCore_CSHARP_BINDINGS)
