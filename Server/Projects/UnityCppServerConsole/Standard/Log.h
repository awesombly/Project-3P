#pragma once
#include "Singleton.hpp"
#include "..\Thread\Thread.h"

class Log : public Thread, public Singleton<Log>
{
public:
	Log();
	virtual ~Log() = default;

public:
	void ExecuteThread() override;
	void Push();
	void Push( const int _errorCode );
	void Push( const std::string& _data );

private:
	std::queue<std::string> errors;
};

