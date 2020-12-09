#include "SessionManager.h"
#include "../Standard/Log.h"

SessionManager::~SessionManager()
{
	std::unordered_map<SOCKET, Session*>::iterator pair( std::begin( sessions ) );
	while ( pair != std::end( sessions ) )
	{
		SafeDelete( pair->second );
		++pair;
	}
	sessions.clear();
}

Session* SessionManager::Find( const SOCKET& _socket ) const
{
	const auto& sessionIter = sessions.find( _socket );
	if ( sessionIter == std::cend( sessions ) )
	{
		return nullptr;
	}

	return sessionIter->second;
}

std::unordered_map<SOCKET, Session*> SessionManager::GetSessions() const
{
	return sessions;
}

void SessionManager::Push( Session* _session )
{
	Log::Instance().Push( ELogType::Log, LOGFUNC( "Enter Session : "_s + _session->GetAddressString() + " : "_s + _session->GetPortString() ) );
	
	cs.Lock();
	sessions[_session->GetSocket()] = _session;
	cs.UnLock();
}

void SessionManager::Erase( Session* _session )
{
	Log::Instance().Push( ELogType::Log, LOGFUNC( "Leave Session : "_s + _session->GetAddressString() + " : "_s + _session->GetPortString() ) );

	SOCKET key = _session->GetSocket();
	cs.Lock();
	_session->ClosedSocket();
	SafeDelete( _session );
	sessions.erase( key );
	cs.UnLock();
}

void SessionManager::BroadCast( const UPACKET& _packet ) const
{
	BroadCast( _packet, sessions );
}

void SessionManager::BroadCast( const UPACKET& _packet, const SessionContainer& _sessions ) const
{
	for ( const std::pair<SOCKET, Session*>& pair : _sessions )
	{
		Session* session = pair.second;
		Log::Instance().Push( ELogType::Log, LOGFUNC( "Send From Session : "_s + session->GetAddressString() + " : " + session->GetPortString() ) );
		session->Send( _packet );
	}
}

void SessionManager::BroadCastExceptSelf( const UPACKET& _packet, const Session* _session ) const
{
	BroadCastExceptSelf( _packet, _session, sessions );
}

void SessionManager::BroadCastExceptSelf( const UPACKET& _packet, const Session* _session, const SessionContainer& _sessions ) const
{
	for ( const std::pair<SOCKET, Session*>& pair : _sessions )
	{
		Session* session = pair.second;
		if ( session == _session )
		{
			continue;
		}

		Log::Instance().Push( ELogType::Log, LOGFUNC( "Send From Session : "_s + session->GetAddressString() + " : " + session->GetPortString() ) );
		session->Send( _packet );
	}
}
