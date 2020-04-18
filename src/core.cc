#include "core.h"

#include <future>
#include <thread>
#include <spdlog/spdlog.h>

#include "asio.h"

using namespace std;

namespace pubsub {

static bool gRunning = false;

void SignalHandler(int signal) {
  spdlog::info("Signal caught: {}", signal);
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

