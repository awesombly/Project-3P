#pragma once
#include "../Network/Network.h"
#include "../Packet/StreamPacket.h"

class Stage;

struct LogicData
{
	ServerActor* Player;
	Stage* CurrentStage;
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
