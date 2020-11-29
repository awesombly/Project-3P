#pragma once
#include "StreamPacket.h"
#include "..\Standard\Protocol.h"
#include "..\Standard\Singleton.hpp"

class PacketManager : public Singleton<PacketManager>
{
public:
	PacketManager();
	virtual ~PacketManager() = default;

public:
	void Push( const PACKET& _packet );

private:
	void WorkPacket();

	void BindProtocols();

	static void Broadcast( const UPACKET& packet );
private:
	std::queue<PACKET> packets;
	std::condition_variable cv;
	std::mutex workMutex;

	std::unordered_map<u_short/*packetType*/, void( * )( const UPACKET& )> protocols;
};