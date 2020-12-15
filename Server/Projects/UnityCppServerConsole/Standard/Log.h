#pragma once
#include "Singleton.hpp"
#include "../IO/OStream.h"

#define LOGFUNC( _log ) __FUNCTION__ + " : "_s + _log

// �α� �ܰ�
// Log : ��κ� �����ϴ� �α�
// Error : ������ �߻��ϸ� �ȵ����� �����Ű�� �� ( outofrange, null ... )
// Warning : �ٲٸ� ���� ��
// exception : ������ ����� ���� ������ ����
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
	// �� ������ �Է��ϰ� EndLine�� �� �ٿ��ּ���.
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

