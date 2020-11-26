#include "CriticalSection.h"

CriticalSection::CriticalSection()
{
	::InitializeCriticalSection( &cs );
}

CriticalSection::~CriticalSection()
{
	::DeleteCriticalSection( &cs );
}