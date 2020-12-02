#pragma once
#include "..\Standard\Header.h"

class OStream
{
public:
	OStream() = default;
	virtual ~OStream();

public:
	bool Open( const std::string& _path );
	void Close();

	void Write( const std::string& _data );

	bool IsOpen() const;

private:
	std::ofstream outStream;
};

