#pragma once
#include "..\\Network\\Network.h"
#include "..\\Packet\\StreamPacket.h"

struct SessionData
{
	unsigned int key;
	std::string nickName;
	std::string id;
	std::string pw;
	std::string startDate;
	std::string lastConnectDate;
};

class Session : public Network
{
public:
	Session() = default;
	Session( const SOCKET& _socket, const SOCKADDR_IN& _address );
	virtual ~Session();

public:
	void Dispatch( const LPOVERLAPPED& _ov );

public:
	const SessionData& GetData() const;

private:
	StreamPacket stream;
	SessionData data;
};