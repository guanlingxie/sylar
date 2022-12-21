#include "servlet.h"
#include <fnmatch.h>

namespace sylar
{
namespace http
{

FunctionServlet::FunctionServlet(callback cb)
    :Servlet("FunctionServlet")
    ,m_cb(cb)
    {}

int32_t FunctionServlet::handle(HttpRequest::ptr req, HttpResponse::ptr rsp, HttpSession::ptr session)
{
    return m_cb(req, rsp, session);
}

ServletDispatch::ServletDispatch()
{
    m_default.reset(new NotFoundServlet("sylar/1.0"));
}

void ServletDispatch::addServlet(const std::string& uri, Servlet::ptr slt)
{
    RWMutexType::WriteLock lock(m_mutex);
    m_datas[uri] = slt;
}
void ServletDispatch::addServlet(const std::string& uri, FunctionServlet::callback cb)
{   
    RWMutexType::WriteLock lock(m_mutex);
    m_datas[uri].reset(new FunctionServlet(cb));
}

void ServletDispatch::addGlobServlet(const std::string& uri, FunctionServlet::callback cb)
{
    RWMutexType::WriteLock lock(m_mutex);
    for(auto it = m_globs.begin();
            it != m_globs.end(); ++it) {
        if(it->first == uri) {
            m_globs.erase(it);
            break;
        }
    }
    m_globs.push_back(std::make_pair(uri, FunctionServlet::ptr(new FunctionServlet(cb))));
}

void ServletDispatch::delServlet(const std::string& uri)
{
    RWMutexType::WriteLock lock(m_mutex);
    m_datas.erase(uri);
}

void ServletDispatch::delGlobServlet(const std::string& uri)
{
    RWMutexType::WriteLock lock(m_mutex);
    for(auto it = m_globs.begin(); it != m_globs.end(); ++it)
    {
        if(it->first == uri)
        {
            m_globs.erase(it);
            return;
        }
    }
}


Servlet::ptr ServletDispatch::getServlet(const std::string& uri)
{
    RWMutex::ReadLock lock(m_mutex);
    auto it = m_datas.find(uri);
    if(it != m_datas.end())
        return it->second;
    return nullptr;
}

Servlet::ptr ServletDispatch::getGlobServlet(const std::string& uri)
{
    RWMutex::ReadLock lock(m_mutex);
    for(auto it = m_globs.begin(); it != m_globs.end(); ++it)
    {
        if(fnmatch(it->first.c_str() ,uri.c_str(), 0) == 0)
        {
            return it->second;
        }
    }
    return nullptr;
}

Servlet::ptr ServletDispatch::getMatchedServlet(const std::string& uri)
{
    auto it = getServlet(uri);
    if(it != nullptr)
        return it;
    it = getGlobServlet(uri);
    if(it != nullptr)
        return it;
    return m_default;
}

void ServletDispatch::dispatch(HttpRequest::ptr req, HttpResponse::ptr rsp, HttpSession::ptr session)
{
    auto it = getMatchedServlet(req->getPath());
    if(it != nullptr)
        it->handle(req, rsp, session);
}

NotFoundServlet::NotFoundServlet(const std::string& name)
    :Servlet("NotFoundServlet")
    ,m_name(name) {
    m_content = "<html><head><title>404 Not Found"
        "</title></head><body><center><h1>404 Not Found</h1></center>"
        "<hr><center>" + name + "</center></body></html>";

}

int32_t NotFoundServlet::handle(sylar::http::HttpRequest::ptr request
                   , sylar::http::HttpResponse::ptr response
                   , sylar::http::HttpSession::ptr session) {
    response->setStatus(sylar::http::HttpStatus::NOT_FOUND);
    response->setHeader("Server", "sylar/1.0.0");
    response->setHeader("Content-Type", "text/html");
    response->setBody(m_content);
    return 0;
}


}
}