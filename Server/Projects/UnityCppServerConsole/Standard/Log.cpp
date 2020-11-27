#include "Log.h"

Log::Log()
{
	CreateThread();
}

void Log::ExecuteThread()
{
	while ( true )
	{
		{
			Synchronize sync( this );
			if ( errors.empty() == false )
			{
				std::cout << errors.front().c_str() << std::endl;
				errors.pop();
			}
		}
		::Sleep( 100 );
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

	{
		Synchronize sync( this );
		LPVOID message;
		FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, _errorCode,
					   MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), ( TCHAR* )&message, 0, NULL );
		error += ( CHAR* )&message;
		errors.push( error );
		::LocalFree( message );
	}
}

void Log::Push( const std::string& _data )
{
	{
		Synchronize sync( this );
		errors.push( _data );
	}
}