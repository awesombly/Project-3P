#pragma once
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
			if ( instance.get() == nullptr )
			{
				instance = std::make_shared<Type>();
			}
			cs.UnLock();
		} );

		return *instance.get();
	}

private:
	static std::once_flag flag;
	static std::shared_ptr<Type> instance;
};

template<class Type>
std::once_flag Singleton<Type>::flag;

template<class Type>
std::shared_ptr<Type> Singleton<Type>::instance;
