#include "PacketManager.h"
#include "..\Standard\Log.h"
#include "..\Session\SessionManager.h"

bool PacketManager::Initialize()
{
	BindProtocols();

	std::thread th( [&] () { PacketManager::WorkPacket(); } );
	th.detach();
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
	protocols[ Protocol::Both::ChatMessage::PacketType ] = &PacketManager::Broadcast;
	protocols[ Protocol::Both::SyncTransform::PacketType ] = &PacketManager::BroadCastExceptSelf;
	protocols[ Protocol::Both::TestProtocol::PacketType ] = &PacketManager::ReceiveTestProtocol;

	protocols[ Protocol::ToServer::EnterStage::PacketType ] = &PacketManager::ReceiveEnterStage;
}

void PacketManager::Broadcast( const PACKET& _packet )
{
	Log::Instance().Push( ELogType::Log, "Broadcast : " + _packet.packet.ToString() );
	SessionManager::Instance().BroadCast( _packet.packet );
}

void PacketManager::BroadCastExceptSelf( const PACKET& _packet )
{
	const Session* session = SessionManager::Instance().Find( _packet.socket );
	if ( session == nullptr )
	{
		Log::Instance().Push( ELogType::Error, "Session is null. socket = " + _packet.socket );
		return;
	}

	SessionManager::Instance().BroadCastExceptSelf( _packet.packet, session );
}

void PacketManager::ReceiveTestProtocol( const PACKET& _packet )
{
	Protocol::Both::TestProtocol protocol = _packet.packet.GetParsedData<Protocol::Both::TestProtocol>();
	Log::Instance().Push( ELogType::Log, protocol.PacketName + " : " + _packet.packet.ToString() );

	{
		protocol.Id = "ResponseTest";
		protocol.ItemList.push_back( Protocol::Both::TestProtocol::Item( "Dildo", 69 ) );
		protocol.ItemList.push_back( Protocol::Both::TestProtocol::Item( "Penis", 74 ) );

		UPACKET response;
		response.SetData( protocol );
		SessionManager::Instance().BroadCast( response );
	}
}

void PacketManager::ReceiveEnterStage( const PACKET& _packet )
{
	Protocol::FromServer::CreatePlayer createPlayer;
	createPlayer.Position = { 1.0f, 30.0f, 2.0f };
	createPlayer.Direction = { 0.1f, 0.2f, 0.3f };
	createPlayer.IsLocal = true;
	/// 테스트용 코드
	static const std::vector<std::string> names( { "Junhwan", "Taehong", "Sungsu" } );
	static int index = 0;
	createPlayer.Name = names[ index % names.size() ];
	++index;

	UPACKET response;
	response.SetData( createPlayer );

	Session* session = SessionManager::Instance().Find( _packet.socket );
	if ( session == nullptr )
	{
		Log::Instance().Push( ELogType::Error, "Session is null. socket = " + _packet.socket );
		return;
	}
	session->Send( response );

	// 직렬화 처리가 또 필요하다.. Id 같은거로 구분해야할듯
	createPlayer.IsLocal = false;
	response.SetData( createPlayer );
	SessionManager::Instance().BroadCastExceptSelf( response, session );
}
