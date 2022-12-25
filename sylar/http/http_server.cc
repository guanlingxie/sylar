#include "http_server.h"
#include "sylar/log.h"
#include "http_session.h"

namespace sylar {
namespace http {

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

HttpServer::HttpServer(bool keepalive
               ,sylar::IOManager* worker
               ,sylar::IOManager* accept_worker)
    :TcpServer(worker, accept_worker)
    ,m_isKeepalive(keepalive) 
{
        m_dispatch.reset(new ServletDispatch);
}

void HttpServer::setName(const std::string& v) {
    TcpServer::setName(v);
}

void HttpServer::handleClient(Socket::ptr client) {
    SYLAR_LOG_DEBUG(g_logger) << "address:" << client->getRemoteAddress()->toString();
    
    HttpSession::ptr session(new HttpSession(client));
    do {
        
        auto req = session->recvRequest();
        if(!req) {
            SYLAR_LOG_WARN(g_logger) << "recv http request fail, errno = " << strerror(errno);
            break;
        }
        std::cout << "httpserver.cc 25 : " << g_logger->getLevel() << std::endl;
        SYLAR_LOG_DEBUG(g_logger) << "req:close : " << req->isClose() ? "close" : "keep-alive";
        HttpResponse::ptr rsp(new HttpResponse(req->getVersion()
                            , req->isClose() || !m_isKeepalive));
        // rsp->setHeader("Server", getName());
        // rsp->setBody("hello xitong");
        

        //req->dump(std::cout);
        //rsp->dump(std::cout);
        // std::cout << std::endl;
        m_dispatch->dispatch(req, rsp, session);
        session->sendResponse(rsp);
        if(!m_isKeepalive || req->isClose() || rsp->isClose()) {
            SYLAR_LOG_INFO(g_logger) << "this connect is close";
            break;
        }
    } while(true);
    session->close();
}

}
}