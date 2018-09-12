#pragma once

#include "asio.h"

namespace pubsub {

class MasterServer {
 public:
  MasterServer();

  ~MasterServer() = default;

  void Run();

  void Stop();
 
 private:
  void DoAccept(); 

  asio::ip::tcp::acceptor acceptor_;
  asio::ip::tcp::socket socket_;
  std::future<void> result_;
};

}  // namespace pubsub
