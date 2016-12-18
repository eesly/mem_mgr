/* 
 * CMemMgr.cpp
 * 2016.12.17
 * sly
 */

#include "CMemMgr.h"
#include "CMemLeakChecker.h"
#include "CMemStatistics.h"

void CMemMgr::Init()
{
    CMemLeakChecker::Instance();
    RegNewExOp(CMemLeakChecker::ExNewOp);
    RegNewArrayExOp(CMemLeakChecker::ExNewArrayOp);
    RegDeleteExOp(CMemLeakChecker::ExDelOp);
    RegDeleteArrayExOp(CMemLeakChecker::ExDelArrayOp);

    CMemStatistics::Instance();
    RegNewExOp(CMemStatistics::ExNewOp);
    RegNewArrayExOp(CMemStatistics::ExNewArrayOp);
    RegDeleteExOp(CMemStatistics::ExDelOp);
    RegDeleteArrayExOp(CMemStatistics::ExDelArrayOp);
}

void CMemMgr::NewExOp(void * pAlloc, size_t nSize, char* pszFileName, int nLineNum)
{
    std::set<ExNewOp>::iterator it = m_setNewExOp.begin();
    for (; it != m_setNewExOp.end(); ++it)
    {
        ExNewOp handler = *it;
        if (handler != NULL) handler(pAlloc, nSize, pszFileName, nLineNum);
    }
}

void CMemMgr::NewArrayOp(void * pAlloc, size_t nSize, char* pszFileName, int nLineNum)
{
    std::set<ExNewOp>::iterator it = m_setNewArrayExOp.begin();
    for (; it != m_setNewArrayExOp.end(); ++it)
    {
        ExNewOp handler = *it;
        if (handler != NULL) handler(pAlloc, nSize, pszFileName, nLineNum);
    }
}

void CMemMgr::DeleteExOp(void *ptr, char* pszFileName, int nLineNum)
{
    std::set<ExDelOp>::iterator it = m_setDeleteExOp.begin();
    for (; it != m_setDeleteExOp.end(); ++it)
    {
        ExDelOp handler = *it;
        if (handler != NULL) handler(ptr, pszFileName, nLineNum);
    }
}

void CMemMgr::DeleteArrayExOp(void *ptr, char* pszFileName, int nLineNum)
{
    std::set<ExDelOp>::iterator it = m_setDeleteArrayExOp.begin();
    for (; it != m_setDeleteArrayExOp.end(); ++it)
    {
        ExDelOp handler = *it;
        if (handler != NULL) handler(ptr, pszFileName, nLineNum);
    }
}
