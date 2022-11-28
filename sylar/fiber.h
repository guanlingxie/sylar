#ifndef __SYLAR_FIBER_H__
#define __SYLAR_FIBER_H__

#include <memory>
#include <functional>
#include <ucontext.h>

namespace sylar
{
class Scheduler;
class Fiber : public std::enable_shared_from_this<Fiber>
{
public:
    friend class Scheduler;
    typedef std::shared_ptr<Fiber> ptr;
    enum State
    {
        INIT,   // init but never call this fiber
        HOLD,   // wait for something happen will re-call fiber
        EXEC,   // executing
        TERM,   // this fiber terminal, executed the this fiber
        READY,  // can execute, wait for call
        EXCEPT  // exception
    };

private:
    Fiber();
public:
    Fiber(std::function<void()> cb,size_t stacksize = 0, bool use_caller = false);
    ~Fiber();
    // reset fiber function
    void reset(std::function<void()> cb);
    // swap fiber to exec
    void swapIn();
    // 
    void call();
    void back();

    void swapOut();
    State getState() const{
        return m_state;
    }
    uint64_t getId() const{return m_id;}
public:
    static void SetThis(Fiber *f);
    // return current fiber
    static Fiber::ptr GetThis();
    // swap fiber to ready
    static void YieldToReady();
    // swap fiber to hold
    static void YieldToHold();

    static uint64_t TotalFibers();
    static uint64_t GerFiberId();
    static void MainFunc();
    static void CallMainFunc();
private:
    uint64_t m_id = 0;
    uint32_t m_stacksize = 0;
    State m_state = INIT;

    ucontext_t m_ctx;
    void *m_stack = nullptr;
    std::function<void()> m_cb;
};

}


#endif