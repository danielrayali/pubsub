#include <pubsub.h>

int main(int argc, char* argv[]) {
  pubsub::SubscriberClient subscriber_client("localhost", 10001);
  int32_t id = subscriber_client.Register();
  pubsub::WaitForSignal();
  subscriber_client.Deregister(id);
  return 0;
}
