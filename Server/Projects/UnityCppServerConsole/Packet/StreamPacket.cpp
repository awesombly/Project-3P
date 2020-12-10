#include "StreamPacket.h"
#include "PacketManager.h"
#include "../Standard/Log.h"

StreamPacket::StreamPacket()
	: originPacket( new UPACKET() ), recvBuffer{}, writePos( 0 ), recvPos( 0 )
{

}

void StreamPacket::Truncate( const SOCKET& _socket, const WSABUF& _buf, DWORD _recvSize )
{
	::memcpy( &recvBuffer[writePos], _buf.buf, sizeof( char ) * _recvSize );
	writePos += _recvSize;

	recvPos = 0;
	originPacket = ( UPACKET* )&recvBuffer[ recvPos ];

	while ( writePos >= originPacket->length )
	{
		PACKET newPacket;
		newPacket.socket = _socket;
		// originPacket이 recvBuffer를 가리키는 포인터여서, 여러 패킷이 들어오면 뒤에 데이터까지 포함된다.
		// 하나의 패킷 length만큼만 복사한다.
		::memcpy( &newPacket.packet, originPacket, originPacket->length );
		PacketManager::Instance().Push( newPacket );

		writePos -= originPacket->length;
		recvPos += originPacket->length;

		originPacket = ( UPACKET* )&recvBuffer[ recvPos ];
		if ( originPacket->length <= 0 )
		{
			break;
		}
	}

	// 전체 메모리를 초기화할 필요는 없을거같다.
	/// TODO : 사용된 부분만 초기화하도록
	if ( writePos > 0 )
	{
		// 처리 안된 데이터는 앞으로 땡긴다.
		char temp[ RecvBufferMaxSize ] = { 0, };
		::memcpy( temp, &recvBuffer[ recvPos ], writePos );
		ZeroMemory( recvBuffer, sizeof( char ) * RecvBufferMaxSize );
		::memcpy( &recvBuffer, &temp, writePos );
		return;
	}

	ZeroMemory( recvBuffer, sizeof( char ) * RecvBufferMaxSize );
}
