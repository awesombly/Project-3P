#include "OStream.h"
#include "../Standard/Log.h"
#include "../Time/Timer.h"

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

	if ( _data.compare( Log::GetType( ELogType::EndLine ) ) )
	{
		datas.push( _data );

		if ( !( _data.back() == '\n' ) )
		{
			return;
		}
	}

	if ( datas.empty() )
	{
		return;
	}

	std::string streamData;
	std::string pushData = datas.front();
	while ( pushData.compare( Log::GetType( ELogType::EndLine ) ) )
	{
		streamData += pushData;
		datas.pop();

		if ( datas.empty() )
		{
			break;
		}

		pushData = datas.front();
	}

	const std::string& date = Timer::Instance().GetCurrentDateString();

	outStream << date << streamData;
}

bool OStream::IsOpen() const
{
	return outStream.is_open();
}