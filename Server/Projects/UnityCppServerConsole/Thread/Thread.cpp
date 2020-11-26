#include "Thread.h"


Thread::Thread() : isStart( false )
{

}

Thread::~Thread()
{
	::CloseHandle( static_cast< HANDLE >( &threadHandle ) );
}

void Thread::Run()
{

}

void Thread::CreateThread()
{
	if ( isStart == false )
	{
		threadHandle = ::_beginthreadex( nullptr, 0, Handler, static_cast< LPVOID >( this ), 0, &threadID );
		isStart = true;
	}
}

unsigned int WINAPI Thread::Handler( LPVOID _param )
{
	Thread* thread( static_cast< Thread* >( _param ) );
	if ( thread != nullptr )
	{
		thread->Run();
	}
	return 0;
}