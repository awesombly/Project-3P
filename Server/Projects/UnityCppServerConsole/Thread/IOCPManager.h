#pragma once
#include "WorkerThread.h"
#include "..\Standard\Singleton.hpp"

class IOCPManager : public CriticalSection, public Singleton<IOCPManager>
{
public:
	IOCPManager();
	virtual ~IOCPManager() = default;

public:
	void Bind( const HANDLE& _sock, const ULONG_PTR _key );
	const HANDLE& GetHandle();

private:
	HANDLE iocpHandle;
	WorkerThread threads[WorkerThreadCount];
};

