#include "sylar/sylar.h"

#include <assert.h>

sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

void test_assert()
{
    //SYLAR_LOG_INFO(g_logger) << "xxx";
    //SYLAR_LOG_INFO(g_logger) << sylar::BacktraceToString(10,1);
    //SYLAR_ASSERT2(false,"abddd")
    //SYLAR_ASSERT(false);
}
int main(int argc,char **argv)
{
    test_assert();
    return 0;
}