#include "SessionManager.h"
#include "..\Standard\Log.h"

void SessionManager::Push( Session* _session )
{
	{
		Synchronize sync( this );
		std::string msg( "Enter Session : " );
		msg += ::inet_ntoa( _session->address.sin_addr );
		msg += " : ";
		msg += std::to_string( ::ntohs( _session->address.sin_port ) );
		Log::Instance().Push( msg );
		
		sessions.push_back( _session );
	}
}

void SessionManager::Erase( Session* _session )
{
	{
		Synchronize sync( this );
		std::list<Session*>::iterator iter( std::find( std::begin( sessions ), std::end( sessions ), _session ) );
		if ( iter != std::end( sessions ) )
		{
			std::string msg( "Leave Session : " );
			msg += ::inet_ntoa( _session->address.sin_addr );
			msg += " : ";
			msg += std::to_string( ::ntohs( _session->address.sin_port ) );
			Log::Instance().Push( msg );

			delete *iter;
			*iter = nullptr;
			sessions.erase( iter );
		}
	}
}

void SessionManager::BroadCast( const UPACKET& _packet )
{
	Log::Instance().Push( "BroadCast" );
	for ( Session* session : sessions )
	{
		std::string msg( "Send From Session : " );
		msg += ::inet_ntoa( session->address.sin_addr );
		msg += " : ";
		msg += std::to_string( ::ntohs( session->address.sin_port ) );
		Log::Instance().Push( msg );

		::send( session->GetSocket(), ( char* )&_packet, _packet.length, 0 );
	}
}