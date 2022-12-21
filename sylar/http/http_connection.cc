#include "http_connection.h"
#include "http_parser.h"
#include "sylar/log.h"

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

}
}

