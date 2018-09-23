#include <pubsub.h>
#include <sargs.h>
#include <memory>
#include "mio.h"

int main(int argc, char* argv[]) {
  SARGS_REQUIRED_FLAG_VALUE("--topic-port", "-tp", "The topic server port to connect to");
  SARGS_OPTIONAL_FLAG_VALUE("--block-size", "-bs", "The block size to use when sending data. All data is set by default");
  SARGS_OPTIONAL_FLAG_VALUE("--file", "-f", "A file to publish");
  SARGS_INITIALIZE(argc, argv);

  uint16_t port = SARGS_GET_UINT16("--topic-port");
  std::unique_ptr<pubsub::PublisherClient> publisher_client;
  if (SARGS_HAS("--block-size")) {
    uint64_t block_size = SARGS_GET_UINT64("--block-size");
    publisher_client.reset(new pubsub::PublisherClient("localhost", port, block_size));
  } else {
    publisher_client.reset(new pubsub::PublisherClient("localhost", port));
  }

  if (SARGS_HAS("--file")) {
    mio::mmap_source mmap(SARGS_GET_STRING("--file"), 0, mio::map_entire_file);
    publisher_client->Publish(mmap.data(), mmap.size());
  } else {
    std::string data = "Hello, World!";
    publisher_client->Publish(data.c_str(), data.size() + 1);
  }

  return 0;
}
