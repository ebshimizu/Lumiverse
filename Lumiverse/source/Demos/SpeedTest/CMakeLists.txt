IF(APPLE)
    SET(CLANG_FLAGS "-std=c++11 -stdlib=libc++")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CLANG_FLAGS}")
ELSEIF(UNIX)
    SET(GCC_FLAGS "-std=c++11 -pthread")
    SET(
	 "${CMAKE_CXX_FLAGS} ${GCC_FLAGS}")
    MESSAGE("Adding -std=c++11 to g++ flags for Demo")
ENDIF(APPLE)

IF(LumiverseCore_INCLUDE_ARNOLD)
	find_package(PNG REQUIRED)
	IF(PNG_FOUND)
		include_directories(
			${PNG_INCLUDE_DIRS}
		)
	ENDIF(PNG_FOUND)
ENDIF(LumiverseCore_INCLUDE_ARNOLD)
include_directories("${CMAKE_CURRENT_LIST_DIR}/../../LumiverseShowControl")

add_executable (SpeedTest speed.cpp)
target_link_libraries(SpeedTest LumiverseCore LumiverseShowControl)
