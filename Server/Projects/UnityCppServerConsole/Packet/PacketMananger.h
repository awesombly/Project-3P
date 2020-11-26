#pragma once
#include "StreamPacket.h"
#include "..\Thread\Thread.h"
#include "..\Standard\Protocol.h"
#include "..\Standard\Singleton.hpp"

class PacketMananger : public Singleton<PacketMananger>, public Thread
{
public:
	PacketMananger();
	virtual ~PacketMananger() = default;

public:
	void Run() override;
	void Push( const PACKET& _packet );

private:
	std::queue<PACKET> packets;
};