#include "PacketManager.h"
#include "..\Standard\Log.h"
#include "..\Session\SessionManager.h"
#include "..\Thread\ThreadPool.h"

bool PacketManager::Initialize()
{
	BindProtocols();
	ThreadPool::Instance().Enqueue( [&] () { PacketManager::WorkPacket(); } );
	return true;
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
			Log::Instance().Push( ELogType::Warning, "Packet Not Bind : " + ToAnsi( ( char* )packet->packet.data ) );
			packets.pop();
			continue;
		}
	
		protocols[ packet->packet.type ]( *packet );
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

	protocols[ Protocol::TestProtocol::Type ] = &PacketManager::ReceiveTestProtocol;
}

void PacketManager::Broadcast( const PACKET& _packet )
{
	Log::Instance().Push( ELogType::Log, "Broadcast : " + _packet.packet.ToString() );
	SessionManager::Instance().BroadCast( _packet.packet );
}

void PacketManager::ReceiveTestProtocol( const PACKET& _packet )
{
	Protocol::TestProtocol protocol = _packet.packet.GetParsedData<Protocol::TestProtocol>();
	Log::Instance().Push( ELogType::Log, protocol.Name + " : " + _packet.packet.ToString() );

	{
		protocol.Id = "ResponseTest";
		protocol.ItemList.push_back( Protocol::TestProtocol::Item( "Dildo", 69 ) );
		protocol.ItemList.push_back( Protocol::TestProtocol::Item( "Penis", 74 ) );

		UPACKET response;
		response.SetData( protocol );
		SessionManager::Instance().BroadCast( response );
	}
}