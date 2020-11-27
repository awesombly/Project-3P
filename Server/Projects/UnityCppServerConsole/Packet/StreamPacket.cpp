#include "StreamPacket.h"
#include "PacketManager.h"
StreamPacket::StreamPacket() : packet( new UPACKET() ), recvBuffer{}, 
							   startPos( 0 ), writePos( 0 ), readPos( 0 )
{

}

void StreamPacket::Truncate( const WSABUF& _buf )
{
	// ���� ��Ŷ�� ũ�Ⱑ ���ۺ��� Ŭ ��
	if ( writePos + OneLineOfText > RecvBufferMaxSize )
	{
		char temp[RecvBufferMaxSize] = { 0, };
		::memcpy( temp, &recvBuffer[startPos], readPos );
		ZeroMemory( recvBuffer, sizeof( char ) * RecvBufferMaxSize );
		::memcpy( recvBuffer, temp, readPos );

		packet = ( UPACKET* )recvBuffer;
		writePos = readPos;
		startPos = 0;
	}

	// ū ���ۿ� ����
	::memcpy( &recvBuffer[writePos], _buf.buf, OneLineOfText );
	packet = ( UPACKET* )&recvBuffer[startPos];
	writePos += packet->length;
	readPos += packet->length;

	// �ϳ��� ��Ŷ�� ����� ���� ��
	if ( readPos >= packet->length )
	{
		do
		{
			PACKET newPacket;
			::memcpy( &newPacket, ( PACKET* )&recvBuffer[startPos], packet->length );

			PacketManager::Instance().Push( newPacket );
			startPos += packet->length;
			readPos -= packet->length;

			if ( readPos < HeaderSize ||
				 newPacket.packet.length == _buf.len )
			{
				break;
			}

			packet = ( UPACKET* )&recvBuffer[startPos];
		} while ( readPos >= packet->length );
	}
}