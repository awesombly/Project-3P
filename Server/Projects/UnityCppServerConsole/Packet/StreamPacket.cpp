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
		// originPacket�� recvBuffer�� ����Ű�� �����Ϳ���, ���� ��Ŷ�� ������ �ڿ� �����ͱ��� ���Եȴ�.
		// �ϳ��� ��Ŷ length��ŭ�� �����Ѵ�.
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

	// ��ü �޸𸮸� �ʱ�ȭ�� �ʿ�� �����Ű���.
	/// TODO : ���� �κи� �ʱ�ȭ�ϵ���
	if ( writePos > 0 )
	{
		// ó�� �ȵ� �����ʹ� ������ �����.
		char temp[ RecvBufferMaxSize ] = { 0, };
		::memcpy( temp, &recvBuffer[ recvPos ], writePos );
		ZeroMemory( recvBuffer, sizeof( char ) * RecvBufferMaxSize );
		::memcpy( &recvBuffer, &temp, writePos );
		return;
	}

	ZeroMemory( recvBuffer, sizeof( char ) * RecvBufferMaxSize );
}
