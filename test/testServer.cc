/**
  ******************************************************************************
  * @file           : testServer.cc
  * @author         : GOD婴宁
  * @brief          : None
  * @attention      : None
  * @date           : 2025/2/14
  ******************************************************************************
  */
#include "workflow/WFFacilities.h"
#include <wfrest/HttpServer.h>
#include <iostream>
#include <csignal>

using namespace std;
using namespace wfrest;

static WFFacilities::WaitGroup waitGroup(1);

void handler(int signum) {
    cout << "signal = " << signum << "\n";
    waitGroup.done();
}

int main(){
    signal(SIGINT, handler);
    HttpServer server;
    server.GET("/test", [](const HttpReq *req, HttpResp *resp) {
        const void *body;
        size_t size;
        req->get_parsed_body(&body, &size);
        string &Body = req->body();

        resp->headers["Content-Type"] = "text/plain";
        resp->headers["Content-Disposition"] = "attachment; filename=\"Server.cpp\"";
        resp->File("Server.cc");
        resp->append_output_body("Hello, World!\n");
        cout << "body = " << (char *)body << "\n";
        cout << "------------------\n";
        cout << "Body = " << Body << "\n";
    });
    server.GET("/index", [](const HttpReq *req, HttpResp *resp) {
//        resp->headers["Content-Type"] = "text/html";
//        resp->String("<html><body><h1>Hello, World!</h1></body></html>");
//        resp->File("index.html");
        resp->headers["Content-Type"] = "application/octet-stream";
        resp->headers["Content-Disposition"] = "attachment; filename=\"番茄动漫.apk\"";
        resp->File("../resource/temp/base.apk");
    });
    server.Static("/public", "../resource/temp");
    if (server.track().start(1234) == 0) {
        server.list_routes();
        waitGroup.wait();
        server.stop();
    }
    return 0;
}
