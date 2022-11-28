#include "sylar/sylar.h"

sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

void run_in_fiber()
{
    SYLAR_LOG_INFO(g_logger) << "run_in_fiber begin";
    sylar::Fiber::YieldToReady();
    SYLAR_LOG_INFO(g_logger) << "run_in_fiber end";
    //sylar::Fiber::YieldToHold();
}

void test_fiber()
{
    SYLAR_LOG_INFO(g_logger) << "main begin";
    {
        sylar::Fiber::GetThis();
        sylar::Fiber::ptr fiber(new sylar::Fiber(run_in_fiber));
        fiber->swapIn();
        SYLAR_LOG_INFO(g_logger) << "main after swapin";
        fiber->swapIn();
        SYLAR_LOG_INFO(g_logger) << "main end";
    }
}

int main()
{
    std::vector<sylar::Thread::ptr> thrs;
    for(int i = 0;i < 1;++i)
    {
        thrs.push_back(sylar::Thread::ptr(new sylar::Thread(test_fiber,"name_" + std::to_string(i))));
    }
    for(auto &i : thrs)
        i->join();
    return 0;
}