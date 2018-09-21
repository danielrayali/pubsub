#include <pubsub.h>
#include <sargs.h>
#include <iostream>

using namespace std;

void callback(pubsub::Buffer&& buffer) {
  string received(reinterpret_cast<char*>(buffer.Get()), buffer.Size() - 1);
  cout << "Data received: " << received << endl;
}

int main(int argc, char* argv[]) {
  SARGS_REQUIRED_FLAG_VALUE("--master-port", "-mp", "The master server's port to connect to");
  SARGS_INITIALIZE(argc, argv);

  uint16_t port = SARGS_GET_UINT16("-mp");
  pubsub::SubscriberClient subscriber_client("localhost", port);
  subscriber_client.Register(callback);
  pubsub::WaitForSignal();
  subscriber_client.Deregister();
  return 0;
}
