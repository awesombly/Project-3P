#include "PacketManager.h"
#include "..\Standard\Log.h"

PacketManager::PacketManager()
{
	CreateThread();
}

void PacketManager::ExecuteThread()
{
	while ( true )
	{
		if ( packets.empty() == false )
		{
			PACKET* packet( &packets.front() );

			switch ( packet->packet.type )
			{
				case PACKET_TYPE::ChatMessage:
				{
					Log::Instance().Push( "메세지 도착" );
				} break;

				default:
				{
				
				} break;
			}

			packets.pop();
		}
	}
}

void PacketManager::Push( const PACKET& _packet )
{
	// 검증 필요?
	packets.push( _packet );
}