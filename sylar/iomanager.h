#ifndef __SYLAR_IOMANAGER_H__
#define __SYLAR_IOMANAGER_H__

#include "scheduler.h"
#include "timer.h"

namespace sylar
{

class IOManager : public Scheduler, public TimerManager
{
public:
    typedef std::shared_ptr<IOManager> ptr;
    typedef RWMutex RWMutexType;

    enum Event
    {
        NONE = 0x0,
        READ = 0X1,
        WRITE = 0X4
    };
private:
    struct FdContext
    {
        typedef Mutex MutexType;
        struct EventContext{
            Scheduler *scheduler = nullptr;       // 
            Fiber::ptr fiber;           // io
            std::function<void()> cb;   // 
        };
        int fd = 0;
        EventContext &getContext(Event event);
        void resetContext(EventContext &ctx);
        void triggerEvent(Event event);
        EventContext read;
        EventContext write;
        Event events = NONE;
        MutexType mutex;
    };
public:
    IOManager(size_t threads = 1, bool use_caller = true, const std::string& name = "");
    ~IOManager();

    // 0 success  -1 error
    int addEvent(int fd, Event event, std::function<void()> cb = nullptr);
    bool delEvent(int fd, Event event);
    bool cancelEvent(int fd, Event event);

    bool cancelAll(int fd);
    static IOManager *GetThis();
protected:
    void tickle() override;
    bool stopping() override;
    void idle() override;
    void onTimerInsertAtFront() override;

    void contextResize(size_t size);
private:
    int m_epfd = 0;
    int m_tickleFds[2];

    std::atomic<size_t> m_pendingEventCount = {0};
    RWMutexType m_mutex;
    std::vector<FdContext *> m_fdContexts;

};

}


#endif