#pragma once
#include "../Network/Network.h"
#include "../Packet/StreamPacket.h"

class Session : public Network
{
public:
	Session() = default;
	Session( const SOCKET& _socket, const SOCKADDR_IN& _address );
	virtual ~Session();

public:
	void Dispatch( const LPOVERLAPPED& _ov );

private:
	StreamPacket stream;
};