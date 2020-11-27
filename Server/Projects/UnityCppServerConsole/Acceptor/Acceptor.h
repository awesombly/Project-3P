#pragma once
#include "..\Thread\Thread.h"
#include "..\Session\Session.h"

class Acceptor : public Thread
{
public:
	Acceptor( const int _port, const char* _address = 0 );
	virtual ~Acceptor();

public:
	void ExecuteThread() override;
	bool SetSocketOption();

private:
	SOCKET listenSocket;
};

