#include "Server.h"
#include "Thread/IOCPManager.h"
#include "Session/SessionManager.h"
#include "Packet/PacketManager.h"

Server::Server( const int _port, const char* _address ) : acceptor( _port, _address )
{
	destroyEvent = CreateEvent( NULL, FALSE, FALSE, _T( "Server" ) );

	// 임시 해결방안으로 미리 인스턴스를 생성해놓음.
	IOCPManager::Instance();
	SessionManager::Instance();
	PacketManager::Instance();

	::WaitForSingleObject( destroyEvent, INFINITE );
}

Server::~Server()
{
	 ::SetEvent( destroyEvent );
}