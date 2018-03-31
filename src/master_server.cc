#include "master_server.h"
#include <iostream>
#include "asio.h"

using namespace std;

namespace pubsub {

void MasterServer::Run() {
  string message("Master server running");
  asio::mutable_buffer buffer(&message.front(), message.size());
  cout << "Master server running (" << buffer.size() << ")" << endl;
}

void MasterServer::Stop() {
  cout << "Master server stopping" << endl;
}

}  // namespace pubsub
