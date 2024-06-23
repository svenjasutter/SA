#include <QCoreApplication>

#include "JSONHandler.hpp"
#include "XMPPClient.hpp"

auto main(int argc, char* argv[]) -> int {
  QCoreApplication app(argc, argv);

  JSONHandler jsonHandler{};
  JSONHandler::UserInfo userInfo =
      jsonHandler.readUserInfo("/user/userinfo.json");

  XMPPClient xmppClient{userInfo.hostname, userInfo.username, userInfo.password,
                        userInfo.certificate};

  xmppClient.connectToServer();

  app.exec();
}
