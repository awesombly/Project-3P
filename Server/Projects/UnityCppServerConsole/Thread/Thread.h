#pragma once
// 스레드를 쓰게되면 동기화처리도 같이 할 수 있도록 Ciritical헤더가 아닌 Synchronize 헤더를 추가해줍니다.
// 안하면 매번 cpp에 Synchronize 헤더를 추가시켜야됨.
#include "..\Synchronize\Synchronize.h"

class Thread : public CriticalSection
{
public:
	Thread();
	virtual ~Thread();

public:
	virtual void ExecuteThread() = 0;
	void CreateThread();
	static unsigned int WINAPI Handler( LPVOID _param );

public:
	bool IsStart();

private:
	unsigned int threadHandle;
	unsigned int threadID;
	bool isStart;
};