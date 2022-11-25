#include "sylar/sylar.h"

//sylar::RWMutex s_mutex;
sylar::Mutex mutex;
long long count = 0;
static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();
void fun1()
{
    SYLAR_LOG_INFO(g_logger) << "thread_name" << sylar::Thread::GetName()
        << " this.name" << sylar::Thread::GetThis()->getName()
        << " id: " << sylar::GetThreadId()
        << " this.id: " << sylar::Thread::GetThis()->getId();
    //sleep(20);
    for(int i = 0;i < 1000000;++i)
    {
        sylar::Mutex::Lock lock(mutex);
        ++count;
    }
}

void fun2()
{

}

int main(int argc,char **argv)
{
    SYLAR_LOG_INFO(g_logger) << "thread test begin";
    // std::vector<sylar::Thread::ptr> thrs;
    
    // for(int i = 0;i < 5;++i)
    // {
    //     sylar::Thread::ptr thr(new sylar::Thread(&fun1,"name_"+std::to_string(i)));
    //     thrs.push_back(thr);
    // }
    // for(int i = 0;i < 5;++i)
    // {
    //     thrs[i]->join();
    // }
    
    // SYLAR_LOG_INFO(g_logger) << "count = " << count;
    // SYLAR_LOG_INFO(g_logger) << "thread test end";

    YAML::Node root = YAML::LoadFile("/home/xitong/sylar/workspace/sylar/bin/conf/config_log.yml");
    sylar::Config::LoadFromYaml(root);

    sylar::Config::Visit([](sylar::ConfigVarBase::ptr var){
        SYLAR_LOG_INFO(g_logger) << var->toString();
    });
    return 0;
}