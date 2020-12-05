#pragma once
#include "../Network/Network.h"
#include "../Packet/StreamPacket.h"

struct SessionData
{
	unsigned int Key;
	std::string NickName;
	std::string Id;
	std::string Pw;
	std::string StartDate;
	std::string LastConnectDate;
};

struct LogicData
{
	std::shared_ptr<ServerObject> Player;
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
	const SessionData& GetSessionData() const;

	LogicData logicData;

private:
	StreamPacket stream;
	SessionData sessionData;
};
