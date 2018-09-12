#include "core.h"
#include <future>
#include <thread>
#include "asio.h"
#include "logging.h"

using namespace std;

namespace pubsub {

static bool gRunning = false;
static future<void> gHandle; 

void SignalHandler(int signal) {
  clog << "Signal caught: " << signal << endl;
  gRunning = false;
}

void InitializePubsub() {
  gHandle = async(launch::async, []{
    InitializeAsio();
  });
}

void WaitForSignal() {
  signal(SIGABRT, SignalHandler);
  signal(SIGFPE, SignalHandler);
  signal(SIGILL, SignalHandler);
  signal(SIGINT, SignalHandler);
  signal(SIGSEGV, SignalHandler);
  signal(SIGTERM, SignalHandler);
 
  gRunning = true;
  while (gRunning) {
    this_thread::sleep_for(chrono::milliseconds(100));
  }

  cout << "Waiting for singal returning" << endl;
}

void ShutdownPubsub() {
  ShutdownAsio();
  future_status status = gHandle.wait_for(chrono::seconds(5));
  if (status == future_status::timeout)
    throw runtime_error("Shutdown taking longer than 5 seconds, exiting");
}

}  // namespace pubsub
