#pragma once
#include "../Network/Network.h"
#include "../Packet/StreamPacket.h"

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
	void Dispatch( const LPOVERLAPPED& _ov, DWORD _byteSize );

public:
	LogicData logicData;

private:
	StreamPacket stream;
};
