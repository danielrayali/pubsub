#include "logging.h"

namespace pubsub {

static std::ostream* gLogStream = &std::clog;
static std::ostream* gErrorStream = &std::cerr;

std::ostream& Log() {
  return *gLogStream;
}

std::ostream& Error() {
  return *gErrorStream;
}

void SetLogStream(std::ostream& log) {
  gLogStream = &log;
}

void SetErrorStream(std::ostream& error) {
  gErrorStream = &error;
}

}  // namespace pubsub

