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

### PointCloudShader
# Set source files
set (POINT_CLOUD_SOURCE PointCloudShader.h
    PointCloudShader.cpp)

# Build the libraries
add_library(PointCloudShader SHARED ${POINT_CLOUD_SOURCE})

### Gobo
# Set source files
set (GOBO_SOURCE PhotometricGobo.h
    PhotometricGobo.cpp)

# Build the libraries
add_library(PhotometricGobo SHARED ${GOBO_SOURCE})

# Build with Arnold SDK (ai)
IF(WIN32)
	add_library(ai STATIC IMPORTED)
	set_property(TARGET ai PROPERTY IMPORTED_LOCATION
		${CMAKE_CURRENT_LIST_DIR}/../bin/ai.lib)
	target_link_libraries(PointCloudShader ai)
	target_link_libraries(PhotometricGobo ai)
ENDIF(WIN32)

IF(APPLE)
	add_library(ai SHARED IMPORTED)
	set_property(TARGET ai PROPERTY IMPORTED_LOCATION
		${CMAKE_CURRENT_LIST_DIR}/../bin/libai.dylib)
	target_link_libraries(PointCloudShader ai)
	target_link_libraries(PhotometricGobo ai)
ENDIF(APPLE)

IF(UNIX)
	add_library(ai SHARED IMPORTED)
	set_property(TARGET ai PROPERTY IMPORTED_LOCATION
		${LumiverseCore_ARNOLD_PATH}/bin/libai.so)
	target_link_libraries(PointCloudShader ai)
	target_link_libraries(PhotometricGobo ai)
ENDIF(UNIX)

# Installation time
FILE(GLOB driverFiles ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
INSTALL(FILES ${driverFiles} DESTINATION include/lib/arnold/include)

INSTALL(TARGETS PointCloudShader DESTINATION Debug/lib/arnold/plugin CONFIGURATIONS Debug)
INSTALL(TARGETS PointCloudShader DESTINATION Release/lib/arnold/plugin CONFIGURATIONS Release)
INSTALL(TARGETS PhotometricGobo DESTINATION Debug/lib/arnold/plugin CONFIGURATIONS Debug)
INSTALL(TARGETS PhotometricGobo DESTINATION Release/lib/arnold/plugin CONFIGURATIONS Release)
