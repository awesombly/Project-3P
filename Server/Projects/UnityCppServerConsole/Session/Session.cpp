#include "Session.h"
#include "..\Standard\Log.h"

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
	if ( ::WSARecv( socket, &wsaBuffer, 1, &transferred, &flags, ( LPOVERLAPPED )&ov, NULL ) == SOCKET_ERROR )
	{
		int iError = WSAGetLastError();
		if ( ::WSAGetLastError() != WSA_IO_PENDING )
		{
			Log::Instance().Push( iError );
		}
	}
}

// 스트림패킷으로 보내는 메서드
void Session::Dispatch( const LPOVERLAPPED& _ov )
{
	OVERLAPPEDEX* ovEx( ( OVERLAPPEDEX* )_ov );
	if ( ovEx->flag == OVERLAPPEDEX::MODE_RECV )
	{
		streamPacket.Truncate( wsaBuffer );
		ZeroMemory( &wsaBuffer, sizeof( WSABUF ) );
	}

	// 보내고 나면 다시 Recv로 대기.
	WaitForPacketRecv();
}

const SOCKET& Session::GetSocket()
{
	return socket;
}