#pragma once
#include "StreamPacket.h"
#include "..\Standard\Protocol.h"
#include "..\Standard\Singleton.hpp"

class PacketManager : public Singleton<PacketManager>
{
public:
	PacketManager() = default;
	virtual ~PacketManager() = default;

public:
	bool Initialize();
	void Push( const PACKET& _packet );

private:
	void WorkPacket();

	void BindProtocols();

	static void Broadcast( const UPACKET& _packet );
private:
	std::queue<PACKET> packets;
	std::condition_variable cv;
	std::mutex workMutex;

	std::unordered_map<u_short/*packetType*/, std::function<void( const UPACKET& )>> protocols;
};