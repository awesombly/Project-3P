#pragma once
#include "..\Standard\Singleton.hpp"

class IOCPManager : public Singleton<IOCPManager>
{
public:
	IOCPManager();
	virtual ~IOCPManager() = default;

public:
	void Bind( const HANDLE& _sock, const ULONG_PTR _key );

private:
	void WaitCompletionStatus();

private:
	HANDLE iocpHandle;
};

