#include "Session.h"

Session::Session( const SOCKET& _socket, const SOCKADDR_IN& _address ) :
	socket( _socket ), address( _address )
{

}

Session::~Session()
{
	::shutdown( socket, SD_SEND );
	::closesocket( socket );
}

void Session::WaitForPacketRecv()
{
	DWORD transferred;
	DWORD flags( 0 );
	ov.flag = OVERLAPPEDEX::MODE_RECV;
	wsaBuffer.buf = buffer;
	wsaBuffer.len = 2048;
	if ( ::WSARecv( socket, &wsaBuffer, 1, &transferred, &flags, reinterpret_cast< LPOVERLAPPED >( &ov ), NULL ) == SOCKET_ERROR )
	{
		if ( ::WSAGetLastError() != WSA_IO_PENDING )
		{
			// some..
		}
	}
}

void Session::Dispatch( const LPOVERLAPPED& _ov )
{
	OVERLAPPEDEX* ovEx( reinterpret_cast< OVERLAPPEDEX* >( _ov ) );
	if ( ovEx->flag == OVERLAPPEDEX::MODE_RECV )
	{
		streamPacket.Truncate( wsaBuffer );
		ZeroMemory( &wsaBuffer, sizeof( wsaBuffer ) );
	}

	WaitForPacketRecv();
}