#include "IOCPManager.h"
#include "../../Session/SessionManager.h"
#include "../../Standard/Log.h"

bool IOCPManager::Initialize()
{
	iocpHandle = ::CreateIoCompletionPort( INVALID_HANDLE_VALUE, 0, 0, 3 );
	for ( int count = 0; count < 3; ++count )
	{
		std::thread th( [&] () { IOCPManager::WaitCompletionStatus(); } );
		th.detach();
	}

	return true;
}

void IOCPManager::Bind( const HANDLE& _socket, const ULONG_PTR _key ) const
{
	::CreateIoCompletionPort( _socket, iocpHandle, _key, 0 );

	Session* session = ( Session* )_key;
	session->Recieve();
}

void IOCPManager::WaitCompletionStatus() const
{
	ULONG_PTR keyValue;
	LPOVERLAPPED ov;
	DWORD transferred;

	while ( true )
	{
		if ( ::GetQueuedCompletionStatus( iocpHandle, &transferred, &keyValue, &ov, INFINITE ) == TRUE )
		{
			Session* session = ( Session* )keyValue;
			if ( transferred != 0 )
			{
				if ( ov != NULL && session != nullptr )
				{
					session->Dispatch( ov, transferred );
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
				if ( transferred == 0 && keyValue != NULL )
				{
					SessionManager::Instance().Erase( session );
					continue;
				}
			}
			LOG_WSAERROR;
		}
	}
}