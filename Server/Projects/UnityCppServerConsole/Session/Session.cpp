#include "Session.h"
#include "../Standard/Log.h"
#include "../Standard/Protocol.h"
#include "../Packet/PacketManager.h"
#include "../Logic/Stage.h"

Session::Session( const SOCKET& _socket, const SOCKADDR_IN& _address ) 
	: Network( _socket, _address )
{

}

Session::~Session()
{
	if ( logicData.CurrentStage != nullptr )
	{
		if ( logicData.Player != nullptr )
		{
			Protocol::FromServer::DestroyActor protocol;
			protocol.Serial = logicData.Player->Serial;
			logicData.CurrentStage->BroadCastExceptSelf( UPACKET( protocol ), this );

			logicData.CurrentStage->Erase( logicData.Player );
			SafeDelete( logicData.Player );
		}

		logicData.CurrentStage->Erase( this );
	}

	::shutdown( socket, SD_SEND );
	::closesocket( socket );
}

void Session::Dispatch( const LPOVERLAPPED& _ov, DWORD _byteSize )
{
	OVERLAPPEDEX* ovEx = ( OVERLAPPEDEX* )_ov;
	if ( ovEx->flag == OVERLAPPEDEX::MODE_RECV )
	{
		stream.Truncate( GetSocket(), wsaBuffer, _byteSize );
		ZeroMemory( &wsaBuffer, sizeof( WSABUF ) );
	}

	Recieve();
}