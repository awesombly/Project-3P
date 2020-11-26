#include "Synchronize.h"

Synchronize::Synchronize( CriticalSection* _cs ) : csObject( _cs )
{
	if ( csObject != nullptr )
	{
		::EnterCriticalSection( &csObject->cs );
	}
}


Synchronize::~Synchronize()
{
	if ( csObject != nullptr )
	{
		::LeaveCriticalSection( &csObject->cs );
	}
}