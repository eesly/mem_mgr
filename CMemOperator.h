/*
 * CMemOperator.h
 * 2016.11.28
 * sly
 */

#ifndef _CMEMOPERATOR_H_

#include "CUtils.h"
#include <stack>
#include <string>

void* operator new( size_t nSize, char* pszFileName, int nLineNum );
void* operator new[]( size_t nSize, char* pszFileName, int nLineNum );

void operator delete(void *ptr, char* pszFileName, int nLineNum );
void operator delete[](void *ptr, char* pszFileName, int nLineNum );

//void operator delete( void *ptr );
//void operator delete[]( void *ptr );

void PreDelRecord(unsigned int nLine, const char* pFile);
void PostDelRecord(void* ptr, MemAction eAction);

#define New         new(__FILE__, __LINE__)
#define Delete      PreDelRecord(__LINE__, __FILE__); delete
#define Malloc(s)   ((void*)(New unsigned char[s]))
#define Free(p)     Delete [] (char*)(p)

#endif
