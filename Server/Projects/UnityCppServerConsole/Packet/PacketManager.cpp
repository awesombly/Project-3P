﻿#include "PacketManager.h"
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
	protocols[ Protocol::Both::SyncUseItem::PacketType ] = &PacketManager::BroadCastExceptSelfToStage;
	protocols[ Protocol::Both::SyncTransform::PacketType ] = &PacketManager::SyncTransform;
	protocols[ Protocol::Both::SyncInterpolation::PacketType ] = &PacketManager::SyncInterpolation;
	protocols[ Protocol::ToServer::EnterStage::PacketType ] = &PacketManager::EnterStage;
	
	/* NPC */
	protocols[ Protocol::ToServer::RequestNpcInfo::PacketType ] = &PacketManager::RequestNpcInfo;
	protocols[ Protocol::ToServer::RequestNpcSync::PacketType ] = &PacketManager::RequestNpcSync;
	protocols[ Protocol::ToServer::ResponseHostNpcInfo::PacketType ] = &PacketManager::ResponseHostNpcInfo;
	protocols[ Protocol::Both::SyncNpcTarget::PacketType ] = &PacketManager::SyncNpcTarget;
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

	session->logicData.Player->Position = protocol.SpawnPosition;
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

	Protocol::FromServer::ResponseNpcInfo responseNpcInfo;
	responseNpcInfo.NpcId = protocol.NpcId;

	ServerNpc* npc = curStage->FindNpc( protocol.NpcId );
	if ( npc == nullptr )
	{
		npc = new ServerNpc();
		npc->Actor.Serial = Protocol::GetNewSerial();
		npc->NpcId = protocol.NpcId;
		curStage->Push( npc );
	}
	responseNpcInfo.Serial = npc->Actor.Serial;

	SOCKET criterionSocket = curStage->GetHostSocket();
	Session* criterion = SessionManager::Instance().Find( criterionSocket );
	if ( criterion == nullptr )
	{
		curStage->SetHostSocket( _packet.socket );
		criterion = session;
		criterionSocket = _packet.socket;
	}

	if( criterionSocket == _packet.socket )
	{
		responseNpcInfo.IsLocal = true;
	}
	else
	{
		responseNpcInfo.IsLocal = false;
	}

	session->Send( responseNpcInfo );
}

void PacketManager::RequestNpcSync( const PACKET& _packet )
{
	Protocol::ToServer::RequestNpcSync protocol = _packet.packet.GetParsedData<Protocol::ToServer::RequestNpcSync>();

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

	SOCKET criterionSocket = curStage->GetHostSocket();
	Session* criterion = SessionManager::Instance().Find( criterionSocket );
	if ( criterion == nullptr )
	{
		LOG_ERROR << "Criterion is null. RequestSocket : " << session->GetSocket() << LOG_END;
		return;
	}

	ServerNpc* npc = curStage->FindNpc( protocol.Serial );
	if ( npc == nullptr )
	{
		LOG_ERROR << "Npc is null. RequestSocket : " << session->GetSocket() << LOG_END;
		return;
	}

	Protocol::FromServer::RequestHostNpcInfo requestHostNpcInfo;
	requestHostNpcInfo.Serial = npc->Actor.Serial;
	criterion->Send( requestHostNpcInfo );
}

void PacketManager::ResponseHostNpcInfo( const PACKET& _packet )
{
	Protocol::ToServer::ResponseHostNpcInfo protocol = _packet.packet.GetParsedData<Protocol::ToServer::ResponseHostNpcInfo>();

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
	
	Protocol::FromServer::SyncNpcInfo syncNpcInfo;
	syncNpcInfo.NpcInfo = protocol.NpcInfo;
	session->logicData.CurrentStage->BroadCastExceptSelf( syncNpcInfo, session );
}

void PacketManager::SyncNpcTarget( const PACKET& _packet )
{
	Protocol::Both::SyncNpcTarget protocol = _packet.packet.GetParsedData<Protocol::Both::SyncNpcTarget>();

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

	if ( session->GetSocket() != curStage->GetHostSocket() )
	{
		LOG_ERROR << "비정상적인 요청 socket : " << session->GetSocket() << LOG_END;
		return;
	}

	Protocol::Both::SyncNpcTarget responseProtocol;
	responseProtocol = protocol;
	session->logicData.CurrentStage->BroadCastExceptSelf( responseProtocol, session );
}