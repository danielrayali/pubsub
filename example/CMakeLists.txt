include_directories (
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/external/asio/asio/include
  ${CMAKE_SOURCE_DIR}/external/sargs/src
  ${CMAKE_SOURCE_DIR}/external/mio/include
)

add_executable (master_server
  master_server_main.cc
)

add_executable (master_client
  master_client_main.cc
)

add_executable (topic_server
  topic_server_main.cc
)

add_executable (subscriber_client
  subscriber_client_main.cc
)

add_executable (publisher_client
  publisher_client_main.cc
  mio.h
)

if (UNIX)
  # Asio requires us to link pthreads when using gcc
  target_link_libraries (master_server LINK_PUBLIC pubsub pthread)
  target_link_libraries (topic_server LINK_PUBLIC pubsub pthread)
  target_link_libraries (master_client LINK_PUBLIC pubsub pthread)
  target_link_libraries (subscriber_client LINK_PUBLIC pubsub pthread)
  target_link_libraries (publisher_client LINK_PUBLIC pubsub pthread)
else ()
  target_link_libraries (master_server LINK_PUBLIC pubsub)
  target_link_libraries (topic_server LINK_PUBLIC pubsub)
  target_link_libraries (master_client LINK_PUBLIC pubsub)
  target_link_libraries (subscriber_client LINK_PUBLIC pubsub)
  target_link_libraries (publisher_client LINK_PUBLIC pubsub)
endif ()
