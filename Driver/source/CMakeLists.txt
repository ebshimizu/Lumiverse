cmake_minimum_required (VERSION 2.6)

project(BufferDriver)

include_directories("${CMAKE_CURRENT_LIST_DIR}/lib/arnold/include")

set (BufferDriver_INCLUDE_ARNOLD ON CACHE BOOL "Build BufferDriver with Arnold Simulator")

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
set (DRIVER_SOURCE BufferDriver.h
    BufferDriver.cpp)

# Build the libraries
add_library(BufferDriver SHARED ${DRIVER_SOURCE})

# Build with Arnold SDK (ai)
IF (BufferDriver_INCLUDE_ARNOLD)
    IF(WIN32)
        add_library(ai STATIC IMPORTED)
        set_property(TARGET ai PROPERTY IMPORTED_LOCATION
            ${PROJECT_SOURCE_DIR}/LumiverseCore/lib/arnold/bin/ai.lib)
        target_link_libraries(BufferDriver ai)
    ENDIF(WIN32)
    IF(APPLE)
        add_library(ai SHARED IMPORTED)
        set_property(TARGET ai PROPERTY IMPORTED_LOCATION
            ${PROJECT_SOURCE_DIR}/lib/arnold/bin/libai.dylib)
        target_link_libraries(BufferDriver ai)
    ENDIF(APPLE)
ENDIF (BufferDriver_INCLUDE_ARNOLD)

# Installation time
FILE(GLOB corefiles ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
INSTALL(FILES ${corefiles} DESTINATION include)

INSTALL(TARGETS BufferDriver LIBRARY DESTINATION lib)
