#include "OStream.h"
//#include "..\Standard\Log.h"

OStream::~OStream()
{
	Close();
}

void OStream::Open( const std::string& _path )
{
	outStream.open( _path, std::ios::out | std::ios::trunc );
	if ( !outStream.is_open() )
	{
		//Log::Instance().Push( ELogType::Warning, "logfile is not open" );
	}
}

void OStream::Close()
{
	if ( outStream.is_open() )
	{
		outStream.close();
	}
}

void OStream::CreateNewFile( const std::string& _path )
{
	if ( outStream.is_open() )
	{
		Close();
	}

	outStream.open( _path.c_str(), std::ios::out | std::ios::trunc );
	if ( !outStream.is_open() )
	{
		//Log::Instance().Push( ELogType::Warning, "logfile is not open" );
	}
}

void OStream::Write( const std::string& _data )
{
	if ( _data.empty() || !outStream.is_open() )
	{
		//Log::Instance().Push( ELogType::Warning, "logfile is not open or data empty" );
	}

	outStream << _data << std::endl;;
}

bool OStream::IsOpen() const
{
	return outStream.is_open();
}