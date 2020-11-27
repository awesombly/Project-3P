#include "Thread.h"
#include "..\Standard\Log.h"

Thread::Thread() : isStart( false )
{

}

Thread::~Thread()
{
	::CloseHandle( ( HANDLE )&threadHandle );
}

void Thread::CreateThread()
{
	if ( !isStart )
	{
		threadHandle = ::_beginthreadex( nullptr, 0, Handler, ( LPVOID )this, 0, &threadID );
		isStart = true;
	}
}

unsigned int WINAPI Thread::Handler( LPVOID _param )
{
	Thread* thread( ( Thread* )_param );
	if ( thread != nullptr )
	{
		thread->ExecuteThread();
	}
	return 0;
}

bool Thread::IsStart() 
{
	return isStart; 
}