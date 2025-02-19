/**
  ******************************************************************************
  * @file           : Server.cc
  * @author         : GOD婴宁
  * @brief          : None
  * @attention      : None
  * @date           : 2025/2/12
  ******************************************************************************
  */
#include "SearchEngineServer.h"
#include <csignal>

SearchEngineServer *server = SearchEngineServer::getInstance();

void SignalHandler(int signum) {
    cout << "Interrupt signal (" << signum << ") received.\n";
    server->stop();
}

int main() {
    signal(SIGINT, SignalHandler);
    server->loadModels();
    server->start(1234);
//    cout << sizeof(server) << endl; 8bytes
//    cout << sizeof(*server) << endl; 720Bytes
    return 0;
}