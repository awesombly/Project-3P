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

			if ( packet->packet.type == PACKET_TYPE::ChatMessage )
			{
				// 유니티 클라이언트에서 UTF-8 형식으로 인코딩한 후 전송되었기 때문에
				// Ansi 형식으로 디코딩 하여 올바른 문자열을 만듭니다.
				Log::Instance().Push( ToAnsi( ( char* )packet->packet.data ) );
				SessionManager::Instance().BroadCast( packet->packet );
			}

			packets.pop();
		}
	}
}

void PacketManager::Push( const PACKET& _packet )
{
	packets.push( _packet );
}