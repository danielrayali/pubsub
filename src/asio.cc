#include "asio.h"

namespace pubsub {

asio::io_service& DefaultIoService() {
  static asio::io_service service;
  return service;
}

}  // namespace pubsub

