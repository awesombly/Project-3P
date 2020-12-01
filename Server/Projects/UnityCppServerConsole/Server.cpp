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
#pragma region Log
	Log::Instance().Push( ELogType::Log, "Log Initialize Start"_s );
	if ( !Log::Instance().Initialize() )
	{
		Log::Instance().Push( ELogType::Warning, "Log Initialize Fail"_s );
	}
	Log::Instance().Push( ELogType::Log, "Log Initialize Success"_s );
#pragma endregion

#pragma region Database
	Log::Instance().Push( ELogType::Log, "Database Initialize Start"_s );
	if ( !Database::Instance().Initialize() )
	{
		Log::Instance().Push( ELogType::Warning, "Database Initialize Fail"_s );
	}
	Log::Instance().Push( ELogType::Log, "Database Initialize Success"_s );
#pragma endregion

#pragma region ThreadPool
	Log::Instance().Push( ELogType::Log, "ThreadPool Initialize Start"_s );
	if ( !ThreadPool::Instance().Initialize() )
	{
		Log::Instance().Push( ELogType::Warning, "ThreadPool Initialize Fail"_s );
	}
	Log::Instance().Push( ELogType::Log, "ThreadPool Initialize Success"_s );
#pragma endregion

#pragma region IOCPManager
	Log::Instance().Push( ELogType::Log, "IOCPManager Initialize Start"_s );
	if ( !IOCPManager::Instance().Initialize() )
	{
		Log::Instance().Push( ELogType::Warning, "IOCPManager Initialize Fail"_s );
	}
	Log::Instance().Push( ELogType::Log, "IOCPManager Initialize Success"_s );
#pragma endregion

#pragma region Acceptor
	Log::Instance().Push( ELogType::Log, "Acceptor Initialize Start"_s );
	if ( !acceptor.Initialize( _port, _address ) )
	{
		Log::Instance().Push( ELogType::Warning, "Acceptor Initialize Fail"_s );
	}
	Log::Instance().Push( ELogType::Log, "Acceptor Initialize Success"_s );
#pragma endregion

#pragma region PacketManager
	Log::Instance().Push( ELogType::Log, "PacketManager Initialize Start"_s );
	if ( !PacketManager::Instance().Initialize() )
	{
		Log::Instance().Push( ELogType::Warning, "PacketManager Initialize Fail"_s );
	}
	Log::Instance().Push( ELogType::Log, "PacketManager Initialize Success"_s );
#pragma endregion

	::WaitForSingleObject( destroyEvent, INFINITE );
}