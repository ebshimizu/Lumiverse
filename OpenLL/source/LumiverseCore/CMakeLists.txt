include_directories("${CMAKE_CURRENT_LIST_DIR}/types"
  "${CMAKE_CURRENT_LIST_DIR}/DMX"
  "${CMAKE_CURRENT_LIST_DIR}/lib/libjson"
  "${CMAKE_CURRENT_LIST_DIR}/lib/ftd2xx")

add_subdirectory(lib)

add_library(LumiverseCore
    LumiverseCore.h
    Logger.h
    Logger.cpp
    Device.h
    Device.cpp
    Rig.h
    Rig.cpp
    DeviceSet.h
    DeviceSet.cpp
    OpenLLType.h
    Patch.h
    types/OpenLLFloat.h
    types/OpenLLFloat.cpp
    DMX/DMXPatch.h
    DMX/DMXPatch.cpp
    DMX/DMXDevicePatch.h
    DMX/DMXDevicePatch.cpp
    DMX/DMXInterface.h
    DMX/DMXPro2Interface.h
    DMX/pro_driver.h
    DMX/DMXPro2Interface.cpp)

target_link_libraries(LumiverseCore libjson)

# Make optional at some point for people to enable or disable desired interfaces
# CMake is a bit weird and wants this defined here instead of inside the ftd2xx folder
add_library(ftd2xx STATIC IMPORTED)
set_property(TARGET ftd2xx PROPERTY IMPORTED_LOCATION
    ${PROJECT_SOURCE_DIR}/LumiverseCore/lib/ftd2xx/ftd2xx.lib)
target_link_libraries(LumiverseCore ftd2xx)