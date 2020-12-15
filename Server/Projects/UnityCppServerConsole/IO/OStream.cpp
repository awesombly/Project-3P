#include "OStream.h"
#include "../Standard/Log.h"
#include "../Time/Timer.h"

OStream::OStream() : writePos( 0 )
{
	writeData.resize( LogDataMaxSize );
}

OStream::~OStream()
{
	Close();
}

bool OStream::Open( const std::string& _path )
{
	if ( outStream.is_open() )
	{
		Close();
	}

	outStream.open( _path, std::ios::out | std::ios::trunc );
	if ( !outStream.is_open() )
	{
		Log::Instance().Push( ELogType::Warning, LOGFUNC( "Failed To Open Logfile"_s ) );

		return false;
	}

	return true;
}

void OStream::Close()
{
	if ( outStream.is_open() )
	{
		outStream.close();
	}
}

void OStream::Write( const std::string& _data )
{
	if ( _data.empty() || !outStream.is_open() )
	{
		Log::Instance().Push( ELogType::Warning, LOGFUNC( "Data is Empty or Logfile Open Failed" ) );
	}

	std::string date = Timer::Instance().GetCurrentDateString();
	if ( LogDataMaxSize - writePos < date.size() + _data.size() )
	{
		// 자르기 작업
	}

	std::copy( std::begin( date ), std::end( date ), &writeData[ writePos ] );
	writePos += date.size();

	std::copy( std::begin( _data ), std::end( _data ), &writeData[ writePos ] );
	writePos += _data.size();

	outStream << writeData;
	writePos = 0;
}

bool OStream::IsOpen() const
{
	return outStream.is_open();
}