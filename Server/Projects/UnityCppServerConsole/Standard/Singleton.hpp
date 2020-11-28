#pragma once
#include "Header.h"
#include "..\Synchronize\CriticalSection.h"
template<class Type>
class Singleton
{
public:
	Singleton() = default;
	virtual ~Singleton() = default;

public:
	static Type& Instance()
	{
		cs.Lock();
		if ( instance == nullptr )
		{
			instance = new Type();
		}
		cs.UnLock();
		return *instance;
	}

private:
	static Type* instance;
	static CriticalSection cs;
};

template<class Type>
CriticalSection Singleton<Type>::cs;

template<class Type>
Type* Singleton<Type>::instance = nullptr;
