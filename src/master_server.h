#pragma once

#include <string>
#include <vector>
#include "asio.h"
#include "config.h"
#include "topic_server.h"

namespace pubsub {

class MasterServer {
 public:
  MasterServer(const std::string& path);

  MasterServer(const Config& config);

  ~MasterServer();

  void Run();

  void Stop();

 private:
  void DoAccept();

  void Configure();

  asio::ip::tcp::acceptor acceptor_;
  asio::ip::tcp::socket socket_;
  std::future<void> result_;
  Config config_;
  std::vector<std::unique_ptr<TopicServer>> topic_servers_;
  bool is_running_;
};

}  // namespace pubsub
