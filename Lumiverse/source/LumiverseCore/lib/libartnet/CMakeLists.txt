include_directories("${CMAKE_CURRENT_LIST_DIR}")

add_library(libartnet
	artnet.c
	misc.c
	network.c
	receive.c
	tod.c
	transmit.c
)

INSTALL(TARGETS libartnet ARCHIVE DESTINATION lib)