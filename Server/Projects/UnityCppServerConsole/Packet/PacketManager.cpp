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
					Log::Instance().Push( "�޼��� ����" );
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
	// ���� �ʿ�?
	packets.push( _packet );
}