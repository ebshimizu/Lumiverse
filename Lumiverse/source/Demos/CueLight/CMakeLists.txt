# CueLight CMake Lists file

add_executable (CueLight
	Cue.h
	CueList.h
	Cue.cpp
	CueList.cpp
	CueLightDemo.cpp)
target_link_libraries(CueLight LumiverseCore)