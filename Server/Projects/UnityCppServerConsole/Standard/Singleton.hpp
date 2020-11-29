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
		std::call_once( flag, [&] () 
		{
			CriticalSection cs;
			cs.Lock();
			if ( instance == nullptr )
			{
				instance = new Type();
			}
			cs.UnLock();
		} );

		return *instance;
	}

private:
	static std::once_flag flag;
	static Type* instance;
};

template<class Type>
std::once_flag Singleton<Type>::flag;

template<class Type>
Type* Singleton<Type>::instance = nullptr;
