#include "sylar/sylar.h"
#include "sylar/socket.h"

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();


void test_socket()
{
    sylar::IPAddress::ptr addr = sylar::IPAddress::LookupAnyIPAddress("www.baidu.com");
    addr->setPort(80);
    if(addr)
        SYLAR_LOG_INFO(g_logger) << "get address :" << addr->toString();
    
    
    sylar::Socket::ptr sock = sylar::Socket::CreateTCP(addr);
    if(!sock->connect(addr))
    {
        SYLAR_LOG_ERROR(g_logger) << "connect failure";
    }else{
        SYLAR_LOG_INFO(g_logger) << "connect successful";
    }
    const char buf[] = "GET / HTTP/1.0\r\n\r\n";
    int rt = sock->send(buf, sizeof(buf));
    if(rt <= 0)
    {
        SYLAR_LOG_INFO(g_logger) << "send failure rt = " << rt;
        return;
    }
    std::string buffs;
    buffs.resize(4096);
    rt = sock->recv(&buffs[0], buffs.size());
    //std::cout << "rt ="<< rt << std::endl;
    if(rt <= 0)
    {
        SYLAR_LOG_INFO(g_logger) << "recv failure rt = " << rt;
        return;
    }
    buffs.resize(rt);
    SYLAR_LOG_INFO(g_logger) << buffs;

}

int main()
{
    sylar::IOManager iom;
    iom.schedule(&test_socket);
    return 0;
}