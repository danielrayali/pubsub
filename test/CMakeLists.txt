include_directories (
	${PUBSUB_SOURCE_DIR}/src
	${PUBSUB_SOURCE_DIR}/external/googletest/googletest/include
	${PUBSUB_SOURCE_DIR}/external/googletest/googletest
	)

add_executable (pubsubtest
	${PUBSUB_SOURCE_DIR}/external/googletest/googletest/src/gtest-all.cc
	${PUBSUB_SOURCE_DIR}/external/googletest/googletest/src/gtest_main.cc
	buffer_test.cc)

target_link_libraries (pubsubtest LINK_PUBLIC pubsub)