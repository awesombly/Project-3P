#pragma once
#include "..\Session\Session.h"

class Acceptor
{
public:
	Acceptor( const int _port, const char* _address = 0 );
	virtual ~Acceptor();

public:
	void ClientAccept();

private:
	bool SetSocketOption();

private:
	SOCKET listenSocket;
};

