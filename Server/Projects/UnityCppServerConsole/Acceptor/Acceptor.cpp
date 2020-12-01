#include "Acceptor.h"
#include "..\Thread\IOCPManager.h"
#include "..\Session\SessionManager.h"
#include "..\Thread\ThreadPool.h"
#include "..\\Standard\Log.h"

Acceptor::~Acceptor()
{
	::WSACleanup();
}

bool Acceptor::Initialize( const int _port, const char* _address )
{
	WSADATA wsa;
	::WSAStartup( MAKEWORD( 2, 2 ), &wsa );
	listenSocket = ::socket( AF_INET, SOCK_STREAM, 0 );
	Log::Instance().Push( ELogType::Log, "ListenSocket Generation Success" );

	if ( !SetSocketOption() )
	{
		Log::Instance().Push( ELogType::Warning, "ListenSocket Setup Failed" );
		return false;
	}
	Log::Instance().Push( ELogType::Log, "ListenSocket Setup Success" );

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

	if ( ::bind( listenSocket, ( sockaddr* )&sa, sizeof( sa ) ) == SOCKET_ERROR )
	{
		Log::Instance().Push();
		return false;
	}
	Log::Instance().Push( ELogType::Log, "ListenSocket Bind Success" );

	if ( ::listen( listenSocket, SOMAXCONN ) == SOCKET_ERROR )
	{
		Log::Instance().Push();
		return false;
	}
	Log::Instance().Push( ELogType::Log, "Connection Queue Generation Success" );

	ThreadPool::Instance().Enqueue( [&] () { Acceptor::ClientAccept(); } );

	return true;
}

void Acceptor::ClientAccept() const
{
	SOCKET clientsock;
	SOCKADDR_IN client {};
	int length = sizeof( client );
	while ( true )
	{
		// 아직 처리되지않은 연결들이 대기하고 있는 큐에서 제일 처음 연결된 소켓을 가져온다.
		clientsock = ::accept( listenSocket, ( sockaddr* )&client, &length );

		Session* session = new Session( clientsock, client );
		SessionManager::Instance().Push( session );
		IOCPManager::Instance().Bind( ( HANDLE )clientsock, ( ULONG_PTR )session );
	}
}

// 소켓 세부 설정
bool Acceptor::SetSocketOption() const
{
	int optionValue = 1;
	if ( ::setsockopt( listenSocket, SOL_SOCKET, SO_REUSEADDR, ( char* )&optionValue, sizeof( optionValue ) ) == SOCKET_ERROR )
	{
		Log::Instance().Push();
		return false;
	}

	char flag = 1;
	if ( ::setsockopt( listenSocket, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof( char ) ) == SOCKET_ERROR )
	{
		Log::Instance().Push();
		return false;
	}
	Log::Instance().Push( ELogType::Log, "Socket Option : TCP NoDelay" );

	linger optLinger;
	int size = sizeof( int );
	if ( ::getsockopt( listenSocket, SOL_SOCKET, SO_LINGER, ( char* )&optLinger, &size ) == SOCKET_ERROR )
	{
		Log::Instance().Push();
		return false;
	}

	optLinger.l_linger = 1000;
	optLinger.l_onoff = 1;
	if ( ::setsockopt( listenSocket, SOL_SOCKET, SO_LINGER, ( char* )&optLinger, sizeof( linger ) ) == SOCKET_ERROR )
	{
		Log::Instance().Push();
		return false;
	}

	int recvSize;
	int sendSize;
	size = sizeof( int );
	if ( ::getsockopt( listenSocket, SOL_SOCKET, SO_RCVBUF, ( char* )&recvSize, &size ) == SOCKET_ERROR ||
		 ::getsockopt( listenSocket, SOL_SOCKET, SO_SNDBUF, ( char* )&sendSize, &size ) == SOCKET_ERROR )
	{
		Log::Instance().Push();
		return false;
	}
	Log::Instance().Push( ELogType::Log, "Socket Option RecvSize : " + std::to_string( recvSize ) );
	Log::Instance().Push( ELogType::Log, "Socket Option SendSize : " + std::to_string( sendSize ) );

	return true;
}
