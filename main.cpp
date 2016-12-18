/*
 *
 *
 *
 */

#include "CMemMgr.h"
#include "CUtils.h"
#include "CMemOperator.h"
#include "CMemLeakChecker.h"
#include "CMemStatistics.h"
#include <string.h>
#include <pthread.h>

class A
{
public:
    A(){pmem = (char *) New char[20];
    strncpy(pmem, "m12345678", 10);}
    ~A(){if (pmem) {Delete[] pmem;}}
private:
    char* pmem;
};

class B
{
public:
    B(){ p = (A*)New A();}
    ~B(){ if(p) {Delete p;} }
private:
    A * p;
};

void* thread_function(void * pParam)
{
    LOG_INFO("\n%s Start!\n", (char*)(pParam));

    for(int n = 0; n < 100; n++)
    {
        char* pchar = (char*)(New char[10]);
        strncpy(pchar, "g12345678", 10);
        int* pint = (int *)(New int);
        *pint = 99;
        B* pObj = (B *) New B;
        char * p = (char*) Malloc(10);

        Free(p);
//        Delete pint;
        Delete[] pchar;
        Delete pObj;
        usleep(1);
        //CMemLeakChecker::Instance().Show();
    }

    LOG_INFO("\n%s Quit!\n", (char*)(pParam));
}

int main(int argc, char ** argv)
{
    CMemMgr::Instance().Init();

    pthread_t t1, t2;
    pthread_create(&t1, NULL, &thread_function, (void *)"thread 1");
    pthread_create(&t2, NULL, &thread_function, (void *)"thread 2");

    void * pRet = NULL;
    pthread_join(t1, &pRet);
    pthread_join(t2, &pRet);

    CMemLeakChecker::Instance().Show();
    CMemStatistics::Instance().Show();
    return 0;
}
