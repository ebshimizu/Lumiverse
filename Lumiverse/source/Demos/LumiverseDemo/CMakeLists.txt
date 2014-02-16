# Demo app version number
set (LumiverseDemo_VERSION_MAJOR 1)
set (LumiverseDemo_VERSION_MINOR 0)

IF(UNIX)
    SET(GCC_FLAGS "-std=c++11 -pthread")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_FLAGS}")
    MESSAGE("Adding -std=c++11 to g++ flags")
ENDIF(UNIX)

configure_file (
  "LumiverseDemoConfig.h.in"
  "${PROJECT_BINARY_DIR}/Demos/LumiverseDemo/LumiverseDemoConfig.h"
)

include_directories("${PROJECT_BINARY_DIR}/Demos/LumiverseDemo")

add_executable (Demo test.cpp)
target_link_libraries(Demo LumiverseCore)
