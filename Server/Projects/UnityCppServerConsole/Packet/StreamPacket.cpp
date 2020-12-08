#include "StreamPacket.h"
#include "PacketManager.h"
#include "../Standard/Log.h"

StreamPacket::StreamPacket()
	: originPacket( new UPACKET() ), packet{}, recvBuffer{}, writePos( 0 ), recvPos( 0 )
{

}

void StreamPacket::Truncate( const SOCKET& _socket, const WSABUF& _buf, DWORD _recvSize )
{
	
	::memcpy( &recvBuffer[writePos], _buf.buf, sizeof( char ) * _recvSize );
	writePos += _recvSize;

	originPacket = ( UPACKET* )recvBuffer;

	if ( writePos >= originPacket->length )
	{
		do {
			::memcpy( &packet, recvBuffer, originPacket->length );

			PACKET newPacket;
			newPacket.packet = packet;
			newPacket.socket = _socket;
			PacketManager::Instance().Push( newPacket );

			ZeroMemory( &packet, sizeof( UPACKET ) );

			writePos -= originPacket->length;

			if ( newPacket.packet.length == _recvSize )
			{
				break;
			}

			recvPos += newPacket.packet.length;
			originPacket = ( UPACKET* )&recvBuffer[recvPos];

		} while ( writePos >= originPacket->length );

		char temp[RecvBufferMaxSize] = { 0, };
		::memcpy( temp, &recvBuffer[recvPos], writePos );
		ZeroMemory( recvBuffer, sizeof( char ) * RecvBufferMaxSize );
		::memcpy( &recvBuffer, &temp, writePos );
		originPacket = ( UPACKET* )recvBuffer;
		recvPos = 0;
	}
	else
	{
		Log::Instance().Push( ELogType::Log, "WritePos : "_s + std::to_string( writePos ) );
	}
}
