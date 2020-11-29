#include "PacketManager.h"
#include "..\Standard\Log.h"
#include "..\Session\SessionManager.h"
#include "..\Thread\ThreadPool.h"

PacketManager::PacketManager()
{
	BindProtocols();

	ThreadPool::Instance().Enqueue( [&] () { PacketManager::WorkPacket(); } );
}

void PacketManager::WorkPacket()
{
	while ( true )
	{
		std::unique_lock<std::mutex> lock( workMutex );
		cv.wait( lock, [&] () { return !packets.empty(); } );
		
		PACKET* packet = std::move( &packets.front() );
		auto findItr = protocols.find( packet->packet.type );
		if ( findItr == protocols.cend() || findItr->second == nullptr )
		{
			Log::Instance().Push( ELogType::Log, "Packet Not Bind : " + ToAnsi( ( char* )packet->packet.data ) );
			packets.pop();
			continue;
		}
	
		protocols[ packet->packet.type ]( packet->packet );
		packets.pop();
	}
}

void PacketManager::Push( const PACKET& _packet )
{
	std::lock_guard<std::mutex> lock( workMutex );
	packets.push( _packet );
	cv.notify_one();
}

void PacketManager::BindProtocols()
{
	protocols[ Protocol::Type::ChatMessage ] = &PacketManager::Broadcast;
}

void PacketManager::Broadcast( const UPACKET& packet )
{
	// 유니티 클라이언트에서 UTF-8 형식으로 인코딩한 후 전송되었기 때문에
	// Ansi 형식으로 디코딩 하여 올바른 문자열을 만듭니다.
	Log::Instance().Push( ELogType::Log, "Broadcast : " + ToAnsi( ( char* )packet.data ) );
	SessionManager::Instance().BroadCast( packet );
}