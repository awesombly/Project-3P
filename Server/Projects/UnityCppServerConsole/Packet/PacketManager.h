#pragma once
#include "StreamPacket.h"
#include "..\Thread\Thread.h"
#include "..\Standard\Protocol.h"
#include "..\Standard\Singleton.hpp"

class PacketManager : public Thread, public Singleton<PacketManager>
{
public:
	PacketManager();
	virtual ~PacketManager() = default;

public:
	void ExecuteThread() override;
	void Push( const PACKET& _packet );

private:
	std::queue<PACKET> packets;
};