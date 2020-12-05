#pragma once
#include "Network.h"

class Acceptor : public Network
{
public:
	Acceptor() = default;
	virtual ~Acceptor();

public:
	bool ListenStart() const;

private:
	void WaitForClients() const;
	bool SetSocketOption() const;
};