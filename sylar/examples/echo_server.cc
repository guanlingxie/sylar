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
    //SYLAR_LOG_INFO(g_logger) << "handleClient" << client->toString();
    sylar::ByteArray::ptr ba(new sylar::ByteArray(1));
    while(true)
    {
        ba->clear();
        std::vector<iovec> iovs;
        ba->setWritePosition(0);
        ba->getWriteBuffers(iovs, 1024);        
        
        //std::cout << iovs.size() << std::endl;

        int rt = client->recv(&iovs[0], iovs.size());
        if(rt <= 0)
        {
            SYLAR_LOG_INFO(g_logger) << "client is close ";
            break;
        }
        ba->setWritePosition(ba->getWritePosition() + rt);
        std::cout << ba->toString();
        std::cout.flush();  
    }
}


void run()
{
    EchoServer::ptr es(new EchoServer(1));
    auto addr = sylar::Address::LookupAny("0.0.0.0:8020");
    es->bind(addr);
    es->start();
}

int main()
{
    sylar::IOManager iom(2);
    iom.schedule(run);
    return 0;
}