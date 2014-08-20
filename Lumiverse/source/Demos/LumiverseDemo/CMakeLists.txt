# Demo app version number
set (LumiverseDemo_VERSION_MAJOR 1)
set (LumiverseDemo_VERSION_MINOR 0)

IF(UNIX)
    SET(GCC_FLAGS "-std=c++11 -pthread")
    SET(
	 "${CMAKE_CXX_FLAGS} ${GCC_FLAGS}")
    MESSAGE("Adding -std=c++11 to g++ flags for Demo")
ENDIF(UNIX)

# TODO: Bug with machine:x86 linker option
SET(CMAKE_EXE_LINKER_FLAGS "")
SET(CMAKE_MODULE_LINKER_FLAGS "")
SET(CMAKE_SHARED_LINKER_FLAGS "")

configure_file (
  "LumiverseDemoConfig.h.in"
  "Demos/LumiverseDemo/LumiverseDemoConfig.h"
)

include_directories("Demos/LumiverseDemo")

add_executable (Demo test.cpp)
target_link_libraries(Demo LumiverseCore)
