#include <iostream>
#include <pubsub.h>

using namespace std;
using namespace pubsub;

int main(int argc, char* argv[]) {
  MasterServer master_server;
  master_server.Run();

  // InitializePubsub();
  WaitForSignal();

  master_server.Stop();

  // ShutdownPubsub();
  return 0;
}