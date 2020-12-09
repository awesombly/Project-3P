#include "PacketManager.h"
#include "../Standard/Log.h"
#include "../Session/SessionManager.h"
#include "../Logic/Stage.h"

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
		std::unique_lock<std::mutex> lock( packetsMutex );
		cv.wait( lock, [&] () { return !packets.empty(); } );
		
		PACKET* packet = &packets.front();
		auto findItr = protocols.find( packet->packet.type );
		if ( findItr == protocols.cend() || findItr->second == nullptr )
		{
			Log::Instance().Push( ELogType::Warning, LOGFUNC( "Packet Not Bind : " + ToAnsi( ( char* )packet->packet.data ) ) );
			packets.pop();
			continue;
		}
	
		protocols[ packet->packet.type ]( *packet );
		packets.pop();
	}
}

void PacketManager::Push( const PACKET& _packet )
{
	std::lock_guard<std::mutex> lock( packetsMutex );
	packets.push( _packet );
	cv.notify_one();
}

void PacketManager::BindProtocols()
{
	protocols[ Protocol::Both::ChatMessage::PacketType ] = &PacketManager::BroadcastToStage;
	protocols[ Protocol::Both::SyncTransform::PacketType ] = &PacketManager::BroadCastExceptSelfToStage;
	protocols[ Protocol::Both::SyncInterpolation::PacketType ] = &PacketManager::BroadCastExceptSelfToStage;

	protocols[ Protocol::ToServer::EnterStage::PacketType ] = &PacketManager::ReceiveEnterStage;
}

void PacketManager::Broadcast( const PACKET& _packet )
{
	Log::Instance().Push( ELogType::Log, "Broadcast : " + _packet.packet.ToString() );
	SessionManager::Instance().BroadCast( _packet.packet );
}

void PacketManager::BroadcastToStage( const PACKET& _packet )
{
	Log::Instance().Push( ELogType::Log, "BroadcastToStage : " + _packet.packet.ToString() );

	const Session* session = SessionManager::Instance().Find( _packet.socket );
	if ( session == nullptr )
	{
		Log::Instance().Push( ELogType::Error, LOGFUNC( "Session is null." ) );
		return;
	}

	if ( session->logicData.CurrentStage == nullptr )
	{
		Log::Instance().Push( ELogType::Error, LOGFUNC( "Stage is null." ) );
		return;
	}

	session->logicData.CurrentStage->BroadCast( _packet.packet );
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

void PacketManager::BroadCastExceptSelfToStage( const PACKET& _packet )
{
	const Session* session = SessionManager::Instance().Find( _packet.socket );
	if ( session == nullptr )
	{
		Log::Instance().Push( ELogType::Error, LOGFUNC( "Session is null." ) );
		return;
	}

	if ( session->logicData.CurrentStage == nullptr )
	{
		Log::Instance().Push( ELogType::Error, LOGFUNC( "Stage is null." ) );
		return;
	}

	session->logicData.CurrentStage->BroadCastExceptSelf( _packet.packet, session );
}

void PacketManager::ReceiveEnterStage( const PACKET& _packet )
{
	Protocol::ToServer::EnterStage protocol = _packet.packet.GetParsedData<Protocol::ToServer::EnterStage>();
	Log::Instance().Push( ELogType::Log, protocol.PacketName + " : " + _packet.packet.ToString() );

	Session* session = SessionManager::Instance().Find( _packet.socket );
	if ( session == nullptr )
	{
		Log::Instance().Push( ELogType::Error, "Session is null. socket = " + _packet.socket );
		return;
	}

	SessionManager::Instance().EnterStage( session, protocol.StageId );
	if ( session->logicData.CurrentStage == nullptr )
	{
		Log::Instance().Push( ELogType::Error, "CurrentStage is null. socket = " + _packet.socket );
		return;
	}

	// 기존 플레이어들 생성
	{
		const SessionContainer sessions = session->logicData.CurrentStage->GetSessions();
		for ( auto pair : sessions )
		{
			ServerObject* player = pair.second->logicData.Player;
			if ( player == nullptr || player == session->logicData.Player )
			{
				continue;
			}

			Protocol::FromServer::CreatePlayer createPlayer;
			createPlayer.Player = *player;
			createPlayer.IsLocal = false;
			session->Send( createPlayer );
		}
	}

	if ( session->logicData.Player == nullptr )
	{
		session->logicData.Player = new ServerObject();
		session->logicData.CurrentStage->Push( session->logicData.Player );
	}

	std::mt19937 rand( ( UINT )::time( nullptr ) );
	session->logicData.Player->Serial = Protocol::GetNewSerial();
	session->logicData.Player->Position = { protocol.SpawnPosition.x + ( float )( rand() % 8 ), protocol.SpawnPosition.y, protocol.SpawnPosition.z + ( float )( rand() % 8 ) };
	session->logicData.Player->Rotation = Quaternion::Identity;

	Protocol::FromServer::CreatePlayer createPlayer;
	createPlayer.Player = *session->logicData.Player;
	createPlayer.IsLocal = true;

	UPACKET response;
	response.SetData( createPlayer );
	session->Send( response );

	// 직렬화 처리가 또 필요하다.. Id 같은거로 구분해야할듯
	createPlayer.IsLocal = false;
	response.SetData( createPlayer );
	session->logicData.CurrentStage->BroadCastExceptSelf( response, session );
}
