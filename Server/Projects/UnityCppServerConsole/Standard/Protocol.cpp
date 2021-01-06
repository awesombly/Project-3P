#include "Protocol.h"


namespace Protocol
{
	SerialType GetNewSerial()
	{
		static SerialType serialIndex = 0;
		return ++serialIndex;
	}

	PacketType GetPacketType( const char* _name )
	{
		unsigned int hash = 0;

		const size_t size = ::strlen( _name );
		for ( size_t count = 0; count < size; ++count )
		{
			hash = _name[ count ] + ( hash << 6 ) + ( hash << 16 ) - hash;
		}

		return ( PacketType )hash;
	}

	PROTOCOL_BODY( Both, SyncTransform );
	PROTOCOL_BODY( Both, SyncInterpolation );
	PROTOCOL_BODY( Both, SyncCrouch );
	PROTOCOL_BODY( Both, SyncGrounded );
	PROTOCOL_BODY( Both, SyncNpcState );
	PROTOCOL_BODY( ToServer, EnterStage );
	PROTOCOL_BODY( ToServer, RequestNpcInfo );
	PROTOCOL_BODY( FromServer, Connected );
	PROTOCOL_BODY( FromServer, CreatePlayer );
	PROTOCOL_BODY( FromServer, DestroyActor );
	PROTOCOL_BODY( FromServer, ResponseNpcInfo );
}
