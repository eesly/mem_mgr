/*
 * CMemStatistics.cpp
 * 2016.12.18
 */

#include "CMemStatistics.h"
#include "CUtils.h"

CMemStatistics::CMemStatistics()
:m_total_new(0)
,m_total_del(0)
{}

void CMemStatistics::DoExNewOp(void * pAlloc, size_t nSize, char* pszFileName, int nLineNum, MemAction eAction)
{
    if (pAlloc == NULL)
        return;

    std::string strPos = std::string(pszFileName);
    POS(strPos, nLineNum);

    {
        m_lock.Lock();

        std::map<std::string, ActionInfo*>::iterator it;
        it = m_mapMems.find(strPos);

        if (it != m_mapMems.end())
        {
            std::map<void*, long>::iterator itm;
            itm = it->second->m_mapSize.find(pAlloc);
            if (itm != it->second->m_mapSize.end())
            {
                LOG_WARN("new addr %p [%s:%d] has already int the recordmap!", 
                    pAlloc, pszFileName, nLineNum);
                m_lock.Unlock();
                return;
            }
            else
            {
                it->second->total_new += nSize;
                it->second->m_mapSize.insert(std::make_pair(pAlloc, nSize));
            }
        }
        else
        {
            ActionInfo* pInfo = new ActionInfo();
            pInfo->strPos = strPos;
            pInfo->total_new += nSize;
            pInfo->m_mapSize.insert(std::make_pair(pAlloc, nSize));
            m_mapMems.insert(std::make_pair(strPos, pInfo));
        }

        m_lock.Unlock();
    }

    return;
}

void CMemStatistics::DoExDelOp(void * ptr, char* pszFileName, int nLineNum, MemAction eAction)
{
    if (ptr == NULL)
    {
        LOG_WARN("delete a NULL point, [%s:%d]", pszFileName, nLineNum);
        return;
    }

    {
        m_lock.Lock();

        std::map<std::string, ActionInfo*>::iterator it;
        it = m_mapMems.begin();
        for (; it != m_mapMems.end(); ++it)
        {
            std::map<void*, long>::iterator itm;
            itm = it->second->m_mapSize.find(ptr);
            if (itm != it->second->m_mapSize.end())
            {
                it->second->total_del += itm->second;
                it->second->m_mapSize.erase(itm);
                break;
            }
        }

        if (it == m_mapMems.end())
        {
            LOG_WARN("delete addr %p, but it isn't in recordmap! [%s:%d]", 
                ptr, pszFileName, nLineNum);
        }

        m_lock.Unlock();
    }

    return;
}

void CMemStatistics::Show()
{
    m_lock.Lock();

    LOG_INFO("================MemStatisticsInfo=====================");
    std::map<std::string, ActionInfo*>::iterator it;
    it = m_mapMems.begin();
    LOG_INFO("detail new\t\tdelete\tpercent\tpos");
    for (; it != m_mapMems.end(); ++it)
    {
        m_total_new += it->second->total_new;
        m_total_del += it->second->total_del;
        float percent = (float)(it->second->total_del) / it->second->total_new * 100;
        LOG_INFO("       %ld\t%ld\t%.1f%%\t%s", it->second->total_new, 
            it->second->total_del, percent, it->first.c_str()); 
    }
    LOG_INFO("total  %ld\t%ld\t%.1f%%\t", m_total_new, m_total_del,
        (float)(m_total_del) / m_total_new * 100);
    LOG_INFO("================End of MemStatisticsInfo==============\n");

    m_lock.Unlock();
}

