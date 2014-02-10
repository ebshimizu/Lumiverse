include_directories("${CMAKE_CURRENT_LIST_DIR}/types"
  "${CMAKE_CURRENT_LIST_DIR}/DMX"
  "${CMAKE_CURRENT_LIST_DIR}/lib/libjson"
  "${CMAKE_CURRENT_LIST_DIR}/lib/ftd2xx")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/cmake/Modules/")

SET(CLANG_FLAGS "-std=c++11 -stdlib=libc++")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CLANG_FLAGS}")

add_subdirectory(lib)

add_library(LumiverseCore
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
    DMX/DMXPatch.h
    DMX/DMXPatch.cpp
    DMX/DMXDevicePatch.h
    DMX/DMXDevicePatch.cpp
    DMX/DMXInterface.h
    DMX/DMXPro2Interface.h
    DMX/pro_driver.h
    DMX/DMXPro2Interface.cpp)

target_link_libraries(LumiverseCore libjson)

# Make optional at some point for people to enable or disable desired interfaces
# CMake is a bit weird and wants this defined here instead of inside the ftd2xx folder
IF(WIN32)
    add_library(ftd2xx STATIC IMPORTED)
    set_property(TARGET ftd2xx PROPERTY IMPORTED_LOCATION
        ${PROJECT_SOURCE_DIR}/LumiverseCore/lib/ftd2xx/ftd2xx.a)
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