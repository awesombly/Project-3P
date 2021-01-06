#include "Stage.h"
#include "../Standard/Log.h"

Stage::Stage( const std::string& _stageId )
	: id( _stageId )
{
}

void Stage::Push( Session* _session )
{
	if ( _session == nullptr )
	{
		LOG_ERROR << "Session is null" << ELogType::EndLine;
		return;
	}

	sessions[ _session->GetSocket() ] = _session;
}

void Stage::Erase( const Session* _session )
{
	if ( _session == nullptr )
	{
		LOG_ERROR << "Session is null" << ELogType::EndLine;
		return;
	}

	sessions.erase( _session->GetSocket() );
}

void Stage::Push( ServerActor* _actor )
{
	if ( _actor == nullptr )
	{
		LOG_ERROR << "Actor is null." << ELogType::EndLine;
		return;
	}

	actors[ _actor->Serial ] = _actor;
}

void Stage::Push( ServerNpc* _npc )
{
	if ( _npc == nullptr )
	{
		LOG_ERROR << "Actor is null." << ELogType::EndLine;
		return;
	}

	actors[ _npc->Serial ] = _npc;
	npcs[ _npc->NpcId ] = _npc;
}

void Stage::Erase( const ServerActor* _actor )
{
	if ( _actor == nullptr )
	{
		LOG_ERROR << "Actor is null." << ELogType::EndLine;
		return;
	}

	actors.erase( _actor->Serial );
}

void Stage::BroadCast( const UPACKET& _packet ) const
{
	SessionManager::BroadCast( _packet, sessions );
}

void Stage::BroadCastExceptSelf( const UPACKET& _packet, const Session* _session ) const
{
	SessionManager::BroadCastExceptSelf( _packet, _session, sessions );
}

ServerActor* Stage::Find( SerialType serial ) const
{
	auto findItr = actors.find( serial );
	if ( findItr == actors.cend() )
	{
		LOG_ERROR << "Actor not found." << ELogType::EndLine;
		return nullptr;
	}

	return findItr->second;
}

ServerNpc* Stage::FindNpc( const std::string& _name ) const
{
	auto iter = npcs.find( _name );
	if ( iter == std::cend( npcs ) )
	{
		LOG << "NpcName : " << _name << " Not Found." << ELogType::EndLine;
		return nullptr;
	}

	return iter->second;
}

const std::string& Stage::GetId() const
{
	return id;
}

SessionContainer Stage::GetSessions() const
{
	return sessions;
}
