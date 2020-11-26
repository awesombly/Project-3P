#include "PacketMananger.h"

PacketMananger::PacketMananger()
{
	CreateThread();
}

void PacketMananger::Run()
{
	while ( true )
	{
		if ( packets.empty() == false )
		{
			PACKET* packet( &( packets.front() ) );

			switch ( packet->packet.type )
			{
				case PACKET_TYPE::ChatMessage:
				{

				} break;

				default:
				{
				
				} break;
			}

			packets.pop();
		}
	}
}

void PacketMananger::Push( const PACKET& _packet )
{
	// 검증 필요?
	packets.push( _packet );
}