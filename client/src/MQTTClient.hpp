#ifndef MQTTCLIENT_HPP
#define MQTTCLIENT_HPP

#include <mqtt/exception.h>

#include <functional>
#include <memory>
#include <string>

#include "iostream"
#include "mqtt/client.h"
class MQTTCallback : public virtual mqtt::callback {
 public:
  std::function<void()> connectionLostHandler;
  std::function<void(const std::string&, const std::string&)>
      messageArrivedHandler;

  void connection_lost(const std::string& cause) override {
    std::cerr << "MQTT Connection Lost. Cause: " << cause << std::endl;
    if (connectionLostHandler) {
      connectionLostHandler();
    }
  }
  void message_arrived(mqtt::const_message_ptr msg) override {
    if (messageArrivedHandler) {
      messageArrivedHandler(msg->get_topic(), msg->to_string());
    }
  }
};

class MQTTClient {
 public:
  MQTTClient(const std::string& serverAddress, const std::string& clientId,
             const std::string& username = "",
             const std::string& password = "");

  MQTTClient();
  ~MQTTClient() { this->disconnect(); }

  auto connect() -> void { mqttClient_->connect(); }
  auto disconnect() -> void { mqttClient_->disconnect(); }
  auto isConnected() const -> bool {
    return mqttClient_ ? mqttClient_->is_connected() : false;
  }

  auto publish(const std::string& topic, const std::string& message,
               int qos = 0) -> void;
  auto subscribe(const std::string& topic, int qos = 0) -> void;

  auto setConnectionLostHandler(std::function<void()> handler) -> void {
    mqttCallback_->connectionLostHandler = std::move(handler);
  }

  auto setConnectionLostHandler_default() -> void {
    setConnectionLostHandler(
        []() { std::cout << "Connection lost." << std::endl; });
  }

  auto setMessageArrivedHandler(
      std::function<void(const std::string&, const std::string&)> handler)
      -> void {
    mqttCallback_->messageArrivedHandler = std::move(handler);
  }

  auto setMessageArrivedHandler_default() -> void {
    this->setMessageArrivedHandler(
        [&](const std::string& topic, const std::string& message) {
          std::cout << "Message arrived on topic, publishing: " << topic
                    << ". Message: " << message << std::endl;
        });
  }

  inline auto subscribeToDefaultTopics() -> void { subscribe(topic_api_); }

 private:
  static constexpr std::string topic_api_ = "qchat/api";
  std::string serverAddress_{};
  std::string clientId_{};
  std::string username_{};
  std::string password_{};
  std::unique_ptr<mqtt::client> mqttClient_;
  std::shared_ptr<MQTTCallback> mqttCallback_;
};

#endif  // MQTTCLIENT_HPP
