#include "sylar/http/httpclient_parser.h"
#include "sylar/http/http11_parser.h"
#include <string>
#include <iostream>

void function(void *data, const char *at, size_t length){}

void field(void *data, const char *field, size_t flen, const char *value, size_t vlen){}

void testRequest()
{
    
    char buffer[] = "GET /sylar/ HTTP/1.1\r\n"
                    "Content-Length:173\r\n"
                    "Content-Type:text/html\r\n"
                    "Content-Length:4\r\n\r\nbody";
    http_parser m_parser;
    http_parser_init(&m_parser);
    m_parser.request_method = nullptr;
    m_parser.request_uri    = nullptr;
    m_parser.fragment       = nullptr;
    m_parser.request_path   = nullptr;
    m_parser.query_string   = nullptr;
    m_parser.http_version   = nullptr;
    m_parser.header_done    = nullptr;
    m_parser.http_field     = nullptr;
    m_parser.data           = nullptr;
    int nread = http_parser_execute(&m_parser, buffer, sizeof(buffer), 0);
    std::cout << std::string(buffer, nread);
}

void testResponse()
{
   char buffer[] =  "1f60\r\n"
                    "Content-Length:173\r\n"
                    "12\r\n"
                    "Content-Length:422\r\n"
                    "12\r\n"
                    "Content-Length:422\r\n"
                    "0\r\n\r\n";
    httpclient_parser m_parser;
    int offset = 0;
    while(true)
    {
        httpclient_parser_init(&m_parser);
        m_parser.reason_phrase = nullptr;
        m_parser.status_code   = nullptr;
        m_parser.chunk_size    = nullptr;
        m_parser.http_version  = nullptr;
        m_parser.header_done   = nullptr;
        m_parser.last_chunk    = nullptr;
        m_parser.http_field    = nullptr;
        m_parser.data          = nullptr;
        int nread = httpclient_parser_execute(&m_parser, buffer, sizeof(buffer), offset);
        if(m_parser.chunks_done)
        {
            std::cout << "parser down" << std::endl;
            break;
        }
        std::cout << "content_len = " << m_parser.content_len << std::endl;
        if(httpclient_parser_has_error(&m_parser))
            std::cout << "error is comming" << std::endl;
        // std::cout << "nread: " << nread <<std::endl;
        // std::cout << std::string(buffer, nread) << std::endl;
        // offset += nread;
        // std::cout << std::string(buffer + offset, m_parser.content_len) << std::endl;
        
        // offset += m_parser.content_len + 2;
        //std::cout << "buffer[offset] = " << buffer[offset + 2] << std::endl;
        break;
    }
}

int main()
{
    testResponse();
    return 0;
}