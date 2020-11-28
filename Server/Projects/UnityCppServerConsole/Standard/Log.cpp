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
		cv.wait( lock, [&] () 
		{
			return !errors.empty(); 
		} );

		std::cout << errors.front().c_str() << std::endl;
		errors.pop();
	}
}

void Log::Push()
{
	Push( ::WSAGetLastError() );
}

void Log::Push( const int _errorCode )
{
	std::string error;
	switch ( _errorCode )
	{
		case WSAEADDRINUSE:
		{
			error += "WSQEADDRINUSE";
			Push( error );
		} break;
		
		case WSAEACCES:
		{
			error += "WSAEACCES";
			Push( error );
		} break;

		default:
		{

		} break;
	}

	std::lock_guard<std::mutex> lock( workMutex );
	LPVOID message;
	FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, _errorCode,
				   MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), ( TCHAR* )&message, 0, NULL );
	error += ( CHAR* )&message;
	errors.push( error );
	::LocalFree( message );
}

void Log::Push( const std::string& _data )
{
	std::lock_guard<std::mutex> lock( workMutex );
	errors.push( _data );
	cv.notify_one();
}