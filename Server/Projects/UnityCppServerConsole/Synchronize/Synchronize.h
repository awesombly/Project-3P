#pragma once
#include "CriticalSection.h"

class Synchronize
{
public:
	Synchronize( CriticalSection* _cs );
	virtual ~Synchronize();

private:
	CriticalSection* csObject;
};