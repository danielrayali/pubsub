include_directories (
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/external/googletest/googletest/include
  ${CMAKE_SOURCE_DIR}/external/googletest/googletest
)

add_executable (pubsub_test
  ${CMAKE_SOURCE_DIR}/external/googletest/googletest/src/gtest-all.cc
  ${CMAKE_SOURCE_DIR}/external/googletest/googletest/src/gtest_main.cc
  buffer_test.cc
  config_test.cc
)

if (UNIX)
    # Google Test needs pthread when using gcc/g++
    target_link_libraries (pubsub_test LINK_PUBLIC pubsub pthread)
else ()
    target_link_libraries (pubsub_test LINK_PUBLIC pubsub)
endif ()
