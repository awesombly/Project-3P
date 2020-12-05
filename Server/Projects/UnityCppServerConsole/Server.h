#pragma once
#include "Network/Acceptor.h"

class Server
{
public:
	Server();
	virtual ~Server() = default;

public:
	void Initialize( const int _port, const char* _address = 0 );

private:
	Acceptor acceptor;
	HANDLE killEvent;
};
