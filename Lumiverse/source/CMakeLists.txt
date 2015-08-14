cmake_minimum_required (VERSION 2.6)

project (Lumiverse)
set (LUMIVERSE_DOCS ON CACHE BOOL "Generate Documentation Project (requires Doxygen)")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/../cmake/modules")

# defaulting to release build
IF (NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE         "Release" CACHE STRING "Type of build (Debug Release RelWithDebInfo MinSizeRel)" FORCE)
ENDIF ()

set (LumiverseCore_INCLUDE_ARNOLD OFF CACHE BOOL   "Build LumiverseCore with Arnold Simulator")
set (LumiverseCore_ARNOLD_PATH "" CACHE PATH "Location of Arnold Renderer tree")

IF (LumiverseCore_INCLUDE_ARNOLD)

  # make sure the an Arnold path was given
  IF (NOT EXISTS ${LumiverseCore_ARNOLD_PATH})
  	message( FATAL_ERROR "INCLUDE_ARNOLD is TRUE but no ARNOLD_PATH specified." )
  ENDIF (NOT EXISTS ${LumiverseCore_ARNOLD_PATH})
  
  SET (LumiverseCore_USE_ARNOLD "#define USE_ARNOLD")
ENDIF (LumiverseCore_INCLUDE_ARNOLD)

# LumiverseCore setings
include_directories(
  "${PROJECT_SOURCE_DIR}/LumiverseCore")
include_directories("${PROJECT_SOURCE_DIR}/LumiverseCore/types"
  "${PROJECT_SOURCE_DIR}/LumiverseCore/DMX"
  "${PROJECT_SOURCE_DIR}/LumiverseCore/lib/libjson"
  "${PROJECT_SOURCE_DIR}/LumiverseCore/lib/ftd2xx"
  "${PROJECT_SOURCE_DIR}/LumiverseCore/lib/libartnet"
  "${PROJECT_SOURCE_DIR}/LumiverseCore/lib/Eigen"
  "${PROJECT_SOURCE_DIR}/LumiverseCore/lib/clp"
  "${PROJECT_SOURCE_DIR}/LumiverseCore/lib/clp/CoinUtils"
  "${PROJECT_SOURCE_DIR}/LumiverseCore/Simulation"
  "${PROJECT_SOURCE_DIR}/LumiverseCore/lib/arnold/include")

# Lumiverse Core Version Number, in case its needed
set (LumiverseCore_VERSION_MAJOR 2)
set (LumiverseCore_VERSION_MINOR 1)
set (LumiverseCore_INCLUDE_DMXPRO2INTERFACE OFF CACHE BOOL "Build LumiverseCore with Enttec USB DMX Pro Mk II Driver")
set (LumiverseCore_INCLUDE_KINET ON CACHE BOOL "Build LumiverseCore with KiNet Driver")
set (LumiverseCore_INCLUDE_ARTNET ON CACHE BOOL "Build LumiverseCore with ArtNet Driver")
set (LumiverseCore_PYTHON_BINDINGS ON CACHE BOOL "Build LumiverseCore bindings for Python")
set (LumiverseCore_INCLUDE_OLA OFF CACHE BOOL "Build LumiverseCore with OLA Driver")
set (LumiverseCore_CSHARP_BINDINGS OFF CACHE BOOL "Build LumiverseCore bindings for C#")
set (LumiverseCore_NODEJS_BINDINGS OFF CACHE BOOL "Build Lumiverse bindings for Node.js")

# Config interface options
IF (LumiverseCore_INCLUDE_DMXPRO2INTERFACE)
    SET (LumiverseCore_USE_DMXPRO2 "#define USE_DMXPRO2")
ENDIF (LumiverseCore_INCLUDE_DMXPRO2INTERFACE)

IF (LumiverseCore_INCLUDE_KINET)
    SET (LumiverseCore_USE_KINET "#define USE_KINET")
ENDIF (LumiverseCore_INCLUDE_KINET)

IF (LumiverseCore_INCLUDE_ARTNET)
    SET (LumiverseCore_USE_ARTNET "#define USE_ARTNET")
ENDIF (LumiverseCore_INCLUDE_ARTNET)

IF (LumiverseCore_INCLUDE_OLA)
    SET (LumiverseCore_USE_OLA "#define USE_OLA")
