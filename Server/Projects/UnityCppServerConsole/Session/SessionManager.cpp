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
	SafeDelete( _session );
	sessions.erase( key );
	cs.UnLock();
}

void SessionManager::BroadCast( const UPACKET& _packet ) const
{
	for ( const std::pair<SOCKET, Session*>& pair : sessions )
	{
		Session* session = pair.second;
		Log::Instance().Push( ELogType::Log, LOGFUNC( "Send From Session : "_s + session->GetAddressString() + " : " + session->GetPortString() ) );
		session->Send( _packet );
	}
}

void SessionManager::BroadCastExceptSelf( const UPACKET& _packet, const Session* _session ) const
{
	for ( const std::pair<SOCKET, Session*>& pair : sessions )
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
