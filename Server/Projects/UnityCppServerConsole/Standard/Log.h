#pragma once
#include "Singleton.hpp"
#include "../IO/OStream.h"

#define LOG			  Log::Instance() << "#" << __FUNCTION__ << "( " << std::to_string( __LINE__ ) << " )#" << ELogType::Log      
#define LOG_WARNING   Log::Instance() << "#" << __FUNCTION__ << "( " << std::to_string( __LINE__ ) << " )#" << ELogType::Warning  
#define LOG_ERROR     Log::Instance() << "#" << __FUNCTION__ << "( " << std::to_string( __LINE__ ) << " )#" << ELogType::Error    
#define LOG_EXCEPTION Log::Instance() << "#" << __FUNCTION__ << "( " << std::to_string( __LINE__ ) << " )#" << ELogType::Exception
#define LOG_WSAERROR  Log::Instance().WSAError()
#define LOG_END       ELogType::EndLine

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

	void WSAError();

	// Ex) Log::Instance() << ELogType::Log << "some logs" << ELogType::EndLine;
	// 한 문장을 입력하고 EndLine을 꼭 붙여주세요.
	Log& operator << ( ELogType _type );
	Log& operator << ( const std::string& _data );
	Log& operator << ( const char* _data );
	Log& operator << ( int _data );
	Log& operator << ( unsigned __int64 _data );

	static const std::string& GetType( ELogType _type );

private:
	void WSAErrorToString( const int _errorCode );
	void PushLog( ELogType _type, const std::string& _data );

	void LogProcess();

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