ENDIF (LumiverseCore_INCLUDE_OLA)

configure_file (
  "${PROJECT_SOURCE_DIR}/LumiverseCore/LumiverseCoreConfig.h.in"
  "${PROJECT_SOURCE_DIR}/LumiverseCore/LumiverseCoreConfig.h"
)

# Compiler options
IF(APPLE)
    SET(CLANG_FLAGS "-std=c++11 -stdlib=libc++")
    
    # HACK(kayvonf): removing frequently occurring warnings due to clp
    # sloppiness.  If possible, someone should try to surpress these
    # warnings only for the relevant file(s).
    SET(CLANG_FLAGS "${CLANG_FLAGS} -Wno-deprecated-register -Wno-extra-tokens")
    
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CLANG_FLAGS}")
ELSEIF(UNIX OR LINUX)
    SET(GCC_FLAGS "-std=c++11 -pthread -fPIC")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_FLAGS}")
    MESSAGE("Detected platform Unix: Adding -std=c++11 to g++ flags")
ENDIF(APPLE)

# TODO: Bug with machine:x86 linker option
SET(CMAKE_EXE_LINKER_FLAGS "")
SET(CMAKE_MODULE_LINKER_FLAGS "")
SET(CMAKE_SHARED_LINKER_FLAGS "")

# Set source files
set (LUMIVERSE_CORE_SOURCE
  ${PROJECT_SOURCE_DIR}/LumiverseCore/LumiverseCoreConfig.h
  ${PROJECT_SOURCE_DIR}/LumiverseCore/LumiverseCore.h
  ${PROJECT_SOURCE_DIR}/LumiverseCore/Logger.h
  ${PROJECT_SOURCE_DIR}/LumiverseCore/Logger.cpp
  ${PROJECT_SOURCE_DIR}/LumiverseCore/Device.h
  ${PROJECT_SOURCE_DIR}/LumiverseCore/Device.cpp
  ${PROJECT_SOURCE_DIR}/LumiverseCore/Rig.h
  ${PROJECT_SOURCE_DIR}/LumiverseCore/Rig.cpp
  ${PROJECT_SOURCE_DIR}/LumiverseCore/DeviceSet.h
  ${PROJECT_SOURCE_DIR}/LumiverseCore/DeviceSet.cpp
  ${PROJECT_SOURCE_DIR}/LumiverseCore/DynamicDeviceSet.h
  ${PROJECT_SOURCE_DIR}/LumiverseCore/DynamicDeviceSet.cpp
  ${PROJECT_SOURCE_DIR}/LumiverseCore/LumiverseType.h
  ${PROJECT_SOURCE_DIR}/LumiverseCore/Patch.h
  ${PROJECT_SOURCE_DIR}/LumiverseCore/types/LumiverseFloat.h
  ${PROJECT_SOURCE_DIR}/LumiverseCore/types/LumiverseFloat.cpp
	${PROJECT_SOURCE_DIR}/LumiverseCore/types/LumiverseEnum.h
	${PROJECT_SOURCE_DIR}/LumiverseCore/types/LumiverseEnum.cpp
  ${PROJECT_SOURCE_DIR}/LumiverseCore/types/LumiverseColor.h
	${PROJECT_SOURCE_DIR}/LumiverseCore/types/LumiverseColor.cpp
	${PROJECT_SOURCE_DIR}/LumiverseCore/types/LumiverseOrientation.h
	${PROJECT_SOURCE_DIR}/LumiverseCore/types/LumiverseOrientation.cpp
	${PROJECT_SOURCE_DIR}/LumiverseCore/types/LumiverseTypeUtils.h
	${PROJECT_SOURCE_DIR}/LumiverseCore/types/LumiverseTypeUtils.cpp
  ${PROJECT_SOURCE_DIR}/LumiverseCore/types/LumiverseColorLib.h
  ${PROJECT_SOURCE_DIR}/LumiverseCore/types/LumiverseColorLib.cpp
  ${PROJECT_SOURCE_DIR}/LumiverseCore/DMX/DMXPatch.h
  ${PROJECT_SOURCE_DIR}/LumiverseCore/DMX/DMXPatch.cpp
  ${PROJECT_SOURCE_DIR}/LumiverseCore/DMX/DMXDevicePatch.h
  ${PROJECT_SOURCE_DIR}/LumiverseCore/DMX/DMXDevicePatch.cpp
  ${PROJECT_SOURCE_DIR}/LumiverseCore/DMX/DMXInterface.h
	${PROJECT_SOURCE_DIR}/LumiverseCore/DMX/KiNetInterface.h
	${PROJECT_SOURCE_DIR}/LumiverseCore/DMX/KiNetInterface.cpp
)

