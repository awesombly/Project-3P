#pragma once
#include "Acceptor/Acceptor.h"

class Server : public Thread
{
public:
	Server( const int _port, const char* _address = 0 );
	virtual ~Server();

public:
	void ExecuteThread() override {}

private:
	Acceptor acceptor;
	HANDLE destroyEvent;
};
