#include "core.h"
#include <future>
#include <thread>
#include "asio.h"
#include "logging.h"

using namespace std;

namespace pubsub {

static bool gRunning = false;

void SignalHandler(int signal) {
  clog << "Signal caught: " << signal << endl;
  gRunning = false;
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
}

}  // namespace pubsub

