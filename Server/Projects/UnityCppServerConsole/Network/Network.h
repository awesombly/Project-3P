#pragma once
#include "../Standard/Header.h"
#include "../Packet/Packet.h"

class Network
{
public:
	Network() = default;
	Network( const SOCKET& _socket, const SOCKADDR_IN& _address );
	virtual ~Network();

public:
<<<<<<< Updated upstream
	bool Initialize( const int _port, const char* _ip );
	bool Connect() const;
=======
	const bool Initialize( const int _port, const char* _ip );
	const bool Connect() const;
>>>>>>> Stashed changes

	void Recieve();
	void Send( const UPACKET& _packet ) const;

public:
	const SOCKET& GetSocket() const;
	const std::string GetAddressString() const;
	const std::string GetPortString() const;

protected:
	const bool ClosedSocket() const;

protected:
	bool ClosedSocket() const;

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
	WSABUF wsaBuffer;

private:
	OVERLAPPEDEX ov;
	char buffer[ DataMaxSize + HeaderSize ];
};

