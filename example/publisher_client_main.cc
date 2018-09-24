#include <pubsub.h>
#include <sargs.h>
#include <memory>
#include "mio.h"

int main(int argc, char* argv[]) {
  SARGS_REQUIRED_FLAG_VALUE("--topic-port", "-tp", "The topic server port to connect to");
  SARGS_OPTIONAL_FLAG_VALUE("--file", "-f", "A file to publish");
  SARGS_INITIALIZE(argc, argv);

  uint16_t port = SARGS_GET_UINT16("--topic-port");
  pubsub::PublisherClient publisher_client("localhost", port);

  if (SARGS_HAS("--file")) {
    mio::mmap_source mmap(SARGS_GET_STRING("--file"), 0, mio::map_entire_file);
    publisher_client.Publish(mmap.data(), mmap.size());
  } else {
    std::string input;
    std::cout << "Enter data to send. Hit return twice to exit" << std::endl;
    for (std::string input; std::getline(std::cin, input);) {
      if (input.empty())
        break;
      publisher_client.Publish(input.c_str(), input.size() + 1);
    }
  }

  return 0;
}
