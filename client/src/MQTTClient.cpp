#include "MQTTClient.hpp"

#include <mqtt/exception.h>

#include "iostream"

MQTTClient::MQTTClient(const std::string& serverAddress,
                       const std::string& clientId, const std::string& username,
                       const std::string& password)
    : serverAddress_(serverAddress),
      clientId_(clientId),
      username_(username),
      password_(password) {
  mqtt::connect_options connOpts;
  connOpts.set_keep_alive_interval(60);
  connOpts.set_clean_session(true);
  connOpts.set_connect_timeout(80);

  if (!username_.empty() && !password_.empty()) {
    connOpts.set_user_name(username_);
    connOpts.set_password(password_);
  }

  mqttCallback_ = std::make_shared<MQTTCallback>();
  mqttClient_ = std::make_unique<mqtt::client>(serverAddress_, clientId_);
  mqttClient_->set_callback(*mqttCallback_);

  setMessageArrivedHandler_default();
  setConnectionLostHandler_default();

  connect();

  subscribeToDefaultTopics();
}

MQTTClient::MQTTClient() : MQTTClient("tcp://mosquitto:1883", "xmppclient") {}

auto MQTTClient::publish(const std::string& topic, const std::string& message,
                         int qos) -> void {
  // First, check if the client is connected
  if (!this->isConnected()) {
    std::cerr
        << "Error: Not connected to MQTT broker. Unable to publish message."
        << std::endl;
    // Attempt to reconnect
    try {
      this->connect();
    } catch (const mqtt::exception& e) {
      std::cerr << "Error: Reconnection attempt failed. " << e.what()
                << std::endl;
      return;
    }
  }

  // Proceed with publishing if connected
  try {
    std::cout << "Publishing message to topic: " << topic << std::endl;
    mqtt::message_ptr pubmsg = mqtt::make_message(topic, message);
    pubmsg->set_qos(qos);
    mqttClient_->publish(pubmsg);
  } catch (const mqtt::exception& e) {
    std::cerr << "Error publishing to topic: " << topic
              << ". Error: " << e.what() << std::endl;
  }
}

auto MQTTClient::subscribe(const std::string& topic, int qos) -> void {
  mqttClient_->subscribe(topic, qos);
}
