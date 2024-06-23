#include "qChatProtocolHandler.hpp"

qChatProtocolHandler::qChatProtocolHandler() {
  pqcWrapper_ = std::make_unique<PQCWrapper>();
  generate_kyber_keys();
  jsonHandler_ = std::make_unique<JSONHandler>();
}

auto qChatProtocolHandler::handleXMPPIncoming(const QString& from,
                                              const QString& message,
                                              const QString& subject) -> void {
  QString m = message;
  if (subject == subjectPqc_) {
    if (!decrypt_message_pqc(from, m)) {
      m = "decryption failed";
    }
  } else if (subject == subjectPqcKeyExchange_) {
    if (!finish_pqc_key_exchange(from.toStdString(), message.toStdString())) {
      std::cerr << "!!!finish_pqc_key_exchange failed!!!" << std::endl;
    }
    return;
  }

  if (subject != subjectDefault_ && subject != subjectPqc_) {
    std::cerr << "Message has unknown subject" << subject.toStdString()
              << std::endl;
  }

  // Handle other Messages
  QJsonObject jsonMessage;
  jsonMessage["emitter"] = "client";
  jsonMessage["kind"] = "recv";
  jsonMessage["from"] = from;
  jsonMessage["message"] = m;

  QJsonDocument doc(jsonMessage);
  QString formattedMessage = doc.toJson(QJsonDocument::Compact);

  std::cout << "Handle incoming, send to frontend: "
            << formattedMessage.toStdString() << std::endl;

  if (mqttPublishFn_) {
    mqttPublishFn_(formattedMessage.toStdString());
  }
}

auto qChatProtocolHandler::handleMqtt(const std::string& message) -> void {
  auto doc = QJsonDocument::fromJson(QString::fromStdString(message).toUtf8());
  if (doc.isNull()) {
    std::cerr << "Invalid JSON message received." << std::endl;
    return;
  }

  auto json = doc.object();
  QString emitter = json["emitter"].toString();
  QString kind = json["kind"].toString();

  // Ignore messages from C++ client
  if (emitter == "client") {
    return;
  }

  std::cout << "Received MQTT message: " << message << std::endl;

  if (kind == "send") {
    handle_send(json["message"].toObject());
  } else if (kind == "manage") {
    handle_manage(json["message"].toObject());
  } else {
    std::cerr << "Unhandled Kind / Invalid Message in Mqtt Message"
              << std::endl;
  }
}

auto qChatProtocolHandler::handle_send(const QJsonObject& msgObj) -> void {
  std::string recipient = msgObj["to"].toString().toStdString();
  std::string messageBody = msgObj["messageBody"].toString().toStdString();
  std::string encryption = msgObj["encryption"].toString().toStdString();

  // Check if the recipient string starts with "qchat@" and prepend if necessary
  if (recipient.find("qchat@") != 0) {
    recipient = "qchat@" + recipient;
  }

  if (encryption == "pqc") {
    send_xmpp_pqc(recipient, messageBody);
  } else {
    send_xmpp_default(recipient, messageBody);
  }
}

auto qChatProtocolHandler::send_xmpp_default(const std::string& recipient,
                                             const std::string& message)
    -> bool {
  if (xmppSendMessageFn_) {
    xmppSendMessageFn_(recipient, message, "default");
    return true;
  }
  return false;
}

auto qChatProtocolHandler::send_xmpp_pqc(const std::string& recipient,
                                         const std::string& message) -> bool {
  std::string m = message;
  if (encrypt_message_pqc(recipient, m)) {
    if (xmppSendMessageFn_) {
      xmppSendMessageFn_(recipient, m, subjectPqc_.toStdString());
      return true;
    }
  }
  return false;
}

auto qChatProtocolHandler::handle_manage(const QJsonObject& manageObj) -> void {
  QJsonObject manageRequestQ = manageObj["manageRequest"].toObject();
  QString manageRequest = manageRequestQ["request"].toString();

  if (manageRequest == "fetchCertificate") {
    fetch_certificate();
  } else if (manageRequest == "fetchPQCPublicKey") {
    fetch_pqc_pubkey();
  } else if (manageRequest == "fetchMyHostname") {
    fetch_my_hostname();
  } else if (manageRequest == "addPQCPubkey") {
    add_pqc_pubkey(manageObj);
  } else if (manageRequest == "addCertificate") {
    add_certificate(manageObj);
  } else {
    std::cerr << "Unhandled Request in Mqtt Message" << std::endl;
  }
}

auto qChatProtocolHandler::fetch_my_hostname() -> void {
  QString hostname =
      jsonHandler_->readJSONObject("/user/my_vcard.json", hostnameJSONName_);

  QJsonObject manageResponseObj;
  manageResponseObj["response"] = "fetchMyHostname";
  manageResponseObj["hostname"] = hostname;

  QJsonObject messageJson;
  messageJson["emitter"] = "client";
  messageJson["kind"] = "manage";
  messageJson["manage"] = manageResponseObj;

  QJsonDocument messageDoc(messageJson);
  QString messageString = messageDoc.toJson(QJsonDocument::Compact);

  if (mqttPublishFn_) {
    mqttPublishFn_(messageString.toStdString());
  }
}

