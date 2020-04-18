#include "subscriber_client.h"

#include <string>
#include <chrono>

#include "asio.h"
#include "spdlog/spdlog.h"

#include "types.h"
#include "messages.h"

using namespace pubsub;
using namespace std;
using asio::ip::tcp;

namespace pubsub {

SubscriberClient::SubscriberClient(const std::string& host, const uint16_t port) :
  host_(host), port_(port), socket_(DefaultIoService()), token_(-1)
{}

SubscriberClient::~SubscriberClient() {
  this->Deregister();
}

void SubscriberClient::Register(const std::function<void(pubsub::Buffer&& buffer)>& callback) {
  if (token_ != -1)
    this->Deregister();

  tcp::resolver resolver(DefaultIoService());
  asio::connect(socket_, resolver.resolve({host_.c_str(), std::to_string(port_)}));

  MessageType type = MessageType::kSubRegister;
  asio::write(socket_, asio::buffer(&type, sizeof(MessageType)));

  asio::read(socket_, asio::buffer(&type, sizeof(MessageType)));
  if (type != MessageType::kSubRegisterReply) {
    spdlog::error("Subscriber client did not receive standard reply type {}", ToString(type));
    return;
  }

  asio::read(socket_, asio::buffer(&token_, sizeof(int32_t)));

  callback_ = callback;
  this->DoRead();

  result_ = std::async(std::launch::async, []{ DefaultIoService().run(); });

  spdlog::info("Subscriber {} registered", token_);
}

void SubscriberClient::DoRead() {
  asio::async_read(socket_, asio::buffer(&message_type_, sizeof(MessageType)),
    [this](const asio::error_code& error, size_t bytes_transferred){
      if (error == asio::error::operation_aborted) {
        socket_.close();
        return;
      }

      if (message_type_ == MessageType::kData) {
        uint64_t data_size = 0;
        asio::read(socket_, asio::buffer(&data_size, sizeof(uint64_t)));
        Buffer buffer(static_cast<size_t>(data_size));
        asio::read(socket_, asio::buffer(buffer.Get(), buffer.Size()));
        callback_(std::move(buffer));
        this->DoRead();
      } else if (message_type_ == MessageType::kShutdown) {
        spdlog::info("Subscriber received shutdown message");
        socket_.close();
      } else {
        spdlog::error("Subscriber receive unknown message type: {}", ToString(message_type_));
        spdlog::error("Disconnecting");
        socket_.close();
      }
    });
}

void SubscriberClient::Deregister() {
  if (token_ == -1)
    return;

  if (socket_.is_open()) {
    tcp::socket client(DefaultIoService());
    tcp::resolver resolver(DefaultIoService());
    asio::connect(client, resolver.resolve({host_.c_str(), std::to_string(port_)}));

    MessageType type = MessageType::kSubDeregister;
    asio::write(client, asio::buffer(&type, sizeof(MessageType)));
    asio::write(client, asio::buffer(&token_, sizeof(int32_t)));

    asio::read(client, asio::buffer(&type, sizeof(MessageType)));
    if (type != MessageType::kSubDeregisterReply) {
      spdlog::error("Subscriber client did not receive standard reply type {}", ToString(type));
      spdlog::error("Closing connection and assuming deregistration");
      socket_.cancel();
    }
  }

  future_status status = result_.wait_for(chrono::milliseconds(100));
  if (status == future_status::timeout) {
    spdlog::error("SubscriberClient timed out waiting to close server connection");
    return;
  }

  token_ = -1;
  spdlog::info("Subscriber deregistered");
}

}  // namespace pubsub
