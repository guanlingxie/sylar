#include "fiber.h"
#include "config.h"
#include "log.h"
#include "macro.h"
#include "scheduler.h"
#include <atomic>


static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");


namespace sylar
{
static std::atomic<u_int64_t> s_fiber_id{0};
static std::atomic<u_int64_t> s_fiber_count{0};
static thread_local Fiber *t_fiber = nullptr; // pointer to exec-ing fiber
static thread_local Fiber::ptr t_threadFiber = nullptr; // pointer to main fiber

static ConfigVar<uint32_t>::ptr g_fiber_stack_size = 
    Config::Lookup<uint32_t>("fiber.stack_size", 1024*1024, "fiber stack size");

class MallocStackAllocator
{
public:
    static void *Alloc(size_t size)
    {
        return malloc(size);
    }
    static void Dealloc(void *vp, size_t size)
    {
        free(vp);
    }
};

using StackAllocator = MallocStackAllocator;

/* static function */
uint64_t Fiber::GerFiberId()
{
    if(t_fiber)
    {
        return t_fiber->getId();
    }
    return 0;
}

Fiber::Fiber()
{
    m_state = EXEC;
    SetThis(this);

    if(getcontext(&m_ctx))
    {
        SYLAR_ASSERT2(false,"getcontext");
    }
    ++s_fiber_count;
    //SYLAR_LOG_INFO(g_logger) << "construct m_id = " << m_id;
}


Fiber::Fiber(std::function<void()> cb,size_t stacksize, bool use_caller):m_id(++s_fiber_id),m_cb(cb)
{
    ++s_fiber_count;
    m_stacksize = stacksize ? stacksize : g_fiber_stack_size->getValue();
    m_stack = StackAllocator::Alloc(m_stacksize);
    if(getcontext(&m_ctx))
    {
        SYLAR_ASSERT2(false,"getcontext");
    }
    m_ctx.uc_link = nullptr;
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stacksize;
    if(!use_caller)
        makecontext(&m_ctx,Fiber::MainFunc,0);
    else    
        makecontext(&m_ctx,Fiber::CallMainFunc,0);
    //SYLAR_LOG_INFO(g_logger) << "construct m_id = " << m_id;
}

Fiber::~Fiber()
{
    --s_fiber_count;
    if(m_stack)
    {
        SYLAR_ASSERT(m_state == TERM || m_state == INIT || m_state == EXCEPT)
        StackAllocator::Dealloc(m_stack,m_stacksize);
    }else{
        SYLAR_ASSERT(!m_cb);
        SYLAR_ASSERT(m_state == EXEC);

        //if the fiber is main fiber, need destory the fiber's
        Fiber *cur = t_fiber;
        if(cur == this)
            SetThis(nullptr);
    }
    //SYLAR_LOG_INFO(g_logger) << "deconstruct m_id = " << m_id;
}
// reset fiber function
void Fiber::reset(std::function<void()> cb)
{
    SYLAR_ASSERT(m_stack);
    SYLAR_ASSERT(m_state == TERM || m_state == INIT || m_state == EXCEPT);
    m_cb = cb;
    if(getcontext(&m_ctx))
    {
        SYLAR_ASSERT2(false,"getcontext");
    }
    m_ctx.uc_link = nullptr;
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stacksize;
    makecontext(&m_ctx,Fiber::MainFunc,0);
    m_state = INIT;
}

// swap fiber to exec
void Fiber::swapIn()
{
    SetThis(this);
    SYLAR_ASSERT(m_state != EXEC);
    m_state = EXEC;
    if(swapcontext(&Scheduler::GetMainFiber()->m_ctx,&m_ctx))
    {
        SYLAR_ASSERT2(false,"swapcontext");
    }
}

void Fiber::call()
{
    SetThis(this);
    m_state = EXEC;
    if(swapcontext(&t_threadFiber->m_ctx,&m_ctx))
    {
        SYLAR_ASSERT2(false,"swapcontext");
    }
}

void Fiber::back()
{
    
    SetThis(t_threadFiber.get());
    //t_threadFiber represent main fiber not scheduler fiber
    if(swapcontext(&m_ctx,&t_threadFiber->m_ctx))
    {
        SYLAR_ASSERT2(false,"swapcontext");
    }
}
// 
void Fiber::swapOut()
{
    //SYLAR_LOG_INFO(g_logger) << "fiber id is" << m_id;
    SetThis(Scheduler::GetMainFiber());
    if(swapcontext(&m_ctx,&Scheduler::GetMainFiber()->m_ctx))
    {
        SYLAR_ASSERT2(false,"swapcontext");
    }
}

void Fiber::SetThis(Fiber *f)
{
    t_fiber = f;
}

// return current fiber
Fiber::ptr Fiber::GetThis()
{
    if(t_fiber)
        return t_fiber->shared_from_this();
    Fiber::ptr main_fiber(new Fiber);
    SYLAR_ASSERT(t_fiber == main_fiber.get());
    t_threadFiber = main_fiber;
    return t_fiber->shared_from_this();
}
// swap fiber to ready
/* static function */
void Fiber::YieldToReady()
{
    Fiber::ptr cur = GetThis();
    cur->m_state = READY;
    cur->swapOut();
}
// swap fiber to hold
/* static function */
void Fiber::YieldToHold()
{
    Fiber::ptr cur = GetThis();
    cur->m_state = HOLD;
    cur->swapOut();
}

/* static function */
uint64_t Fiber::TotalFibers()
{
    return s_fiber_count;
}
/* static function */
void Fiber::MainFunc()
{
    Fiber::ptr cur = GetThis();
    try
    {
        cur->m_cb();
        cur->m_cb = nullptr;
        cur->m_state = TERM;
    }catch(std::exception &ex)
    {
        cur->m_state = EXCEPT;
        SYLAR_LOG_ERROR(g_logger) << "Fiber Except: " << ex.what()
            << " fiber id = " << cur->getId()
            << std::endl
            << sylar::BacktraceToString();
    }catch(...)
    {
        cur->m_state = EXCEPT;
        SYLAR_LOG_ERROR(g_logger) << "Fiber Except: "
            << std::endl
            << sylar::BacktraceToString();
    }
    Fiber *row_ptr = cur.get();
    cur.reset();
    row_ptr->swapOut();
    SYLAR_ASSERT2(false,"never reach fiber_id=" + std::to_string(row_ptr->getId()));
}


void Fiber::CallMainFunc()
{
    Fiber::ptr cur = GetThis();
    try
    {
        cur->m_cb();
        cur->m_cb = nullptr;
        cur->m_state = TERM;
    }catch(std::exception &ex)
    {
        cur->m_state = EXCEPT;
        SYLAR_LOG_ERROR(g_logger) << "Fiber Except: " << ex.what()
            << " fiber id = " << cur->getId()
            << std::endl
            << sylar::BacktraceToString();
    }catch(...)
    {
        cur->m_state = EXCEPT;
        SYLAR_LOG_ERROR(g_logger) << "Fiber Except: "
            << std::endl
            << sylar::BacktraceToString();
    }
    Fiber *row_ptr = cur.get();
    cur.reset();
    row_ptr->back();
    SYLAR_ASSERT2(false,"never reach fiber_id=" + std::to_string(row_ptr->getId()));
}

}