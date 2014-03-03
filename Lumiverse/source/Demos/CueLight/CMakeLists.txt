# CueLight CMake Lists file

IF(APPLE)
    SET(CLANG_FLAGS "-std=c++11 -stdlib=libc++")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CLANG_FLAGS}")
ENDIF(APPLE)

add_executable (CueLight
	Cue.h
	CueList.h
	Playback.h
	Cue.cpp
	CueList.cpp
	Playback.cpp
	CueLightDemo.cpp)
target_link_libraries(CueLight LumiverseCore)