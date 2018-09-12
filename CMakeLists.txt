# This matches the default cmake version on Ubuntu 16.04
cmake_minimum_required (VERSION 3.5.1)

project (PUBSUB)

if (UNIX)
  # Need to set this explicitly for gcc/g++
  SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++11")
endif ()

add_subdirectory (src)
add_subdirectory (test)
add_subdirectory (example)
