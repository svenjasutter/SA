#ifndef JSONHANDLER_HPP
#define JSONHANDLER_HPP

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <iostream>

class JSONHandler {
 public:
  JSONHandler() = default;
  ~JSONHandler() = default;

  auto readJSONObject(const QString& filename, const QString& key) const
      -> QString {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      return "";
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    if (doc.isNull()) {
      return "";
    }

    QJsonObject json = doc.object();
    return json[key].toString();
  }

  auto writeJSONObject(const QString& filename, const QString& key,
                       const QString& value) -> bool {
    QFile file(filename);
    QJsonObject json;

    // Open the file for reading first
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
      if (!doc.isNull() && doc.isObject()) {
        json = doc.object();
      } else {
        std::cerr << "Failed to parse JSON from file: "
                  << filename.toStdString() << std::endl;
      }
      // Close the file after reading
      file.close();
    } else {
      std::cerr << "Failed to open file for reading or file does not exist, "
                   "will create a new one: "
                << filename.toStdString() << std::endl;
    }

    // Modify or append the key-value pair
    json[key] = value;

    // Open the file for writing
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
      std::cerr << "Failed to open file for writing: " << filename.toStdString()
                << ". Error: " << file.errorString().toStdString() << std::endl;
      return false;
    }

    QJsonDocument doc_modify(json);
    if (file.write(doc_modify.toJson()) == -1) {
      std::cerr << "Failed to write JSON to file: " << filename.toStdString()
                << ". Error: " << file.errorString().toStdString() << std::endl;
      file.close();
      return false;
    }

    file.close();
    std::cout << "Successfully wrote JSON to file: " << filename.toStdString()
              << std::endl;
    return true;
  }

  struct UserInfo {
    std::string username;
    std::string password;
    std::string hostname;
    std::string certificate;
  };

  // To Read the own user when connecting to client
  auto readUserInfo(const QString& filePath) -> UserInfo {
    UserInfo userInfo;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      qWarning("Couldn't open the file.");
      return userInfo;
    }

    QString val = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject json = doc.object();

    if (!json.contains("username") || !json.contains("password") ||
        !json.contains("hostname") || !json.contains("certificate")) {
      qWarning("Missing required information in JSON.");
    }
    // Ok to continue, will set to empty string uf json element is missing

    userInfo.username = json["username"].toString().toStdString();
    userInfo.password = json["password"].toString().toStdString();
    userInfo.hostname = json["hostname"].toString().toStdString();
    userInfo.certificate = json["certificate"].toString().toStdString();

    return userInfo;
  }
};

#endif  // JSONHANDLER_HPP
