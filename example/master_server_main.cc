#include <pubsub.h>

int main(int argc, char* argv[]) {
  pubsub::MasterServer master_server;
  master_server.Run();
  pubsub::WaitForSignal();
  master_server.Stop();
  return 0;
}
