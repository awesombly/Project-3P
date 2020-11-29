#pragma once
#include "Singleton.hpp"
#include "..\Synchronize\CriticalSection.h"

class Log : public Singleton<Log>
{
public:
	Log();
	virtual ~Log() = default;

public:
	void PrintText();
	void Push();
	void Push( const int _errorCode );
	void Push( const std::string& _data );

private:
	std::queue<std::string> texts;
	std::condition_variable cv;
	std::mutex workMutex;
};

