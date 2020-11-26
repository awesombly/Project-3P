#pragma once
#include "..\Synchronize\Synchronize.h"
#include "..\Packet\StreamPacket.h"

struct OVERLAPPEDEX
{
	OVERLAPPEDEX() : flag( MODE_RECV ) { }
	enum : char 
	{ 
		MODE_RECV = 0, 
		MODE_SEND = 1, 
	};

	u_int flag;
};

class Session
{
public:
	Session( const SOCKET& _socket, const SOCKADDR_IN& _address );
	virtual ~Session();

public:
	void WaitForPacketRecv();
	void Dispatch( const LPOVERLAPPED& _ov );

private:
	SOCKET socket;
	SOCKADDR_IN address;
	OVERLAPPEDEX ov;
	WSABUF wsaBuffer;
	char buffer[2048];
	StreamPacket streamPacket;
};

