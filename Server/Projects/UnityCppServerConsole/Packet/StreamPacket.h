#pragma once
#include "../Standard/Header.h"
#include "../Packet/Packet.h"

class StreamPacket
{
public:
	StreamPacket();
	virtual ~StreamPacket() = default;

public:
	void Truncate( const SOCKET& _socket, const WSABUF& _buf, DWORD _recvSize );

private:
	UPACKET* originPacket;
	byte recvBuffer[ RecvBufferMaxSize ];
	u_int writePos;
	u_int recvPos;
};

