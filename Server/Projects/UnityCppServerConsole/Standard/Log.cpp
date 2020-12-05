#include "Log.h"
#include "../Time/Timer.h"

Log::Log()
{
	std::thread th( [&] () { Log::PrintText(); } );
	th.detach();

	types.insert( std::make_pair( ELogType::Log, std::string( "[Log]" ) ) );
	types.insert( std::make_pair( ELogType::Warning, std::string( "[Warning]" ) ) );
	types.insert( std::make_pair( ELogType::Error, std::string( "[Error]" ) ) );
	types.insert( std::make_pair( ELogType::Error, std::string( "[Exception]" ) ) );
}

bool Log::Initialize()
{
	return file.Open( PATH::LogPath + Timer::Instance().GetCurrentDateString( true ).c_str() + EXT::Text );
}

void Log::PrintText()
{
	while ( true )
	{
		std::unique_lock<std::mutex> lock( textsMutex );
		cv.wait( lock, [&] () { return !texts.empty(); } );

		LogData& data = texts.front();
		const std::string& date = Timer::Instance().GetCurrentDateString();
		std::cout << data.text.c_str() << std::endl;
		if ( file.IsOpen() )
		{
			file.Write( types[data.type] + date + data.text );
		}

		texts.pop();
		::Sleep( 1 );
	}
}

void Log::Push()
{
	Push( ::WSAGetLastError() );
}

void Log::Push( const int _errorCode )
{
	LPVOID message;
	FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, _errorCode,
				   MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), ( TCHAR* )&message, 0, NULL );
	
	std::unique_lock<std::mutex> lock( textsMutex );
	texts.push( LogData( ELogType::Warning, ( CHAR* )&message ) );
	lock.unlock();

	::LocalFree( message );
}

void Log::Push( ELogType _type, const std::string& _data )
{
	std::unique_lock<std::mutex> lock( textsMutex );
	texts.push( LogData( _type, _data ) );
	lock.unlock();

	cv.notify_one();
}