#include "master_server.h"
#include <future>
#include <system_error>
#include <thread>
#include "logging.h"
#include "asio.h"
#include "types.h"
#include "messages.h"

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

//
// MasterServer definitions
//

MasterServer::MasterServer() : 
  acceptor_(DefaultIoService(), tcp::endpoint(tcp::v4(), 10000)),
  socket_(DefaultIoService()) 
{} 

void MasterServer::Run() {
  clog << "Starting master server" << endl;
  this->DoAccept();
  result_ = std::async(std::launch::async, []{ DefaultIoService().run(); });
}

void MasterServer::Stop() {
  clog << "Stopping master server" << endl;
  acceptor_.cancel();
  future_status status = result_.wait_for(chrono::milliseconds(100));
  if (status == future_status::timeout)
    cerr << "Timed out waiting for MasterServer thread to return. This may cause erroneous shutdown" << endl;
}

void MasterServer::DoAccept() {
  acceptor_.async_accept(socket_, [this](std::error_code ec){
    if (!ec) {
      std::make_shared<MasterSession>(std::move(socket_))->ServiceClient();
      this->DoAccept();
    } else if (ec != asio::error::operation_aborted) {
      cerr << "Error in async_accpet lambda: " << ec << " " << ec.message() << endl;
    }
  });
}

}  // namespace pubsub