set (ACTIVE_LIBRARIES libjson clp)

# Add relevant files to build if using DMXPRO2
IF (LumiverseCore_INCLUDE_DMXPRO2INTERFACE)
    SET (LumiverseCore_USE_DMXPRO2 "#define USE_DMXPRO2" STRING)
    SET (LUMIVERSE_CORE_SOURCE ${LUMIVERSE_CORE_SOURCE}
      ${PROJECT_SOURCE_DIR}/LumiverseCore/DMX/DMXPro2Interface.h
      ${PROJECT_SOURCE_DIR}/LumiverseCore/DMX/pro_driver.h
      ${PROJECT_SOURCE_DIR}/LumiverseCore/DMX/DMXPro2Interface.cpp
    )
    SET (ACTIVE_LIBRARIES ${ACTIVE_LIBRARIES} ftd2xx)
ENDIF(LumiverseCore_INCLUDE_DMXPRO2INTERFACE)

# Add Arnold simulation code
IF (LumiverseCore_INCLUDE_ARNOLD)
  SET (LUMIVERSE_CORE_SOURCE ${LUMIVERSE_CORE_SOURCE}
    ${PROJECT_SOURCE_DIR}/LumiverseCore/Simulation/SimulationPatch.h
    ${PROJECT_SOURCE_DIR}/LumiverseCore/Simulation/SimulationPatch.cpp
    ${PROJECT_SOURCE_DIR}/LumiverseCore/Simulation/SimulationAnimationPatch.h
    ${PROJECT_SOURCE_DIR}/LumiverseCore/Simulation/SimulationAnimationPatch.cpp
    ${PROJECT_SOURCE_DIR}/LumiverseCore/Simulation/PhotoPatch.h
    ${PROJECT_SOURCE_DIR}/LumiverseCore/Simulation/PhotoPatch.cpp
    ${PROJECT_SOURCE_DIR}/LumiverseCore/Simulation/PhotoAnimationPatch.h
    ${PROJECT_SOURCE_DIR}/LumiverseCore/Simulation/PhotoAnimationPatch.cpp
    ${PROJECT_SOURCE_DIR}/LumiverseCore/Simulation/ArnoldPatch.h
    ${PROJECT_SOURCE_DIR}/LumiverseCore/Simulation/ArnoldPatch.cpp
    ${PROJECT_SOURCE_DIR}/LumiverseCore/Simulation/ArnoldInterface.h
    ${PROJECT_SOURCE_DIR}/LumiverseCore/Simulation/ArnoldInterface.cpp
    ${PROJECT_SOURCE_DIR}/LumiverseCore/Simulation/ArnoldAnimationPatch.h
    ${PROJECT_SOURCE_DIR}/LumiverseCore/Simulation/ArnoldAnimationPatch.cpp
    ${PROJECT_SOURCE_DIR}/LumiverseCore/Simulation/ArnoldFrameManager.h
    ${PROJECT_SOURCE_DIR}/LumiverseCore/Simulation/ArnoldMemoryFrameManager.h
    ${PROJECT_SOURCE_DIR}/LumiverseCore/Simulation/ArnoldMemoryFrameManager.cpp
    ${PROJECT_SOURCE_DIR}/LumiverseCore/Simulation/ArnoldFileFrameManager.h
    ${PROJECT_SOURCE_DIR}/LumiverseCore/Simulation/ArnoldFileFrameManager.cpp
    ${PROJECT_SOURCE_DIR}/LumiverseCore/Simulation/imageio.h
    ${PROJECT_SOURCE_DIR}/LumiverseCore/Simulation/imageio.cpp
    ${PROJECT_SOURCE_DIR}/LumiverseCore/Simulation/ArnoldParameterVector.h
  )
ENDIF (LumiverseCore_INCLUDE_ARNOLD)

