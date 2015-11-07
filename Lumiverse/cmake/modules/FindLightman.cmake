# Find Lightman library and include paths
# This defines the following:
#
# LIGHTMAN_FOUND             If Lightman is found
# LIGHTMAN_CORE              Lightman core library
# LIGHTMAN_APP               Lightman application library
# LIGHTMAN_SIM               Lightman simulation library
# LIGHTMAN_LIBRARIES         Lightman libraries
# LIGHTMAN_INCLUDE_DIRS      Lightman headers

# Lightman core 
find_library(LIGHTMAN_CORE
    NAMES LightmanCore 
    PATHS /usr/local/lib
    DOC "Lightman core library")
set(LIGHTMAN_LIBRARIES ${LIGHTMAN_LIBRARIES} ${LIGHTMAN_CORE})

# Lightman app 
find_library(LIGHTMAN_APP
    NAMES LightmanApp 
    PATHS /usr/local/lib
    DOC "Lightman application library")
set(LIGHTMAN_LIBRARIES ${LIGHTMAN_LIBRARIES} ${LIGHTMAN_APP})

# Lightman sim 
find_library(LIGHTMAN_SIM
    NAMES LightmanSim
    PATHS /usr/local/lib
    DOC "Lightman application library")
set(LIGHTMAN_LIBRARIES ${LIGHTMAN_LIBRARIES} ${LIGHTMAN_SIM})

# Lightman headers 
find_path(LIGHTMAN_INCLUDE_DIRS
    NAMES LightmanCore.h LightmanApp.h LightmanSim.h
    PATHS /usr/local/include/Lightman
    DOC "Lightman include directories")

# Version
set(LIGHTMAN_VERSION 1.0)

# Set standard arguments for find package
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LIGHTMAN 
    REQUIRED_VARS LIGHTMAN_LIBRARIES LIGHTMAN_INCLUDE_DIRS
    VERSION_VAR LIGHTMAN_VERSION)
