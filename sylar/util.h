#ifndef __UTIL_H__
#define __UTIL_H__

#include <pthread.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include <string>


namespace sylar{

pid_t GetThreadId();
u_int32_t GetFiberId();
void Backtrace(std::vector<std::string> &bt,int size = 64, int skip = 1);
std::string BacktraceToString(int size = 64, int skip = 2,const std::string &prefix = "");
}

#endif