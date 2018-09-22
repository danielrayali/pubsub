#include "logging.h"

namespace pubsub {

static std::ostream* gLogStream = &std::clog;
static std::ostream* gErrorStream = &std::cerr;

namespace internal {

std::ostream& _Log() {
  return *gLogStream;
}

std::ostream& _Error() {
  return *gErrorStream;
}

}  // namespace internal

void SetLogStream(std::ostream& log) {
  gLogStream = &log;
}

void SetErrorStream(std::ostream& error) {
  gErrorStream = &error;
}

}  // namespace pubsub

