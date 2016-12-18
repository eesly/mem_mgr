/* 
 * CMemOperator.cpp
 * 2016.11.28
 * sly
 */

#include "CMemOperator.h"
#include "CMemMgr.h"
#include <stdlib.h>

CLock GLOCK;
bool bRecord = false;
std::stack<std::pair<unsigned int, std::string> > DelPosInfoStack;

void* operator new(size_t nSize, char* pszFileName, int nLineNum)
{
    void *pResult = ::operator new(nSize);

    CMemMgr::Instance().NewExOp(pResult, nSize, pszFileName, nLineNum);

    return pResult;
}

void* operator new[](size_t nSize, char* pszFileName, int nLineNum)
{
    void *pResult = ::operator new[] (nSize);

    CMemMgr::Instance().NewArrayOp(pResult, nSize, pszFileName, nLineNum);

    return pResult;
}

void PreDelRecord(unsigned int nLine, const char* pFile)
{
    GLOCK.Lock();
    DelPosInfoStack.push(std::make_pair(nLine, std::string(pFile))); 
    CAS(bRecord, false, true);
}

void PostDelRecord(void* ptr, MemAction eAction)
{
    if (CAS(bRecord, true, false) == true)
    {
        if (!DelPosInfoStack.empty())
        {
            std::pair<unsigned int, std::string> sPos = DelPosInfoStack.top();
            DelPosInfoStack.pop();

            if (eAction == ACTION_DELETE_SINGLE)
                CMemMgr::Instance().DeleteExOp(ptr, (char *)sPos.second.c_str(), sPos.first);

            if (eAction == ACTION_DELETE_ARRAY)
                CMemMgr::Instance().DeleteArrayExOp(ptr, (char *)sPos.second.c_str(), sPos.first);
        }

        if (!DelPosInfoStack.empty())
            CAS(bRecord, false, true);

        GLOCK.Unlock();
    }
}

void operator delete(void *ptr)
{
    PostDelRecord(ptr, ACTION_DELETE_SINGLE);

    free(ptr);
}

void operator delete[](void *ptr)
{
    PostDelRecord(ptr, ACTION_DELETE_ARRAY);

    free(ptr);
}

void operator delete(void *ptr, char* pszFileName, int nLineNum )
{
    free(ptr);
}

void operator delete[](void *ptr, char* pszFileName, int nLineNum )
{
    free(ptr);
}
