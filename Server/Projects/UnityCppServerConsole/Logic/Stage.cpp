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
		Log::Instance().Push( ELogType::Error, LOGFUNC( "Session is null." ) );
		return;
	}

	sessions[ _session->GetSocket() ] = _session;
}

void Stage::Erase( const Session* _session )
{
	if ( _session == nullptr )
	{
		Log::Instance().Push( ELogType::Warning, LOGFUNC( "Session is null." ) );
		return;
	}

	sessions.erase( _session->GetSocket() );
}

void Stage::Push( ServerActor* _actor )
{
	if ( _actor == nullptr )
	{
		Log::Instance().Push( ELogType::Error, LOGFUNC( "Actor is null." ) );
		return;
	}

	actors[ _actor->Serial ] = _actor;
}

void Stage::Erase( const ServerActor* _actor )
{
	if ( _actor == nullptr )
	{
		Log::Instance().Push( ELogType::Warning, LOGFUNC( "Actor is null." ) );
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
		Log::Instance().Push( ELogType::Error, LOGFUNC( "Actor not found." ) );
		return nullptr;
	}

	return findItr->second;
}

const std::string& Stage::GetId() const
{
	return id;
}

SessionContainer Stage::GetSessions() const
{
	return sessions;
}
