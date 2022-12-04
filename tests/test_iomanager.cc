#include "sylar/sylar.h"
#include "sylar/iomanager.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <fcntl.h>

sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

void test_fiber()
{
    SYLAR_LOG_INFO(g_logger) << "test fiber";
}

void test1()
{
    sylar::IOManager iom(1);
    iom.schedule(&test_fiber);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(sock, F_SETFL, O_NONBLOCK);

    sockaddr_in addr;
    memset(&addr, 0, sizeof(sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    inet_pton(AF_INET, "180.101.49.13", &addr.sin_addr.s_addr);
    
    iom.addEvent(sock, sylar::IOManager::READ, [](){
        SYLAR_LOG_INFO(g_logger) <<  "connected read";
        sleep(1);
    });
    iom.addEvent(sock, sylar::IOManager::WRITE, [&](){
        SYLAR_LOG_INFO(g_logger) <<  "connected write";
        sylar::IOManager::GetThis()->cancelEvent(sock ,sylar::IOManager::READ);
        sleep(1);
        close(sock);
    });

    int rt = connect(sock, (sockaddr *)&addr, sizeof(addr));
}

void test_timer()
{
    sylar::IOManager iom(2);
    sylar::Timer::ptr timer = iom.addTimer(100,[&timer](){
        SYLAR_LOG_INFO(g_logger) << "hello timer";
        // static int i = 0;
        // if(++i == 5)
        // {
        //     timer->cancel();
        // }
    },true);
}
int main()
{
    test1();
    return 0;
}