IF(LumiverseCore_INCLUDE_ARNOLD)
    IF(APPLE)
        SET(CLANG_FLAGS "-std=c++11 -stdlib=libc++")
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CLANG_FLAGS}")
    ELSEIF(UNIX)
        SET(GCC_FLAGS "-std=c++11 -pthread")
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_FLAGS}")
        MESSAGE("Adding -std=c++11 to g++ flags for Test")
    ENDIF(APPLE)

        find_package(PNG REQUIRED)
        IF(PNG_FOUND)
            include_directories(
                ${PNG_INCLUDE_DIRS}
            )
        ENDIF(PNG_FOUND)

    include_directories("${CMAKE_CURRENT_LIST_DIR}/../LumiverseShowControl")

    SET(LUMIVERSE_ARNOLD_DEBUG
        Main.cpp
    )

    add_executable(ArnoldDebug ${LUMIVERSE_ARNOLD_DEBUG})

    target_link_libraries(ArnoldDebug LumiverseCore)
    target_link_libraries(ArnoldDebug LumiverseShowControl)

ENDIF(LumiverseCore_INCLUDE_ARNOLD)