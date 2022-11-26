#ifndef __SYLAR_THREAD_H__
#define __SYLAR_THREAD_H__

#include <thread>
#include <memory>
#include <functional>
#include <pthread.h>
#include <semaphore.h>


namespace sylar
{

class Semaphore
{
public:
    Semaphore(uint32_t count = 0);
    ~Semaphore();
    void wait();
    void notify();
private:
    Semaphore(const Semaphore &) = delete;
    Semaphore(const Semaphore &&) = delete;
    Semaphore operator=(const Semaphore &) = delete;
private:
    sem_t m_semaphore;
};

template<class T>
struct ScopedLockImpl
{
public:
    ScopedLockImpl(T &mutex)
        :m_mutex(mutex)
    {
        lock();
        m_locked = true;
    }
    ~ScopedLockImpl()
    {
        unlock();
    }

    void lock()
    {
        if(!m_locked)
        {
            m_mutex.lock();
            m_locked = true;
        }
    }
    void unlock()
    {
        if(m_locked)
        {
            m_mutex.unlock();
            m_locked = false;
        }
    }
private:
    T &m_mutex;
    bool m_locked = false;
};

class Mutex
{
public:
    typedef ScopedLockImpl<Mutex> Lock;
    Mutex()
    {
        pthread_mutex_init(&m_mutex,nullptr);
    }
    ~Mutex()
    {
        pthread_mutex_destroy(&m_mutex);
    }
    void lock()
    {
        pthread_mutex_lock(&m_mutex);
    }
    void unlock()
    {
        pthread_mutex_unlock(&m_mutex);
    }
private:
    pthread_mutex_t m_mutex;
};

template<class T>
struct ReadScopedLockImpl
{
public:
    ReadScopedLockImpl(T &mutex)
        :m_mutex(mutex)
    {
        lock();
    }
    ~ReadScopedLockImpl()
    {
        unlock();
    }

    void lock()
    {
        if(!m_locked)
        {
            m_mutex.rdlock();
            m_locked = true;
        }
    }
    void unlock()
    {
        if(m_locked)
        {
            m_mutex.unlock();
            m_locked = false;
        }
    }
private:
    T &m_mutex;
    bool m_locked = false;
};

template<class T>
struct WriteScopedLockImpl
{
public:
    WriteScopedLockImpl(T &mutex)
        :m_mutex(mutex)
    {
        lock();
    }
    ~WriteScopedLockImpl()
    {
        unlock();
    }

    void lock()
    {
        if(!m_locked)
        {
            m_mutex.wrlock();
            m_locked = true;
        }
    }
    void unlock()
    {
        if(m_locked)
        {
            m_mutex.unlock();
            m_locked = false;
        }
    }
private:
    T &m_mutex;
    bool m_locked = false;
};

class RWMutex
{
public:
    typedef ReadScopedLockImpl<RWMutex> ReadLock;
    typedef WriteScopedLockImpl<RWMutex> WriteLock;
    RWMutex()
    {
        pthread_rwlock_init(&m_lock,nullptr);
    }
    ~RWMutex()
    {
        pthread_rwlock_destroy(&m_lock);
    }
    void rdlock()
    {
        pthread_rwlock_rdlock(&m_lock);
    }
    void wrlock()
    {
        pthread_rwlock_wrlock(&m_lock);
    }
    void unlock()
    {
        pthread_rwlock_unlock(&m_lock);
    }
private:
    pthread_rwlock_t m_lock;
};

class Spinlock
{
public:
    typedef ScopedLockImpl<Spinlock> Lock;
    Spinlock()
    {
        pthread_spin_init(&m_mutex,PTHREAD_PROCESS_PRIVATE);
    }
    ~Spinlock()
    {
        pthread_spin_destroy(&m_mutex);
    }
    void lock()
    {
        pthread_spin_lock(&m_mutex);
    }
    void unlock()
    {
        pthread_spin_unlock(&m_mutex);
    }
private:
    pthread_spinlock_t m_mutex;
};


class Thread
{
public:
    typedef std::shared_ptr<Thread> ptr;
    Thread(std::function<void()> cb, const std::string &name);
    ~Thread();
    const std::string &getName() const{return m_name;}
    pid_t getId()const {return m_id;}
    void join();
    void setCb(std::function<void()> cb){
        m_cb = cb;
    }
    static Thread *GetThis();
    static const std::string &GetName();
    static void SetName(const std::string &name);
private:
    Thread(const Thread &) = delete;
    Thread(const Thread &&) = delete;
    Thread &operator=(const Thread&) = delete;
    static void *run(void *arg);
private:
    pid_t m_id = -1;
    pthread_t m_thread = 0;
    std::function<void()> m_cb;
    std::string m_name;

    Semaphore m_semaphore;
};

}


#endif