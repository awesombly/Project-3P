#include "Server.h"
#include "Standard/Log.h"
#include "Packet/PacketManager.h"
#include "Network/IOCP/IOCPManager.h"
#include "DB/Database.h"

Server::Server() 
{
	killEvent = ::CreateEvent( NULL, FALSE, FALSE, _T( "ServerKillEvent" ) );
}

void Server::Initialize( const int _port, const char* _address )
{
	if ( !Log::Instance().Initialize() )
	{
		LOG_WARNING << "Log Initialize Fail" << ELogType::EndLine;
	}

	//if ( !Database::Instance().Initialize() )
	//{
	//	LOG_WARNING << "Database Initialize Fail" << ELogType::EndLine;
	//}
	//Infomation info = Database::Instance().GetUserInfomation( 7 );

	if ( !IOCPManager::Instance().Initialize() )
	{
		LOG_WARNING << "IOCPManager Initialize Fail" << ELogType::EndLine;
	}

	if ( !acceptor.Initialize( _port, _address ) || !acceptor.ListenStart() )
	{
		LOG_WARNING << "Client Lieten Fail" << ELogType::EndLine;
	}

	if ( !PacketManager::Instance().Initialize() )
	{
		LOG_WARNING << "PacketManager Initialize Fail" << ELogType::EndLine;
	}

	if ( ::WaitForSingleObject( killEvent, INFINITE ) == WAIT_FAILED )
	{
		LOG_WARNING << "KillEvent Wait Failed" << ELogType::EndLine;
	}
}