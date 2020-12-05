#include "OStream.h"
#include "../Standard/Log.h"

OStream::~OStream()
{
	Close();
}

const bool OStream::Open( const std::string& _path )
{
	if ( outStream.is_open() )
	{
		Close();
	}

	outStream.open( _path, std::ios::out | std::ios::trunc );
	if ( !outStream.is_open() )
	{
<<<<<<< Updated upstream
		Log::Instance().Push( ELogType::Warning, LOGFUNC( "Failed To Open Logfile"_s ) );
=======
		Log::Instance().Push( ELogType::Warning, "Failed To Open Logfile" );
>>>>>>> Stashed changes

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
		Log::Instance().Push( ELogType::Warning, LOGFUNC( "Data is Empty or Logfile Open Failed"_s ) );
	}

	outStream << _data << std::endl;
}

const bool OStream::IsOpen() const
{
	return outStream.is_open();
}