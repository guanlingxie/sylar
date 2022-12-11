#ifndef __SYLAR_HTTP_PARSER_H__
#define __SYLAR_HTTP_PARSER_H__

#include <memory>
#include <string>

#include "http.h"
#include "http11_parser.h"
#include "httpclient_parser.h"

namespace sylar
{
namespace http
{
class HttpRequestParser
{
public:
    typedef std::shared_ptr<HttpRequestParser> ptr;
    HttpRequestParser();
    int isFinished();
    int hasError();
    size_t execute(char *data, size_t len, size_t off = 0);

    HttpRequest::ptr getRequest() const {return m_request;}
    void setError(int v){m_error = v;}
    uint64_t getContentLength();

    static uint64_t GetHttpRequestBufferSize();
    static uint64_t GetHttpRequestMaxBodysize();
private:
    http_parser m_parser;
    HttpRequest::ptr m_request;
    // 1000 invalid method
    // 1001 invalid version
    // 1002 invalid field
    int m_error;
};

class HttpResponseParser
{
public:
    typedef std::shared_ptr<HttpResponseParser> ptr;
    HttpResponseParser();
    int isFinished();
    int hasError();
    size_t execute(char *data, size_t len, size_t off);
    HttpResponse::ptr getResponse() const {return m_response;}
    void setError(int v){m_error = v;}
    uint64_t getContentLength();
private:
    httpclient_parser m_parser;
    HttpResponse::ptr m_response;
    int m_error;
};

}
}

#endif