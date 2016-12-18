/* 
 * CMemMgr.h
 * 2016.12.17
 * sly
 */

#ifndef _CMEMMGR_H_
#define _CMEMMGR_H_

#include <string>
#include <set>
#include "CUtils.h"

typedef void (*ExNewOp)(void * pAlloc, size_t nSize, char* pszFileName, int nLineNum);
typedef void (*ExDelOp)(void *ptr, char* pszFileName, int nLineNum);

class CMemMgr : public Singleton<CMemMgr>
{
public:
    void Init();

public:
    //简化操作不考虑加锁，需程序初始单线程情况下调用
    void RegNewExOp(ExNewOp fun){m_setNewExOp.insert(fun);}
    void RegNewArrayExOp(ExNewOp fun){m_setNewArrayExOp.insert(fun);}

    void RegDeleteExOp(ExDelOp fun){m_setDeleteExOp.insert(fun);}
    void RegDeleteArrayExOp(ExDelOp fun){m_setDeleteArrayExOp.insert(fun);}

    void NewExOp(void * pAlloc, size_t nSize, char* pszFileName, int nLineNum);
    void NewArrayOp(void * pAlloc, size_t nSize, char* pszFileName, int nLineNum);

    void DeleteExOp(void *ptr, char* pszFileName, int nLineNum);
    void DeleteArrayExOp(void *ptr, char* pszFileName, int nLineNum);

private:
    std::set<ExNewOp> m_setNewExOp;
    std::set<ExNewOp> m_setNewArrayExOp;
    std::set<ExDelOp> m_setDeleteExOp;
    std::set<ExDelOp> m_setDeleteArrayExOp;
};

#endif
