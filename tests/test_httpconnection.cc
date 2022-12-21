#include "sylar/sylar.h"
#include "sylar/http/http_connection.h"
#include <iostream>
#include <fstream>

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");
void test_connection()
{
    sylar::Address::ptr addr = sylar::IPAddress::LookupAnyIPAddress("www.sylar.top:80");
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
    req->setPath("/blog/");
    req->setHeader("Accept","text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9");
    req->setHeader("Accept-Encoding","deflate, br");
    req->setHeader("Accept-Language", "zh-CN,zh;q=0.9");
    req->setHeader("Connection", "keep-alive");
    req->setHeader("host", "www.sylar.top");
    req->dump(std::cout);
    con->sendRequest(req);
    auto it = con->recvResponse();
    if(it == nullptr)
    {
        SYLAR_LOG_ERROR(g_logger) << "recvResponse is nullptr";
    }
    //std::cout << it.use_count() << std::endl;
    std::ofstream of("/home/xitong/html.gzip", std::ios::binary);
    it->dump(std::cout);
}

void test_connection_static()
{
    //std::cout << "this is test_connection_static" << std::endl;
    std::map<std::string, std::string> m_map;
    m_map["Accept"] = "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9";
    m_map["Accept-Encoding"]="deflate, br";
    m_map["Accept-Language"]="zh-CN,zh;q=0.9";
    m_map["Connection"]= "keep-alive";
    //m_map["Cookie"]= "td_cookie=2214810093";
    m_map["Host"]="www.sylar.top";
    //m_map["Upgrade-Insecure-Requests"] = "1";
    //m_map["User-Agent" ]="Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/108.0.0.0 Safari/537.36";
    sylar::http::HttpResult::ptr res = sylar::http::HttpConnection::DoGet("http://www.sylar.top/blog/", 300, m_map);
    res->response->dump(std::cout);
}

int main()
{
    sylar::IOManager iom(2);
    iom.schedule(test_connection_static);
    iom.start();
    return 0;
}