#include "topic_server.h"
#include <future>
#include <system_error>
#include <thread>
#include "asio.h"
#include "buffer.h"
#include "logging.h"
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
    Log() << "Servicing topic client" << endl;
    MessageType type = MessageType::kUnknown;
    asio::read(socket_, asio::buffer(&type, sizeof(MessageType)));

    if (type == MessageType::kSubRegister) {
      auto inserted = subscribers_.emplace(id_counter_, std::move(socket_));

      type = MessageType::kSubRegisterReply;
      asio::write(inserted.first->second, asio::buffer(&type, sizeof(MessageType)));
      asio::write(inserted.first->second, asio::buffer(&id_counter_, sizeof(int32_t)));

      Log() << "Subscriber " << id_counter_ << " registered" << endl;
      id_counter_++;
    } else if (type == MessageType::kSubDeregister) {
      int32_t client_id = -1;
      asio::read(socket_, asio::buffer(&client_id, sizeof(int32_t)));

      auto iter = subscribers_.find(client_id);
      if (iter == subscribers_.end()) {
        Log() << "Client " << client_id << " is not registered, cannot deregister" << endl;
        type = MessageType::kError;
        asio::write(socket_, asio::buffer(&type, sizeof(MessageType)));
        return;
      }

      type = MessageType::kShutdown;
      asio::write(iter->second, asio::buffer(&type, sizeof(MessageType)));
      subscribers_.erase(iter);

      type = MessageType::kSubDeregisterReply;
      asio::write(socket_, asio::buffer(&type, sizeof(MessageType)));

      Log() << "Subscriber " + to_string(client_id) + " deregistered" << endl;
    } else if (type == MessageType::kPublisher) {
      Log() << "Receiving and sending publisher data" << endl;

      uint64_t data_size = 0;
      asio::read(socket_, asio::buffer(&data_size, sizeof(uint64_t)));

      size_t data_size_t = static_cast<size_t>(data_size);
      Buffer buffer(data_size_t);
      asio::read(socket_, asio::buffer(buffer.Get(), data_size_t));

      printf("TopicServer received %s\n", reinterpret_cast<char*>(buffer.Get()));

      type = MessageType::kData;
      for (auto& iter : subscribers_) {
        asio::write(iter.second, asio::buffer(&type, sizeof(MessageType)));
        asio::write(iter.second, asio::buffer(&data_size, sizeof(uint64_t)));
        asio::write(iter.second, asio::buffer(buffer.Get(), data_size_t));
      }

      type = MessageType::kPublisherReply;
      asio::write(socket_, asio::buffer(&type, sizeof(MessageType)));

      Log() << "Data sent to " << subscribers_.size() << " subscribers" << endl;
    } else {
      Error() << "Topic session cannot handle received message type: " << ToString(type) << endl;
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
  socket_(DefaultIoService()), topic_config_(topic_config)
{}

TopicServer::TopicServer(TopicServer&& other) :
  acceptor_(DefaultIoService()),
  socket_(DefaultIoService())
{
  if (&other == this)
    return;

  acceptor_ = std::move(other.acceptor_);
  socket_ = std::move(other.socket_);
  result_ = std::move(other.result_);
  subscribers_ = std::move(other.subscribers_);
  std::swap(id_counter_, other.id_counter_);
  topic_config_ = std::move(other.topic_config_);
}

void TopicServer::Run() {
  Log() << "Starting topic server" << endl;
  this->DoAccept();
  result_ = std::async(std::launch::async, []{ DefaultIoService().run(); });
}

void TopicServer::Stop() {
  Log() << "Stopping topic server" << endl;
  acceptor_.cancel();
  future_status status = result_.wait_for(chrono::milliseconds(100));
  if (status == future_status::timeout)
    Error() << "Timed out waiting for TopicServer thread to return. This may cause erroneous shutdown" << endl;

  MessageType type = MessageType::kShutdown;
  for (auto& client : subscribers_) {
    asio::write(client.second, asio::buffer(&type, sizeof(MessageType)));
    client.second.close();
  }
}

std::string TopicServer::GetName() const {
  return topic_config_.name;
}

void TopicServer::DoAccept() {
  acceptor_.async_accept(socket_, [this] (std::error_code ec) {
    if (!ec) {
      std::make_shared<TopicSession>(std::move(socket_), subscribers_, id_counter_)->ServiceClient();
      this->DoAccept();
    } else if (ec != asio::error::operation_aborted) {
      Error() << "Error in async_accpet lambda: " << ec << " " << ec.message() << endl;
    }
  });
}

}  // namespace pubsub
