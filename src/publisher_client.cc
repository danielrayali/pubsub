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
  port_(port)
{}

void PublisherClient::Publish(const void* const data, const uint64_t size) const {
  tcp::socket client(DefaultIoService());
  tcp::resolver resolver(DefaultIoService());
  asio::connect(client, resolver.resolve({host_.c_str(), std::to_string(port_)}));

  MessageType type = MessageType::kPublisher;
  asio::write(client, asio::buffer(&type, sizeof(MessageType)));
  asio::write(client, asio::buffer(&size, sizeof(uint64_t)));
  asio::write(client, asio::buffer(data, size));

  asio::read(client, asio::buffer(&type, sizeof(MessageType)));

  if (type != MessageType::kPublisherReply)
    Error() << "Problem publishing data. Error received: " << ToString(type) << endl;
}

}
