#include "publisher_client.h"
#include <string>
#include "asio.h"
#include "buffer.h"
#include "logging.h"
#include "messages.h"

using asio::ip::tcp;
using namespace pubsub;
using namespace std;

namespace pubsub {

PublisherClient::PublisherClient(const string& host, const uint16_t port) :
  host_(host),
  port_(port),
  block_size_(0)
{}

PublisherClient::PublisherClient(const string& host, const uint16_t port, const uint64_t block_size) :
  host_(host),
  port_(port),
  block_size_(block_size)
{}

void PublisherClient::Publish(const void* data, const uint64_t size) const {
  tcp::socket client(DefaultIoService());
  tcp::resolver resolver(DefaultIoService());
  asio::connect(client, resolver.resolve({host_.c_str(), std::to_string(port_)}));

  MessageType type = MessageType::kPublisher;
  asio::write(client, asio::buffer(&type, sizeof(MessageType)));
  asio::write(client, asio::buffer(&size, sizeof(uint64_t)));

  uint64_t count = 0;
  uint64_t remaining = size;
  const Byte* current = reinterpret_cast<const Byte*>(data);
  while (remaining > 0) {
    uint64_t to_send;
    if (block_size_ == 0)
      to_send = remaining;
    else
      to_send = min(remaining, block_size_);

    asio::write(client, asio::buffer(&to_send, sizeof(uint64_t)));
    asio::write(client, asio::buffer(current, static_cast<size_t>(to_send)));
    current += to_send;
    remaining -= to_send;
    count++;
  }

  Log() << "Published " << count << " blocks, containing " << size << " bytes" << endl;

  asio::read(client, asio::buffer(&type, sizeof(MessageType)));

  if (type != MessageType::kPublisherReply)
    Error() << "Problem publishing data. Error received: " << ToString(type) << endl;
}

}
