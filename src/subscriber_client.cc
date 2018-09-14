#include "subscriber_client.h"
#include <string>
#include "asio.h"
#include "types.h"
#include "messages.h"
#include "logging.h"

using namespace pubsub;
using namespace std;
using asio::ip::tcp;

namespace pubsub {

SubscriberClient::SubscriberClient(const std::string& host, const uint16_t port) : 
  host_(host), port_(port), socket_(DefaultIoService())
{}

int32_t SubscriberClient::Register(const std::function<void(pubsub::Buffer&& buffer)>& callback) {
  callback_ = callback;

  tcp::resolver resolver(DefaultIoService());
  asio::connect(socket_, resolver.resolve({host_.c_str(), std::to_string(port_)}));

  MessageType type = MessageType::kSubRegister;
  asio::write(socket_, asio::buffer(&type, sizeof(MessageType)));

  asio::read(socket_, asio::buffer(&type, sizeof(MessageType)));
  if (type != MessageType::kSubRegisterReply) {
    Error() << "Subscriber client did not receive standard reply type" << ToString(type) << endl;
    return -1;
  }

  int32_t client_id = -1;
  asio::read(socket_, asio::buffer(&client_id, sizeof(int32_t)));

  this->DoRead();

  Log() << "Subscriber registered" << endl;
  return client_id;
}

void SubscriberClient::DoRead() {  
  asio::async_read(socket_, asio::buffer(&message_type_, sizeof(MessageType)), 
    [this](const asio::error_code& error, size_t bytes_transferred){
      if (error == asio::error::operation_aborted)
        return;

      if (message_type_ == MessageType::kData) {
        uint64_t data_size = 0;
        asio::read(socket_, asio::buffer(&data_size, sizeof(uint64_t)));
        Buffer buffer(static_cast<size_t>(data_size));
        asio::read(socket_, asio::buffer(buffer.Get(), buffer.Size()));
        callback_(std::move(buffer));
        this->DoRead();
      } else if (message_type_ == MessageType::kShutdown) {
        Log() << "Subscriber received shutdown message" << endl;
      } else {
        Log() << "Subscriber receive unknown message type: " << ToString(message_type_) << endl;
      }
    });

  result_ = std::async(std::launch::async, []{ DefaultIoService().run(); });
}

void SubscriberClient::Deregister(const int32_t client_id) {
  tcp::socket client(DefaultIoService());
  tcp::resolver resolver(DefaultIoService());
  asio::connect(client, resolver.resolve({host_.c_str(), std::to_string(port_)}));

  MessageType type = MessageType::kSubDeregister;
  asio::write(client, asio::buffer(&type, sizeof(MessageType)));
  asio::write(client, asio::buffer(&client_id, sizeof(int32_t)));

  asio::read(client, asio::buffer(&type, sizeof(MessageType)));
  if (type != MessageType::kSubDeregisterReply) {
    Error() << "Subscriber client did not receive standard reply type" << ToString(type) << endl;
    return;
  }

  socket_.cancel();

  future_status status = result_.wait_for(chrono::milliseconds(100));
  if (status == future_status::timeout) {
    Error() << "SubscriberClient timed out waiting to close server connection" << endl;
    return;
  }

  Log() << "Subscriber deregistered" << endl;
}

}  // namespace pubsub
