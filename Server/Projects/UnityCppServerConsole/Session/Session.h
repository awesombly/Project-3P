#pragma once
#include "../Network/Network.h"
#include "../Packet/StreamPacket.h"
<<<<<<< Updated upstream
=======

struct SessionData
{
	unsigned int Key;
	std::string NickName;
	std::string Id;
	std::string Pw;
	std::string StartDate;
	std::string LastConnectDate;
};
>>>>>>> Stashed changes

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