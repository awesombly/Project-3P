#pragma once
#include "Singleton.hpp"
#include "../IO/OStream.h"

#define LOGFUNC( _log ) __FUNCTION__ + " : "_s + _log

#define LOG       Log::Instance()
#define LOGEX     Log::Instance() << ELogType::Log       << __FUNCTION__ << "( " << std::to_string( __LINE__ ) << " ) "
#define WARNING   Log::Instance() << ELogType::Warning   << __FUNCTION__ << "( " << std::to_string( __LINE__ ) << " ) "
#define ERROR     Log::Instance() << ELogType::Error     << __FUNCTION__ << "( " << std::to_string( __LINE__ ) << " ) "
#define EXCEPTION Log::Instance() << ELogType::Exception << __FUNCTION__ << "( " << std::to_string( __LINE__ ) << " ) "

// 로그 단계
// Log : 대부분의 로그
// Warning : 바꾸면 좋은 것
// Error : 로직상 발생하면 안되지만 진행시키는 것 ( outofrange, null ... )
// Exception : 서버의 기능을 못할 정도의 에러
enum class ELogType : char
{
	Log = 0,
	Warning = 1,
	Error = 2,
	Exception = 3,
	EndLine = '\n',
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

	// Ex) Log::Instance() << ELogType::Log << "some logs" << ELogType::EndLine;
	// 한 문장을 입력하고 EndLine을 꼭 붙여주세요.
	Log& operator << ( ELogType _type );
	Log& operator << ( const std::string& _data );
	Log& operator << ( const char* _data );

	static const std::string& GetType( ELogType _type );

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
	std::ostream& logStream;

	std::string logData;
	size_t curLogPos;

	std::queue<std::string> texts;
	std::condition_variable cv;
	std::mutex textsMutex;
	std::map<ELogType, std::string> types;
};

