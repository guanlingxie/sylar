#include "http_parser.h"
#include "sylar/log.h"
#include "sylar/config.h"

#include <string.h>

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");
static sylar::ConfigVar<uint64_t>::ptr g_http_request_buffersize = 
    sylar::Config::Lookup("http.request.buffersize",(uint64_t)(4 * 1024), "http request buffersize");
static sylar::ConfigVar<uint64_t>::ptr g_http_request_max_bodysize = 
    sylar::Config::Lookup("http.request.max_bodysize", (uint64_t)(64 * 1024 * 1024), "http request max bodysize");

static uint64_t s_http_request_buffersize;
static uint64_t s_http_request_max_badysize;

struct _RequestSizeIniter
{
    _RequestSizeIniter()
    {
        s_http_request_buffersize = g_http_request_buffersize->getValue();
        s_http_request_max_badysize = g_http_request_max_bodysize->getValue();
        g_http_request_buffersize->addListener([](const int64_t &oldvalue, const int64_t &newvalue){
            SYLAR_LOG_INFO(g_logger) << "http request buffersize from" << oldvalue << " to " << newvalue;

            s_http_request_buffersize = newvalue;
        });
        g_http_request_max_bodysize->addListener([](const int64_t &oldvalue, const int64_t &newvalue){
            SYLAR_LOG_INFO(g_logger) << "http request max bodysize from" << oldvalue << " to " << newvalue;
            s_http_request_max_badysize = newvalue;
        });
    }
};
static _RequestSizeIniter _initer;

