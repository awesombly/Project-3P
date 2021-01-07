#include "PacketManager.h"
#include "../Standard/Log.h"
#include "../Session/SessionManager.h"
#include "../Logic/Stage.h"

bool PacketManager::Initialize()
{
	BindProtocols();

	std::thread th( [&] () { PacketManager::PacketProcess(); } );
	th.detach();

	return true;
}

void PacketManager::PacketProcess()
{
	while ( true )
	{
		std::unique_lock<std::mutex> lock( packetsMutex );
		cv.wait( lock, [&] () { return !packets.empty(); } );
		
		PACKET* packet = &packets.front();
		auto findItr = protocols.find( packet->packet.type );
		if ( findItr == protocols.cend() || findItr->second == nullptr )
		{
			LOG_WARNING << "Packet Not Bind : " << ToAnsi( ( char* )packet->packet.data ) << ELogType::EndLine;
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
	/* Chat, Shop ... */
	protocols[ Protocol::Both::ChatMessage::PacketType ] = &PacketManager::BroadcastToStage;

	/* Player Character */
	protocols[ Protocol::Both::SyncCrouch::PacketType ] = &PacketManager::BroadCastExceptSelfToStage;
	protocols[ Protocol::Both::SyncGrounded::PacketType ] = &PacketManager::BroadCastExceptSelfToStage;
	protocols[ Protocol::Both::SyncTransform::PacketType ] = &PacketManager::SyncTransform;
	protocols[ Protocol::Both::SyncInterpolation::PacketType ] = &PacketManager::SyncInterpolation;
	protocols[ Protocol::ToServer::EnterStage::PacketType ] = &PacketManager::EnterStage;
	
	/* NPC */
	protocols[ Protocol::Both::SyncNpcState::PacketType ] = &PacketManager::SyncNpcState;
	protocols[ Protocol::ToServer::RequestNpcInfo::PacketType ] = &PacketManager::RequestNpcInfo;
}

void PacketManager::Broadcast( const PACKET& _packet )
{
	LOG << "Broadcast : " << _packet.packet.ToString() << ELogType::EndLine;
	SessionManager::Instance().BroadCast( _packet.packet );
}

void PacketManager::BroadcastToStage( const PACKET& _packet )
{
	LOG <<"BroadcastToStage : " << _packet.packet.ToString() << ELogType::EndLine;

	const Session* session = SessionManager::Instance().Find( _packet.socket );
	if ( session == nullptr )
	{
		LOG_ERROR << "Session is null." << ELogType::EndLine;
		return;
	}

	if ( session->logicData.CurrentStage == nullptr )
	{
		LOG_ERROR << "Stage is null." << ELogType::EndLine;
		return;
	}

	session->logicData.CurrentStage->BroadCast( _packet.packet );
}

void PacketManager::BroadCastExceptSelf( const PACKET& _packet )
{
	const Session* session = SessionManager::Instance().Find( _packet.socket );
	if ( session == nullptr )
	{
		LOG_ERROR << "Session is null. socket = " << _packet.socket << ELogType::EndLine;
		return;
	}

	SessionManager::Instance().BroadCastExceptSelf( _packet.packet, session );
}

void PacketManager::BroadCastExceptSelfToStage( const PACKET& _packet )
{
	const Session* session = SessionManager::Instance().Find( _packet.socket );
	if ( session == nullptr )
	{
		LOG_ERROR << "Session is null" << ELogType::EndLine;
		return;
	}

	if ( session->logicData.CurrentStage == nullptr )
	{
		LOG_ERROR << "Stage is null" << ELogType::EndLine;
		return;
	}

	session->logicData.CurrentStage->BroadCastExceptSelf( _packet.packet, session );
}

void PacketManager::SyncTransform( const PACKET& _packet )
{
	const Session* session = SessionManager::Instance().Find( _packet.socket );
	if ( session == nullptr )
	{
		LOG_ERROR << "Session is null" << ELogType::EndLine;
		return;
	}

	if ( session->logicData.CurrentStage == nullptr )
	{
		LOG_ERROR << "Stage is null" << ELogType::EndLine;
		return;
	}

	Protocol::Both::SyncTransform protocol = _packet.packet.GetParsedData<Protocol::Both::SyncTransform>();
	ServerActor* actor = session->logicData.CurrentStage->Find( protocol.Actor.Serial );
	if ( actor == nullptr )
	{
		LOG_ERROR << "Actor is null" << ELogType::EndLine;
		return;
	}
	actor->Position = protocol.Actor.Position;
	actor->Rotation = protocol.Actor.Rotation;

	session->logicData.CurrentStage->BroadCastExceptSelf( _packet.packet, session );
}

void PacketManager::SyncInterpolation( const PACKET& _packet )
{
	const Session* session = SessionManager::Instance().Find( _packet.socket );
	if ( session == nullptr )
	{
		LOG_ERROR << "Session is null" << ELogType::EndLine;
		return;
	}

	if ( session->logicData.CurrentStage == nullptr )
	{
		LOG_ERROR << "Stage is null" << ELogType::EndLine;
		return;
	}

	Protocol::Both::SyncInterpolation protocol = _packet.packet.GetParsedData<Protocol::Both::SyncInterpolation>();
	ServerActor* actor = session->logicData.CurrentStage->Find( protocol.Actor.Serial );
	if ( actor == nullptr )
	{
		LOG_ERROR << "Actor is null" << ELogType::EndLine;
		return;
	}
	actor->Position = protocol.Actor.Position;
	actor->Rotation = protocol.Actor.Rotation;

	session->logicData.CurrentStage->BroadCastExceptSelf( _packet.packet, session );
}

void PacketManager::EnterStage( const PACKET& _packet )
{
	Protocol::ToServer::EnterStage protocol = _packet.packet.GetParsedData<Protocol::ToServer::EnterStage>();
	LOG << "Stage ID : " << protocol.StageId << " SpawnPosition" << protocol.SpawnPosition << LOG_END;

	Session* session = SessionManager::Instance().Find( _packet.socket );
	if ( session == nullptr )
	{
		LOG_ERROR << "Session is null. socket = "<< _packet.socket << ELogType::EndLine;
		return;
	}

	SessionManager::Instance().EnterStage( session, protocol.StageId );
	if ( session->logicData.CurrentStage == nullptr )
	{
		LOG_ERROR << "CurrentStage is null. socket = " << _packet.socket << ELogType::EndLine;
		return;
	}

	// 기존 플레이어들 생성
	{
		const SessionContainer sessions = session->logicData.CurrentStage->GetSessions();
		for ( auto pair : sessions )
		{
			ServerActor* player = pair.second->logicData.Player;
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
		session->logicData.Player = new ServerActor( Protocol::GetNewSerial() );
		session->logicData.CurrentStage->Push( session->logicData.Player );
	}

	std::mt19937 rand( ( UINT )::time( nullptr ) );
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

void PacketManager::RequestNpcInfo( const PACKET& _packet )
{
	Protocol::ToServer::RequestNpcInfo protocol = _packet.packet.GetParsedData<Protocol::ToServer::RequestNpcInfo>();

	Session* session = SessionManager::Instance().Find( _packet.socket );
	if ( session == nullptr )
	{
		LOG_ERROR << "Session is null." << ELogType::EndLine;
		return;
	}
	
	Stage* curStage = session->logicData.CurrentStage;
	if ( curStage == nullptr )
	{
		LOG_ERROR << "CurrentStage is null. socket : " << session->GetSocket() << ELogType::EndLine;
		return;
	}

	ServerNpc* npc = curStage->FindNpc( protocol.NpcInfo.NpcId );
	if ( npc == nullptr )
	{
		LOG << "Npc : " << protocol.NpcInfo.NpcId << " New Creation. " << "Socket : " << _packet.socket << ELogType::EndLine;
		curStage->SetNpcCriterion( _packet.socket );
		npc = new ServerNpc( protocol.NpcInfo.NpcId, protocol.NpcInfo.Target, protocol.NpcInfo.CurPosition );
		curStage->Push( npc );
	}

	Protocol::FromServer::ResponseNpcInfo responseNpc;
	responseNpc.NpcInfo = *npc;
	session->Send( responseNpc );
}

void PacketManager::SyncNpcState( const PACKET& _packet )
{
	Protocol::Both::SyncNpcState protocol = _packet.packet.GetParsedData<Protocol::Both::SyncNpcState>();

	Session* session = SessionManager::Instance().Find( _packet.socket );
	if ( session == nullptr )
	{
		LOG_ERROR << "Session is null." << ELogType::EndLine;
		return;
	}

	Stage* curStage = session->logicData.CurrentStage;
	if ( curStage == nullptr )
	{
		LOG_ERROR << "CurrentStage is null. socket : " << session->GetSocket() << ELogType::EndLine;
		return;
	}

	ServerNpc* npc = curStage->FindNpc( protocol.NpcInfo.NpcId );
	if ( npc == nullptr )
	{
		LOG_ERROR << "npc is null. socket : " << session->GetSocket() << ELogType::EndLine;
		return;
	}

	if ( curStage->GetNpcCriterion() == NULL )
	{
		curStage->SetNpcCriterion( _packet.socket );
	}

	if ( curStage->GetNpcCriterion() == _packet.socket )
	{
		curStage->UpdateNpc( protocol.NpcInfo );
	}

	Protocol::Both::SyncNpcState syncNpcState;
	syncNpcState.NpcInfo = *npc;
	session->Send( syncNpcState );
}