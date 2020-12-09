#include "Stage.h"
#include "../Standard/Log.h"

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
		Log::Instance().Push( ELogType::Error, LOGFUNC( "Session is null." ) );
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
		Log::Instance().Push( ELogType::Error, LOGFUNC( "Object is null." ) );
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
