#include "PacketManager.h"
#include "..\Standard\Log.h"
#include "..\Session\SessionManager.h"
#include "..\Thread\ThreadPool.h"

PacketManager::PacketManager()
{
	ThreadPool::Instance().Enqueue( [&] () { PacketManager::WorkPacket(); } );
}

void PacketManager::WorkPacket()
{
	while ( true )
	{
		std::unique_lock<std::mutex> lock( workMutex );
		cv.wait( lock, [this] () { return !packets.empty(); } );
		
		PACKET* packet = std::move( &packets.front() );
		if ( packet->packet.type == PACKET_TYPE::ChatMessage )
		{
			// 유니티 클라이언트에서 UTF-8 형식으로 인코딩한 후 전송되었기 때문에
			// Ansi 형식으로 디코딩 하여 올바른 문자열을 만듭니다.
			// packet->packet이라.. 좀 더 명확한 이름이 생각난다면 바꾸겠습니다.
			
			Log::Instance().Push( "Broadcast : " + ToAnsi( ( char* )packet->packet.data ) );
			SessionManager::Instance().BroadCast( packet->packet );
		}

		packets.pop();
	}
}

void PacketManager::Push( const PACKET& _packet )
{
	std::lock_guard<std::mutex> lock( workMutex );
	packets.push( _packet );
	cv.notify_one();
}