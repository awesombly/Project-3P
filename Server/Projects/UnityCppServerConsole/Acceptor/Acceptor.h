#pragma once
#include "..\Session\Session.h"

class Acceptor
{
public:
	Acceptor( const int _port, const char* _address = 0 );
	virtual ~Acceptor();

public:
	void ClientAccept() const;

private:
	bool SetSocketOption() const;

private:
	SOCKET listenSocket;
};

