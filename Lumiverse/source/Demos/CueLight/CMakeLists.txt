# CueLight CMake Lists file

IF(APPLE)
    SET(CLANG_FLAGS "-std=c++11 -stdlib=libc++")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CLANG_FLAGS}")
ENDIF(APPLE)

IF(UNIX)
    SET(GCC_FLAGS "-std=c++11 -pthread")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_FLAGS}")
    MESSAGE("Adding -std=c++11 to g++ flags for CueLight")
ENDIF(UNIX)

add_library (CueLight
	Cue.h
	CueList.h
	Playback.h
	Cue.cpp
	CueList.cpp
	Playback.cpp)

add_executable (CueLightDemo
	CueLightDemo.cpp)

target_link_libraries(CueLightDemo LumiverseCore)
target_link_libraries(CueLightDemo CueLight)