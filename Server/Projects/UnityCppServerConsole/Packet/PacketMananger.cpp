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
	// ���� �ʿ�?
	packets.push( _packet );
}