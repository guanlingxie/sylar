#ifndef __SYLAR_HTTP_CONNECTION_H__
#define __SYLAR_HTTP_CONNECTION_H__

#include "sylar/socket_stream.h"
#include "sylar/socket.h"
#include "http.h"
#include "sylar/uri.h"
#include "sylar/thread.h"

#include <string>
#include <list>
#include <memory>
#include <atomic>


namespace sylar{
namespace http{


struct HttpResult{
    typedef std::shared_ptr<HttpResult> ptr;
    enum class Error {
        /// 正常
        OK = 0,
        /// 非法URL
        INVALID_URL = 1,
        /// 无法解析HOST
        INVALID_HOST = 2,
        /// 连接失败
        CONNECT_FAIL = 3,
        /// 连接被对端关闭
        SEND_CLOSE_BY_PEER = 4,
        /// 发送请求产生Socket错误
        SEND_SOCKET_ERROR = 5,
        /// 超时
        TIMEOUT = 6,
        /// 创建Socket失败
        CREATE_SOCKET_ERROR = 7,
        /// 从连接池中取连接失败
        POOL_GET_CONNECTION = 8,
        /// 无效的连接
        POOL_INVALID_CONNECTION = 9,
    };
    HttpResult(Error _result, HttpResponse::ptr _response, const std::string &_error)
        :result(_result)
        ,response(_response)
        ,error(_error)
        {}
    Error result;
    HttpResponse::ptr response;
    std::string error;
};


class HttpConnectionPool;
class HttpConnection : public SocketStream
{
friend class HttpConnectionPool;
public:
    typedef std::shared_ptr<HttpConnection> ptr;

    static HttpResult::ptr DoGet(const std::string& url
                            , uint64_t timeout_ms
                            , const std::map<std::string, std::string>& headers = {}
                            , const std::string& body = "");

    static HttpResult::ptr DoGet(Uri::ptr uri
                            , uint64_t timeout_ms
                            , const std::map<std::string, std::string>& headers = {}
                            , const std::string& body = "");

    static HttpResult::ptr DoPost(const std::string& url
                            , uint64_t timeout_ms
                            , const std::map<std::string, std::string>& headers = {}
                            , const std::string& body = "");
    static HttpResult::ptr DoPost(Uri::ptr uri
                            , uint64_t timeout_ms
                            , const std::map<std::string, std::string>& headers = {}
                            , const std::string& body = "");

    static HttpResult::ptr DoRequest(HttpMethod method
                            , const std::string& url
                            , uint64_t timeout_ms
                            , const std::map<std::string, std::string>& headers = {}
                            , const std::string& body = "");

    static HttpResult::ptr DoRequest(HttpMethod method
                            , Uri::ptr uri
                            , uint64_t timeout_ms
                            , const std::map<std::string, std::string>& headers = {}
                            , const std::string& body = "");


    static HttpResult::ptr DoRequest(HttpRequest::ptr req
                            , Uri::ptr uri
                            , uint64_t timeout_ms);

    HttpConnection(Socket::ptr sock, bool owner = true);
    HttpResponse::ptr recvResponse();
    int sendRequest(HttpRequest::ptr req);
    

private:
    uint64_t m_createTime = 0;
    uint64_t m_request = 0;
};

class HttpConnectionPool
{
public:
    typedef std::shared_ptr<HttpConnectionPool> ptr;
    typedef Mutex MutexType;
    HttpConnectionPool(const std::string &host
                        ,const std::string &vhost
                        ,uint32_t port
                        ,uint32_t maxsize 
                        ,uint32_t maxAlivetime
                        ,uint32_t maxRequest);

    HttpConnection::ptr getConnection();
    static void ReleasePtr(HttpConnection* ptr, HttpConnectionPool* pool);
    HttpResult::ptr doGet(const std::string &path_query_fragment
                            , uint64_t timeout_ms
                            , const std::map<std::string, std::string> &headers = {} 
                            , const std::string &body = "");
    HttpResult::ptr doPost(const std::string &path_query_fragment
                            , uint64_t timeout_ms
                            , const std::map<std::string, std::string> &headers = {} 
                            , const std::string &body = "");
    
    HttpResult::ptr doRequest(HttpMethod method
                            , const std::string &path_query_fragment
                            , uint64_t timeout_ms
                            , const std::map<std::string, std::string> &headers = {} 
                            , const std::string &body = "");
    HttpResult::ptr doRequest(HttpRequest::ptr req,
                            uint64_t timeout_ms);


private:
    std::string m_host;
    std::string m_vhost;
    uint32_t m_port;
    uint32_t m_maxsize;
    uint32_t m_maxAliveTime;
    uint32_t m_maxRequest;

    MutexType m_mutex;
    std::list<HttpConnection *> m_conns;
    std::atomic<int32_t> m_total = {0};
};

}
}

#endif