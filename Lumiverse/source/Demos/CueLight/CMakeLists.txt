# CueLight CMake Lists file

add_executable (CueLight
	Cue.h
	CueList.h
	Playback.h
	Cue.cpp
	CueList.cpp
	Playback.cpp
	CueLightDemo.cpp)
target_link_libraries(CueLight LumiverseCore)