#pragma once
#include "../Standard/Header.h"

class OStream
{
public:
	OStream() = default;
	virtual ~OStream();

public:
	const bool Open( const std::string& _path );
	void Close();

	void Write( const std::string& _data );

	const bool IsOpen() const;

private:
	std::ofstream outStream;
};