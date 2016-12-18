/*
 * CUtils.cpp
 * 2016.12.9
 */

#ifndef _CUtils_H__
#define _CUtils_H__

#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>

#define gettid() syscall(SYS_gettid)

#ifdef DEBUG
    #define LOG_INFO(fmt, args...)
    #define LOG_WARN(fmt, args...)
    #define LOG_ERROR(fmt, args...)
#else
    #define LOG_INFO(fmt, args...)  printf("info :");printf(fmt, ##args);printf("\n")
    #define LOG_WARN(fmt, args...)  printf("warn :");printf(fmt, ##args);printf("\n")
    #define LOG_ERROR(fmt, args...) printf("error:");printf(fmt, ##args);printf("\n")
#endif

#define CAS(val, cur, next) __sync_val_compare_and_swap(&val, cur, next)
#define ADD(val, count) __sync_fetch_and_add(&val, count)

typedef enum
{
    ACTION_NULL,
    ACTION_NEW_SINGLE,
    ACTION_NEW_ARRAY,
    ACTION_DELETE_SINGLE,
    ACTION_DELETE_ARRAY,
} MemAction;

class CLock
{
public:
    CLock():m_count(0), m_pid(0){pthread_mutex_init(&m_lock, NULL);}
    ~CLock(){pthread_mutex_destroy(&m_lock);}

public:
    void Lock()
    {
        if (CAS(m_pid, 0, gettid()) == 0)
        {
            pthread_mutex_lock(&m_lock);
            ADD(m_count,1);
        }
        else if (m_pid == gettid())
        {
            ADD(m_count,1);
        }
        else
        {
            while (CAS(m_pid, 0, gettid()) != 0); usleep(10);
            pthread_mutex_lock(&m_lock);
            ADD(m_count,1);
        }
    }

    void Unlock()
    {
        if (m_pid == gettid())
        {
            if (CAS(m_count, 1, 0) == 1)
            {
                pthread_mutex_unlock(&m_lock);
                m_pid = 0;
            }
            else
            {
                ADD(m_count, -1);
            }
        }
    }

private:
    pthread_mutex_t m_lock;
    volatile pid_t m_pid;
    volatile signed int m_count;
};

template<typename T>
class Singleton
{
public:
    static T& Instance()
    {
        static T obj;
        return obj;
    }

private:
    Singleton(const Singleton &other){}
    Singleton &operator=(const Singleton &other){}

protected:
    Singleton(){}
    ~Singleton(){}
};

#endif
