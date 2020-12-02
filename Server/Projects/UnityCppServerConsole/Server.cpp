#include "Server.h"
#include "Standard/Log.h"
#include "DB/Database.h"
#include "Packet/PacketManager.h"
#include "Thread/IOCPManager.h"
#include "Thread/ThreadPool.h"

Server::Server()
{
	destroyEvent = CreateEvent( NULL, FALSE, FALSE, _T( "Server" ) );
}

Server::~Server()
{
	 ::SetEvent( destroyEvent );
}

void Server::Initialize( const int _port, const char* _address )
{
	if ( !Log::Instance().Initialize() )
	{
		Log::Instance().Push( ELogType::Warning, "Log Initialize Fail"_s );
	}

	//if ( !Database::Instance().Initialize() )
	//{
	//	Log::Instance().Push( ELogType::Warning, "Database Initialize Fail"_s );
	//}

	if ( !ThreadPool::Instance().Initialize() )
	{
		Log::Instance().Push( ELogType::Warning, "ThreadPool Initialize Fail"_s );
	}

	if ( !IOCPManager::Instance().Initialize() )
	{
		Log::Instance().Push( ELogType::Warning, "IOCPManager Initialize Fail"_s );
	}

	if ( !acceptor.Initialize( _port, _address ) ||
		 !acceptor.ListenStart() )
	{
		Log::Instance().Push( ELogType::Warning, "Client Lieten Fail"_s );
	}

	if ( !PacketManager::Instance().Initialize() )
	{
		Log::Instance().Push( ELogType::Warning, "PacketManager Initialize Fail"_s );
	}

	if ( !loginServer.Initialize( _port, "127.0.0.1" ) ||
		 !loginServer.Connect() )
	{
		Log::Instance().Push( ELogType::Warning, "LoginServer Connect Fail"_s );
	}

	::WaitForSingleObject( destroyEvent, INFINITE );
}