# CueLight CMake Lists file

IF(APPLE)
    SET(CLANG_FLAGS "-std=c++11 -stdlib=libc++")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CLANG_FLAGS}")
ELSEIF(UNIX)
    SET(GCC_FLAGS "-std=c++11 -pthread")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_FLAGS}")
    MESSAGE("Adding -std=c++11 to g++ flags for CueLight")
ENDIF(APPLE)

# TODO: Bug with machine:x86 linker option
SET(CMAKE_EXE_LINKER_FLAGS "")
SET(CMAKE_MODULE_LINKER_FLAGS "")
SET(CMAKE_SHARED_LINKER_FLAGS "")

IF(LumiverseCore_INCLUDE_ARNOLD)
	find_package(PNG REQUIRED)
	IF(PNG_FOUND)
		include_directories(
			${PNG_INCLUDE_DIRS}
		)
	ENDIF(PNG_FOUND)
ENDIF(LumiverseCore_INCLUDE_ARNOLD)

set (LUMIVERSE_SHOW_CONTROL_SOURCE
  Cue.h
  CueList.h
  Playback.h
  Layer.h
  Programmer.h
  Snapshot.h
  Cue.cpp
  CueList.cpp
  Playback.cpp
  Layer.cpp
  Programmer.cpp
  Snapshot.cpp
  Timeline.h
  Timeline.cpp
  Keyframe.h
  Keyframe.cpp
  SineWave.h
  SineWave.cpp
  LumiverseShowControl.h)

add_library (LumiverseShowControl
  ${LUMIVERSE_SHOW_CONTROL_SOURCE}
)

add_executable (PlaybackDemo
	CueLightDemo.cpp)

target_link_libraries(LumiverseShowControl LumiverseCore)
target_link_libraries(PlaybackDemo LumiverseCore LumiverseShowControl)

FILE(GLOB cuefiles ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
INSTALL(FILES ${cuefiles} DESTINATION include/LumiverseShowControl)
INSTALL(TARGETS LumiverseShowControl ARCHIVE DESTINATION lib)
