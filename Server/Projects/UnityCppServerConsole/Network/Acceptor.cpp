#include "Acceptor.h"
#include "../Network/IOCP/IOCPManager.h"
#include "../Session/SessionManager.h"
#include "../Standard/Log.h"


bool Acceptor::ListenStart() const
{
	if ( !SetSocketOption()
		 || ::bind( socket, ( sockaddr* )&address, sizeof( address ) ) == SOCKET_ERROR
		 || ::listen( socket, SOMAXCONN ) == SOCKET_ERROR )
	{
		ClosedSocket();

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
		clientsock = ::WSAAccept( socket, ( sockaddr* )&client, &length, NULL, NULL );
		Session* session = new Session( clientsock, client );
		SessionManager::Instance().Push( session );
		IOCPManager::Instance().Bind( ( HANDLE )clientsock, ( ULONG_PTR )session );
	}
}

// 家南 技何 汲沥
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
	Log::Instance().Push( ELogType::Log, LOGFUNC( "Socket Option : TCP NoDelay" ) );

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
	Log::Instance().Push( ELogType::Log, LOGFUNC( "Socket Option RecvSize : " + std::to_string( recvSize ) ) );
	Log::Instance().Push( ELogType::Log, LOGFUNC( "Socket Option SendSize : " + std::to_string( sendSize ) ) );
	
	return true;
}
