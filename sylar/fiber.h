#ifndef __SYLAR_FIBER_H__
#define __SYLAR_FIBER_H__

#include <memory>
#include <functional>
#include <ucontext.h>

namespace sylar
{
class Fiber : public std::enable_shared_from_this<Fiber>
{
public:
    typedef std::shared_ptr<Fiber> ptr;
    enum State
    {
        INIT,
        HOLD,
        EXEC,
        TERM,
        READY,
        EXCEPT
    };

private:
    Fiber();
public:
    Fiber(std::function<void()> cb,size_t stacksize = 0);
    ~Fiber();
    // reset fiber function
    void reset(std::function<void()> cb);
    // swap fiber to exec
    void swapIn();
    // 
    void swapOut();
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