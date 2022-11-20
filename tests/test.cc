#include "../sylar/log.h"
#include "../sylar/util.h"
#include <iostream>


int main(int argc,char **argv)
{

    
    sylar::Logger::ptr logger(new sylar::Logger);
    //"%d [%p] %f %l %m %n"
    logger->addAppender(sylar::LogAppender::ptr(new sylar::StdoutLogAppender()));
    //sylar::LogEvent::ptr event(new sylar::LogEvent(logger,sylar::LogLevel::DEBUG,__FILE__,__LINE__,0,sylar::GetThreadId(),sylar::GetFiberId(),time(0)));
    SYLAR_LOG_DEBUG(logger) << "sdsfds";
    SYLAR_LOG_FMT_DEBUG(logger,"%d",100);
    std::cout << "hello sylar log" << std::endl;

    auto l = sylar::LoggerMgr::GetInstance()->getLogger("XXX");
    SYLAR_LOG_INFO(l) << "XXXX";
    return 0;
}