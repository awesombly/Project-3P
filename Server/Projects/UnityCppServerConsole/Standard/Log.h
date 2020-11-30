#pragma once
#include "Singleton.hpp"
#include "..\IO\OStream.h"

enum class ELogType : char
{
	Log = 0,
	Warning = 1,
	Error = 2,
	Exception = 3,
};

class Log : public Singleton<Log>
{
public:
	Log();
	virtual ~Log() = default;

public:
	void PrintText();
	void Push();
	void Push( const int _errorCode );
	void Push( ELogType _type, const std::string& _data );

private:
	struct LogData
	{
		ELogType type;
		std::string text;
		LogData( ELogType _type, const std::string& _text ) : type( _type ), text( _text )
		{

		}
	};

private:
	std::queue<LogData> texts;
	std::condition_variable cv;
	std::mutex workMutex;

	OStream file;
	std::map<ELogType, std::string> types;
};

