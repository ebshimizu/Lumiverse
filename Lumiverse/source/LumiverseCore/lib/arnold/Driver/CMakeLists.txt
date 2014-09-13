cmake_minimum_required (VERSION 2.6)

include_directories("${CMAKE_CURRENT_LIST_DIR}/../include")

# Compiler options
IF(APPLE)
    SET(CLANG_FLAGS "-std=c++11 -stdlib=libc++")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CLANG_FLAGS}")
ELSEIF(UNIX)
    SET(GCC_FLAGS "-std=c++11 -pthread -fPIC")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_FLAGS}")
    MESSAGE("Adding -std=c++11 to g++ flags for Arnold driver")
ENDIF(APPLE)

# Set source files
set (DRIVER_SOURCE BufferDriver.h
    BufferDriver.cpp)

# Build the libraries
add_library(BufferDriver SHARED ${DRIVER_SOURCE})

# Build with Arnold SDK (ai)
IF(WIN32)
	add_library(ai STATIC IMPORTED)
	set_property(TARGET ai PROPERTY IMPORTED_LOCATION
		${CMAKE_CURRENT_LIST_DIR}/../bin/ai.lib)
	target_link_libraries(BufferDriver ai)
ENDIF(WIN32)

IF(APPLE)
	add_library(ai SHARED IMPORTED)
	set_property(TARGET ai PROPERTY IMPORTED_LOCATION
		${CMAKE_CURRENT_LIST_DIR}/../bin/libai.dylib)
	target_link_libraries(BufferDriver ai)
ENDIF(APPLE)

# Installation time
FILE(GLOB driverFiles ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
INSTALL(FILES ${driverFiles} DESTINATION include/lib/arnold/include)

INSTALL(TARGETS BufferDriver DESTINATION lib/arnold/plugin)
