#pragma once
#include "Header.h"
template<class Type>
class Singleton
{
public:
	Singleton() = default;
	virtual ~Singleton() = default;

public:
	static Type& Instance()
	{
		if ( instance == nullptr )
		{
			instance = new Type();
		}
		return *instance;
	}

private:
	static Type* instance;
};

template<class Type>
Type* Singleton<Type>::instance = new Type();
