# - Find Lumiverse
# Find the native Lumiverse includes and library
#
#  LUMIVERSE_INCLUDE_DIR    - where to find .h
#  LUMIVERSE_LIBRARIES   - List of libraries when using Lumiverse.
#  LUMIVERSE_FOUND       - True if Lumiverse found.

message("Searching for Lumiverse library.")  
if (LUMIVERSE_INCLUDE_DIR)
  # Already in cache, be silent
  set (LUMIVERSE_FIND_QUIETLY TRUE)
endif (LUMIVERSE_INCLUDE_DIR)

find_path (LUMIVERSE_INCLUDE_DIR LumiverseCore.h
  HINTS /usr/local/lib/Lumiverse/include /usr/include/Lumiverse/include C:/lib/Lumiverse/include
)

find_library(LUMIVERSE_CORE_LIBRARY 
  NAMES LumiverseCore.lib LumiverseCore.a LumiverseCore.so
  HINTS /usr/local/lib/Lumiverse/lib C:/lib/Lumiverse/lib
)

find_library(LUMIVERSE_CONTROL_LIBRARY 
  NAMES LumiverseShowControl.lib LumiverseShowControl.a LumiverseShowControl.so
  HINTS /usr/local/lib/Lumiverse/lib C:/lib/Lumiverse/lib
)

find_library(LUMIVERSE_CLP_LIBRARY 
  NAMES clp.lib clp.a clp.so
  HINTS /usr/local/lib/Lumiverse/lib C:/lib/Lumiverse/lib
)

find_library(LUMIVERSE_JSON_LIBRARY 
  NAMES libjson.lib libjson.a libjson.so
  HINTS /usr/local/lib/Lumiverse/lib C:/lib/Lumiverse/lib
)

SET(LUMIVERSE_LIBRARIES ${LUMIVERSE_CORE_LIBRARY} ${LUMIVERSE_CONTROL_LIBRARY} ${LUMIVERSE_CLP_LIBRARY} ${LUMIVERSE_JSON_LIBRARY})

find_library(LUMIVERSE_ARNOLD_LIBRARY 
  NAMES ai.lib ai.a ai.so
  HINTS /usr/local/lib/Lumiverse/lib C:/lib/Lumiverse/lib
)

IF (LUMIVERSE_ARNOLD_LIBRARY)
  find_package(PNG REQUIRED)

  IF(PNG_FOUND)
    SET(LUMIVERSE_INCLUDE_DIR ${LUMIVERSE_INCLUDE_DIR} ${PNG_INCLUDE_DIRS})
    SET (LUMIVERSE_LIBRARIES ${LUMIVERSE_LIBRARIES} ${PNG_LIBRARIES})
  ENDIF(PNG_FOUND)

  SET(LUMIVERSE_LIBRARIES ${LUMIVERSE_LIBRARIES} ${LUMIVERSE_ARNOLD_LIBRARY})
ENDIF (LUMIVERSE_ARNOLD_LIBRARY)

find_library(LUMIVERSE_FTD2XX_LIBRARY 
  NAMES ftd2xx.lib ftd2xx.a ftd2xx.so
  HINTS /usr/local/lib/Lumiverse/lib C:/lib/Lumiverse/lib
)

IF (LUMIVERSE_FTD2XX_LIBRARY)
  SET(LUMIVERSE_LIBRARIES ${LUMIVERSE_LIBRARIES} ${LUMIVERSE_FTD2XX_LIBRARY})
ENDIF (LUMIVERSE_FTD2XX_LIBRARY)

find_library(LUMIVERSE_ARTNET_LIBRARY 
  NAMES libartnet.lib libartnet.a libartnet.so
  HINTS /usr/local/lib/Lumiverse/lib C:/lib/Lumiverse/lib
)

IF (LUMIVERSE_ARTNET_LIBRARY)
  SET(LUMIVERSE_LIBRARIES ${LUMIVERSE_LIBRARIES} ${LUMIVERSE_ARTNET_LIBRARY})
ENDIF (LUMIVERSE_ARTNET_LIBRARY)

# handle the QUIETLY and REQUIRED arguments and set OLA_FOUND to TRUE if
# all listed variables are TRUE
include (FindPackageHandleStandardArgs)
find_package_handle_standard_args (LUMIVERSE DEFAULT_MSG LUMIVERSE_LIBRARIES LUMIVERSE_INCLUDE_DIR)

mark_as_advanced (LUMIVERSE_LIBRARIES LUMIVERSE_INCLUDE_DIR)
