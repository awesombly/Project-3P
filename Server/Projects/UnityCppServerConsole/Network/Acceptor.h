#pragma once
#include "Network.h"

class Acceptor : public Network
{
public:
	Acceptor() = default;
	virtual ~Acceptor() = default;

public:
	bool ListenStart() const;

private:
	void WaitForClients() const;
	bool SetSocketOption() const;
};