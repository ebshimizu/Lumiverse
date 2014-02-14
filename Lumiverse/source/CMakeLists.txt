cmake_minimum_required (VERSION 2.6)

project (Lumiverse)

include_directories(
  "${PROJECT_SOURCE_DIR}/LumiverseCore"
  "${PROJECT_SOURCE_DIR}/LumiverseCore")
add_subdirectory(LumiverseCore)

add_subdirectory(Demos)