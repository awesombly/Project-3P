#pragma once
#include "..\Synchronize\CriticalSection.h"

class Thread : public CriticalSection
{
public:
	Thread();
	virtual ~Thread();

public:
	virtual void Run();
	void CreateThread();
	static unsigned int WINAPI Handler( LPVOID _param );

private:
	unsigned int threadHandle;
	unsigned int threadID;
	bool isStart;
};