/*
 * CMemLeakChecker.h
 * 2016.12.18
 */

#ifndef _CMemLeakChecker_H_
#define _CMemLeakChecker_H_

#include <string>
#include <map>
#include "CUtils.h"

typedef enum 
{
    MEM_NULL,
    MEM_ALLOC,
    MEM_FREE,
} MemStatus;

typedef struct PosInfo 
{
    unsigned int nLine;
    std::string strFile;
    MemAction eAction;
    PosInfo():nLine(0), strFile(""), eAction(ACTION_NULL){}
} PosInfo;

typedef struct MemChunk 
{
    void * pAddr;
    unsigned int nSize;
    MemStatus eStatus;
    PosInfo stPosInfoNew;
    PosInfo stPosInfoDelete;

    MemChunk():pAddr(NULL)
    , nSize(0)
    , eStatus(MEM_NULL)
    {}
} MemChunk;

class CMemLeakChecker: public Singleton<CMemLeakChecker>
{
public:
    CMemLeakChecker(){}
    ~CMemLeakChecker(){}

public:
    static void ExNewOp(void * pAlloc, size_t nSize, char* pszFileName, int nLineNum)
    {
        CMemLeakChecker::Instance().DoExNewOp(pAlloc, nSize, pszFileName, nLineNum, ACTION_NEW_SINGLE);
    }

    static void ExNewArrayOp(void * pAlloc, size_t nSize, char* pszFileName, int nLineNum)
    {
        CMemLeakChecker::Instance().DoExNewOp(pAlloc, nSize, pszFileName, nLineNum, ACTION_NEW_ARRAY);
    }

    static void ExDelOp(void * ptr, char* pszFileName, int nLineNum)
    {
        CMemLeakChecker::Instance().DoExDelOp(ptr, pszFileName, nLineNum, ACTION_DELETE_SINGLE);
    }

    static void ExDelArrayOp(void * ptr, char* pszFileName, int nLineNum)
    {
        CMemLeakChecker::Instance().DoExDelOp(ptr, pszFileName, nLineNum, ACTION_DELETE_ARRAY);
    }

public:
    void Show();

public:
    void DoExNewOp(void * pAlloc, size_t nSize, char* pszFileName, int nLineNum, MemAction eAction);
    void DoExDelOp(void * ptr, char* pszFileName, int nLineNum, MemAction eAction);

private:
    std::map<void*, MemChunk*> m_mapMems;
    CLock m_lock;
};

#endif
