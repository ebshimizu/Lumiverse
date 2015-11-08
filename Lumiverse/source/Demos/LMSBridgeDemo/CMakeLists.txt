# Demo app version number
set (LMSPBridge_VERSION_MAJOR 1)
set (LMSPBridge_VERSION_MINOR 0)

IF(APPLE)
    SET(CLANG_FLAGS "-std=c++11 -stdlib=libc++")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CLANG_FLAGS}")
ELSEIF(UNIX)
    SET(GCC_FLAGS "-std=c++11 -pthread")
    SET("${CMAKE_CXX_FLAGS} ${GCC_FLAGS}")
ENDIF(APPLE)

include_directories("Demos/LMSBridgeDemo")

add_executable (LMSBridgeDemo BridgeDemo.cpp)
target_link_libraries(LMSBridgeDemo LumiverseCore)

add_custom_command(TARGET LMSBridgeDemo PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_CURRENT_SOURCE_DIR}/PBridge $<TARGET_FILE_DIR:LMSBridgeDemo>)
