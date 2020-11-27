#pragma once
#include "Thread.h"

class WorkerThread : public Thread
{
public:
	WorkerThread() = default;
	virtual ~WorkerThread() = default;

public:
	void ExecuteThread() override;
};

