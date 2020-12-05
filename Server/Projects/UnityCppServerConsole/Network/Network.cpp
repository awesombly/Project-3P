#include "Network.h"
#include "../Standard/Log.h"

Network::Network( const SOCKET& _socket, const SOCKADDR_IN& _address )
	: socket( _socket ), address( _address ), wsaBuffer{}, ov{}, buffer{}
{

}

Network::~Network()
{
	::WSACleanup();
}

const bool Network::Initialize( const int _port, const char* _ip )
{
	WSADATA wsa;
	int errorCode = ::WSAStartup( MAKEWORD( 2, 2 ), &wsa );
	if ( errorCode != 0 )
	{
		switch ( errorCode )
		{
			// WSAStartup 함수는 오류 코드를 직접 반환합니다.
			// WSAGetLastError에서 받을 수 없습니다.
			case WSASYSNOTREADY:
			{
				Log::Instance().Push( ELogType::Error, std::to_string( errorCode ) + " : 네트워크 통신에 대한 준비가 되지 않았습니다."_s );
			} break;
			case WSAVERNOTSUPPORTED:
			{
				Log::Instance().Push( ELogType::Error, std::to_string( errorCode ) + " : 요청된 윈도우 소켓지원버전은 제공되지 않습니다."_s );
			} break;
			case WSAEINPROGRESS:
			{
				Log::Instance().Push( ELogType::Error, std::to_string( errorCode ) + " : 차단 윈도우 소켓 1.1작업이 진행 중입니다."_s );
			} break;
			case WSAEPROCLIM:
			{
				Log::Instance().Push( ELogType::Error, std::to_string( errorCode ) + " : 윈도우 소켓 구현에서 지원하는 작업 수에 대한 제한에 도달했습니다."_s );
			} break;
			case WSAEFAULT:
			{
				Log::Instance().Push( ELogType::Error, std::to_string( errorCode ) + " : WSAData가 유효한 포인터가 아닙니다."_s );
			} break;
			default:
			{

			} break;
		}
	}

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
	address.sin_port = ::htons( ( u_short )_port );

	return true;
}

const bool Network::Connect() const
{
	if ( ::connect( socket, ( sockaddr* )&address, sizeof( address ) ) == SOCKET_ERROR )
	{
		Log::Instance().Push();
		ClosedSocket();

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
			Log::Instance().Push();
		}
	}
}

void Network::Send( const UPACKET& _packet ) const
{
	if ( ::send( socket, ( char* )&_packet, _packet.length, 0 ) == SOCKET_ERROR )
	{
		Log::Instance().Push();
	}
}

const SOCKET& Network::GetSocket() const
{
	return socket;
}

const std::string Network::GetAddressString() const
{
	return ::inet_ntoa( address.sin_addr );
}

const std::string Network::GetPortString() const
{
	return std::to_string( ::ntohs( address.sin_port ) );
}

const bool Network::ClosedSocket() const
{
	if ( ::closesocket( socket ) == SOCKET_ERROR )
	{
		Log::Instance().Push();

		return false;
	}

	return true;
}