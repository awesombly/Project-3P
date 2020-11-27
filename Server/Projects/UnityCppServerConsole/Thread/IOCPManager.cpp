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

const HANDLE& IOCPManager::GetHandle()
{
	return iocpHandle;
}