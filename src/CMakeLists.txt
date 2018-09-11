add_library (pubsub 
	buffer.cc 
	buffer.h)

target_include_directories (pubsub PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})