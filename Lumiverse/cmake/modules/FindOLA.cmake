# - Find OLA
# Find the native OLA includes and library
#
#  OLA_INCLUDE_DIR    - where to find .h
#  OLA_LIBRARIES   - List of libraries when using OLA.
#  OLA_FOUND       - True if OLA found.

message("Searching for OLA library.")  
if (OLA_INCLUDE_DIR)
  # Already in cache, be silent
  set (OLA_FIND_QUIETLY TRUE)
endif (OLA_INCLUDE_DIR)

find_path (OLA_INCLUDE_DIR ola
  HINTS /usr/local/lib /usr/include
  )

find_library(OLA_LIBRARY 
  NAMES libola.so libola.a
  HINTS /usr/local/lib /lib
  )

find_library(OLA_COMMON_LIBRARY 
  NAMES libolacommon.so libolacommon.a
  HINTS /usr/local/lib /lib
  )

SET(OLA_LIBRARIES ${OLA_LIBRARY} ${OLA_COMMON_LIBRARY})

# handle the QUIETLY and REQUIRED arguments and set OLA_FOUND to TRUE if
# all listed variables are TRUE
include (FindPackageHandleStandardArgs)
find_package_handle_standard_args (OLA DEFAULT_MSG OLA_LIBRARIES OLA_INCLUDE_DIR)

mark_as_advanced (OLA_LIBRARIES OLA_INCLUDE_DIR)
