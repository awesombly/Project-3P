#include "Server.h"
#include "Standard/Log.h"
#include "Packet/PacketManager.h"
#include "Network/IOCP/IOCPManager.h"

Server::Server() 
{
	killEvent = ::CreateEvent( NULL, FALSE, FALSE, _T( "ServerKillEvent" ) );
}

void Server::Initialize( const int _port, const char* _address )
{
	if ( !Log::Instance().Initialize() )
	{
		Log::Instance().Push( ELogType::Warning, LOGFUNC( "Log Initialize Fail"_s ) );
	}

	if ( !IOCPManager::Instance().Initialize() )
	{
		Log::Instance().Push( ELogType::Warning, LOGFUNC( "IOCPManager Initialize Fail"_s ) );
	}

	if ( !acceptor.Initialize( _port, _address ) ||
		 !acceptor.ListenStart() )
	{
		Log::Instance().Push( ELogType::Warning, LOGFUNC( "Client Lieten Fail"_s ) );
	}

	if ( !PacketManager::Instance().Initialize() )
	{
		Log::Instance().Push( ELogType::Warning, LOGFUNC( "PacketManager Initialize Fail"_s ) );
	}

	if ( ::WaitForSingleObject( killEvent, INFINITE ) == WAIT_FAILED )
	{
		Log::Instance().Push( ELogType::Warning, LOGFUNC( "KillEvent Wait Fail"_s ) );
	}
}