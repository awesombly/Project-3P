#include "PacketManager.h"
#include "..\Standard\Log.h"
#include "..\Session\SessionManager.h"
#include "..\Thread\ThreadPool.h"
#include "..\\DB\Database.h"

bool PacketManager::Initialize()
{
	BindProtocols();
	ThreadPool::Instance().Enqueue( [&] () { PacketManager::WorkPacket(); } );
	return true;
}

void PacketManager::WorkPacket()
{
	while ( true )
	{
		std::unique_lock<std::mutex> lock( workMutex );
		cv.wait( lock, [&] () { return !packets.empty(); } );

		PACKET* packet = std::move( &packets.front() );
		auto findItr = protocols.find( packet->packet.type );
		if ( findItr == protocols.cend() || findItr->second == nullptr )
		{
			Log::Instance().Push( ELogType::Warning, "Packet Not Bind : " + ToAnsi( ( char* )packet->packet.data ) );
			packets.pop();
			continue;
		}

		protocols[ packet->packet.type ]( *packet );
		packets.pop();
	}
}

void PacketManager::Push( const PACKET& _packet )
{
	std::lock_guard<std::mutex> lock( workMutex );
	packets.push( _packet );
	cv.notify_one();
}

void PacketManager::BindProtocols()
{
	protocols[Protocol::Type::ChatMessage] = &PacketManager::Broadcast;
	protocols[Protocol::Type::LoginReq] = &PacketManager::LoginCompareCheck;
}

void PacketManager::Broadcast( const PACKET& _packet )
{
	// 유니티 클라이언트에서 UTF-8 형식으로 인코딩한 후 전송되었기 때문에
	// Ansi 형식으로 디코딩 하여 올바른 문자열을 만듭니다.
	Log::Instance().Push( ELogType::Log, "Broadcast : " + _packet.packet.ToString() );
	SessionManager::Instance().BroadCast( _packet.packet );
}

void PacketManager::LoginCompareCheck( const PACKET& _packet )
{
	Session* session = SessionManager::Instance().Find( _packet.socket );
	
	UPACKET sendPacket;
	sendPacket.type = Protocol::Type::LoginAck;

	LoginInfo info = SimpleJsonToLoginInfo( ( char* )_packet.packet.data );
	if ( !Database::Instance().CompareID( info.ID ) || 
		 !Database::Instance().ComparePW( info.PW ) )
	{
		std::memcpy( sendPacket.data, ToUFT8( "아이디 또는 비밀번호를 다시 입력하십시오." ).c_str(), 2048 );
		sendPacket.length = sizeof( sendPacket.data ) + HeaderSize;
		
		session->Send( sendPacket );
		return;
	}

	std::memcpy( ( char* )sendPacket.data, ToUFT8( "로그인에 성공하셨습니다." ).c_str(), 2048 );
	sendPacket.length = sizeof( sendPacket.data ) + HeaderSize;
	session->Send( sendPacket );
}