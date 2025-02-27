/**
  ******************************************************************************
  * @file           : SearchEngineServer.cc
  * @author         : GOD婴宁
  * @brief          : None
  * @attention      : None
  * @date           : 2025/2/12
  ******************************************************************************
  */

#include "SearchEngineServer.h"
#include "wfrest/CodeUtil.h"
#include "KeyRecommender.h"
#include "WebPageQuery.h"
#include "SplitToolCppJieba.h"

SearchEngineServer::SearchEngineServer(int cnt)
        : _waitGroup(cnt) {

}

SearchEngineServer::~SearchEngineServer() {

}

void SearchEngineServer::start(us port) {
    if (_httpServer.track().start(port) == 0) {
        _httpServer.list_routes();
        _waitGroup.wait();
        _httpServer.stop();
    } else {
        cerr << "Server start failed" << endl;
    }
}

void SearchEngineServer::loadModels() {
    loadStaticResourcesModel();
    loadCandidateModel();
    loadSearchModel();
}

void SearchEngineServer::loadStaticResourcesModel() {
    _httpServer.GET("/candidate", [](const HttpReq *req, HttpResp *resp) {
        resp->add_header("Content-Type", "text/html; charset=utf-8");
        resp->File("../resource/static/view/search.html");
    });
    _httpServer.GET("/search", [](const HttpReq *req, HttpResp *resp) {
        resp->add_header("Content-Type", "text/html; charset=utf-8");
        resp->File("../resource/static/view/search.html");
    });
}

void SearchEngineServer::loadCandidateModel() {
    _httpServer.POST("/candidate", [](const HttpReq *req, HttpResp *resp) {
        if (req->content_type() != APPLICATION_URLENCODED) {
            resp->set_status(HttpStatusBadRequest);
            return;
        }
        auto query = req->form_kv();
        string context = query["query"];
        string decode = context;
        if (CodeUtil::is_url_encode(context)) {
//            cout << "is url encode" << endl;
            decode = CodeUtil::url_decode(context);
        }
        cout << "query: " << decode << endl;
        KeyRecommender keyRecommender(decode);
        keyRecommender.doQuery();
        string result = keyRecommender.getCandidateWord();
        resp->add_header("Content-Type", "application/json; charset=utf-8");
        resp->String(result);
    });
}

void SearchEngineServer::loadSearchModel() {
    _httpServer.POST("/search", [](const HttpReq *req, HttpResp *resp) {
        if (req->content_type() != APPLICATION_URLENCODED) {
            resp->set_status(HttpStatusBadRequest);
            return;
        }
        auto query = req->form_kv();
        string context = query["query"];
        string decode = context;
        if (CodeUtil::is_url_encode(context)) {
            decode = CodeUtil::url_decode(context);
        }
        cout << "query: " << decode << endl;
        SplitToolCppJieBa *splitTool = SplitToolCppJieBa::getInstance();
        WebPageQuery webPageQuery(splitTool);
        string&& result = webPageQuery.doQuery(decode);
        resp->add_header("Content-Type", "application/json; charset=utf-8");
        cout << "result size: " << result.size() << endl;
        resp->String(std::move(result));
    });
}

SearchEngineServer *SearchEngineServer::getInstance() {
    static SearchEngineServer instance(1);
    return &instance;
}
