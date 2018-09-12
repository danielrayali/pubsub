#include <pubsub.h>
#include <iostream>
#include <vector>

using namespace std;
using namespace pubsub;

int main(int argc, char* argv[]) {
  QueryClient query_client("localhost", 10000);
  vector<string> topic_ids = query_client.QueryForTopics();
  for (auto topic_id : topic_ids)
    cout << "Found topic: " << topic_id << endl;
  return 0;
}