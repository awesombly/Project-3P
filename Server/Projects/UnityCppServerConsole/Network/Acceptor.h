#pragma once
#include "Network.h"

class Acceptor : public Network
{
public:
	Acceptor() = default;
	virtual ~Acceptor() = default;

public:
	const bool ListenStart() const;

private:
	void WaitForClients() const;
	const bool SetSocketOption() const;
};