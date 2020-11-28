#include "Protocol.h"

namespace PACKET_TYPE
{
	u_short GetPacketType( const char* name )
	{
		unsigned int hash = 0;

		size_t size = ::strlen( name );
		for ( size_t i = 0; i < size; ++i )
		{
			hash = name[ i ] + ( hash << 6 ) + ( hash << 16 ) - hash;
		}

		return ( u_short )hash;
	}
}