# Add ArtNet code if used
IF (LumiverseCore_INCLUDE_ARTNET)
    SET (LUMIVERSE_CORE_SOURCE ${LUMIVERSE_CORE_SOURCE}
      ${PROJECT_SOURCE_DIR}/LumiverseCore/DMX/ArtNetInterface.h
      ${PROJECT_SOURCE_DIR}/LumiverseCore/DMX/ArtNetInterface.cpp)
    SET (ACTIVE_LIBRARIES ${ACTIVE_LIBRARIES} libartnet)
ENDIF (LumiverseCore_INCLUDE_ARTNET)

# Add OLA code if used
IF (LumiverseCore_INCLUDE_OLA)
  find_package(OLA REQUIRED)
  include_directories(${OLA_INCLUDE_DIR})
  set(ACTIVE_LIBRARIES ${ACTIVE_LIBRARIES} ${OLA_LIBRARIES})
  SET (LUMIVERSE_CORE_SOURCE ${LUMIVERSE_CORE_SOURCE}
    ${PROJECT_SOURCE_DIR}/LumiverseCore/DMX/OLAInterface.h
    ${PROJECT_SOURCE_DIR}/LumiverseCore/DMX/OLAInterface.cpp)
ENDIF (LumiverseCore_INCLUDE_OLA)

# Build the libraries
add_library(LumiverseCore ${LUMIVERSE_CORE_SOURCE})
add_library(LumiverseCoreShared SHARED ${LUMIVERSE_CORE_SOURCE})
add_subdirectory(${PROJECT_SOURCE_DIR}/LumiverseCore/lib)

# DMXPRO2 Interface build settings
IF (LumiverseCore_INCLUDE_DMXPRO2INTERFACE)
    IF(WIN32)
        add_library(ftd2xx STATIC IMPORTED)
        set_property(TARGET ftd2xx PROPERTY IMPORTED_LOCATION
            ${PROJECT_SOURCE_DIR}/LumiverseCore/lib/ftd2xx/ftd2xx.lib)
    ENDIF(WIN32)

    IF(APPLE)
        add_library(ftd2xx SHARED IMPORTED)
        set_property(TARGET ftd2xx PROPERTY IMPORTED_LOCATION
            ${PROJECT_SOURCE_DIR}/LumiverseCore/lib/ftd2xx/libftd2xx.1.2.2.dylib)
    ENDIF(APPLE)
ENDIF(LumiverseCore_INCLUDE_DMXPRO2INTERFACE)

# Simulation module needs libpng to output png
# Build with Arnold SDK (ai)
IF (LumiverseCore_INCLUDE_ARNOLD)
	find_package(PNG REQUIRED)

	IF(PNG_FOUND)
		include_directories(
			${PNG_INCLUDE_DIRS}
		)

		SET(ACTIVE_LIBRARIES ${ACTIVE_LIBRARIES} ${PNG_LIBRARIES})
	ENDIF(PNG_FOUND)

    IF(WIN32)
        add_library(ai STATIC IMPORTED)
        set_property(TARGET ai PROPERTY IMPORTED_LOCATION
            ${LumiverseCore_ARNOLD_PATH}/bin/ai.lib)
    ENDIF(WIN32)

    IF(APPLE)
        add_library(ai SHARED IMPORTED)
        set_property(TARGET ai PROPERTY IMPORTED_LOCATION
            ${LumiverseCore_ARNOLD_PATH}/bin/libai.dylib)
    ENDIF(APPLE)

    SET (ACTIVE_LIBRARIES ${ACTIVE_LIBRARIES} ai)
ENDIF (LumiverseCore_INCLUDE_ARNOLD)

target_link_libraries(LumiverseCore ${ACTIVE_LIBRARIES})
target_link_libraries(LumiverseCoreShared libjson ${ACTIVE_LIBRARIES})

