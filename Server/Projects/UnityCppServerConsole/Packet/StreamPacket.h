#pragma once
#include "..\\Standard\\Protocol.h"

class StreamPacket
{
public:
	StreamPacket();
	virtual ~StreamPacket() = default;

public:
	void Truncate( const SOCKET& _socket, const WSABUF& _buf );

private:
	UPACKET* packet;
	byte recvBuffer[RecvBufferMaxSize];
	u_int startPos;
	u_int writePos;
	u_int readPos;
};

