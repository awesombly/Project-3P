#pragma once
#include "Acceptor\Acceptor.h"

class Server
{
public:
	Server();
	virtual ~Server();

public:
	void Initialize( const int _port, const char* _address = 0 );

private:
	Acceptor acceptor;
	HANDLE destroyEvent;
};
