#pragma once
#include "..\Packet\StreamPacket.h"
#include "..\\Network\\Network.h"

class Session : public Network
{
public:
	Session( const SOCKET& _socket, const SOCKADDR_IN& _address );
	virtual ~Session();

public:
	void Dispatch( const LPOVERLAPPED& _ov );

private:
	friend class SessionManager;
	StreamPacket streamPacket;
};

