#pragma once
#include "..\Standard\Header.h"

class CriticalSection
{
public:
	CriticalSection();
	virtual ~CriticalSection();

public:
	void Lock();
	void UnLock();

private:
	friend class Synchronize;
	CRITICAL_SECTION cs;
};
