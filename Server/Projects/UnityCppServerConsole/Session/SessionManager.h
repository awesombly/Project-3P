#pragma once
#include "..\Standard\Singleton.hpp"
#include "..\Synchronize\CriticalSection.h"
#include "Session.h"

class SessionManager : public Singleton<SessionManager>
{
public:
	SessionManager() = default;
	virtual ~SessionManager();

public:
	void Push( Session* _session );
	void Erase( Session* _session );
	void BroadCast( const UPACKET& _packet );

private:
	std::list<Session*> sessions;
	CriticalSection cs;
};