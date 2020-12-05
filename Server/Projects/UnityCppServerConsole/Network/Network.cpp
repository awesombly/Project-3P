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
			// WSAStartup �Լ��� ���� �ڵ带 ���� ��ȯ�մϴ�.
			// WSAGetLastError���� ���� �� �����ϴ�.
			case WSASYSNOTREADY:
			{
<<<<<<< Updated upstream
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
			default:
			{

			} break;
		}
	}

	socket = ::WSASocket( AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED );
	Log::Instance().Push( ELogType::Log, LOGFUNC( "Socket Generation Success"_s ) );
=======
				Log::Instance().Push( ELogType::Error, "��Ʈ��ũ ��ſ� ���� �غ� ���� �ʾҽ��ϴ�."_s );
			} break;
			case WSAVERNOTSUPPORTED:
			{
				Log::Instance().Push( ELogType::Error, "��û�� ������ �������������� �������� �ʽ��ϴ�."_s );
			} break;
			case WSAEINPROGRESS:
			{
				Log::Instance().Push( ELogType::Error, "���� ������ ���� 1.1�۾��� ���� ���Դϴ�."_s );
			} break;
			case WSAEPROCLIM:
			{
				Log::Instance().Push( ELogType::Error, "������ ���� �������� �����ϴ� �۾� ���� ���� ���ѿ� �����߽��ϴ�."_s );
			} break;
			case WSAEFAULT:
			{
				Log::Instance().Push( ELogType::Error, "WSAData�� ��ȿ�� �����Ͱ� �ƴմϴ�."_s );
			} break;
			default:
			{

			} break;
		}
	}

	socket = ::WSASocket( AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED );
	Log::Instance().Push( ELogType::Log, "Socket Generation Success" );
>>>>>>> Stashed changes

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

<<<<<<< Updated upstream
bool Network::Connect() const
=======
const bool Network::Connect() const
>>>>>>> Stashed changes
{
	if ( ::connect( socket, ( sockaddr* )&address, sizeof( address ) ) == SOCKET_ERROR )
	{
		Log::Instance().Push();
		ClosedSocket();

		return false;
	}
<<<<<<< Updated upstream
	Log::Instance().Push( ELogType::Log, LOGFUNC( "Connect Success : "_s + ::inet_ntoa( address.sin_addr ) + " : "_s + std::to_string( ::ntohs( address.sin_port ) ) ) );
=======
	Log::Instance().Push( ELogType::Log, "Connect Success : "_s + ::inet_ntoa( address.sin_addr ) + " : "_s + std::to_string( ::ntohs( address.sin_port ) ) );
>>>>>>> Stashed changes

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

<<<<<<< Updated upstream
std::string Network::GetAddressString() const
=======
const std::string Network::GetAddressString() const
>>>>>>> Stashed changes
{
	return ::inet_ntoa( address.sin_addr );
}

<<<<<<< Updated upstream
std::string Network::GetPortString() const
=======
const std::string Network::GetPortString() const
>>>>>>> Stashed changes
{
	return std::to_string( ::ntohs( address.sin_port ) );
}

<<<<<<< Updated upstream
bool Network::ClosedSocket() const
=======
const bool Network::ClosedSocket() const
>>>>>>> Stashed changes
{
	if ( ::closesocket( socket ) == SOCKET_ERROR )
	{
		Log::Instance().Push();

		return false;
	}

	return true;
}