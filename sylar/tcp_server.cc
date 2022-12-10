#include "tcp_server.h"
#include "config.h"

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system"); 

static sylar::ConfigVar<uint64_t>::ptr g_tcp_server_readtimeout = 
    sylar::Config::Lookup("cpserver.readTimeout", (uint64_t)(60 * 1000 * 2), "tcp server readTimeout");

namespace sylar
{

TcpServer::TcpServer(sylar::IOManager *work, sylar::IOManager *acceptWork)
    :m_worker(work)
    ,m_acceptWorker(acceptWork)
    ,m_readTimeout(g_tcp_server_readtimeout->getValue())
    ,m_name("sylar/1.0.0")
    ,m_isStop(true)
{
    
}

TcpServer::~TcpServer()
{
    for(auto &sock : m_socks)
    {
        sock->close();
    }
    m_socks.clear();
}

bool TcpServer::bind(Address::ptr addr)
{
    Socket::ptr sock = Socket::CreateTCP(addr);
    if(!(sock->bind(addr) ))
    {
        SYLAR_LOG_ERROR(g_logger) << "bind fail errno = " << strerror(errno) << "address is " << addr->toString();
        return false; 
    }
    if(!sock->listen())
    {
        SYLAR_LOG_ERROR(g_logger) << "listen fail errno = " << strerror(errno) << "address is " << addr->toString();
        return false; 
    }
    SYLAR_LOG_DEBUG(g_logger) << "server bind success";
    sock->dump(std::cout);
    m_socks.push_back(sock);
    return true;
}

bool TcpServer::bind(const std::vector<Address::ptr> &addrs, std::vector<Address::ptr> &fail_addr)
{
    int rt = true;
    for(auto &addr : addrs)
    {
        if(!bind(addr))
        {
            fail_addr.push_back(addr);
            rt = false;
        }
    }
    return rt;
}

bool TcpServer::start()
{
    if(!m_isStop)
        return true;
    m_isStop = false;
    SYLAR_LOG_DEBUG(g_logger) << "m_socks size = " << m_socks.size();
    for(auto &sock : m_socks)
    {
        m_acceptWorker->schedule(std::bind(&TcpServer::startAccept, shared_from_this(), sock));
    }
    return true;
}

void TcpServer::stop()
{
    m_isStop = true;
    auto self = shared_from_this();
    m_acceptWorker->schedule([this,self]()
    {
        for(auto &sock : m_socks)
        {
            sock->cancelAll();
            sock->close();
        }
    });
}

void TcpServer::handleClient(Socket::ptr clent)
{
    SYLAR_LOG_INFO(g_logger) << "handleclient";
}
void TcpServer::startAccept(Socket::ptr sock)
{
    while(!m_isStop)
    {
        Socket::ptr client = sock->accept();
        client->setRecvTimeout(m_readTimeout);
        if(client)
        {
            m_worker->schedule(std::bind(&TcpServer::handleClient, shared_from_this(), client));
        }else{
            SYLAR_LOG_ERROR(g_logger) << "accept errno = " << strerror(errno);
        }
    }
}


}