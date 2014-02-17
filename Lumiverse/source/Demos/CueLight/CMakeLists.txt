# CueLight CMake Lists file

add_executable (CueLight
	Cue.h
	CueList.h
	Playback.h
	Cue.cpp
	CueList.cpp
	Playback.h
	CueLightDemo.cpp)
target_link_libraries(CueLight LumiverseCore)