#include "WorkerThread.h"
#include "IOCPManager.h"
#include "..\Session\SessionManager.h"
#include "..\Standard\Log.h"

void WorkerThread::ExecuteThread()
{
	ULONG_PTR keyValue;
	LPOVERLAPPED ov;
	// �Ϸ�� I/O �۾����� ���۵� ����Ʈ�� �����ϴ� ������ ���� ������
	DWORD transfer;

	while ( IsStart() )
	{
		// IOCP�� ����� �Ϸ� ��⿭���� ����� �ϷḦ ��ٸ��ϴ�.
		// ��⿭�� ���ٸ� ��⿭�� ����� �Ϸᰡ ���� ������ ��ٸ��ϴ�.
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