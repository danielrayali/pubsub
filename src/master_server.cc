#include "master_server.h"
#include <future>
#include <system_error>
#include <thread>
#include "asio.h"
#include "config.h"
#include "logging.h"
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
  MasterSession(tcp::socket socket) : socket_(std::move(socket)) {}

  ~MasterSession() = default;

  void ServiceClient() {
    Log() << "Servicing client" << endl;
    MessageType type = MessageType::kUnknown;
    asio::read(socket_, asio::buffer(&type, sizeof(MessageType)));

    if (type == MessageType::kTopicsQuery) {
      type = MessageType::kTopicsQueryReply;
      asio::write(socket_, asio::buffer(&type, sizeof(MessageType)));

      uint32_t num_topics = 1;
      asio::write(socket_, asio::buffer(&num_topics, sizeof(uint32_t)));

      string topic_id = "Hello, World!";
      ByteSize byte_size = topic_id.size();
      asio::write(socket_, asio::buffer(&byte_size, sizeof(ByteSize)));
      asio::write(socket_, asio::buffer(&topic_id.front(), topic_id.size()));
    } else if (type == MessageType::kTopicAdd) {
      uint64_t size = 0;
      asio::read(socket_, asio::buffer(&size, sizeof(uint64_t)));
      cout << "Size is " << size << endl;
      string buffer(static_cast<size_t>(size), '\0');
      asio::read(socket_, asio::buffer(&buffer.front(), static_cast<size_t>(size)));

      TopicConfig topic_config;
      topic_config.FromString(buffer);
      cout << "TopicConfig.name: " << topic_config.name << endl;
      cout << "TopicConfig.port: " << topic_config.port << endl;

      type = MessageType::kTopicAddReply;
      asio::write(socket_, asio::buffer(&type, sizeof(MessageType)));
    } else {
      Error() << "Master session cannot handle received message type: " << ToString(type) << endl;
    }
  }

 private:
  tcp::socket socket_;
};

//
// MasterServer definitions
//

MasterServer::MasterServer() :
  acceptor_(DefaultIoService(), tcp::endpoint(tcp::v4(), 10000)),
  socket_(DefaultIoService())
{}

void MasterServer::Run() {
  Log() << "Starting master server" << endl;
  this->DoAccept();
  result_ = std::async(std::launch::async, []{ DefaultIoService().run(); });
}

void MasterServer::Stop() {
  Log() << "Stopping master server" << endl;
  acceptor_.cancel();
  future_status status = result_.wait_for(chrono::milliseconds(100));
  if (status == future_status::timeout)
    Error() << "Timed out waiting for MasterServer thread to return. This may cause erroneous shutdown" << endl;
}

void MasterServer::DoAccept() {
  acceptor_.async_accept(socket_, [this] (std::error_code ec) {
    if (!ec) {
      std::make_shared<MasterSession>(std::move(socket_))->ServiceClient();
      this->DoAccept();
    } else if (ec != asio::error::operation_aborted) {
      Error() << "Error in async_accpet lambda: " << ec << " " << ec.message() << endl;
    }
  });
}

}  // namespace pubsub

