# Lumiverse Test Suite CMake Lists file

IF(APPLE)
    SET(CLANG_FLAGS "-std=c++11 -stdlib=libc++")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CLANG_FLAGS}")
ENDIF(APPLE)

IF(UNIX)
    SET(GCC_FLAGS "-std=c++11 -pthread")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_FLAGS}")
    MESSAGE("Adding -std=c++11 to g++ flags for CueLight")
ENDIF(UNIX)

IF(LumiverseCore_INCLUDE_ARNOLD)
	find_package(PNG REQUIRED)
	IF(PNG_FOUND)
		include_directories(
			${PNG_INCLUDE_DIRS}
		)
	ENDIF(PNG_FOUND)
ENDIF(LumiverseCore_INCLUDE_ARNOLD)

add_executable(LumiverseTest
	TestMain.cpp
	DeviceTests.cpp
	DeviceTests.h
	TypeTests.cpp
	TypeTests.h
	RigTests.h
	RigTests.cpp
)

target_link_libraries(LumiverseTest LumiverseCore)
target_link_libraries(LumiverseTest CueLight)