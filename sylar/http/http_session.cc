#include "http_session.h"
#include "http_parser.h"
#include "sylar/log.h"

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

namespace sylar
{
    namespace http
    {

        HttpSession::HttpSession(Socket::ptr sock, bool owner)
            : SocketStream(sock, owner)
        {
        }

        HttpRequest::ptr HttpSession::recvRequest()
        {
            HttpRequestParser::ptr parser(new HttpRequestParser);
            uint64_t buffsize = HttpRequestParser::GetHttpRequestBufferSize();
            // uint64_t buffsize = 100;
            std::shared_ptr<char> buffer(new char[buffsize], [](char *ptr)
                                         { delete[] ptr; });

            char *data = buffer.get();
            memset(data, 0, buffsize);
            int offset = 0;
            int readChar = 0;
            int i = 0;
            do
            {
                int readLen = read(data + offset, buffsize);
                if (readLen <= 0)
                {
                    // SYLAR_LOG_DEBUG(g_logger) << "remote is close";
                    return nullptr;
                }
                readChar += readLen;
                size_t nparse = parser->execute(data, readChar, offset);
                if (parser->hasError())
                {
                    close();
                    return nullptr;
                }
                offset = nparse;
                buffsize -= readLen;
                if (parser->isFinished())
                    break;
                if (buffsize == 0)
                {
                    close();
                    SYLAR_LOG_WARN(g_logger) << "Http head is too large";
                    return nullptr;
                }
            } while (true);
            int64_t length = parser->getContentLength();
            if (length > 0)
            {
                std::string body;
                body.resize(length);
                int len = 0;
                if (readChar > offset)
                {
                    memcpy(&body[0], data + offset, readChar - offset);
                    len = readChar - offset;
                    length -= (readChar - offset);
                }
                if (length > 0)
                {
                    if (readFixSize(&body[len], length) <= 0)
                    {
                        close();
                        SYLAR_LOG_ERROR(g_logger) << "cannot readFixsize : " << length;
                        return nullptr;
                    }
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
