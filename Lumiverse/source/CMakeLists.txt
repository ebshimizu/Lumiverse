cmake_minimum_required (VERSION 2.6)

project (Lumiverse)
set (LUMIVERSE_DOCS ON CACHE BOOL "Generate Documentation Project (requires Doxygen)")

# defaulting to release build
IF (NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE         "Release" CACHE STRING "Type of build (Debug Release RelWithDebInfo MinSizeRel)" FORCE)
ENDIF ()

include_directories(
  "${PROJECT_SOURCE_DIR}/LumiverseCore")
add_subdirectory(LumiverseCore)

add_subdirectory(Demos)
add_subdirectory(Test)

if (LUMIVERSE_DOCS)
  find_package(Doxygen)
  if(DOXYGEN_FOUND)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    add_custom_target(doc
      ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
      WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
      COMMENT "Generating API documentation with Doxygen" VERBATIM
    )
  endif(DOXYGEN_FOUND)
endif(LUMIVERSE_DOCS)
