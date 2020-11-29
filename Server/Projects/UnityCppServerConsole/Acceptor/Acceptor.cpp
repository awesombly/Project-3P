#include "Acceptor.h"
#include "..\Thread\IOCPManager.h"
#include "..\Session\SessionManager.h"
#include "..\Thread\ThreadPool.h"
#include "..\\Standard\Log.h"

Acceptor::Acceptor( const int _port, const char* _address )
{
	WSADATA wsa;
	::WSAStartup( MAKEWORD( 2, 2 ), &wsa );
	listenSocket = ::socket( AF_INET, SOCK_STREAM, 0 );

	if ( !SetSocketOption() )
	{
		Log::Instance().Push( "Socket Setting Fail.." );
		return;
	}
	Log::Instance().Push( "Socket Create Success : IPv4, TCP" );

	SOCKADDR_IN sa;
	ZeroMemory( &sa, sizeof( sa ) );
	sa.sin_family = AF_INET;
	if ( _address == nullptr )
	{
		sa.sin_addr.S_un.S_addr = ::htonl( INADDR_ANY );
	}
	else
	{
		sa.sin_addr.S_un.S_addr = ::inet_addr( _address );
	}
	sa.sin_port = ::htons( _port );

	if ( ::bind( listenSocket, ( sockaddr* )&sa, sizeof( sa ) ) == SOCKET_ERROR ||
		::listen( listenSocket, SOMAXCONN ) == SOCKET_ERROR )
	{
		Log::Instance().Push();
		return;
	}

	//ThreadPool::Instance().Enqueue( [&] () 
	//{
	//	Acceptor::ClientAccept();
	//} );

	// ����
	const std::future<void>& fut( std::async( std::launch::async, [&] () { ClientAccept(); } ) );
}

Acceptor::~Acceptor()
{
	::WSACleanup();
}

void Acceptor::ClientAccept()
{
	SOCKET clientsock;
	SOCKADDR_IN client{};
	int length( sizeof( client ) );
	while ( true )
	{
		// ���� ó���������� ������� ����ϰ� �ִ� ť���� ���� ó�� ����� ������ �����´�.
		clientsock = ::accept( listenSocket, ( sockaddr* )&client, &length );
		
		Session* session( new Session( clientsock, client ) );
		SessionManager::Instance().Push( session );
		IOCPManager::Instance().Bind( ( HANDLE )clientsock, ( ULONG_PTR )session );
	}
}

// ���� ���� ����
bool Acceptor::SetSocketOption()
{
	int optionValue( 1 );
	if ( ::setsockopt( listenSocket, SOL_SOCKET, SO_REUSEADDR, ( char* )&optionValue, sizeof( optionValue ) ) == SOCKET_ERROR )
	{
		Log::Instance().Push();
		return false;
	}

	char flag( 1 );
	if ( ::setsockopt( listenSocket, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof( char ) ) == SOCKET_ERROR )
	{
		Log::Instance().Push();
		return false;
	}

	int size( sizeof( int ) );
	linger optLinger;
	::getsockopt( listenSocket, SOL_SOCKET, SO_LINGER, ( char* )&optLinger, &size );
	optLinger.l_linger = 1000;
	optLinger.l_onoff = 1;
	::setsockopt( listenSocket, SOL_SOCKET, SO_LINGER, ( char* )&optLinger, sizeof( linger ) );

	int recvSize;
	int sendSize;
	size = sizeof( int );
	::getsockopt( listenSocket, SOL_SOCKET, SO_RCVBUF, ( char* )&recvSize, &size );
	::getsockopt( listenSocket, SOL_SOCKET, SO_SNDBUF, ( char* )&sendSize, &size );

	return true;
}
