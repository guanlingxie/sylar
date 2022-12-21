#include "sylar/http/http_server.h"
#include "sylar/sylar.h"
#include <sstream>

sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

void run()
{
    sylar::http::HttpServer::ptr server(new sylar::http::HttpServer);
    sylar::Address::ptr addr = sylar::Address::LookupAny("0.0.0.0:8020");
    auto dispatch = server->getDispatch();
    dispatch->addServlet("/xitong/xx", [](sylar::http::HttpRequest::ptr req, sylar::http::HttpResponse::ptr rsp
                    , sylar::http::HttpSession::ptr session){
        std::stringstream ss;
        req->dump(ss);
        rsp->setBody(ss.str());
        return 0;
    });
    dispatch->addGlobServlet("/xitong/*", [](sylar::http::HttpRequest::ptr req, sylar::http::HttpResponse::ptr rsp
                    , sylar::http::HttpSession::ptr session){
        std::stringstream ss;
        req->dump(ss);
        rsp->setBody("global \r\n" + ss.str());
        return 0;
    });
    server->bind(addr);
    server->start();
}


int main()
{
    sylar::IOManager iom(2);
    iom.schedule(run);
    return 0;
}