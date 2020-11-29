#include "Server.h"

Server::Server( const int _port, const char* _address ) : acceptor( _port, _address )
{
	::WaitForSingleObject( destroyEvent, INFINITE );
}
