#include "sylar/iomanager.h"
#include "sylar/sylar.h"
#include "sylar/log.h"
#include "sylar/tcp_server.h"

sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

void run()
{
    auto addr = sylar::Address::LookupAny("0.0.0.0:8033");
    //auto addr1 = sylar::UnixAddress::ptr(new sylar::UnixAddress("/tmp/unix_addr"));
    std::vector<sylar::Address::ptr> vecAddr;
    vecAddr.push_back(addr);
    //vecAddr.push_back(addr1);
    sylar::TcpServer::ptr tcpserver(new sylar::TcpServer);
    std::vector<sylar::Address::ptr> fail;
    tcpserver->bind(addr);
    tcpserver->start();
    //SYLAR_LOG_INFO(g_logger) << addr->toString();
}

int main()
{
    //SYLAR_LOG_DEBUG(g_logger) << "xxxxxxxxxxxxx";
    sylar::IOManager *iom = new sylar::IOManager(2, true, "xitong");
    iom->schedule(run);
    delete iom;

    return 0;
}