add_subdirectory(libjson)
add_subdirectory(clp)
add_subdirectory(Eigen)

IF (LumiverseCore_INCLUDE_ARNOLD)
	add_subdirectory(arnold/Driver)
	add_subdirectory(arnold/Shader)
ENDIF (LumiverseCore_INCLUDE_ARNOLD)