auto qChatProtocolHandler::fetch_certificate() -> void {
  QString certificate =
      jsonHandler_->readJSONObject("/user/my_vcard.json", certJSONName_);

  QJsonObject manageResponseObj;
  manageResponseObj["response"] = "fetchCertificate";
  manageResponseObj["certificate"] = certificate;

  QJsonObject messageJson;
  messageJson["emitter"] = "client";
  messageJson["kind"] = "manage";
  messageJson["manage"] = manageResponseObj;

  QJsonDocument messageDoc(messageJson);
  QString messageString = messageDoc.toJson(QJsonDocument::Compact);

  if (mqttPublishFn_) {
    mqttPublishFn_(messageString.toStdString());
  }
}

auto qChatProtocolHandler::fetch_pqc_pubkey() -> void {
  QString publicKey =
      jsonHandler_->readJSONObject("/user/my_pqc.json", pubkeyJSONName_);

  QJsonObject manageResponseObj;
  manageResponseObj["response"] = "fetchPQCPublicKey";
  manageResponseObj["pubkey"] = publicKey;

  QJsonObject messageJson;
  messageJson["emitter"] = "client";
  messageJson["kind"] = "manage";
  messageJson["manage"] = manageResponseObj;

  QJsonDocument messageDoc(messageJson);
  QString messageString = QString(messageDoc.toJson(QJsonDocument::Compact));

  if (mqttPublishFn_) {
    mqttPublishFn_(messageString.toStdString());
  }
}

auto qChatProtocolHandler::format_certificate(std::string& cert) -> void {
  const std::string beginCert = "-----BEGIN CERTIFICATE-----";
  const std::string endCert = "-----END CERTIFICATE-----";
  const size_t lineLength = 64;

  size_t startPos = cert.find(beginCert);
  size_t endPos = cert.find(endCert);

  if (startPos == std::string::npos || endPos == std::string::npos) {
    std::cerr << "Invalid certificate format" << std::endl;
    return;
  }

  startPos += beginCert.length();

  std::string certContent = cert.substr(startPos, endPos - startPos);

  certContent.erase(
      std::remove_if(certContent.begin(), certContent.end(), ::isspace),
      certContent.end());

  std::string formattedCert = beginCert + "\n";
  for (size_t i = 0; i < certContent.length(); i += lineLength) {
    formattedCert += certContent.substr(i, lineLength) + "\n";
  }
  formattedCert += endCert + "\n";

  cert = formattedCert;
}

auto qChatProtocolHandler::add_certificate(const QJsonObject& manageObj)
    -> void {
  QJsonObject manage = manageObj["manageRequest"].toObject();
  QString friendHostname = manage["friendHostname"].toString();
  QString certificate = manage["certificate"].toString();
  certificate.replace("\\n", "\n");

  std::string cert_std = certificate.toStdString();
  format_certificate(cert_std);

  QString filename = "/friends/" + friendHostname + ".json";
  QString certificate_formated = QString::fromStdString(cert_std);
  jsonHandler_->writeJSONObject(filename, certJSONName_, certificate_formated);

  // Update the watch file
  std::string watchFile = "/friends/watch/watchfile";
  std::ofstream watchFileStream(watchFile, std::ios::out | std::ios::trunc);
  if (watchFileStream.is_open()) {
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    watchFileStream << std::ctime(&now_c);
    watchFileStream.close();
  }
}

auto qChatProtocolHandler::add_pqc_pubkey(const QJsonObject& manageObj)
    -> void {
  QJsonObject manage = manageObj["manageRequest"].toObject();
  QString friendHostnameQ = manage["friendHostname"].toString();
  QString pubkeyQ = manage["pubkey"].toString();

  std::string filename =
      "/friends/" + friendHostnameQ.toStdString() + ".pqc.pub";
  jsonHandler_->writeJSONObject(QString::fromStdString(filename),
                                pubkeyJSONName_, pubkeyQ);

  if (!start_pqc_key_exchange(friendHostnameQ, pubkeyQ)) {
    std::cerr << "!!!start_pqc_key_exchange failed!!!" << std::endl;
  }
}

auto qChatProtocolHandler::generate_kyber_keys() -> void {
  auto keypair = pqcWrapper_->createKeyPair();
  std::string filename = "/user/my_pqc.json";
  jsonHandler_->writeJSONObject(QString::fromStdString(filename),
                                pubkeyJSONName_, encode_hex(keypair.first));
  jsonHandler_->writeJSONObject(QString::fromStdString(filename),
                                seckeyJSONName_, encode_hex(keypair.second));
}

