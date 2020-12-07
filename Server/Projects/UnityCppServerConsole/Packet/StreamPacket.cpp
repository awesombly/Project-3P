#include "StreamPacket.h"
#include "PacketManager.h"

StreamPacket::StreamPacket()
	: packet( new UPACKET() ), recvBuffer{}, startPos( 0 ), writePos( 0 ), readPos( 0 )
{

}

void StreamPacket::Truncate( const SOCKET& _socket, const WSABUF& _buf )
{
	while ( true )
	{
		// 들어온 패킷의 크기가 버퍼보다 클 때
		if ( writePos + OneLineOfText > RecvBufferMaxSize )
		{
			char temp[ RecvBufferMaxSize ] = { 0, };
			::memcpy( temp, &recvBuffer[ startPos ], readPos );
			ZeroMemory( recvBuffer, sizeof( char ) * RecvBufferMaxSize );
			::memcpy( recvBuffer, temp, readPos );

			packet = ( UPACKET* )recvBuffer;
			writePos = readPos;
			startPos = 0;
		}

		// 큰 버퍼에 저장
		::memcpy_s( &recvBuffer[ writePos ], RecvBufferMaxSize - writePos, &_buf.buf[ readPos ], OneLineOfText );
		packet = ( UPACKET* )&recvBuffer[ startPos ];
		if ( packet->length == 0 )
		{
			return;
		}

		const u_short copiedLength = min( packet->length - ( u_short )readPos, OneLineOfText );
		writePos += copiedLength;
		readPos += copiedLength;

		/// TODO: 패킷 하나씩만 처리중인데, _buf 다 읽고서 처리하도록
		// 하나의 패킷이 만들어 졌을 때
		if ( readPos >= packet->length )
		{
			break;
		}
	}

	do
	{
		PACKET newPacket;
		::memcpy( &newPacket, ( PACKET* )&recvBuffer[ startPos ], packet->length );

		newPacket.socket = _socket;
		PacketManager::Instance().Push( newPacket );
		startPos += packet->length;
		readPos -= packet->length;

		if ( readPos < HeaderSize || newPacket.packet.length == _buf.len )
		{
			break;
		}

		packet = ( UPACKET* )&recvBuffer[ startPos ];
	} while ( readPos >= packet->length );
}
