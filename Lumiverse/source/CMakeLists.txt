cmake_minimum_required (VERSION 2.6)

project (Lumiverse)
set (LUMIVERSE_DOCS ON CACHE BOOL "Generate Documentation Project (requires Doxygen)")

# defaulting to release build
IF (NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE         "Release" CACHE STRING "Type of build (Debug Release RelWithDebInfo MinSizeRel)" FORCE)
ENDIF ()


# NOTE(kayvonf): needed to pull all the Arnold config out here so it
# is in scope for the Demo and Tests directories (not just
# LumiverseCore).  We may wish to remove the LumiverseCore_ prefix so
# as to not be sloppy.

set (LumiverseCore_INCLUDE_ARNOLD OFF CACHE BOOL   "Build LumiverseCore with Arnold Simulator")
set (LumiverseCore_ARNOLD_PATH "" CACHE PATH "Location of Arnold Renderer tree")

IF (LumiverseCore_INCLUDE_ARNOLD)

  # make sure the an Arnold path was given
  IF (NOT EXISTS ${LumiverseCore_ARNOLD_PATH})
  	message( FATAL_ERROR "INCLUDE_ARNOLD is TRUE but no ARNOLD_PATH specified." )
  ENDIF (NOT EXISTS ${LumiverseCore_ARNOLD_PATH})
  
  SET (LumiverseCore_USE_ARNOLD "#define USE_ARNOLD")
ENDIF (LumiverseCore_INCLUDE_ARNOLD)

include_directories(
  "${PROJECT_SOURCE_DIR}/LumiverseCore")
add_subdirectory(LumiverseCore)
add_subdirectory(LumiverseShowControl)

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
