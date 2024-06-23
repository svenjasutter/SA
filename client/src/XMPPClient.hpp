#ifndef XMPP_CLIENT_HPP
#define XMPP_CLIENT_HPP

#include <QXmppClient.h>
#include <QXmppConfiguration.h>
#include <QXmppLogger.h>
#include <QXmppMessage.h>
#include <QXmppPresence.h>

#include <QCoreApplication>
#include <QFile>
#include <QNetworkProxy>
#include <QSslCertificate>
#include <QTextStream>
#include <functional>
#include <iomanip>
#include <iostream>

#include "MQTTClient.hpp"
#include "qChatProtocolHandler.hpp"
class XMPPClient : public QObject {
  Q_OBJECT

 public:
  XMPPClient(const std::string& domain, const std::string& user,
             const std::string& password, const std::string& certString,
             const std::string& proxyAdress = "localhost",
             const int& proxyPort = 9050);
  ~XMPPClient() { disconnectFromServer(); }

  auto connectToServer() -> void;
  auto disconnectFromServer() -> void;

  auto sendMessage(const std::string& recipient, const std::string& messageBody,
                   const std::string subject) -> void {
    send_message(QString::fromStdString(recipient),
                 QString::fromStdString(messageBody),
                 QString::fromStdString(subject));
  }

 public slots:
  auto OnConnected() -> void;
  auto OnMessageReceived(const QXmppMessage& message) -> void;
  auto OnPresenceReceived(const QXmppPresence& presence) -> void;

 private:
  auto send_message(const QString& recipient, const QString& messageBody,
                    const QString& subject) -> void;

  static constexpr std::string topic_api_ = "qchat/api";
  std::string hostname_{};

  QXmppClient client_{};
  QXmppConfiguration config_{};

  std::unique_ptr<MQTTClient> mqttClient_;
  std::unique_ptr<qChatProtocolHandler> qChatProtocolHandler_;
};

#endif  // XMPP_CLIENT_HPP
