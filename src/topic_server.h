#pragma once

#include <future>
#include <map>
#include "asio.h"
#include "config.h"

namespace pubsub {

class TopicServer {
 public:
  TopicServer(const TopicConfig& topic_config);

  ~TopicServer() = default;

  void Run();

  void Stop();

 private:
  void DoAccept();

  asio::ip::tcp::acceptor acceptor_;
  asio::ip::tcp::socket socket_;
  std::future<void> result_;
  std::map<int, asio::ip::tcp::socket> subscribers_;
  int32_t id_counter_ = 0;  // for clients
  TopicConfig topic_config_;
};

}  // namespace pubsub