namespace sylar
{
namespace http
{


void on_request_method(void *data, const char *at, size_t length)
{
    HttpRequestParser *parser = static_cast<HttpRequestParser *>(data);
    HttpMethod m = StringToHttpMethod(std::string(at, length));
    if(m == HttpMethod::INVALID)
    {
        SYLAR_LOG_WARN(g_logger) << "invalid http request method" << std::string(at, length);
        parser->setError(1000);
        return;
    }
    parser->getRequest()->setMethod(m);
}

void on_request_uri(void *data, const char *at, size_t length)
{
    //HttpRequestParser *parser = static_cast<HttpRequestParser *>(data);
}

void on_request_fragment(void *data, const char *at, size_t length)
{
    HttpRequestParser *parser = static_cast<HttpRequestParser *>(data);
    parser->getRequest()->setFragment(std::string(at,length));
}

void on_request_path(void *data, const char *at, size_t length)
{
    HttpRequestParser *parser = static_cast<HttpRequestParser *>(data);
    parser->getRequest()->setPath(std::string(at, length));
}

void on_request_query(void *data, const char *at, size_t length)
{
    HttpRequestParser *parser = static_cast<HttpRequestParser *>(data);
    parser->getRequest()->setQuery(std::string(at, length));
}

void on_request_version(void *data, const char *at, size_t length)
{
    HttpRequestParser *parser = static_cast<HttpRequestParser *>(data);
    if(strncmp(at, "HTTP/1.1", length) == 0)
    {
        parser->getRequest()->setVersion((uint8_t)0x11);
    }else if(strncmp(at, "HTTP/1.0", length) == 0)
    {
        parser->getRequest()->setVersion((uint8_t)0x10);
    }else{
        parser->setError(1001);
        SYLAR_LOG_WARN(g_logger) << "invalid http request version: " << std::string(at, length);
    }
}

void on_request_header_done(void *data, const char *at, size_t length)
{
    //HttpRequestParser *parser = static_cast<HttpRequestParser *>(data);
}

void on_request_http_field(void *data, const char *field, size_t flen, const char *value, size_t vlen)
{
    HttpRequestParser *parser = static_cast<HttpRequestParser *>(data);
    if(flen == 0)
    {
        SYLAR_LOG_WARN(g_logger) << "invalid http request field length = 0";
        parser->setError(1002);
        return;
    }
    parser->getRequest()->setHeader(std::string(field, flen), std::string(value, vlen));
}

HttpRequestParser::HttpRequestParser()
    :m_error(0){
    m_request.reset(new HttpRequest);
    http_parser_init(&m_parser);
    m_parser.request_method = on_request_method;
    m_parser.request_uri = on_request_uri;
    m_parser.fragment = on_request_fragment;
    m_parser.request_path = on_request_path;
    m_parser.query_string = on_request_query;
    m_parser.http_version = on_request_version;
    m_parser.header_done = on_request_header_done;
    m_parser.http_field = on_request_http_field;
    m_parser.data = this;
}

int HttpRequestParser::isFinished()
{
    return http_parser_finish(&m_parser);
}

int HttpRequestParser::hasError()
{
    return m_error || http_parser_has_error(&m_parser);
}

size_t HttpRequestParser::execute(char *data, size_t len, size_t off)
{
    return  http_parser_execute(&m_parser, data, len, off);
}

uint64_t HttpRequestParser::getContentLength()
{
    return m_request->getHeaderAs<uint64_t>("content-length", 0);
}

void on_response_reason(void *data, const char *at, size_t length)
{
    HttpResponseParser *parser = static_cast<HttpResponseParser *>(data);
    parser->getResponse()->setReason(std::string(at, length));
}

void on_response_status(void *data, const char *at, size_t length)
{
    HttpResponseParser *parser = static_cast<HttpResponseParser *>(data);
    HttpStatus status = (HttpStatus)(atoi(at));
    parser->getResponse()->setStatus(status);  
}

void on_response_chunk(void *data, const char *at, size_t length)
{

}

void on_response_version(void *data, const char *at, size_t length)
{
    HttpResponseParser *parser = static_cast<HttpResponseParser *>(data);
    if(strncmp(at, "HTTP/1.1", length) == 0)
    {
        parser->getResponse()->setVersion((uint8_t)0x11);
    }else if(strncmp(at, "HTTP/1.0", length) == 0)
    {
        parser->getResponse()->setVersion((uint8_t)0x10);
    }else{
        SYLAR_LOG_WARN(g_logger) << "invalid http response version: " << std::string(at, length);
        parser->setError(1001);
    }
}

void on_response_header_done(void *data, const char *at, size_t length)
{

}

void on_response_last_chunk(void *data, const char *at, size_t length)
{

}

void on_response_http_field(void *data, const char *field, size_t flen, const char *value, size_t vlen)
{
    HttpResponseParser *parser = static_cast<HttpResponseParser *>(data);
    if(flen == 0)
    {
        SYLAR_LOG_WARN(g_logger) << "invalid http request field length = 0";
        parser->setError(1002);
        return;
    }
    parser->getResponse()->setHeader(std::string(field, flen), std::string(value, vlen));
}


HttpResponseParser::HttpResponseParser()
    :m_error(0)
{ 
    m_response.reset(new HttpResponse);
    httpclient_parser_init(&m_parser);
    m_parser.reason_phrase = on_response_reason;
    m_parser.status_code = on_response_status;
    m_parser.chunk_size = on_response_chunk;
    m_parser.http_version = on_response_version;
    m_parser.header_done = on_response_header_done;
    m_parser.last_chunk = on_response_last_chunk;
    m_parser.http_field = on_response_http_field;
    m_parser.data = this;
}
int HttpResponseParser::isFinished()
{
    return httpclient_parser_finish(&m_parser);
}
int HttpResponseParser::hasError()
{
    return m_error || httpclient_parser_finish(&m_parser);
}
size_t HttpResponseParser::execute(char *data, size_t len, size_t off)
{
    
    return httpclient_parser_execute(&m_parser, data, len, off);
}

uint64_t HttpResponseParser::getContentLength()
{
    return m_response->getHeaderAs<uint64_t>("content-length", 0);
}

}
}