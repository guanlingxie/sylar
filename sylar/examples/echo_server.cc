#include "sylar/sylar.h"
#include "sylar/tcp_server.h"

#include "sylar/bytearray.h"

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

class EchoServer : public sylar::TcpServer
{
public:
    EchoServer(int type);
    virtual void handleClient(sylar::Socket::ptr client);
private:
    int m_type = 0;
};
EchoServer::EchoServer(int type)
    :m_type(type)
{
    
}
void EchoServer::handleClient(sylar::Socket::ptr client)
{
    SYLAR_LOG_INFO(g_logger) << "handleClient" << client->toString();
    sylar::ByteArray::ptr ba(new sylar::ByteArray);
    while(true)
    {
        ba->clear();
        
    }
}

int main()
{
    return 0;
}