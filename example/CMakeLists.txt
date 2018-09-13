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

add_executable (topic_server
  topic_server_main.cc
)

add_executable (subscriber_client
  subscriber_client_main.cc
)

if (UNIX)
  # Asio requires us to link pthreads when using gcc
  target_link_libraries (master_server LINK_PUBLIC pubsub pthread)
  target_link_libraries (topic_server LINK_PUBLIC pubsub pthread)
  target_link_libraries (query_client LINK_PUBLIC pubsub pthread)
  target_link_libraries (subscriber_client LINK_PUBLIC pubsub pthread)
else ()
  target_link_libraries (master_server LINK_PUBLIC pubsub)
  target_link_libraries (topic_server LINK_PUBLIC pubsub)
  target_link_libraries (query_client LINK_PUBLIC pubsub)
  target_link_libraries (subscriber_client LINK_PUBLIC pubsub)
endif ()
