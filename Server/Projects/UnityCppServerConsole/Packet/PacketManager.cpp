#include "PacketManager.h"
#include "..\Standard\Log.h"
#include "..\Session\SessionManager.h"

PacketManager::PacketManager()
{
	CreateThread();
}

void PacketManager::ExecuteThread()
{
	while ( true )
	{
		if ( !packets.empty() )
		{
			PACKET* packet( &packets.front() );

			switch ( packet->packet.type )
			{
				case PACKET_TYPE::ChatMessage:
				{
					// ����Ƽ Ŭ���̾�Ʈ���� UTF-8 �������� ���ڵ��� �� ���۵Ǿ��� ������
					// Ansi �������� ���ڵ� �Ͽ� �ùٸ� ���ڿ��� ����ϴ�.
					Log::Instance().Push( ToAnsi( ( char* )packet->packet.data ) );
					SessionManager::Instance().BroadCast( packet->packet );
				} break;

				default:
				{
					// �ӽ� ó��. Ŭ��� packet.type �������
					Log::Instance().Push( ToAnsi( ( char* )packet->packet.data ) );
					SessionManager::Instance().BroadCast(packet->packet);
				} break;
			}

			packets.pop();
		}
	}
}

void PacketManager::Push( const PACKET& _packet )
{
	packets.push( _packet );
}