#pragma once
#include "../Standard/Singleton.hpp"
#include "../Synchronize/CriticalSection.h"
#include "../Session/Session.h"

using SessionContainer = std::unordered_map<SOCKET, Session*>;

class Stage;

class SessionManager : public Singleton<SessionManager>
{
public:
	SessionManager() = default;
	virtual ~SessionManager();

public:
	Session* Find( const SOCKET& _socket ) const;
	SessionContainer GetSessions() const;

public:
	void Push( Session* _session );
	void Erase( Session* _session );

public:
	static void BroadCast( const UPACKET& _packet, const SessionContainer& _sessions );
	static void BroadCastExceptSelf( const UPACKET& _packet, const Session* _session, const SessionContainer& _sessions );

	void BroadCast( const UPACKET& _packet ) const;
	void BroadCastExceptSelf( const UPACKET& _packet, const Session* _session ) const;

	void EnterStage( Session* _session, const std::string& _stageId );

private:
	SessionContainer sessions;
	CriticalSection cs;
	
	std::unordered_map<std::string/*stageId*/, Stage*> stages;
};