#include "util.h"
#include <stdint.h>

namespace sylar{


pid_t GetThreadId()
{
    return syscall(SYS_gettid);
}
uint32_t GetFiberId()
{
    return 0;
}
}