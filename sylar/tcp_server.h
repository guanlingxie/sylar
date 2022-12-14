#ifndef __SYLAR_TCP_SERVER_H__
#define __SYLAR_TCP_SERVER_H__

#include <memory>
#include <functional>
#include <vector>
#include "address.h"
#include "iomanager.h"
#include "socket.h"
#include "noncopyable.h"

namespace sylar
{

class TcpServer : public std::enable_shared_from_this<TcpServer>, Noncopyable
{
public:
    typedef std::shared_ptr<TcpServer> ptr;
    TcpServer(sylar::IOManager *work = sylar::IOManager::GetThis(), sylar::IOManager *acceptWork = sylar::IOManager::GetThis());
    virtual ~TcpServer();

    virtual bool bind(Address::ptr addr);
    virtual bool bind(const std::vector<Address::ptr> &addrs, std::vector<Address::ptr> &fail_addr);
    virtual bool start();
    virtual void stop();

    uint64_t getReadTimeout() const { return m_readTimeout;}
    std::string getName() const {return m_name;}
    void setReadTimeOut(uint64_t v){m_readTimeout = v;}
    virtual void setName(const std::string &v){m_name = v;}
    bool isStop() const {return m_isStop;}
protected:
    virtual void handleClient(Socket::ptr client);
    virtual void startAccept(Socket::ptr sock);
private:
    std::vector<Socket::ptr> m_socks;
    IOManager *m_worker;
    IOManager *m_acceptWorker;
    uint64_t m_readTimeout;
    std::string m_name;
    bool m_isStop;
    
};

}

#endif
