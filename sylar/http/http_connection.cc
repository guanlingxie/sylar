#include "http_connection.h"
#include "http_parser.h"
#include "sylar/log.h"

#include <functional>

sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

namespace sylar
{
namespace http
{



HttpResult::ptr HttpConnection::DoGet(const std::string& url
                            , uint64_t timeout_ms
                            , const std::map<std::string, std::string>& headers
                            , const std::string& body)
{
    return DoRequest(HttpMethod::GET, url, timeout_ms, headers, body); 
}

HttpResult::ptr HttpConnection::DoGet(Uri::ptr uri
                        , uint64_t timeout_ms
                        , const std::map<std::string, std::string>& headers
                        , const std::string& body)
{
    return DoRequest(HttpMethod::GET, uri, timeout_ms, headers, body);
}

HttpResult::ptr HttpConnection::DoPost(const std::string& url
                        , uint64_t timeout_ms
                        , const std::map<std::string, std::string>& headers
                        , const std::string& body)
{
    return DoRequest(HttpMethod::POST, url, timeout_ms, headers, body);
}
HttpResult::ptr HttpConnection::DoPost(Uri::ptr uri
                        , uint64_t timeout_ms
                        , const std::map<std::string, std::string>& headers
                        , const std::string& body)
{
    return DoRequest(HttpMethod::POST, uri, timeout_ms, headers, body);
}

HttpResult::ptr HttpConnection::DoRequest(HttpMethod method
                        , const std::string& url
                        , uint64_t timeout_ms
                        , const std::map<std::string, std::string>& headers
                        , const std::string& body){
    Uri::ptr uri = Uri::Create(url);
    return DoRequest(method, uri, timeout_ms, headers, body);
}

HttpResult::ptr HttpConnection::DoRequest(HttpMethod method
                        , Uri::ptr uri
                        , uint64_t timeout_ms
                        , const std::map<std::string, std::string>& headers
                        , const std::string& body)
{
   
    HttpRequest::ptr req = std::make_shared<HttpRequest>();
    req->setMethod(method);
    req->setPath(uri->getPath());
    req->setQuery(uri->getQuery());
    req->setFragment(uri->getFragment());
    req->setHeader("host", uri->getHost());
    for(auto &v : headers)
    {
        req->setHeader(v.first, v.second);
    }
    req->setBody(body);
    return DoRequest(req, uri, timeout_ms);
}


HttpResult::ptr HttpConnection::DoRequest(HttpRequest::ptr req
                        , Uri::ptr uri
                        , uint64_t timeout_ms)
{
    if(uri == nullptr)
    {
        return std::make_shared<HttpResult>(HttpResult::Error::INVALID_URL, nullptr, "invalid url" + uri->toString());
    }
    Address::ptr addr = uri->createAddress();
    if(addr == nullptr)
        return std::make_shared<HttpResult>(HttpResult::Error::INVALID_HOST, nullptr, "invalid host" + uri->getHost());
    Socket::ptr sock = Socket::CreateTCP(addr);
    if(!sock->connect(addr))
        return std::make_shared<HttpResult>(HttpResult::Error::INVALID_HOST, nullptr, "invalid host" + uri->getHost());
    sock->setRecvTimeout(timeout_ms);
    HttpConnection::ptr conn = std::make_shared<HttpConnection>(sock);
    conn->sendRequest(req);
    HttpResponse::ptr rsp = conn->recvResponse();
    if(rsp == nullptr)
    {
        return std::make_shared<HttpResult>(HttpResult::Error::TIMEOUT
                    , nullptr, "recv response timeout: " + addr->toString()
                    + " timeout_ms:" + std::to_string(timeout_ms));
    }
    return std::make_shared<HttpResult>(HttpResult::Error::OK, rsp, "ok");

}

HttpConnection::HttpConnection(Socket::ptr sock, bool owner)
    :SocketStream(sock,owner)
{

}

HttpResponse::ptr HttpConnection::recvResponse()
{
    HttpResponseParser::ptr parser(new HttpResponseParser);
    uint64_t originBuffsize = HttpResponseParser::GetHttpResponseBufferSize();
    uint64_t buffsize = originBuffsize;
    std::shared_ptr<char> buffer(new char[originBuffsize + 1],[](char *ptr)
    {
        delete[] ptr;
    });
    char *data = buffer.get();
    memset(data, 0, originBuffsize + 1);
    int offset = 0;
    int readChar = 0;
    do{
        int readLen = read(data + offset, buffsize);
        if(readLen <= 0){
            close();
            SYLAR_LOG_DEBUG(g_logger) << "remote is close";
            return nullptr;
            
        }
        readChar += readLen;
        size_t nparse = parser->execute(data, readChar, offset);
        if(parser->hasError()){
            close();
            SYLAR_LOG_ERROR(g_logger) << "parser is error";
            return nullptr;
        }
        offset = nparse;
        buffsize -= readLen;
        if(parser->isFinished())
            break;
        if(buffsize == 0){
            close();
            SYLAR_LOG_WARN(g_logger) << "Http head is too large";
            return nullptr;
        }
    }while(true);        
    auto &client_parser = parser->getParser();
    if(client_parser.chunked)
    {
        std::string body;
        do{ 
            if(offset > 0){
                memmove(data, data + offset, readChar - offset);
                memset(data + readChar - offset, 0, originBuffsize - readChar + offset);
                readChar -= offset;
                offset = 0;
            }
            buffsize = originBuffsize - readChar;
            bool begin = true;
            httpclient_parser_init(&client_parser);
            do{
                if(buffsize == 0){
                    close();
                    SYLAR_LOG_WARN(g_logger) << "Http chunk head is too large";
                    return nullptr;
                }
                if(!begin || readChar == 0)
                {
                    int readLen = read(data + offset, buffsize);
                    if(readLen < 0)
                    {
                        close();
                        SYLAR_LOG_INFO(g_logger) << "connection is close" << m_socket->toString();
                        return nullptr;
                    }
                    readChar += readLen;
                    buffsize -= readLen;
                }
                int nread = parser->execute(data, readChar, offset);
                if (parser->hasError())
                {
                    close();
                    SYLAR_LOG_ERROR(g_logger) << "parser has error";
                    return nullptr;
                }
                offset = nread;
                begin = false;
            }while(!parser->isFinished());
            int64_t length = client_parser.content_len;
            if(client_parser.chunks_done)
            {
                if(readChar < offset + 2)
                {
                    readFixSize(data, 2 - readChar - offset);
                }
                break;
            }
            
            if(length + 2 < readChar - offset)
            {
                body.append(data + offset, length);
                offset += (length + 2);
            }else{
                size_t bodyLen = body.size();
                body.resize(bodyLen + length);
                if(readChar > offset)
                {
                    memcpy(&body[bodyLen], data + offset, readChar - offset);
                    bodyLen += readChar - offset;
                    length -= readChar - offset;
                }
                if(length > 0)
                {
                    if(readFixSize(&body[bodyLen], length) <= 0)
                    {
                        close();
                        SYLAR_LOG_ERROR(g_logger) << "cannot readFixsize : " << length;
                        return nullptr;
                    }
                    if(readFixSize(data, 2) <= 0)
                    {
                        close();
                        SYLAR_LOG_ERROR(g_logger) << "cannot readFixsize : " << length;
                        return nullptr;
                    }
                }else{
                    if(2 + length > 0){
                        if(readFixSize(data, 2 + length) <= 0)
                        {
                            close();
                            SYLAR_LOG_ERROR(g_logger) << "cannot readFixsize : " << length + 2;
                            return nullptr;
                        }
                    }
                }
                offset = 0;
                readChar = 0;
                memset(data, 0 , originBuffsize);
            }
        }while(true);
        parser->getResponse()->setBody(body);
    }else{
        int64_t length = parser->getContentLength();

        if(length > 0)
        {
            std::string body;
            body.resize(length);
            int len = 0;
            if(readChar > offset)
            {
                memcpy(&body[0] , data + offset, readChar - offset);
                len = readChar - offset;
                length -= (readChar - offset);
            }
            if(length > 0)
            {
                if(readFixSize(&body[len] , length) <= 0){
                    close();
                    SYLAR_LOG_ERROR(g_logger) << "readFixSize if error";
                    return nullptr;
                }
            }
            parser->getResponse()->setBody(body);
        }
    }
    return parser->getResponse();
}

int HttpConnection::sendRequest(HttpRequest::ptr rsp)
{
    std::stringstream ss;
    rsp->dump(ss);
    std::string data = ss.str();
    
    writeFixSize(data.c_str(), data.size());
    return 0;
}


HttpConnectionPool::HttpConnectionPool(const std::string &host
                        ,const std::string &vhost
                        ,uint32_t port
                        ,uint32_t maxsize 
                        ,uint32_t maxAlivetime
                        ,uint32_t maxRequest):
    m_host(host)
    ,m_vhost(vhost)
    ,m_port(port)
    ,m_maxsize(maxsize)
    ,m_maxAliveTime(maxAlivetime)
    ,m_maxRequest(maxRequest)
{

}

HttpConnection::ptr HttpConnectionPool::getConnection()
{
    uint64_t now_ms = sylar::GetCurrentMS();
    std::vector<HttpConnection*> invalid_conns;
    HttpConnection* ptr = nullptr;
    MutexType::Lock lock(m_mutex);
    while(!m_conns.empty()) {
        auto conn = *m_conns.begin();
        m_conns.pop_front();
        if(!conn->isConnected()) {
            invalid_conns.push_back(conn);
            continue;
        }
        if((conn->m_createTime + m_maxAliveTime) > now_ms) {
            invalid_conns.push_back(conn);
            continue;
        }
        ptr = conn;
        break;
    }
    lock.unlock();
    for(auto i : invalid_conns) {
        delete i;
    }
    m_total -= invalid_conns.size();

    if(!ptr) {
        IPAddress::ptr addr = IPAddress::LookupAnyIPAddress(m_host);
        if(!addr) {
            SYLAR_LOG_ERROR(g_logger) << "get addr fail: " << m_host;
            return nullptr;
        }
        addr->setPort(m_port);
        Socket::ptr sock = Socket::CreateTCP(addr);
        if(!sock) {
            SYLAR_LOG_ERROR(g_logger) << "create sock fail: " << addr->toString();
            return nullptr;
        }
        if(!sock->connect(addr)) {
            SYLAR_LOG_ERROR(g_logger) << "sock connect fail: " << addr->toString();
            return nullptr;
        }

        ptr = new HttpConnection(sock);
        ++m_total;
    }
    return HttpConnection::ptr(ptr, std::bind(&HttpConnectionPool::ReleasePtr
                               , std::placeholders::_1, this));
}

void HttpConnectionPool::ReleasePtr(HttpConnection* ptr, HttpConnectionPool* pool) {
    ++ptr->m_request;
    if(!ptr->isConnected()
            || ((ptr->m_createTime + pool->m_maxAliveTime) >= sylar::GetCurrentMS())
            || (ptr->m_request >= pool->m_maxRequest)) {
        delete ptr;
        --pool->m_total;
        return;
    }
    MutexType::Lock lock(pool->m_mutex);
    pool->m_conns.push_back(ptr);
}

HttpResult::ptr HttpConnectionPool::doGet(const std::string &path_query_fragment
                        , uint64_t timeout_ms
                        , const std::map<std::string, std::string> &headers
                        , const std::string &body)
{
    return doRequest(HttpMethod::GET, path_query_fragment, timeout_ms, headers, body);
}
HttpResult::ptr HttpConnectionPool::doPost(const std::string &path_query_fragment
                        , uint64_t timeout_ms
                        , const std::map<std::string, std::string> &headers
                        , const std::string &body)
{
    return doRequest(HttpMethod::POST, path_query_fragment, timeout_ms, headers, body);
}

HttpResult::ptr HttpConnectionPool::doRequest(HttpMethod method
                        , const std::string &path_query_fragment
                        , uint64_t timeout_ms
                        , const std::map<std::string, std::string> &headers
                        , const std::string &body)
{
    std::string path;
    std::string query;
    std::string fragment;
    auto len = path_query_fragment.size();
    auto pos1 = path_query_fragment.find_first_of('?');
    auto pos2 = path_query_fragment.find_last_of('#');
    path = std::string(path_query_fragment, 0, std::min(std::min(len, pos1), pos2));
    if(pos1 != std::string::npos)
        query = std::string(path_query_fragment, pos1 + 1, std::min(len, pos2) - pos1 - 1);
    if(pos2 != std::string::npos)
        fragment = std::string(path_query_fragment, pos2 + 1, len - pos2 - 1);
    HttpRequest::ptr req = std::make_shared<HttpRequest>();
    req->setMethod(method);
    req->setPath(path);
    req->setClose(false);
    req->setHeader("host", m_host);
    for(auto &v : headers)
    {
        req->setHeader(v.first, v.second);
    }
    req->setBody(body);
    return doRequest(req, timeout_ms);
}
HttpResult::ptr HttpConnectionPool::doRequest(HttpRequest::ptr req,
                        uint64_t timeout_ms)
{
    auto conn = getConnection();
    if(!conn) {
        return std::make_shared<HttpResult>(HttpResult::Error::POOL_GET_CONNECTION
                , nullptr, "pool host:" + m_host + " port:" + std::to_string(m_port));
    }
    auto sock = conn->getSocket();
    if(!sock) {
        return std::make_shared<HttpResult>(HttpResult::Error::POOL_INVALID_CONNECTION
                , nullptr, "pool host:" + m_host + " port:" + std::to_string(m_port));
    }
    sock->setRecvTimeout(timeout_ms);
    conn->sendRequest(req);
    HttpResponse::ptr rsp = conn->recvResponse();
    if(rsp == nullptr)
    {
        return std::make_shared<HttpResult>(HttpResult::Error::TIMEOUT
                    , nullptr, "recv response timeout: " + sock->toString()
                    + " timeout_ms:" + std::to_string(timeout_ms));
    }
    return std::make_shared<HttpResult>(HttpResult::Error::OK, rsp, "ok");
}

}
}

