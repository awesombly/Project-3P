#include "IOCPManager.h"
#include "..\Standard\Log.h"
#include "..\Session\Session.h"

IOCPManager::IOCPManager()
{
	Log::Instance().Push( "Create IOCP Handler" );
	iocpHandle = ::CreateIoCompletionPort( INVALID_HANDLE_VALUE, 0, 0, WorkerThreadCount );

	for ( int count = 0; count < WorkerThreadCount; count++ )
	{
		Log::Instance().Push( "Create WorkerThread" );
		threads[count].CreateThread();
	}
}

void IOCPManager::Bind( const HANDLE& _socket, const ULONG_PTR _key )
{
	// IOCP를 사용하기 위해서는 해당 소켓을 바인드 해줘야 합니다.
	// CreateIoCompletionPort 메서드의 2가지 용도
	// 1. 통지모델인 CP를 생성하고 그 핸들을 반환
	// 2. 디바이스와 CP를 바인드
	::CreateIoCompletionPort( _socket, iocpHandle, _key, 0 );

	// 바인드를 하고나면 첫 WSARecv를 해줍니다.
	// 이제 무언가 전송되면 GetQueuedCompletionStatus에 들어갈 것입니다.
	Session* session( ( Session* )_key );
	session->WaitForPacketRecv();
}

const HANDLE& IOCPManager::GetHandle()
{
	return iocpHandle;
}