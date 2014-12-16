# Demo app version number
set (LumiverseDemo_VERSION_MAJOR 1)
set (LumiverseDemo_VERSION_MINOR 0)

IF(APPLE)
    SET(CLANG_FLAGS "-std=c++11 -stdlib=libc++")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CLANG_FLAGS}")
ELSEIF(UNIX)
    SET(GCC_FLAGS "-std=c++11 -pthread")
    SET(
	 "${CMAKE_CXX_FLAGS} ${GCC_FLAGS}")
    MESSAGE("Adding -std=c++11 to g++ flags for Demo")
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

configure_file (
  "${CMAKE_CURRENT_LIST_DIR}/LumiverseDemoConfig.h.in"
  "${CMAKE_CURRENT_LIST_DIR}/LumiverseDemoConfig.h"
)

include_directories("Demos/LumiverseDemo")

add_executable (Demo test.cpp LumiverseDemoConfig.h)
target_link_libraries(Demo LumiverseCore)
