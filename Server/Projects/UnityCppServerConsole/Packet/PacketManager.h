#pragma once
#include "StreamPacket.h"
#include "../Packet/Packet.h"
#include "../Standard/Singleton.hpp"

class PacketManager : public Singleton<PacketManager>
{
public:
	PacketManager() = default;
	virtual ~PacketManager() = default;

public:
	bool Initialize();
	void Push( const PACKET& _packet );

private:
	void PacketProcess();

	void BindProtocols();

	static void Broadcast( const PACKET& _packet );
	static void BroadcastToStage( const PACKET& _packet );
	static void BroadCastExceptSelf( const PACKET& _packet );
	static void BroadCastExceptSelfToStage( const PACKET& _packet );

	/// 프로토콜 처리 부분은 별도로 분리하는게 좋을듯
	static void SyncTransform( const PACKET& _packet );
	static void SyncInterpolation( const PACKET& _packet );
	static void EnterStage( const PACKET& _packet );

private:
	std::queue<PACKET> packets;
	std::condition_variable cv;
	std::mutex packetsMutex;

	std::unordered_map<PacketType, std::function<void( const PACKET& )>> protocols;
};