#ifndef QCHATPROTOCOLHANDLER_HPP
#define QCHATPROTOCOLHANDLER_HPP
#include <QString>
#include <chrono>
#include <ctime>
#include <fstream>
#include <functional>
#include <iostream>
#include <string>

#include "JSONHandler.hpp"
#include "PQCWrapper.hpp"

/**
Message Format JSON:
{
    emitter: "frontend" / "client",
    kind: "send" / "recieve" / "manage" / "pqckeyexchange",
    message: {
        to: "recipient.onion",
        from: "sender",
        messageBody: ...,
        encryption: "pqc" / "default",
    }
    manage: {
        request: "addCertificate" /  ...,
        response: ...,
        friendHostname: ...,
        pubkey: ...,
        certificate: cert...
    }
}
*/

class qChatProtocolHandler {
 public:
  qChatProtocolHandler();
  ~qChatProtocolHandler() = default;

  auto handleMqtt(const std::string& message) -> void;

  auto handleXMPPIncoming(const QString& from, const QString& message,
                          const QString& subject) -> void;

  using XMPPSendMessageFunction_t = std::function<void(
      const std::string& recipient, const std::string& messageBody,
      const std::string& subject)>;

  auto setXMPPSendMessageFunction(const XMPPSendMessageFunction_t& fn) -> void {
    xmppSendMessageFn_ = fn;
  }

  using MQTTPublishFunction_t = std::function<void(const std::string& message)>;
  auto setMQTTPublishFunction(const MQTTPublishFunction_t& fn) -> void {
    mqttPublishFn_ = fn;
  }

 private:
  auto handle_send(const QJsonObject& msgObj) -> void;
  auto send_xmpp_default(const std::string& recipient,
                         const std::string& message) -> bool;

  auto send_xmpp_pqc(const std::string& recipient, const std::string& message)
      -> bool;

  auto decrypt_message_pqc(const QString& from, QString& message) -> bool;
  auto encrypt_message_pqc(const std::string& recipient, std::string& message)
      -> bool;

  auto handle_manage(const QJsonObject& manageObj) -> void;

  auto fetch_my_hostname() -> void;
  auto fetch_certificate() -> void;
  auto fetch_pqc_pubkey() -> void;

  auto add_certificate(const QJsonObject& manageObj) -> void;
  auto add_pqc_pubkey(const QJsonObject& manageObj) -> void;
  auto format_certificate(std::string& cert) -> void;

  auto start_pqc_key_exchange(const QString& hostname, const QString& pubkey)
      -> bool;
  auto finish_pqc_key_exchange(const std::string& friendHostname,
                               const std::string& ciphertext) -> bool;

  auto generate_kyber_keys() -> void;

  auto get_friends_shared_secret_pqc(const std::string& myfriend)
      -> std::string;

  auto encode_hex(const std::string& input) -> QString {
    QByteArray byteArray(input.c_str(), static_cast<int>(input.size()));
    return byteArray.toHex();
  }

  auto decode_hex(const QString& input) -> std::string {
    QByteArray byteArray = QByteArray::fromHex(input.toUtf8());
    return std::string(byteArray.constData(), byteArray.size());
  }
  auto encode_base64(const std::string& input) -> QString {
    QByteArray byteArray(input.c_str(), static_cast<int>(input.size()));
    return byteArray.toBase64();
  }

  auto decode_base64(const QString& input) -> std::string {
    QByteArray byteArray = QByteArray::fromBase64(input.toUtf8());
    return std::string(byteArray.constData(), byteArray.size());
  }

  auto extract_hostname(const std::string& recipient) -> std::string {
    std::string hostname = recipient;

    size_t atPosition = recipient.find('@');
    if (atPosition != std::string::npos) {
      hostname = recipient.substr(atPosition + 1);
    }

    size_t slashPosition = hostname.find('/');
    if (slashPosition != std::string::npos) {
      hostname = hostname.substr(0, slashPosition);
    }

    std::cout << "Extracted hostname: " << hostname
              << " original: " << recipient << std::endl;
    return hostname;
  }

  const QString subjectPqc_{"pqc"};
  const QString subjectPqcKeyExchange_{"pqckeyexchange"};
  const QString subjectDefault_{"default"};

  const QString pubkeyJSONName_{"pubkey"};
  const QString seckeyJSONName_{"seckey"};
  const QString hostnameJSONName_{"hostname"};
  const QString certJSONName_{"certificate"};
  const QString sharedSecretJSONName_{"sharedSecret"};

  XMPPSendMessageFunction_t xmppSendMessageFn_;
  MQTTPublishFunction_t mqttPublishFn_;

  std::unique_ptr<PQCWrapper> pqcWrapper_;
  std::unique_ptr<JSONHandler> jsonHandler_;
};

#endif  // QCHATPROTOCOLHANDLER_HPP
