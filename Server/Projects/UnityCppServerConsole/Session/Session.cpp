#include "Session.h"
#include "..\Standard\Log.h"

Session::Session( const SOCKET& _socket, const SOCKADDR_IN& _address ) : Network( _socket, _address )
{

}

Session::~Session()
{
	::shutdown( socket, SD_SEND );
	::closesocket( socket );
}

void Session::Dispatch( const LPOVERLAPPED& _ov )
{
	OVERLAPPEDEX* ovEx = ( OVERLAPPEDEX* )_ov;
	if ( ovEx->flag == OVERLAPPEDEX::MODE_RECV )
	{
		streamPacket.Truncate( wsaBuffer );
		ZeroMemory( &wsaBuffer, sizeof( WSABUF ) );
	}

	Recieve();
}