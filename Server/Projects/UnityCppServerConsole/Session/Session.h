#pragma once
#include "..\Packet\StreamPacket.h"

class Session
{
public:
	Session( const SOCKET& _socket, const SOCKADDR_IN& _address );
	virtual ~Session();

public:
	void WaitForPacketRecv();
	void Dispatch( const LPOVERLAPPED& _ov );
	const SOCKET& GetSocket() const;

private:
	struct OVERLAPPEDEX : OVERLAPPED
	{
		OVERLAPPEDEX() : flag( MODE_RECV ) { }
		enum : char
		{
			MODE_RECV = 0,
			MODE_SEND = 1,
		};

		u_int flag;
	};

private:
	friend class SessionManager;
	SOCKET socket;
	SOCKADDR_IN address;
	OVERLAPPEDEX ov;
	WSABUF wsaBuffer;
	StreamPacket streamPacket;
	char buffer[DataMaxSize + HeaderSize];
};

