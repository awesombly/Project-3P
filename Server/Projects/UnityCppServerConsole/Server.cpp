#include "Server.h"
#include "Thread/IOCPManager.h"
#include "Session/SessionManager.h"
#include "Packet/PacketManager.h"

Server::Server( const int _port, const char* _address ) : acceptor( _port, _address )
{
	destroyEvent = CreateEvent( NULL, FALSE, FALSE, _T( "Server" ) );
	::WaitForSingleObject( destroyEvent, INFINITE );
}

Server::~Server()
{
	 ::SetEvent( destroyEvent );
}