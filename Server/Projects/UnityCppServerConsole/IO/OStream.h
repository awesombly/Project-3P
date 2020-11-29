#pragma once
#include "..\Standard\Header.h"

class OStream
{
public:
	OStream() = default;
	virtual ~OStream();

public:
	void Open( const std::string& _path );
	void Close();

	void CreateNewFile( const std::string& _path );

	void Write( const std::string& _data );

	bool IsOpen() const;

private:
	std::ofstream outStream;
};

