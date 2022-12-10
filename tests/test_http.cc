#include "sylar/sylar.h"
#include "sylar/http/http.h"
#include <iostream>

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();
void test_http_req()
{
    sylar::http::HttpRequest::ptr req(new sylar::http::HttpRequest);
    req->setHeader("host", "www.baidu.com");
    req->setBody("hello sylar");
    req->setMethod(sylar::http::HttpMethod::SOURCE);
    req->dump(std::cout) << std::endl;
}
void test_http_resp()
{
    sylar::http::HttpResponse::ptr req(new sylar::http::HttpResponse);
    req->setHeader("x-x", "sylar");
    req->setBody("hello sylar");
    req->setStatus(sylar::http::HttpStatus::BAD_REQUEST);
    req->dump(std::cout) << std::endl;
}


int main()
{

    test_http_resp();
    return 0;
}