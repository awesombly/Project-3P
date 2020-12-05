#include "Network.h"
#include "../Standard/Log.h"

Network::Network( const SOCKET& _socket, const SOCKADDR_IN& _address )
	: socket( _socket ), address( _address ), wsaBuffer{}, ov{}, buffer{}
{

}

bool Network::Initialize( const int _port, const char* _ip )
{
	WSADATA wsa;
	int errorCode = ::WSAStartup( MAKEWORD( 2, 2 ), &wsa );
	if ( errorCode != 0 )
	{
		switch ( errorCode )
		{
			case WSASYSNOTREADY:
			{
				Log::Instance().Push( ELogType::Error, LOGFUNC( std::to_string( errorCode ) + " : ��Ʈ��ũ ��ſ� ���� �غ� ���� �ʾҽ��ϴ�."_s ) );
			} break;
			case WSAVERNOTSUPPORTED:
			{
				Log::Instance().Push( ELogType::Error, LOGFUNC( std::to_string( errorCode ) + " : ��û�� ��������� ���������� �������� �ʽ��ϴ�."_s ) );
			} break;
			case WSAEINPROGRESS:
			{
				Log::Instance().Push( ELogType::Error, LOGFUNC( std::to_string( errorCode ) + " : ��������� 1.1�۾��� ���� ���Դϴ�."_s ) );
			} break;
			case WSAEPROCLIM:
			{
				Log::Instance().Push( ELogType::Error, LOGFUNC( std::to_string( errorCode ) + " : ��������� �������� �����ϴ� �۾� ���� ���ѿ� �����߽��ϴ�."_s ) );
			} break;
			case WSAEFAULT:
			{
				Log::Instance().Push( ELogType::Error, LOGFUNC( std::to_string( errorCode ) + " : WSAData�� ��ȿ���� �ʽ��ϴ�."_s ) );
			} break;
		}
	}

	socket = ::socket( AF_INET, SOCK_STREAM, 0 );
	Log::Instance().Push( ELogType::Log, LOGFUNC( "Socket Generation Success" ) );

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

bool Network::Connect() const
{
	if ( ::connect( socket, ( sockaddr* )&address, sizeof( address ) ) == SOCKET_ERROR )
	{
		Log::Instance().Push();
		ClosedSocket();

		return false;
	}
	Log::Instance().Push( ELogType::Log, LOGFUNC( "Connect Success : "_s + ::inet_ntoa( address.sin_addr ) + " : "_s + std::to_string( ::ntohs( address.sin_port ) ) ) );

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

std::string Network::GetAddressString() const
{
	return ::inet_ntoa( address.sin_addr );
}

std::string Network::GetPortString() const
{
	return std::to_string( ::ntohs( address.sin_port ) );
}

bool Network::ClosedSocket() const
{
	if ( ::closesocket( socket ) == SOCKET_ERROR )
	{
		Log::Instance().Push();

		return false;
	}

	return true;
}