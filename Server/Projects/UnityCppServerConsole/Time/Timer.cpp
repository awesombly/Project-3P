#include "Timer.h"

Timer::Timer() : startRunTime( std::chrono::system_clock::now() )
{

}

std::string Timer::GetCurrentDateString( bool _isFolder ) const
{
	const std::time_t now = std::chrono::system_clock::to_time_t( std::chrono::system_clock::now() );
	std::string data( 22, '\0' );
	if ( _isFolder )
	{
		std::strftime( &data[0], data.size(), "%Y-%m-%d_%H-%M-%S", std::localtime( &now ) );

		return data;
	}

	std::strftime( &data[0], data.size(), "[%Y-%m-%d %H:%M:%S]", std::localtime( &now ) );

	return data;
}