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

void Stage::Push( ServerObject* _object )
{
	if ( _object == nullptr )
	{
		Log::Instance().Push( ELogType::Error, LOGFUNC( "Object is null." ) );
		return;
	}

	objects[ _object->Serial ] = _object;
}

void Stage::Erase( const ServerObject* _object )
{
	if ( _object == nullptr )
	{
		Log::Instance().Push( ELogType::Warning, LOGFUNC( "Object is null." ) );
		return;
	}

	objects.erase( _object->Serial );
}

void Stage::BroadCast( const UPACKET& _packet ) const
{
	SessionManager::BroadCast( _packet, sessions );
}

void Stage::BroadCastExceptSelf( const UPACKET& _packet, const Session* _session ) const
{
	SessionManager::BroadCastExceptSelf( _packet, _session, sessions );
}

ServerObject* Stage::Find( SerialType serial ) const
{
	auto findItr = objects.find( serial );
	if ( findItr == objects.cend() )
	{
		Log::Instance().Push( ELogType::Error, LOGFUNC( "Object not found." ) );
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