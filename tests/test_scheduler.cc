#include "sylar/sylar.h"


sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();


void test_fiber()
{
    SYLAR_LOG_ERROR(g_logger) << "****";
}
int main()
{
    sylar::Scheduler sc;

    
    sc.start();
    sc.schedule(&test_fiber);
    
    sc.stop();
    SYLAR_LOG_INFO(g_logger) << "over";
    return 0;
}