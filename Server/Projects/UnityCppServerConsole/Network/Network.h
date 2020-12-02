#pragma once
#include "..\\Standard\Header.h"
#include "..\\Packet\Packet.h"

class Network
{
public:
	Network() = default;
	Network( const SOCKET& _socket, const SOCKADDR_IN& _address );
	virtual ~Network();

public:
	bool Initialize( const int _port, const char* _ip );
	bool Connect();

	void Recieve();
	void Send( const UPACKET& _packet );

public:
	const SOCKET& GetSocket() const;
	std::string GetAddressString() const;
	std::string GetPortString() const;

protected:
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

protected:
	SOCKET socket;
	SOCKADDR_IN address;
	OVERLAPPEDEX ov;
	WSABUF wsaBuffer;
	char buffer[DataMaxSize + HeaderSize];
};

