#include "sylar/sylar.h"


sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();


void test_fiber()
{
    SYLAR_LOG_INFO(g_logger) << "****";
    static int count = 5;
    sleep(1);
    while(count-- > 0)
        sylar::Scheduler::GetThis()->schedule(&test_fiber);
}

int main()
{
    sylar::Scheduler sc(1);
    SYLAR_LOG_INFO(g_logger) << "over1";
    
    sc.start();
    sc.schedule(&test_fiber);
    //sc.schedule(&test_fiber);
    sc.stop();

    SYLAR_LOG_INFO(g_logger) << "over";
    return 0;
}