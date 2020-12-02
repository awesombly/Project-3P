#include "SessionManager.h"
#include "..\Standard\Log.h"

SessionManager::~SessionManager()
{
	std::unordered_map<SOCKET, Session*>::iterator iter( std::begin( sessions ) );
	while ( iter != std::end( sessions ) )
	{
		SafeDelete( iter++->second );
	}
	sessions.clear();
}

Session* SessionManager::Find( const SOCKET& _socket )
{
	const auto& sessionIter = sessions.find( _socket );
	if ( sessionIter == std::cend( sessions ) )
	{
		return nullptr;
	}

	return sessionIter->second;
}

Session* SessionManager::Find( const std::string& _name )
{
	auto sessionIter = std::cbegin( sessions );
	while ( sessionIter++ == std::cend( sessions ) )
	{
		Session* session = sessionIter->second;
		if ( _name.compare( session->GetData().nickName ) == 0 )
		{
			return session;
		}
	}
	return nullptr;
}

void SessionManager::Push( Session* _session )
{
	cs.Lock();
	Log::Instance().Push( ELogType::Log, "Enter Session : "_s + _session->GetAddressString() + " : "_s + _session->GetPortString() );
	
	sessions[_session->GetSocket()] = _session;
	cs.UnLock();
}

void SessionManager::Erase( Session* _session )
{
	cs.Lock();
	Log::Instance().Push( ELogType::Log, "Leave Session : "_s + _session->GetAddressString() + " : "_s + _session->GetPortString() );

	int key = _session->GetSocket();
	SafeDelete( _session );
	sessions.erase( key );
	cs.UnLock();
}

void SessionManager::BroadCast( const UPACKET& _packet ) const
{
	for ( const std::pair<SOCKET, Session*>& pair : sessions )
	{
		Session* session = pair.second;
		Log::Instance().Push( ELogType::Log, "Send From Session : "_s + session->GetAddressString() + " : " + session->GetPortString() );
		session->Send( _packet );
	}
}