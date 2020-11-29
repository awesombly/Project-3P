#include "Log.h"
#include "..\Thread\ThreadPool.h"
#include "..\Time\Timer.h"

Log::Log()
{
	ThreadPool::Instance().Enqueue( [&] () { Log::PrintText(); } );

	// Enum ���� String���� ��ȯ�ϴ� ���
	// 1. #define EnumToString(s) #s
	// 2. map���� �̸� �����Ͽ� Ű������ ã��
	types.insert( std::make_pair( ELogType::Log, std::string( "[Log]" ) ) );
	types.insert( std::make_pair( ELogType::Warning, std::string( "[Warning]" ) ) );
	types.insert( std::make_pair( ELogType::Error, std::string( "[Error]" ) ) );
}

void Log::PrintText()
{
	while ( true )
	{
		std::unique_lock<std::mutex> lock( workMutex );
		cv.wait( lock, [&] () { return !texts.empty(); } );
		
		if ( !file.IsOpen() )
		{
			std::string pathName = Timer::Instance().GetCurrentDateString( true );
			file.CreateNewFile( "..\\..\\..\\Log\\"_s + pathName.c_str() + ".txt"_s );
		}

		LogData data = std::move( texts.front() );
		std::string date = Timer::Instance().GetCurrentDateString();
		file.Write( types[data.type] + date + data.text );
		std::cout << data.text.c_str() << std::endl;

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
	texts.push( LogData( ELogType::Error, ( CHAR* )&message ) );
	::LocalFree( message );
}

void Log::Push( ELogType _type, const std::string& _data )
{
	std::unique_lock<std::mutex> lock( workMutex );
	texts.push( LogData( _type, _data ) );
	lock.unlock();
	cv.notify_one();
}