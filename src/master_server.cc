#include "master_server.h"
#include "logging.h"
#include "asio.h"
#include "types.h"
#include "messages.h"

using namespace pubsub;
using namespace std;
using asio::ip::tcp;

namespace pubsub {

class MasterSession : public std::enable_shared_from_this<MasterSession> {
 public:
  MasterSession(tcp::socket socket) : socket_(std::move(socket)) {}

  ~MasterSession() = default;

  void ServiceClient() {
    clog << "Servicing client" << endl;
    MessageType type = MessageType::kUnknown;
    asio::read(socket_, asio::buffer(reinterpret_cast<char*>(&type), sizeof(MessageType)));

    uint32_t num_topics = 1;
    asio::write(socket_, asio::buffer(&num_topics, sizeof(uint32_t)));

    string topic_id = "Hello, World!";
    ByteSize byte_size = topic_id.size();
    asio::write(socket_, asio::buffer(reinterpret_cast<char*>(&byte_size), sizeof(ByteSize)));
    asio::write(socket_, asio::buffer(&topic_id.front(), topic_id.size()));
  }

 private:
  tcp::socket socket_;
};

MasterServer::MasterServer() : 
  acceptor_(DefaultIoService(), tcp::endpoint(tcp::v4(), 10000)),
  socket_(DefaultIoService()) 
{} 

void MasterServer::Run() {
  clog << "Starting master server" << endl;
  this->DoAccept();
  std::async(std::launch::async, []{ DefaultIoService().run(); });
}

void MasterServer::Stop() {
  clog << "Stopping master server" << endl;
  acceptor_.cancel();
}

void MasterServer::DoAccept() {
  acceptor_.async_accept(socket_, [this](std::error_code ec){
    if (!ec) {
      // std::async(std::launch::async, [this] {
        std::make_shared<MasterSession>(std::move(socket_))->ServiceClient();
        // TODO(dali) See if this works
        // GetIoService().run();
      // });
      this->DoAccept();
    } else {
      clog << "Error code is not ok: " << ec.category().name() << endl;
    }
  });
}

}  // namespace pubsub
