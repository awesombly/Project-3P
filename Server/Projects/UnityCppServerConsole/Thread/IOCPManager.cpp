#include "IOCPManager.h"
#include "..\Session\SessionManager.h"
#include "..\Thread\ThreadPool.h"
#include "..\Standard\Log.h"

bool IOCPManager::Initialize()
{
	iocpHandle = ::CreateIoCompletionPort( INVALID_HANDLE_VALUE, 0, 0, 3 );
	for ( int count = 0; count < 3; count++ )
	{
		ThreadPool::Instance().Enqueue( [&] () { IOCPManager::WaitCompletionStatus(); } );
	}
	return true;
}

void IOCPManager::Bind( const HANDLE& _socket, const ULONG_PTR _key ) const
{
	// IOCP를 사용하기 위해서는 해당 소켓을 바인드 해줘야 합니다.
	// CreateIoCompletionPort 메서드의 2가지 용도
	// 1. 통지모델인 CP를 생성하고 그 핸들을 반환
	// 2. 디바이스와 CP를 바인드
	::CreateIoCompletionPort( _socket, iocpHandle, _key, 0 );

	// 바인드를 하고나면 첫 WSARecv를 해줍니다.
	// 이제 무언가 전송되면 GetQueuedCompletionStatus에 들어갈 것입니다.
	Session* session = ( Session* )_key;
	session->Recieve();
}

void IOCPManager::WaitCompletionStatus() const
{
	ULONG_PTR keyValue;
	LPOVERLAPPED ov;
	DWORD transfer;

	while ( true )
	{
		// IOCP의 입출력 완료 대기열부터 입출력 완료를 기다립니다.
		// 대기열이 없다면 대기열에 입출력 완료가 있을 때까지 기다립니다.
		if ( ::GetQueuedCompletionStatus( iocpHandle, &transfer, &keyValue, &ov, INFINITE ) == TRUE )
		{
			Session* session = ( Session* )keyValue;
			if ( transfer != 0 )
			{
				if ( ov != NULL && session != nullptr )
				{
					session->Dispatch( ov );
				}
			}
			else
			{
				SessionManager::Instance().Erase( session );
			}
		}
		else
		{
			Session* session = ( Session* )keyValue;
			if ( ::GetLastError() != ERROR_OPERATION_ABORTED )
			{
				if ( transfer == 0 && keyValue != NULL )
				{
					SessionManager::Instance().Erase( session );
					continue;
				}
			}
			Log::Instance().Push();
		}
	}
}