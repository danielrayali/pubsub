#include "master_client.h"
#include "asio.h"
#include "messages.h"
#include "logging.h"
#include "types.h"

using namespace std;
using asio::ip::tcp;

namespace pubsub {

MasterClient::MasterClient(const string& host, const uint16_t port) :
  host_(host),
  port_(port)
{}

vector<string> MasterClient::QueryForTopics() {
  tcp::socket client(DefaultIoService());
  tcp::resolver resolver(DefaultIoService());
  asio::connect(client, resolver.resolve({host_.c_str(), std::to_string(port_)}));

  MessageType type = MessageType::kTopicsQuery;
  asio::write(client, asio::buffer(&type, sizeof(MessageType)));

  asio::read(client, asio::buffer(&type, sizeof(MessageType)));
  if (type != MessageType::kTopicsQueryReply) {
    Error() << "Query client did not receive standard reply type" << ToString(type) << endl;
    return {};
  }

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

void MasterClient::AddTopic(const TopicConfig& topic_config) {
  tcp::socket client(DefaultIoService());
  tcp::resolver resolver(DefaultIoService());
  asio::connect(client, resolver.resolve({host_.c_str(), std::to_string(port_)}));

  MessageType type = MessageType::kTopicAdd;
  asio::write(client, asio::buffer(&type, sizeof(MessageType)));

  string buffer = topic_config.ToString();
  uint64_t size = static_cast<uint64_t>(buffer.size());
  asio::write(client, asio::buffer(&size, sizeof(uint64_t)));
  asio::write(client, asio::buffer(buffer.c_str(), buffer.size()));

  asio::read(client, asio::buffer(&type, sizeof(MessageType)));
  if (type != MessageType::kTopicAddReply)
    Error() << "Master client AddTopic did not receive AddTopicReply" << ToString(type) << endl;
}

void MasterClient::RemoveTopic(const std::string& topic_id) {

}

}  // namespace pubsub
