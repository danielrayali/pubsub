include_directories (
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/external/asio/asio/include
)

add_library (pubsub
  asio.cc
  asio.h
  buffer.cc
  buffer.h
  buffer_ptr.h
  config.cc
  config.h
  core.cc
  core.h
  logging.cc
  logging.h
  master_server.cc
  master_server.h
  messages.cc
  messages.h
  publisher_client.cc
  publisher_client.h
  master_client.cc
  master_client.h
  subscriber_client.cc
  subscriber_client.h
  topic_server.cc
  topic_server.h
  types.h
)

target_include_directories (pubsub PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
