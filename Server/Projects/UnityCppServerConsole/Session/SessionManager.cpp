#include "SessionManager.h"
#include "..\Standard\Log.h"

SessionManager::~SessionManager()
{
	std::list<Session*>::iterator iter( std::begin( sessions ) );
	while ( iter != std::end( sessions ) )
	{
		delete *iter;
		*iter = nullptr;
		sessions.erase( iter++ );
	}
}

void SessionManager::Push( Session* _session )
{
	cs.Lock();
	std::string msg( "Enter Session : "_s + ::inet_ntoa( _session->address.sin_addr ) + " : "_s  + std::to_string( ::ntohs( _session->address.sin_port ) ) );
	Log::Instance().Push( ELogType::Log, msg );
	
	sessions.push_back( _session );
	cs.UnLock();
}

void SessionManager::Erase( Session* _session )
{
	cs.Lock();
	std::list<Session*>::iterator iter( std::find( std::begin( sessions ), std::end( sessions ), _session ) );
	if ( iter != std::end( sessions ) )
	{
		Log::Instance().Push( ELogType::Log, "Leave Session : "_s + ::inet_ntoa( _session->address.sin_addr ) + " : "_s + std::to_string( ::ntohs( _session->address.sin_port ) ) );

		delete *iter;
		*iter = nullptr;
		sessions.erase( iter );
	}
	cs.UnLock();
}

void SessionManager::BroadCast( const UPACKET& _packet ) const
{
	for ( Session* session : sessions )
	{
		Log::Instance().Push( ELogType::Log, "Send From Session : "_s + ::inet_ntoa( session->address.sin_addr ) + " : " + std::to_string( ::ntohs( session->address.sin_port ) ) );
		session->Send( _packet );
	}
}