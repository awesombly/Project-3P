#pragma once
#include "Network\Acceptor.h"

class Server
{
public:
	Server() = default;
	virtual ~Server() = default;

public:
	void Initialize( const int _port, const char* _address = 0 );

private:
	Acceptor acceptor;
	Network loginServer;
};
