#include "master_server.h"
#include <iostream>

using namespace std;

namespace pubsub {

void MasterServer::Run() {
  cout << "Master server running" << endl;
}

void MasterServer::Stop() {
  cout << "Master server stopping" << endl;
}

}  // namespace pubsub
