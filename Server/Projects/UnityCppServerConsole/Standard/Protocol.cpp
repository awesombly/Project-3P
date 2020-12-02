#include "Protocol.h"


namespace Protocol
{
	namespace Type
	{
		u_short GetPacketType( const char* _name )
		{
			unsigned int hash = 0;

			const size_t size = ::strlen( _name );
			for ( size_t count = 0; count < size; ++count )
			{
				hash = _name[ count ] + ( hash << 6 ) + ( hash << 16 ) - hash;
			}

			return ( u_short )hash;
		}
	}

	PROTOCOL_BODY( TestProtocol )
}
