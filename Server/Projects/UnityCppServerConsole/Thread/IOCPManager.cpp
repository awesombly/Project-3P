#include "IOCPManager.h"
#include "..\Session\SessionManager.h"
#include "..\Thread\ThreadPool.h"
#include "..\Standard\Log.h"

IOCPManager::IOCPManager()
{
	Log::Instance().Push( "Create IOCP Handler" );
	iocpHandle = ::CreateIoCompletionPort( INVALID_HANDLE_VALUE, 0, 0, 3 );

	for ( int count = 0; count < 3; count++ )
	{
		ThreadPool::Instance().Enqueue( [&] ()
		{
			Log::Instance().Push( "Wait GetQueuedCompletionStatus From Thread." );
			IOCPManager::WaitCompletionStatus();
		} );
	}
}

void IOCPManager::Bind( const HANDLE& _socket, const ULONG_PTR _key )
{
	// IOCP�� ����ϱ� ���ؼ��� �ش� ������ ���ε� ����� �մϴ�.
	// CreateIoCompletionPort �޼����� 2���� �뵵
	// 1. �������� CP�� �����ϰ� �� �ڵ��� ��ȯ
	// 2. ����̽��� CP�� ���ε�
	::CreateIoCompletionPort( _socket, iocpHandle, _key, 0 );

	// ���ε带 �ϰ��� ù WSARecv�� ���ݴϴ�.
	// ���� ���� ���۵Ǹ� GetQueuedCompletionStatus�� �� ���Դϴ�.
	Session* session( ( Session* )_key );
	session->WaitForPacketRecv();
}

void IOCPManager::WaitCompletionStatus()
{
	ULONG_PTR keyValue;
	LPOVERLAPPED ov;
	DWORD transfer;

	while ( true )
	{
		// IOCP�� ����� �Ϸ� ��⿭���� ����� �ϷḦ ��ٸ��ϴ�.
		// ��⿭�� ���ٸ� ��⿭�� ����� �Ϸᰡ ���� ������ ��ٸ��ϴ�.
		if ( ::GetQueuedCompletionStatus( iocpHandle, &transfer, &keyValue, &ov, INFINITE ) == TRUE )
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