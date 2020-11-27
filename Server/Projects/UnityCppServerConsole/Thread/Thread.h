#pragma once
// �����带 ���ԵǸ� ����ȭó���� ���� �� �� �ֵ��� Ciritical����� �ƴ� Synchronize ����� �߰����ݴϴ�.
// ���ϸ� �Ź� cpp�� Synchronize ����� �߰����Ѿߵ�.
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