#include "WorkerThread.h"
#include "IOCPManager.h"
#include "..\Session\SessionManager.h"
#include "..\Standard\Log.h"

void WorkerThread::ExecuteThread()
{
	ULONG_PTR keyValue;
	LPOVERLAPPED ov;
	// 완료된 I/O 작업에서 전송된 바이트를 수신하는 변수에 대한 포인터
	DWORD transfer;

	while ( IsStart() )
	{
		// IOCP의 입출력 완료 대기열부터 입출력 완료를 기다립니다.
		// 대기열이 없다면 대기열에 입출력 완료가 있을 때까지 기다립니다.
		if ( ::GetQueuedCompletionStatus( IOCPManager::Instance().GetHandle(), &transfer, &keyValue, &ov, INFINITE ) == TRUE )
		{
			Session* session( ( Session* )keyValue );
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
			Session* session( ( Session* )keyValue );
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