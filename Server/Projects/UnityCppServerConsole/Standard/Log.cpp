#include "Log.h"
#include "..\Thread\ThreadPool.h"

Log::Log()
{
	ThreadPool::Instance().Enqueue( [&] () 
	{
		Log::PrintText(); 
	} );
}

void Log::PrintText()
{
	while ( true )
	{
		std::unique_lock<std::mutex> lock( workMutex );
		cv.wait( lock, [&] () { return !texts.empty(); } );

		std::cout << std::move( texts.front() ).c_str() << std::endl;
		texts.pop();
	}
}

void Log::Push()
{
	Push( ::WSAGetLastError() );
}

void Log::Push( const int _errorCode )
{
	std::lock_guard<std::mutex> lock( workMutex );
	LPVOID message;
	FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, _errorCode,
				   MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), ( TCHAR* )&message, 0, NULL );
	std::string error( ( CHAR* )&message );
	texts.push( error );
	::LocalFree( message );
}

void Log::Push( const std::string& _data )
{
	std::unique_lock<std::mutex> lock( workMutex );
	texts.push( _data );
	lock.unlock();
	cv.notify_one();
}