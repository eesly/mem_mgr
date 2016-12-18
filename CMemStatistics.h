/*
 * CMemStatistics.h
 * 2016.12.18
 */

#ifndef _CMemStatistics_H_
#define _CMemStatistics_H_

#include <string>
#include <map>
#include "CUtils.h"
#include <sstream>

#define POS(strFile,nLine) {std::string strLine; std::stringstream ss; \
                            ss << nLine; ss >> strLine;\
                            strFile = strFile + ":" + strLine;}

typedef struct ActionInfo
{
    std::string strPos;
    long total_new;
    long total_del;
    std::map<void*, long> m_mapSize;
    ActionInfo():total_new(0), total_del(0){}
} ActionInfo;

class CMemStatistics: public Singleton<CMemStatistics>
{
public:
    CMemStatistics();
    ~CMemStatistics(){}

public:
    static void ExNewOp(void * pAlloc, size_t nSize, char* pszFileName, int nLineNum)
    {
        CMemStatistics::Instance().DoExNewOp(pAlloc, nSize, pszFileName, nLineNum, ACTION_NEW_SINGLE);
    }

    static void ExNewArrayOp(void * pAlloc, size_t nSize, char* pszFileName, int nLineNum)
    {
        CMemStatistics::Instance().DoExNewOp(pAlloc, nSize, pszFileName, nLineNum, ACTION_NEW_ARRAY);
    }

    static void ExDelOp(void * ptr, char* pszFileName, int nLineNum)
    {
        CMemStatistics::Instance().DoExDelOp(ptr, pszFileName, nLineNum, ACTION_DELETE_SINGLE);
    }

    static void ExDelArrayOp(void * ptr, char* pszFileName, int nLineNum)
    {
        CMemStatistics::Instance().DoExDelOp(ptr, pszFileName, nLineNum, ACTION_DELETE_ARRAY);
    }

public:
    void Show();

public:
    void DoExNewOp(void * pAlloc, size_t nSize, char* pszFileName, int nLineNum, MemAction eAction);
    void DoExDelOp(void * ptr, char* pszFileName, int nLineNum, MemAction eAction);

private:
    CLock m_lock;
    std::map<std::string, ActionInfo*> m_mapMems;
    long m_total_new;
    long m_total_del;
};

#endif
