#include "Timer.h"

Timer::Timer() : startRunTime( std::chrono::system_clock::now() )
{

}

const std::string Timer::GetCurrentDateString( bool _isFolder ) const
{
<<<<<<< Updated upstream
	const std::time_t now = std::chrono::system_clock::to_time_t( std::chrono::system_clock::now() );
=======
	const std::time_t& now = std::chrono::system_clock::to_time_t( std::chrono::system_clock::now() );
>>>>>>> Stashed changes
	std::string data( 30, '\0' );
	if ( _isFolder )
	{
		std::strftime( &data[0], data.size(), "%Y-%m-%d_%H-%M-%S", std::localtime( &now ) );

		return data;
	}

	std::strftime( &data[0], data.size(), "[%Y-%m-%d %H:%M:%S]", std::localtime( &now ) );
<<<<<<< Updated upstream

	return data;
=======

	return data;
}

const ULONGLONG Timer::GetElapsedNanoSeconds() const
{
	return std::chrono::duration_cast< std::chrono::nanoseconds >( std::chrono::system_clock::now() - startRunTime ).count();
}

const ULONGLONG Timer::GetElapsedMicroSeconds() const
{
	return std::chrono::duration_cast< std::chrono::microseconds >( std::chrono::system_clock::now() - startRunTime ).count();
}

const ULONGLONG Timer::GetElapsedMilliSeconds() const
{
	return std::chrono::duration_cast< std::chrono::milliseconds >( std::chrono::system_clock::now() - startRunTime ).count();
}

const ULONGLONG Timer::GetElapsedSeconds() const
{
	return std::chrono::duration_cast< std::chrono::seconds >( std::chrono::system_clock::now() - startRunTime ).count();
}

const ULONGLONG Timer::GetElapsedMinutes() const
{
	return std::chrono::duration_cast< std::chrono::minutes >( std::chrono::system_clock::now() - startRunTime ).count();
}

const ULONGLONG Timer::GetElapsedHours() const
{
	return std::chrono::duration_cast< std::chrono::hours >( std::chrono::system_clock::now() - startRunTime ).count();
>>>>>>> Stashed changes
}