#include "query_client.h"
#include <string>
#include "asio.h"
#include "types.h"
#include "messages.h"
#include "logging.h"

using namespace std;
using asio::ip::tcp;

namespace pubsub {

QueryClient::QueryClient(const std::string& host, const uint16_t port) : host_(host), port_(port) {}

std::vector<std::string> QueryClient::QueryForTopics() {
  tcp::socket client(DefaultIoService());
  tcp::resolver resolver(DefaultIoService());
  asio::connect(client, resolver.resolve({host_.c_str(), std::to_string(port_)}));

  const MessageType type = MessageType::kTopicsQuery;
  asio::write(client, asio::buffer(&type, sizeof(MessageType)));

  uint32_t num_topics = 0;
  asio::read(client, asio::buffer(reinterpret_cast<char*>(&num_topics), sizeof(uint32_t)));

  vector<string> topic_ids;
  for (uint32_t i = 0; i < num_topics; ++i) {
    ByteSize topic_id_size = 0;
    asio::read(client, asio::buffer(reinterpret_cast<char*>(&topic_id_size), sizeof(ByteSize)));

    string topic_id(ToSizeT(topic_id_size), '\0');
    asio::read(client, asio::buffer(reinterpret_cast<char*>(&topic_id.front()), ToSizeT(topic_id_size)));

    topic_ids.push_back(topic_id);
  }

  return topic_ids;
}

}  // namespace pubsub
