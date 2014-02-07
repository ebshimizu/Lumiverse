cmake_minimum_required (VERSION 2.6)

project (LumiverseDemo)
# Demo app version number
set (LumiverseDemo_VERSION_MAJOR 1)
set (LumiverseDemo_VERSION_MINOR 0)

configure_file (
  "${PROJECT_SOURCE_DIR}/LumiverseDemo/LumiverseDemoConfig.h.in"
  "${PROJECT_BINARY_DIR}/LumiverseDemo/LumiverseDemoConfig.h"
)

include_directories("${PROJECT_BINARY_DIR}/LumiverseDemo"
  "${PROJECT_SOURCE_DIR}/LumiverseCore"
  "${PROJECT_SOURCE_DIR}/LumiverseCore")
add_subdirectory(LumiverseCore)

add_executable (Demo LumiverseDemo/test.cpp)
target_link_libraries(Demo LumiverseCore)