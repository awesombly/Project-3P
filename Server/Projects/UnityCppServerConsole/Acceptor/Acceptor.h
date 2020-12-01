#pragma once
#include "..\Session\Session.h"

class Acceptor
{
public:
	Acceptor() = default;
	virtual ~Acceptor();

public:
	bool Initialize( const int _port, const char* _address = 0 );

private:
	void ClientAccept() const;
	bool SetSocketOption() const;

private:
	SOCKET listenSocket;
};

