#include "sylar/sylar.h"
#include "sylar/bytearray.h"

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

void test_typearray()
{
    sylar::ByteArray ba(1);
    ba.writeInt32(INT_MIN);
    std::cout << INT_MIN << std::endl;
    SYLAR_LOG_INFO(g_logger) << ba.getPosition();
    SYLAR_LOG_INFO(g_logger) << ba.getBaseSize();
    ba.setPosition(0);
    int32_t c = ba.readInt32();
    SYLAR_LOG_INFO(g_logger) << c;
}
int main()
{
    test_typearray();
}