# Demo app version number
set (LumiverseDemo_VERSION_MAJOR 1)
set (LumiverseDemo_VERSION_MINOR 0)

configure_file (
  "LumiverseDemoConfig.h.in"
  "${PROJECT_BINARY_DIR}/Demos/LumiverseDemo/LumiverseDemoConfig.h"
)

include_directories("${PROJECT_BINARY_DIR}/Demos/LumiverseDemo")

add_executable (Demo test.cpp)
target_link_libraries(Demo LumiverseCore)
