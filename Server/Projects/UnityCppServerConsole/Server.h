#pragma once
#include "Acceptor\Acceptor.h"

class Server
{
public:
	Server( const int _port, const char* _address = 0 );
	virtual ~Server();

private:
	Acceptor acceptor;
	HANDLE destroyEvent;
};
