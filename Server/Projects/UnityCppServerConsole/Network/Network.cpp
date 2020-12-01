#include "Network.h"
#include "..\\Standard\Log.h"

Network::Network( const SOCKET& _socket, const SOCKADDR_IN& _address ) :
	socket( _socket ), address( _address )
{

}

Network::~Network()
{
	::WSACleanup();
}

bool Network::Initialize( const int _port, const char* _ip )
{
	WSADATA wsa;
	::WSAStartup( MAKEWORD( 2, 2 ), &wsa );
	socket = ::WSASocket( AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED );

	Log::Instance().Push( ELogType::Log, "Socket Generation Success" );

	ZeroMemory( &address, sizeof( address ) );
	address.sin_family = AF_INET;
	if ( _ip == nullptr )
	{
		address.sin_addr.S_un.S_addr = ::htonl( INADDR_ANY );
	}
	else
	{
		address.sin_addr.S_un.S_addr = ::inet_addr( _ip );
	}
	address.sin_port = ::htons( _port );

	return true;
}

bool Network::Connect()
{
	if ( ::connect( socket, ( sockaddr* )&address, sizeof( address ) ) == SOCKET_ERROR )
	{
		Log::Instance().Push();
		::closesocket( socket );
		return false;
	}
	Log::Instance().Push( ELogType::Log, "Connect Success : "_s + ::inet_ntoa( address.sin_addr ) + " : "_s + std::to_string( ::ntohs( address.sin_port ) ) );
	return true;
}

void Network::Recieve()
{
	DWORD flags = 0;
	DWORD transferred = 0;
	ov.flag = OVERLAPPEDEX::MODE_RECV;
	wsaBuffer.buf = ( char* )buffer;
	wsaBuffer.len = DataMaxSize + HeaderSize;
	if ( ::WSARecv( socket, &wsaBuffer, 1, &transferred, &flags, ( LPOVERLAPPED )&ov, NULL ) == SOCKET_ERROR )
	{
		if ( ::WSAGetLastError() != WSA_IO_PENDING )
		{
			Log::Instance().Push( ::WSAGetLastError() );
		}
	}
}

void Network::Send( const UPACKET& _packet )
{
	::send( socket, ( char* )&_packet, _packet.length, 0 );
	// ov.flag = OVERLAPPEDEX::MODE_SEND;
	// wsaBuffer.buf = ( char* )_packet.data;
	// wsaBuffer.len = _packet.length;
	// 
	// DWORD sendBytes = _packet.length;
	// if ( ::WSASend( socket, &wsaBuffer, 1, &sendBytes, 0, ( LPOVERLAPPED )&ov, NULL ) == SOCKET_ERROR )
	// {
	// 	if ( ::WSAGetLastError() != WSA_IO_PENDING )
	// 	{
	// 		Log::Instance().Push( ::WSAGetLastError() );
	// 	}
	// }
}

const SOCKET& Network::GetSocket() const
{
	return socket;
}
