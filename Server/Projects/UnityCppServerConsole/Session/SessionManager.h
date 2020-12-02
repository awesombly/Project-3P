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
	Session* Find( const SOCKET& _socket );
	Session* Find( const std::string& _name );

public:
	void Push( Session* _session );
	void Erase( Session* _session );

public:
	void BroadCast( const UPACKET& _packet ) const;

private:
	std::unordered_map<SOCKET, Session*> sessions;
	CriticalSection cs;
};