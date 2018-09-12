include_directories (
  ${PUBSUB_SOURCE_DIR}/src
  ${PUBSUB_SOURCE_DIR}/external/asio/asio/include
)

add_library (pubsub
  asio.cc
  asio.h
  buffer.cc
  buffer.h
  core.cc
  core.h
  master_server.cc
  master_server.h
  query_client.cc
  query_client.h
)

target_include_directories (pubsub PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
