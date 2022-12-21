#include "sylar/sylar.h"
#include "sylar/http/http_connection.h"
#include <iostream>
#include <fstream>

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");
void test_connection()
{
    sylar::Address::ptr addr = sylar::IPAddress::LookupAnyIPAddress("www.baidu.com:80");
    if(!addr)
    {
        SYLAR_LOG_ERROR(g_logger) << "addr error";
    }
    sylar::Socket::ptr sock = sylar::Socket::CreateTCP(addr);
    int rt = sock->connect(addr);
    if(!rt)
    {
        SYLAR_LOG_ERROR(g_logger) << "connect";
    }
    
    sylar::http::HttpConnection::ptr con(new sylar::http::HttpConnection(sock));
    sylar::http::HttpRequest::ptr req(new sylar::http::HttpRequest);
    //req->setPath("/blog/");
    req->setHeader("Accept","text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9");
    req->setHeader("Accept-Encoding","gzip, deflate, br");
    req->setHeader("Accept-Language", "zh-CN,zh;q=0.9");
    req->setHeader("Connection", "keep-alive");
    //req->setHeader("host", "www.sylar.top");
    req->dump(std::cout);
    con->sendRequest(req);
    auto it = con->recvResponse();
    if(it == nullptr)
    {
        SYLAR_LOG_ERROR(g_logger) << "recvResponse is nullptr";
    }
    //std::cout << it.use_count() << std::endl;
    std::ofstream of("/home/xitong/html.gzip", std::ios::binary);
    it->dump(of);
}

int main()
{
    sylar::IOManager iom(2);
    iom.schedule(test_connection);
    iom.start();
    return 0;
}