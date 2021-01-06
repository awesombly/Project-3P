#pragma once
#include "Singleton.hpp"
#include "../IO/OStream.h"
#include "../Logic/Vector.h"
#include "../Logic/Quaternion.h"

#define LOG			  Log::Instance() << "#" << __FUNCTION__ << "( " << std::to_string( __LINE__ ) << " )\n#" << ELogType::Log      
#define LOG_WARNING   Log::Instance() << "#" << __FUNCTION__ << "( " << std::to_string( __LINE__ ) << " )\n#" << ELogType::Warning  
#define LOG_ERROR     Log::Instance() << "#" << __FUNCTION__ << "( " << std::to_string( __LINE__ ) << " )\n#" << ELogType::Error    
#define LOG_EXCEPTION Log::Instance() << "#" << __FUNCTION__ << "( " << std::to_string( __LINE__ ) << " )\n#" << ELogType::Exception
#define LOG_WSAERROR  Log::Instance().WSAError()
#define LOG_END       ELogType::EndLine;

// �α� �ܰ�
// Log : ��κ��� �α�
// Warning : �ٲٸ� ���� ��
// Error : ������ �߻��ϸ� �ȵ����� �����Ű�� �� ( outofrange, null ... )
// Exception : ������ ����� ���� ������ ����
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
	// �� ������ �Է��ϰ� EndLine�� �� �ٿ��ּ���.
	Log& operator << ( ELogType _type );
	Log& operator << ( const std::string& _data );
	Log& operator << ( const char* _data );
	Log& operator << ( int _data );
	Log& operator << ( unsigned __int64 _data );
	//Log& operator << ( const Vector2& _data );
	Log& operator << ( const Vector3& _data );
	//Log& operator << ( const Vector4& _data );
	Log& operator << ( const Quaternion& _data );

	static const std::string& GetType( ELogType _type );

private:
	std::string ToString( float _value, int _maxDecimalPoint = 3 );

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