# Installation time
FILE(GLOB corefiles ${PROJECT_SOURCE_DIR}/LumiverseCore/*.h)
INSTALL(FILES ${corefiles} DESTINATION include)

FILE(GLOB typefiles ${PROJECT_SOURCE_DIR}/LumiverseCore/types/*.h)
INSTALL(FILES ${typefiles} DESTINATION include/types)

FILE(GLOB dmxfiles ${PROJECT_SOURCE_DIR}/LumiverseCore/DMX/*.h)
INSTALL(FILES ${dmxfiles} DESTINATION include/DMX)

FILE(GLOB simulationfiles ${PROJECT_SOURCE_DIR}/LumiverseCore/Simulation/*.h)
INSTALL(FILES ${simulationfiles} DESTINATION include/Simulation)

FILE(GLOB jsonfiles ${PROJECT_SOURCE_DIR}/LumiverseCore/lib/libjson/*.h)
INSTALL(FILES ${jsonfiles} DESTINATION include/lib/libjson)

FILE(GLOB jsoninternal ${PROJECT_SOURCE_DIR}/LumiverseCore/lib/libjson/_internal/Source/*.h)
INSTALL(FILES ${jsoninternal} DESTINATION include/lib/libjson/_internal/Source)

FILE(GLOB jsoninternal2 ${PROJECT_SOURCE_DIR}/LumiverseCore/lib/libjson/_internal/Source/JSONDefs/*.h)
INSTALL(FILES ${jsoninternal2} DESTINATION include/lib/libjson/_internal/Source/JSONDefs)

INSTALL(FILES ${PROJECT_SOURCE_DIR}/LumiverseCore/lib/libjson/_internal/Dependencies/libbase64++/libbase64++.h
	DESTINATION include/lib/libjson/_internal/Dependencies/libbase64++)
INSTALL(FILES ${PROJECT_SOURCE_DIR}/LumiverseCore/lib/libjson/_internal/Dependencies/mempool++/mempool.h
	DESTINATION include/lib/libjson/_internal/Dependencies/mempool)

FILE(GLOB clpfiles ${PROJECT_SOURCE_DIR}/LumiverseCore/lib/clp/*.hpp ${PROJECT_SOURCE_DIR}/LumiverseCore/lib/clp/*.h)
INSTALL(FILES ${clpfiles} DESTINATION include/lib/clp)

FILE(GLOB pngbinfiles ${PROJECT_SOURCE_DIR}/LumiverseCore/lib/libpng/*.lib)
INSTALL(FILES ${pngbinfiles} DESTINATION lib)

FILE(GLOB pngfiles ${PROJECT_SOURCE_DIR}/LumiverseCore/lib/libpng/*.h)
INSTALL(FILES ${pngfiles} DESTINATION include/lib/libpng)

IF (LumiverseCore_INCLUDE_ARNOLD)
    FILE(GLOB arnoldfiles ${LumiverseCore_ARNOLD_PATH}/include/*.h)
    INSTALL(FILES ${arnoldfiles} DESTINATION include/lib/arnold/include)

    FILE(GLOB arnoldbinfiles ${LumiverseCore_ARNOLD_PATH}/bin/*)
    INSTALL(FILES ${arnoldbinfiles} DESTINATION lib)
ENDIF (LumiverseCore_INCLUDE_ARNOLD)

INSTALL(TARGETS LumiverseCore ARCHIVE DESTINATION lib)
INSTALL(TARGETS LumiverseCoreShared DESTINATION lib)

IF (LumiverseCore_INCLUDE_DMXPRO2INTERFACE)
    FILE(GLOB pro2files ${PROJECT_SOURCE_DIR}/LumiverseCore/lib/ftd2xx/*.H)
    INSTALL(FILES ${pro2files} DESTINATION include/lib/ftd2xx)
    # You'll have to copy over the right file for ftd2xx to use it properly until I
    # can get this library replaced.
ENDIF (LumiverseCore_INCLUDE_DMXPRO2INTERFACE)

add_subdirectory(LumiverseShowControl)

add_subdirectory(Demos)
add_subdirectory(Test)

if (LUMIVERSE_DOCS)
  find_package(Doxygen)
  if(DOXYGEN_FOUND)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    add_custom_target(doc
      ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
      WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
      COMMENT "Generating API documentation with Doxygen" VERBATIM
    )
  endif(DOXYGEN_FOUND)
endif(LUMIVERSE_DOCS)


# Time for fun other library binding generation time
IF (LumiverseCore_PYTHON_BINDINGS OR LumiverseCore_CSHARP_BINDINGS OR LumiverseCore_NODEJS_BINDINGS)
	FIND_PACKAGE(SWIG REQUIRED)
	INCLUDE(${SWIG_USE_FILE})

	INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/LumiverseCore"  "${PROJECT_SOURCE_DIR}/LumiverseShowControl")

  set (LUMIVERSE_SHOW_CONTROL_SOURCE
    ${PROJECT_SOURCE_DIR}/LumiverseShowControl/Cue.h
    ${PROJECT_SOURCE_DIR}/LumiverseShowControl/CueList.h
    ${PROJECT_SOURCE_DIR}/LumiverseShowControl/Playback.h
    ${PROJECT_SOURCE_DIR}/LumiverseShowControl/Layer.h
    ${PROJECT_SOURCE_DIR}/LumiverseShowControl/Programmer.h
    ${PROJECT_SOURCE_DIR}/LumiverseShowControl/Snapshot.h
    ${PROJECT_SOURCE_DIR}/LumiverseShowControl/Cue.cpp
    ${PROJECT_SOURCE_DIR}/LumiverseShowControl/CueList.cpp
    ${PROJECT_SOURCE_DIR}/LumiverseShowControl/Playback.cpp
    ${PROJECT_SOURCE_DIR}/LumiverseShowControl/Layer.cpp
    ${PROJECT_SOURCE_DIR}/LumiverseShowControl/Programmer.cpp
    ${PROJECT_SOURCE_DIR}/LumiverseShowControl/Snapshot.cpp
    ${PROJECT_SOURCE_DIR}/LumiverseShowControl/Timeline.h
    ${PROJECT_SOURCE_DIR}/LumiverseShowControl/Timeline.cpp
    ${PROJECT_SOURCE_DIR}/LumiverseShowControl/Keyframe.h
    ${PROJECT_SOURCE_DIR}/LumiverseShowControl/Keyframe.cpp
    ${PROJECT_SOURCE_DIR}/LumiverseShowControl/SineWave.h
    ${PROJECT_SOURCE_DIR}/LumiverseShowControl/SineWave.cpp
    ${PROJECT_SOURCE_DIR}/LumiverseShowControl/LumiverseShowControl.h)

	SET(CMAKE_SWIG_FLAGS "")

	SET_SOURCE_FILES_PROPERTIES(LumiverseCore.i PROPERTIES CPLUSPLUS ON)
	SET_SOURCE_FILES_PROPERTIES(LumiverseCore.i PROPERTIES SWIG_FLAGS "")
	
	IF (LumiverseCore_PYTHON_BINDINGS)
		MESSAGE("Including python bindings...")
		FIND_PACKAGE(PythonLibs)
		INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})

		SWIG_ADD_MODULE(lumiversepython python ${LUMIVERSE_CORE_SOURCE} ${LUMIVERSE_SHOW_CONTROL_SOURCE} LumiverseCore.i)

		SWIG_LINK_LIBRARIES(lumiversepython ${PYTHON_LIBRARY} ${ACTIVE_LIBRARIES})
	ENDIF (LumiverseCore_PYTHON_BINDINGS)

	IF (LumiverseCore_CSHARP_BINDINGS)
		MESSAGE("Including C# bindings...")
		SWIG_ADD_MODULE(lumiversecsharp csharp ${LUMIVERSE_CORE_SOURCE} LumiverseCore.i)

		SWIG_LINK_LIBRARIES(lumiversecsharp ${ACTIVE_LIBRARIES})
	ENDIF (LumiverseCore_CSHARP_BINDINGS)

	IF (LumiverseCore_NODEJS_BINDINGS)
		MESSAGE("Including Node.js bindings...")
		SET_SOURCE_FILES_PROPERTIES(LumiverseCore.i PROPERTIES SWIG_FLAGS "-node")
		SWIG_ADD_MODULE(lumiversenode javascript ${LUMIVERSE_CORE_SOURCE} ${LUMIVERSE_SHOW_CONTROL_SOURCE} LumiverseCore.i)

		SWIG_LINK_LIBRARIES(lumiversenode ${ACTIVE_LIBRARIES})
	ENDIF (LumiverseCore_NODEJS_BINDINGS)
ENDIF (LumiverseCore_PYTHON_BINDINGS OR LumiverseCore_CSHARP_BINDINGS OR LumiverseCore_NODEJS_BINDINGS)