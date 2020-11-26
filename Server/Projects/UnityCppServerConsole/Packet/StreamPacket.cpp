#include "StreamPacket.h"
#include "PacketMananger.h"
StreamPacket::StreamPacket() : packet( new UPACKET() ), recvBuffer{}, 
							   startPos( 0 ), writePos( 0 ), readPos( 0 )
{

}

void StreamPacket::Truncate( const WSABUF& _buf )
{
	// 들어온 패킷의 크기가 버퍼보다 클 때
	if ( writePos + PROTOCOL::OneLineOfText > PROTOCOL::RecvMaxSize )
	{
		char temp[PROTOCOL::RecvMaxSize] = { 0, };
		::memcpy( temp, &recvBuffer[startPos], readPos );
		ZeroMemory( recvBuffer, sizeof( char ) * PROTOCOL::RecvMaxSize );
		::memcpy( recvBuffer, temp, readPos );

		packet = reinterpret_cast< UPACKET* >( recvBuffer );
		writePos = readPos;
		startPos = 0;
	}

	// 큰 버퍼에 저장
	::memcpy( &recvBuffer[writePos], _buf.buf, PROTOCOL::OneLineOfText );
	packet = reinterpret_cast< UPACKET* >( &recvBuffer[startPos] );
	writePos += packet->length;
	readPos += packet->length;

	// 하나의 패킷이 만들어 졌을 때
	if ( readPos >= packet->length )
	{
		do
		{
			PACKET newPacket;
			::memcpy( &newPacket, reinterpret_cast< PACKET* >( &recvBuffer[startPos] ), packet->length );

			PacketMananger::Instance().Push( newPacket );
			startPos += packet->length;
			readPos -= packet->length;

			if ( readPos < PROTOCOL::HeaderSize ||
				 newPacket.packet.length == _buf.len )
			{
				break;
			}

			packet = reinterpret_cast< UPACKET* >( &recvBuffer[startPos] );
		} while ( readPos >= packet->length );
	}
}