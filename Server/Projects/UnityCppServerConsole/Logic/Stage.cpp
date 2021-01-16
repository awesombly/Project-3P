#include "Stage.h"
#include "../Standard/Log.h"

Stage::Stage( const std::string& _stageId )
	: id( _stageId )
{
}

void Stage::BroadCast( const UPACKET& _packet ) const
{
	SessionManager::BroadCast( _packet, sessions );
}

void Stage::BroadCastExceptSelf( const UPACKET& _packet, const Session* _session ) const
{
	SessionManager::BroadCastExceptSelf( _packet, _session, sessions );
}

void Stage::LeaveStage( Session* _session, bool _removePlayer )
{
	if ( _session == nullptr )
	{
		LOG_ERROR << "Session is null." << ELogType::EndLine;
		return;
	}

	if ( _session->logicData.Player != nullptr )
	{
		Protocol::FromServer::DestroyActor protocol;
		protocol.Serial = _session->logicData.Player->Serial;
		BroadCastExceptSelf( protocol, _session );

		EraseActor( _session->logicData.Player );
		if ( _removePlayer )
		{
			SafeDelete( _session->logicData.Player );
		}
	}

	EraseSession( _session );
}

void Stage::PushSession( Session* _session )
{
	if ( _session == nullptr )
	{
		LOG_ERROR << "Session is null" << ELogType::EndLine;
		return;
	}

	sessions[ _session->GetSocket() ] = _session;
}

void Stage::EraseSession( const Session* _session )
{
	if ( _session == nullptr )
	{
		LOG_ERROR << "Session is null" << ELogType::EndLine;
		return;
	}

	SOCKET sessionSocket = _session->GetSocket();
	sessions.erase( sessionSocket );

	if ( sessionSocket == hostSocket )
	{
		if ( sessions.empty() )
		{
			hostSocket = NULL;
		}
		else
		{
			Session* session = sessions.begin()->second;
			hostSocket = session->GetSocket();

			Protocol::FromServer::ChangedStageHost protocol;
			session->Send( protocol );
		}
	}
}

void Stage::PushActor( ServerActor* _actor )
{
	if ( _actor == nullptr )
	{
		LOG_ERROR << "Actor is null." << ELogType::EndLine;
		return;
	}

	actors[ _actor->Serial ] = _actor;
}

void Stage::EraseActor( const ServerActor* _actor )
{
	if ( _actor == nullptr )
	{
		LOG_ERROR << "Actor is null." << ELogType::EndLine;
		return;
	}

	actors.erase( _actor->Serial );
}

ServerActor* Stage::FindActor( SerialType serial ) const
{
	auto findItr = actors.find( serial );
	if ( findItr == actors.cend() )
	{
		LOG_ERROR << "Actor not found." << ELogType::EndLine;
		return nullptr;
	}

	return findItr->second;
}

SOCKET Stage::GetHostSocket()
{
	return hostSocket;
}

void Stage::SetHostSocket( SOCKET _socket )
{
	hostSocket = _socket;
}

ServerNpc* Stage::FindNpc( const std::string& _name ) const
{
	auto iter = npcs.find( _name );
	if ( iter == npcs.cend() )
	{
		return nullptr;
	}

	return iter->second;
}


ServerNpc* Stage::FindNpc( SerialType _serial ) const
{
	for ( auto iter : npcs )
	{
		if ( iter.second->Actor.Serial == _serial )
		{
			return iter.second;
		}
	}

	return nullptr;
}

void Stage::PushNpc( ServerNpc* _npc )
{
	if ( _npc == nullptr )
	{
		LOG_ERROR << "Npc paremeter is null." << LOG_END;
	}

	npcs[ _npc->NpcId ] = _npc;
}

const std::string& Stage::GetId() const
{
	return id;
}

SessionContainer Stage::GetSessions() const
{
	return sessions;
}
