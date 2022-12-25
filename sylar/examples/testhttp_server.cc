#include "sylar/http/http_server.h"
#include "sylar/log.h"

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();
sylar::IOManager::ptr worker;
void run() {
    //g_logger->setLevel(sylar::LogLevel::INFO);
    //SYLAR_LOG_ERROR(g_logger) << "get address error";
    sylar::Address::ptr addr = sylar::IPAddress::LookupAnyIPAddress("0.0.0.0:8020");
    if(!addr) {
        SYLAR_LOG_ERROR(g_logger) << "get address error";
        return;
    }

    //sylar::http::HttpServer::ptr http_server(new sylar::http::HttpServer(true, worker.get()));
    sylar::http::HttpServer::ptr http_server(new sylar::http::HttpServer());
    while(!http_server->bind(addr)) {
        SYLAR_LOG_ERROR(g_logger) << "bind " <<  " fail";
        sleep(1);
    }

    http_server->start();
}

int main(int argc, char** argv) {
    //g_logger->setLevel(sylar::LogLevel::INFO);
    sylar::IOManager iom(1);
    worker.reset(new sylar::IOManager(4, false));
    iom.schedule(run);
    return 0;
}