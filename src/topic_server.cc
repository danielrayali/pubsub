#include "topic_server.h"

#include <future>
#include <system_error>
#include <thread>

#include "asio.h"
#include "spdlog/spdlog.h"

#include "buffer.h"
#include "messages.h"
#include "types.h"

using namespace pubsub;
using namespace std;
using asio::ip::tcp;

namespace pubsub {

//
// TopicSession definitions
//

class TopicSession : public std::enable_shared_from_this<TopicSession> {
 public:
  TopicSession(tcp::socket socket, map<int32_t, tcp::socket>& subscribers, int32_t& id_counter) :
    socket_(std::move(socket)),
    subscribers_(subscribers),
    id_counter_(id_counter)
  {}

  ~TopicSession() = default;

  void ServiceClient() {
    spdlog::info("Servicing topic client");
    MessageType type = MessageType::kUnknown;
    asio::read(socket_, asio::buffer(&type, sizeof(MessageType)));

    if (type == MessageType::kSubRegister) {
      auto inserted = subscribers_.emplace(id_counter_, std::move(socket_));

      type = MessageType::kSubRegisterReply;
      asio::write(inserted.first->second, asio::buffer(&type, sizeof(MessageType)));
      asio::write(inserted.first->second, asio::buffer(&id_counter_, sizeof(int32_t)));

      spdlog::info("Subscriber {} registered", id_counter_);
      id_counter_++;
    } else if (type == MessageType::kSubDeregister) {
      int32_t client_id = -1;
      asio::read(socket_, asio::buffer(&client_id, sizeof(int32_t)));

      auto iter = subscribers_.find(client_id);
      if (iter == subscribers_.end()) {
        spdlog::error("Client {} is not registered, cannot deregister", client_id);
        type = MessageType::kError;
        asio::write(socket_, asio::buffer(&type, sizeof(MessageType)));
        return;
      }

      type = MessageType::kShutdown;
      asio::write(iter->second, asio::buffer(&type, sizeof(MessageType)));
      subscribers_.erase(iter);

      type = MessageType::kSubDeregisterReply;
      asio::write(socket_, asio::buffer(&type, sizeof(MessageType)));

      spdlog::info("Subscriber {} deregistered", to_string(client_id));
    } else if (type == MessageType::kPublisher) {
      spdlog::info("Receiving and sending publisher data");

      uint64_t total_data_size = 0;
      asio::read(socket_, asio::buffer(&total_data_size, sizeof(uint64_t)));

      Buffer buffer;
      uint64_t remaining = total_data_size;
      while (remaining > 0) {
        uint64_t block_size = 0;
        asio::read(socket_, asio::buffer(&block_size, sizeof(uint64_t)));

        buffer.Allocate(static_cast<size_t>(block_size));
        asio::read(socket_, asio::buffer(buffer.Get(), static_cast<size_t>(block_size)));

        type = MessageType::kData;
        for (auto& iter : subscribers_) {
          asio::write(iter.second, asio::buffer(&type, sizeof(MessageType)));
          asio::write(iter.second, asio::buffer(&block_size, sizeof(uint64_t)));
          asio::write(iter.second, asio::buffer(buffer.Get(), static_cast<size_t>(block_size)));
        }

        remaining -= block_size;
      }

      type = MessageType::kPublisherReply;
      asio::write(socket_, asio::buffer(&type, sizeof(MessageType)));

      spdlog::info("{} bytes sent to {} subscribers", total_data_size, subscribers_.size());
    } else {
      spdlog::error("Topic session cannot handle received message type: {}", ToString(type));
    }
  }

 private:
  tcp::socket socket_;
  map<int32_t, tcp::socket>& subscribers_;
  int32_t& id_counter_;
};

//
// TopicServer definitions
//

TopicServer::TopicServer(const TopicConfig& topic_config) :
  acceptor_(DefaultIoService(), tcp::endpoint(tcp::v4(), topic_config.port)),
  socket_(DefaultIoService()),
  topic_config_(topic_config),
  is_running_(false)
{}

void TopicServer::Run() {
  if (is_running_)
    return;

  spdlog::info("Starting topic server");
  this->DoAccept();
  result_ = std::async(std::launch::async, []{ DefaultIoService().run(); });
  is_running_ = true;
}

void TopicServer::Stop() {
  if (!is_running_)
    return;

  spdlog::info("Stopping topic server");
  acceptor_.cancel();
  DefaultIoService().stop();

  future_status status = result_.wait_for(chrono::milliseconds(100));
  if (status == future_status::timeout)
    spdlog::error("Timed out waiting for TopicServer thread to return. This may cause erroneous shutdown");

  MessageType type = MessageType::kShutdown;
  for (auto& client : subscribers_) {
    asio::write(client.second, asio::buffer(&type, sizeof(MessageType)));
    client.second.close();
  }

  is_running_ = false;
}

TopicConfig TopicServer::GetTopicConfig() const {
  return topic_config_;
}

void TopicServer::DoAccept() {
  acceptor_.async_accept(socket_, [this] (std::error_code ec) {
    if (!ec) {
      std::make_shared<TopicSession>(std::move(socket_), subscribers_, id_counter_)->ServiceClient();
      this->DoAccept();
    } else if (ec != asio::error::operation_aborted) {
      spdlog::error("Error in async_accpet lambda: {} {}", ec.value(), ec.message());
    }
  });
}

}  // namespace pubsub
