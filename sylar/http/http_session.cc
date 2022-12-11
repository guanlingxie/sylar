#include "http_session.h"
#include "http_parser.h"

namespace sylar
{
namespace http
{

HttpSession::HttpSession(Socket::ptr sock, bool owner)
    :SocketStream(sock,owner)
{

}

HttpRequest::ptr HttpSession::recvRequest()
{
    HttpRequestParser::ptr parser(new HttpRequestParser);
    uint64_t buffsize = HttpRequestParser::GetHttpRequestBufferSize();
    std::shared_ptr<char> buffer(new char[buffsize],[](char *ptr)
    {
        delete[] ptr;
    });
    
    char *data = buffer.get();
    int offset = 0;
    int readChar = 0;
    do{
        int readLen = read(data + offset, buffsize);
        if(readLen <= 0)
            return nullptr;
        size_t nparse = parser->execute(data, readLen, offset);
        if(parser->hasError())
            return nullptr;
        offset += nparse;
        readChar += readLen;
        buffsize -= readLen;
        if(parser->isFinished())
            break;
        if(buffsize == 0)
            return nullptr;
    }while(true);
    int64_t length = parser->getContentLength();
    if(length > 0)
    {
        std::string body;
        body.reserve(length);
        if(readChar > offset)
        {
            body.append(data + offset, readChar - offset);
            length -= (readChar - offset);
        }
        if(length > 0)
        {
            if(readFixSize(&body[body.size()] , length) <= 0)
                return nullptr;
        }
        parser->getRequest()->setBody(body);
    }
    return parser->getRequest();
}
int HttpSession::sendResponse(HttpResponse::ptr rsp)
{
    std::stringstream ss;
    rsp->dump(ss);
    std::string data = ss.str();
    writeFixSize(data.c_str(), data.size());
    return 0;
}

}
}