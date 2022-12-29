#include "sylar/http/http_server.h"
#include "sylar/log.h"


sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

void run()
{
    sylar::Address::ptr addr = sylar::IPAddress::LookupAnyIPAddress("0.0.0.0:8020");
    if(!addr)
    {
        SYLAR_LOG_ERROR(g_logger) << "get address error";
    }
    
    sylar::http::HttpServer::ptr http_server(new sylar::http::HttpServer(true));
    
    while(!http_server->bind(addr))
    {
        SYLAR_LOG_ERROR(g_logger) << "bind " << addr->toString() << "error";
        return;
    }

    http_server->start();
}

int main()
{
    sylar::IOManager::ptr iom(new sylar::IOManager(1));
    sylar::IOManager::ptr iom2(new sylar::IOManager(2, false));
    sylar::Address::ptr addr = sylar::IPAddress::LookupAnyIPAddress("0.0.0.0:8020");
    if(!addr)
    {
        SYLAR_LOG_ERROR(g_logger) << "get address error";
    }
    
    sylar::http::HttpServer::ptr http_server(new sylar::http::HttpServer(true, iom.get(), iom2.get()));
    
    while(!http_server->bind(addr))
    {
        SYLAR_LOG_ERROR(g_logger) << "bind " << addr->toString() << "error";
        return 0;
    }

    http_server->start();
    return 0;
}