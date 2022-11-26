#include "sylar/sylar.h"

//sylar::RWMutex s_mutex;
sylar::Mutex mutex;
long long count = 0;
static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();


void fun2()
{

}
void fun1()
{
    std::shared_ptr<int> ptr(new int(10));
    SYLAR_LOG_INFO(g_logger) << "thread_name " << sylar::Thread::GetName()
        << " this.name " << sylar::Thread::GetThis()->getName()
        << " id: " << sylar::GetThreadId()
        << " this.id: " << sylar::Thread::GetThis()->getId();
    while(1)
    {
        sleep(1); 
        std::cout << *ptr << " ptr->use " << ptr.use_count() << std::endl;       
    }

}
int main(int argc,char **argv)
{
    {
        sylar::Thread::ptr t(new sylar::Thread(fun1,"sixxxx"));
        sleep(1);
        t->setCb(fun2);
    }
    //t->join();
    sleep(50);
    return 0;
}