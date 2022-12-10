#include "sylar/sylar.h"
#include "sylar/http/http_parser.h"

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");


char request_http[] = "GET / HTTP/1.1\r\nHost: www.sylar.top\r\ncontent-length: 10\r\n\r\n12222121";
void test_parser_req()
{
    sylar::http::HttpRequestParser hrp;
    size_t ss = hrp.execute(request_http, sizeof(request_http), 0);
    SYLAR_LOG_INFO(g_logger) << ss << "has error " << hrp.hasError();
    SYLAR_LOG_INFO(g_logger) << "\n" << &request_http[ss];
    hrp.getContentLength();
    hrp.getRequest()->dump(std::cout);
}
char response_http[] = "HTTP/1.0 200 OK\r\n"
                        "Accept-Ranges: bytes\r\n"
                        "Cache-Control: no-cache\r\n"
                        "Content-Length: 9508\r\n"
                        "Content-Type: text/html\r\n"
                        "Date: Fri, 09 Dec 2022 04:50:20 GMT\r\n"
                        "P3p: CP=\" OTI DSP COR IVA OUR IND COM \"\r\n"
                        "P3p: CP=\" OTI DSP COR IVA OUR IND COM \"\r\n"
                        "Pragma: no-cache\r\n"
                        "Server: BWS/1.1\r\n";
void test_parser_resp()
{
    sylar::http::HttpResponseParser hrp;
    size_t readn = hrp.execute(response_http, sizeof(response_http), 0);
    SYLAR_LOG_INFO(g_logger) << readn << "has error " << hrp.hasError();
    hrp.getContentLength();
    hrp.getResponse()->dump(std::cout);
}

int main()
{
    test_parser_req();
    return 0;
}