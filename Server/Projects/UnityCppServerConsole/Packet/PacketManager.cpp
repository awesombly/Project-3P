#include "PacketManager.h"
#include "..\Standard\Log.h"
#include "..\Session\SessionManager.h"
#include "..\Thread\ThreadPool.h"

PacketManager::PacketManager()
{
	ThreadPool::Instance().Enqueue( [&] () { PacketManager::WorkPacket(); } );
}

void PacketManager::WorkPacket()
{
	while ( true )
	{
		std::unique_lock<std::mutex> lock( workMutex );
		cv.wait( lock, [this] () { return !packets.empty(); } );
		
		PACKET* packet = std::move( &packets.front() );
		if ( packet->packet.type == PACKET_TYPE::ChatMessage )
		{
			// ����Ƽ Ŭ���̾�Ʈ���� UTF-8 �������� ���ڵ��� �� ���۵Ǿ��� ������
			// Ansi �������� ���ڵ� �Ͽ� �ùٸ� ���ڿ��� ����ϴ�.
			// packet->packet�̶�.. �� �� ��Ȯ�� �̸��� �������ٸ� �ٲٰڽ��ϴ�.
			
			Log::Instance().Push( "Broadcast : " + ToAnsi( ( char* )packet->packet.data ) );
			SessionManager::Instance().BroadCast( packet->packet );
		}

		packets.pop();
	}
}

void PacketManager::Push( const PACKET& _packet )
{
	std::lock_guard<std::mutex> lock( workMutex );
	packets.push( _packet );
	cv.notify_one();
}