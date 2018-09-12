include_directories (
  ${PUBSUB_SOURCE_DIR}/src
  ${PUBSUB_SOURCE_DIR}/include
  ${PUBSUB_SOURCE_DIR}/external/asio/asio/include
)

add_executable (master_server
  master_server_main.cc
)

add_executable (query_client
  query_client_main.cc
)

target_link_libraries (master_server LINK_PUBLIC pubsub)
target_link_libraries (query_client LINK_PUBLIC pubsub)
