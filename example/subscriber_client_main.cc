#include <pubsub.h>
#include <iostream>

using namespace std;

void callback(pubsub::Buffer&& buffer) {
  string received(reinterpret_cast<char*>(buffer.Get()), buffer.Size() - 1);
  cout << "Data received: " << received << endl;
}

int main(int argc, char* argv[]) {
  pubsub::SubscriberClient subscriber_client("localhost", 10001);
  subscriber_client.Register(callback);
  pubsub::WaitForSignal();
  subscriber_client.Deregister();
  return 0;
}
