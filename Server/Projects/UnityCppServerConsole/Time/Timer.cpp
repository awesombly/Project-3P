#include "Timer.h"

Timer::Timer() : startRunTime( std::chrono::system_clock::now() )
{

}

std::string Timer::GetCurrentDateString( bool _isFolder ) const
{
	std::time_t now = std::chrono::system_clock::to_time_t( std::chrono::system_clock::now() );
	std::string data( 30, '\0' );
	if ( _isFolder )
	{
		std::strftime( &data[0], data.size(), "%Y-%m-%d_%H-%M-%S", std::localtime( &now ) );
		return data;
	}

	std::strftime( &data[0], data.size(), "[%Y-%m-%d %H:%M:%S]", std::localtime( &now ) );
	return data;
}

ULONGLONG Timer::GetElapsedNanoSeconds() const
{
	return std::chrono::duration_cast< std::chrono::nanoseconds >( std::chrono::system_clock::now() - startRunTime ).count();
}

ULONGLONG Timer::GetElapsedMicroSeconds() const
{
	return std::chrono::duration_cast< std::chrono::microseconds >( std::chrono::system_clock::now() - startRunTime ).count();
}

ULONGLONG Timer::GetElapsedMilliSeconds() const
{
	return std::chrono::duration_cast< std::chrono::milliseconds >( std::chrono::system_clock::now() - startRunTime ).count();
}

ULONGLONG Timer::GetElapsedSeconds() const
{
	return std::chrono::duration_cast< std::chrono::seconds >( std::chrono::system_clock::now() - startRunTime ).count();
}

ULONGLONG Timer::GetElapsedMinutes() const
{
	return std::chrono::duration_cast< std::chrono::minutes >( std::chrono::system_clock::now() - startRunTime ).count();
}

ULONGLONG Timer::GetElapsedHours() const
{
	return std::chrono::duration_cast< std::chrono::hours >( std::chrono::system_clock::now() - startRunTime ).count();
}