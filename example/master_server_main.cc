#include <pubsub.h>
#include <sargs.h>

int main(int argc, char* argv[]) {
  SARGS_REQUIRED_FLAG_VALUE("--port", "-p", "The port the master server should run on");
  SARGS_INITIALIZE(argc, argv);

  pubsub::Config config;
  config.master_port = SARGS_GET_UINT16("--port");

  pubsub::MasterServer master_server(config);
  master_server.Run();
  pubsub::WaitForSignal();
  master_server.Stop();
  return 0;
}
