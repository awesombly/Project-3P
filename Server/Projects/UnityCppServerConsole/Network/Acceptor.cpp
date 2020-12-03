#include "Acceptor.h"
#include "..\Network\\IOCP\\IOCPManager.h"
#include "..\Session\SessionManager.h"
#include "..\\Standard\Log.h"


bool Acceptor::ListenStart()
{
	if ( !SetSocketOption() )
	{
		Log::Instance().Push( ELogType::Warning, "Socket Setup Failed" );
		::closesocket( socket );
		return false;
	}

	if ( ::bind( socket, ( sockaddr* )&address, sizeof( address ) ) == SOCKET_ERROR )
	{
		Log::Instance().Push();
		::closesocket( socket );
		return false;
	}

	if ( ::listen( socket, SOMAXCONN ) == SOCKET_ERROR )
	{
		Log::Instance().Push();
		::closesocket( socket );
		return false;
	}

	std::thread th( [&] () { Acceptor::WaitForClients(); } );
	th.detach();
	return true;
}



void Acceptor::WaitForClients() const
{
	SOCKET clientsock;
	SOCKADDR_IN client {};
	int length = sizeof( client );
	while ( true )
	{
		// 아직 처리되지않은 연결들이 대기하고 있는 큐에서 제일 처음 연결된 소켓을 가져온다.
		clientsock = ::WSAAccept( socket, ( sockaddr* )&client, &length, NULL, NULL );
		Session* session = new Session( clientsock, client );
		SessionManager::Instance().Push( session );
		IOCPManager::Instance().Bind( ( HANDLE )clientsock, ( ULONG_PTR )session );
	}
}

// 소켓 세부 설정
bool Acceptor::SetSocketOption() const
{
	int optionValue = 1;
	if ( ::setsockopt( socket, SOL_SOCKET, SO_REUSEADDR, ( char* )&optionValue, sizeof( optionValue ) ) == SOCKET_ERROR )
	{
		Log::Instance().Push();
		return false;
	}

	char flag = 1;
	if ( ::setsockopt( socket, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof( char ) ) == SOCKET_ERROR )
	{
		Log::Instance().Push();
		return false;
	}
	Log::Instance().Push( ELogType::Log, "Socket Option : TCP NoDelay" );

	linger optLinger;
	int size = sizeof( int );
	if ( ::getsockopt( socket, SOL_SOCKET, SO_LINGER, ( char* )&optLinger, &size ) == SOCKET_ERROR )
	{
		Log::Instance().Push();
		return false;
	}

	optLinger.l_linger = 1000;
	optLinger.l_onoff = 1;
	if ( ::setsockopt( socket, SOL_SOCKET, SO_LINGER, ( char* )&optLinger, sizeof( linger ) ) == SOCKET_ERROR )
	{
		Log::Instance().Push();
		return false;
	}

	int recvSize;
	int sendSize;
	size = sizeof( int );
	if ( ::getsockopt( socket, SOL_SOCKET, SO_RCVBUF, ( char* )&recvSize, &size ) == SOCKET_ERROR ||
		 ::getsockopt( socket, SOL_SOCKET, SO_SNDBUF, ( char* )&sendSize, &size ) == SOCKET_ERROR )
	{
		Log::Instance().Push();
		return false;
	}
	Log::Instance().Push( ELogType::Log, "Socket Option RecvSize : " + std::to_string( recvSize ) );
	Log::Instance().Push( ELogType::Log, "Socket Option SendSize : " + std::to_string( sendSize ) );
	
	return true;
}
