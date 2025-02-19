/**
  ******************************************************************************
  * @file           : SearchEngineServer.h
  * @author         : GOD婴宁
  * @brief          : None
  * @attention      : None
  * @date           : 2025/2/12
  ******************************************************************************
  */
#pragma once

#include <workflow/WFFacilities.h>
#include <wfrest/HttpServer.h>

#include <vector>
#include <string>

using namespace wfrest;
using namespace std;

class SearchEngineServer {
    using us = unsigned short;
private:

    explicit SearchEngineServer(int cnt);

    ~SearchEngineServer();
public:
    SearchEngineServer(const SearchEngineServer &) = delete;

    SearchEngineServer &operator=(const SearchEngineServer &) = delete;


    void start(us port);

    inline void stop() {
        _waitGroup.done();
    }

    void loadModels();

    static SearchEngineServer * getInstance();
private:
    void loadStaticResourcesModel();
    void loadSearchModel();
private:
    WFFacilities::WaitGroup _waitGroup;
    wfrest::HttpServer _httpServer;
};


