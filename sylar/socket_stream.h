#ifndef __SYLAR_SOCKET_STREAM_H__
#define __SYLAR_SOCKET_STREAM_H__

#include <memory>
#include "stream.h"
#include "socket.h"
#include "address.h"

namespace sylar
{
class SocketStream : public Stream
{
public:
    typedef std::shared_ptr<SocketStream> ptr;
    SocketStream(Socket::ptr, bool owner = true);
    ~SocketStream();

    virtual int read(void *buffer, size_t length) override;
    virtual int read(ByteArray::ptr ba, size_t length) override;
    virtual int write(const void *buffer, size_t length) override;
    virtual int write(ByteArray::ptr ba, size_t length) override;
    virtual void close() override;

    bool isConnected() const { return m_socket != nullptr && m_socket->isConnected();}
    Socket::ptr getSocket() const {return m_socket;}

    Address::ptr getRemoteAddress();
    Address::ptr getLocalAddress();
    std::string getRemoteAddressString();
    std::string getLocalAddressString();
protected:
    Socket::ptr m_socket;
    bool m_owner;
};
}

#endif