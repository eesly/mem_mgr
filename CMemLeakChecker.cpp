/*
 * CMemLeakChecker.cpp
 * 2016.12.18
 */

#include "CMemLeakChecker.h"
#include "CUtils.h"

void CMemLeakChecker::DoExNewOp(void * pAlloc, size_t nSize, char* pszFileName, int nLineNum, MemAction eAction)
{
    if (pAlloc == NULL)
        return;

    MemChunk* pstChunk = (MemChunk *)new MemChunk();
    pstChunk->pAddr = pAlloc;
    pstChunk->nSize = nSize;
    pstChunk->eStatus = MEM_ALLOC;
    pstChunk->stPosInfoNew.nLine = nLineNum;
    pstChunk->stPosInfoNew.strFile = pszFileName;
    pstChunk->stPosInfoNew.eAction = eAction;

    {
        m_lock.Lock();

        std::map<void*, MemChunk*>::iterator it;
        it = m_mapMems.find(pAlloc);

        if (it != m_mapMems.end())
        {
            LOG_WARN("new addr %p [%s:%d] has already int the recordmap [%s:%d]!", 
                pAlloc, pszFileName, nLineNum, 
                it->second->stPosInfoNew.strFile.c_str(), 
                it->second->stPosInfoNew.nLine);
            m_lock.Unlock();
            delete pstChunk;
            return;
        }

        m_mapMems.insert(std::make_pair(pAlloc, pstChunk));

        m_lock.Unlock();
    }

    return;
}

void CMemLeakChecker::DoExDelOp(void * ptr, char* pszFileName, int nLineNum, MemAction eAction)
{
    if (ptr == NULL)
    {
        LOG_WARN("delete a NULL point, [%s:%d]", pszFileName, nLineNum);
        return;
    }

    {
        m_lock.Lock();

        std::map<void*, MemChunk*>::iterator it;
        it = m_mapMems.find(ptr);

        if (it == m_mapMems.end())
        {
            LOG_WARN("delete addr %p, but it isn't in recordmap! [%s:%d]", 
                ptr, pszFileName, nLineNum);
            m_lock.Unlock();
            return;
        }

        if ((eAction == ACTION_DELETE_SINGLE && it->second->stPosInfoNew.eAction == ACTION_NEW_SINGLE) ||
           (eAction == ACTION_DELETE_ARRAY && it->second->stPosInfoNew.eAction == ACTION_NEW_ARRAY))
        {
            delete it->second;
            m_mapMems.erase(it);
        }
        else
        {
            it->second->eStatus = MEM_FREE;
            it->second->stPosInfoDelete.nLine = nLineNum;
            it->second->stPosInfoDelete.strFile = pszFileName;
            it->second->stPosInfoDelete.eAction = eAction;
        }

        m_lock.Unlock();
    }

    return;
}

void CMemLeakChecker::Show()
{
    m_lock.Lock();

    LOG_INFO("================MemLeakInfo=====================");
    std::map<void*, MemChunk*>::iterator it;
    it = m_mapMems.begin();
    LOG_INFO("addr\t\ttype\tsize(byte)\tpos");
    for (; it != m_mapMems.end(); ++it)
    {
        LOG_INFO("%p\t%s\t%d\t\t[%s:%d]", it->second->pAddr, 
            it->second->stPosInfoNew.eAction == ACTION_NEW_SINGLE ? "single" : "array", 
            it->second->nSize,
            it->second->stPosInfoNew.strFile.c_str(),
            it->second->stPosInfoNew.nLine);
    }
    LOG_INFO("================End of MemLeakInfo==============\n");

    m_lock.Unlock();
}

