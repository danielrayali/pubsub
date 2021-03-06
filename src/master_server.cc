#include "master_server.h"

#include <fstream>
#include <future>
#include <set>
#include <system_error>
#include <thread>

#include "spdlog/spdlog.h"
#include "asio.h"

#include "config.h"
#include "messages.h"
#include "types.h"

using namespace pubsub;
using namespace std;
using asio::ip::tcp;

namespace pubsub {

//
// MasterSession definitions
//

class MasterSession : public std::enable_shared_from_this<MasterSession> {
 public:
  MasterSession(tcp::socket socket, vector<unique_ptr<TopicServer>>& topic_servers) :
    socket_(std::move(socket)),
    topic_servers_(topic_servers)
  {}

  ~MasterSession() = default;

  void ServiceClient() {
    MessageType type = MessageType::kUnknown;
    asio::read(socket_, asio::buffer(&type, sizeof(MessageType)));

    if (type == MessageType::kTopicsQuery) {
      spdlog::info("Servicing topics query client");

      type = MessageType::kTopicsQueryReply;
      asio::write(socket_, asio::buffer(&type, sizeof(MessageType)));

      uint32_t num_topics = static_cast<uint32_t>(topic_servers_.size());
      asio::write(socket_, asio::buffer(&num_topics, sizeof(uint32_t)));

      for (uint32_t i = 0; i < num_topics; i++) {
        string topic_config = topic_servers_[i]->GetTopicConfig().ToString();
        ByteSize byte_size = topic_config.size();
        asio::write(socket_, asio::buffer(&byte_size, sizeof(ByteSize)));
        asio::write(socket_, asio::buffer(&topic_config.front(), topic_config.size()));
      }

      spdlog::info("Sent {} topics", num_topics);
    } else if (type == MessageType::kTopicAdd) {
      spdlog::info("Servicing topic add client");

      uint64_t size = 0;
      asio::read(socket_, asio::buffer(&size, sizeof(uint64_t)));
      string buffer(static_cast<size_t>(size), '\0');
      asio::read(socket_, asio::buffer(&buffer.front(), static_cast<size_t>(size)));

      TopicConfig topic_config;
      topic_config.FromString(buffer);

      spdlog::info("Adding topic");
      spdlog::info("  Name: {}", topic_config.name);
      spdlog::info("  Port: {}", topic_config.port);
      topic_servers_.emplace_back(new TopicServer(topic_config));
      topic_servers_.back()->Run();

      type = MessageType::kTopicAddReply;
      asio::write(socket_, asio::buffer(&type, sizeof(MessageType)));
    } else {
      spdlog::error("Master session cannot handle received message type: {}", ToString(type));
    }
  }

 private:
  tcp::socket socket_;
  vector<unique_ptr<TopicServer>>& topic_servers_;
};

//
// MasterServer definitions
//

MasterServer::MasterServer(const string& path) :
  acceptor_(DefaultIoService()),
  socket_(DefaultIoService()),
  is_running_(false)
{
  ifstream input(path, ifstream::app);
  streampos pos = input.tellg();
  input.seekg(0);
  string buffer(static_cast<unsigned>(pos), '\0');
  input.read(&buffer.front(), pos);
  config_.FromString(buffer);

  this->Configure();
}

MasterServer::MasterServer(const Config& config) :
  acceptor_(DefaultIoService()),
  socket_(DefaultIoService()),
  config_(config),
  is_running_(false)
{
  this->Configure();
}

MasterServer::~MasterServer() {
  this->Stop();

  for (auto& topic_server : topic_servers_)
    topic_server->Stop();
}

void MasterServer::Run() {
  if (is_running_)
    return;

  spdlog::info("Starting master server");
  this->DoAccept();
  result_ = std::async(std::launch::async, []{ DefaultIoService().run(); });
  is_running_ = true;
}

void MasterServer::Stop() {
  if (!is_running_)
    return;
  spdlog::info("Stopping master server");

  acceptor_.cancel();

  for (auto& topic_server : topic_servers_)
    topic_server->Stop();

  DefaultIoService().stop();
  future_status status = result_.wait_for(chrono::milliseconds(100));
  if (status == future_status::timeout)
    spdlog::error("Timed out waiting for MasterServer thread to return. This may cause erroneous shutdown");

  is_running_ = false;
}

void MasterServer::DoAccept() {
  acceptor_.async_accept(socket_, [this] (std::error_code ec) {
    if (!ec) {
      std::make_shared<MasterSession>(std::move(socket_), topic_servers_)->ServiceClient();
      this->DoAccept();
    } else if (ec != asio::error::operation_aborted) {
      spdlog::error("Error in async_accpet lambda: {} {}", ec.value(), ec.message());
    }
  });
}

void MasterServer::Configure() {
  tcp::endpoint endpoint(tcp::v4(), config_.master_port);
  acceptor_.open(endpoint.protocol());
  acceptor_.bind(endpoint);
  acceptor_.listen();

  set<uint16_t> ports;
  for (const TopicConfig& config : config_.topic_configs) {
    auto iter = ports.find(config.port);
    if (iter != ports.end()) {
      throw runtime_error("Configuration invalid, port already in use");
    }

    ports.insert(config.port);
    topic_servers_.emplace_back();
    topic_servers_.back().reset(new TopicServer(config));
  }

  for (auto& topic_server : topic_servers_)
    topic_server->Run();
}

}  // namespace pubsub
