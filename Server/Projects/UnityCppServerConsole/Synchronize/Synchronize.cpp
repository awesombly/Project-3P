#include "Synchronize.h"

Synchronize::Synchronize( CriticalSection* _cs ) : csObject( _cs )
{
	if ( _cs != nullptr )
	{
		::EnterCriticalSection( &_cs->cs );
	}
}

Synchronize::~Synchronize()
{
	if ( csObject != nullptr )
	{
		::LeaveCriticalSection( &csObject->cs );
	}
}