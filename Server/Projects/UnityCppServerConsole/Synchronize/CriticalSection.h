#pragma once
#include "..\Standard\Header.h"

class CriticalSection
{
public:
	CriticalSection();
	virtual ~CriticalSection();

private:
	friend class Synchronize;
	CRITICAL_SECTION cs;
};
