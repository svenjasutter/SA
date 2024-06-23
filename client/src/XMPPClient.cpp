#include "XMPPClient.hpp"

XMPPClient::XMPPClient(const std::string& domain, const std::string& user,
                       const std::string& password,
                       const std::string& certString,
                       const std::string& proxyAdress, const int& proxyPort) {
  hostname_ = domain;
  QString qDomain = QString::fromStdString(domain);
  QString qUser = QString::fromStdString(user);
  QString qPassword = QString::fromStdString(password);
  QString qProxyAdress = QString::fromStdString(proxyAdress);

  client_.logger()->setLoggingType(QXmppLogger::StdoutLogging);

  QNetworkProxy proxy;
  proxy.setType(QNetworkProxy::Socks5Proxy);
  proxy.setHostName(qProxyAdress);
  proxy.setPort(proxyPort);
  QNetworkProxy::setApplicationProxy(proxy);

  QSslCertificate certificate(QString::fromStdString(certString).toUtf8());

  config_.setDomain(qDomain);
  config_.setUser(qUser);
  config_.setPassword(qPassword);
  config_.setIgnoreSslErrors(true);
  config_.setCaCertificates({certificate});
  config_.setStreamSecurityMode(QXmppConfiguration::TLSEnabled);

  mqttClient_ = std::make_unique<MQTTClient>();
  mqttClient_->setMessageArrivedHandler(
      [&](const std::string& topic, const std::string& message) {
        qChatProtocolHandler_->handleMqtt(message);
      });

  qChatProtocolHandler_ = std::make_unique<qChatProtocolHandler>();
  qChatProtocolHandler::XMPPSendMessageFunction_t sendFn =
      std::bind(&XMPPClient::sendMessage, this, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3);
  qChatProtocolHandler_->setXMPPSendMessageFunction(sendFn);

  qChatProtocolHandler::MQTTPublishFunction_t mqttFn =
      [&](const std::string& message) {
        mqttClient_->publish(topic_api_, message);
      };
  qChatProtocolHandler_->setMQTTPublishFunction(mqttFn);

  QObject::connect(&client_, &QXmppClient::connected, this,
                   &XMPPClient::OnConnected);

  QObject::connect(&client_, &QXmppClient::messageReceived, this,
                   &XMPPClient::OnMessageReceived);

  QObject::connect(&client_, &QXmppClient::presenceReceived, this,
                   &XMPPClient::OnPresenceReceived);
}

auto XMPPClient::connectToServer() -> void { client_.connectToServer(config_); }

auto XMPPClient::disconnectFromServer() -> void {
  client_.disconnectFromServer();
}

auto XMPPClient::OnPresenceReceived(const QXmppPresence& presence) -> void {
  switch (presence.type()) {
    case QXmppPresence::Subscribe:
      // Automatically accept subscription requests
      QXmppPresence subscribedPresence;
      subscribedPresence.setTo(presence.from());
      subscribedPresence.setType(QXmppPresence::Subscribed);
      client_.sendPacket(subscribedPresence);
      break;
  }
}

auto XMPPClient::OnMessageReceived(const QXmppMessage& message) -> void {
  if (!message.body().isEmpty()) {
    QString sender = message.from();
    QString messageBody = message.body();
    QString messageSubject = message.subject();
    qChatProtocolHandler_->handleXMPPIncoming(sender, messageBody,
                                              messageSubject);
  }
}

auto XMPPClient::OnConnected() -> void {
  QXmppPresence presence;
  presence.setType(QXmppPresence::Available);
  client_.setClientPresence(presence);
}

auto XMPPClient::send_message(const QString& recipient,
                              const QString& messageBody,
                              const QString& subject) -> void {
  QXmppMessage message;

  message.setTo(recipient);
  message.setBody(messageBody);
  message.setSubject(subject);
  client_.sendPacket(message);
}
