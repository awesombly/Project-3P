#include "OStream.h"

OStream::~OStream()
{
	Close();
}

void OStream::Open( const std::string& _path )
{
	outStream.open( _path, std::ios::out | std::ios::trunc );
	if ( !outStream.is_open() )
	{
		throw std::exception();
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
		throw std::exception();
	}
}

void OStream::Write( const std::string& _data )
{
	if ( _data.empty() || !outStream.is_open() )
	{
		throw std::exception();
	}

	//if ( *--_data.end() == '\n' )
	//{
	//	outStream << _data << std::endl;
	//	return;
	//}
	outStream << _data << std::endl;;
}

bool OStream::IsOpen() const
{
	return outStream.is_open();
}