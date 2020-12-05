#pragma once
#include "Singleton.hpp"
#include "../IO/OStream.h"

#define LOGFUNC( _log ) __FUNCTION__ + " : "_s + _log

// 로그 단계
// Log : 대부분 생각하는 로그
// Error : 로직상 발생하면 안되지만 진행시키는 것 ( outofrange, null ... )
// Warning : 바꾸면 좋은 것
// exception : 서버의 기능을 못할 정도의 에러
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
	bool Initialize();
	void Push();
	void Push( const int _errorCode );
	void Push( ELogType _type, const std::string& _data );

private:
	void PrintText();

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
	OStream file;
	std::queue<LogData> texts;
	std::condition_variable cv;
	std::mutex textsMutex;
	std::map<ELogType, std::string> types;
};