auto qChatProtocolHandler::get_friends_shared_secret_pqc(
    const std::string& myfriend) -> std::string {
  std::string friendFile =
      "/friends/" + extract_hostname(myfriend) + ".pqc.pub";

  QString sharedSecretQ = jsonHandler_->readJSONObject(
      QString::fromStdString(friendFile), sharedSecretJSONName_);
  std::string sharedSecret = decode_hex(sharedSecretQ);
  if (sharedSecret.empty()) {
    std::cerr << "Shared secret is empty for friend: " << friendFile
              << std::endl;
  }
  return sharedSecret;
}

auto qChatProtocolHandler::start_pqc_key_exchange(const QString& hostname,
                                                  const QString& pubkey)
    -> bool {
  const std::string pubkey_decoded = decode_hex(pubkey);
  auto [ciphertext, sharedSecret] = pqcWrapper_->encrypt(pubkey_decoded);

  std::string filename = "/friends/" + hostname.toStdString() + ".pqc.pub";

  std::cout << "START KEY EXCHANGE, Shared Secret: "
            << encode_hex(sharedSecret).toStdString() << std::endl;

  jsonHandler_->writeJSONObject(QString::fromStdString(filename),
                                sharedSecretJSONName_,
                                encode_hex(sharedSecret));

  QByteArray rawCiphertext = QByteArray::fromStdString(ciphertext);
  QString base64Ciphertext = rawCiphertext.toBase64();

  xmppSendMessageFn_("qchat@" + hostname.toStdString(),
                     base64Ciphertext.toStdString(),
                     subjectPqcKeyExchange_.toStdString());
  return true;
}
auto qChatProtocolHandler::finish_pqc_key_exchange(
    const std::string& friendHostname, const std::string& ciphertext) -> bool {
  try {
    auto seckey_enc =
        jsonHandler_->readJSONObject("/user/my_pqc.json", seckeyJSONName_);
    auto secretKey = decode_hex(seckey_enc);

    QByteArray decodedByteArray =
        QByteArray::fromBase64(QByteArray::fromStdString(ciphertext));
    std::string decodedCiphertext = decodedByteArray.toStdString();

    auto sharedSecret = pqcWrapper_->decrypt(secretKey, decodedCiphertext);

    std::cout << "FINISH KEY EXCHANGE, decrypted Shared Secret: "
              << encode_hex(sharedSecret).toStdString() << std::endl;

    QString sharedSecretEnc = encode_hex(sharedSecret);

    std::string filename =
        "/friends/" + extract_hostname(friendHostname) + ".pqc.pub";

    std::cout << "filename :" << filename << std::endl;

    jsonHandler_->writeJSONObject(QString::fromStdString(filename),
                                  sharedSecretJSONName_, sharedSecretEnc);
    return true;

  } catch (const std::exception& e) {
    std::cerr << "Exception in finish_pqc_key_exchange: " << e.what()
              << std::endl;
    return false;
  }
}

auto qChatProtocolHandler::encrypt_message_pqc(const std::string& recipient,
                                               std::string& message) -> bool {
  std::string sharedSecret = get_friends_shared_secret_pqc(recipient);
  if (sharedSecret == "") {
    return false;
  }

  std::vector<unsigned char> messageVec(message.begin(), message.end());

  std::vector<unsigned char> sharedSecretVec(sharedSecret.begin(),
                                             sharedSecret.end());
  std::vector<unsigned char> messageEncrypted =
      pqcWrapper_->aes_encrypt(messageVec, sharedSecretVec);

  QByteArray encryptedArray(
      reinterpret_cast<const char*>(messageEncrypted.data()),
      messageEncrypted.size());
  QString messageEncryptedQ = encryptedArray.toBase64();

  std::cout << "Send to : " << recipient
            << "\nMessage:" << messageEncryptedQ.toStdString() << std::endl;
  message = messageEncryptedQ.toStdString();
  return true;
}

auto qChatProtocolHandler::decrypt_message_pqc(const QString& from,
                                               QString& message) -> bool {
  auto hostname = extract_hostname(from.toStdString());
  std::cout << "Decrypt, get shared secret for hostname: " << hostname
            << std::endl;
  std::string sharedSecretStr = get_friends_shared_secret_pqc(hostname);

  QByteArray decodedByteArray = QByteArray::fromBase64(message.toUtf8());
  std::string decodedMessage = decodedByteArray.toStdString();

  if (sharedSecretStr == "") {
    std::cerr << "!!! EMPTY SHARED SECRET FOR: "
              << extract_hostname(from.toStdString()) << std::endl;
    return false;
  }

  std::vector<unsigned char> sharedSecret(sharedSecretStr.begin(),
                                          sharedSecretStr.end());
  std::vector<unsigned char> messageVec(decodedMessage.begin(),
                                        decodedMessage.end());

  std::vector<unsigned char> decryptedMessageVec =
      pqcWrapper_->aes_decrypt(messageVec, sharedSecret);
  std::string decryptedMessage(decryptedMessageVec.begin(),
                               decryptedMessageVec.end());
  std::cout << "Decrypted Message: " << decryptedMessage << std::endl;
  message = QString::fromStdString(decryptedMessage);

  return true;
}
