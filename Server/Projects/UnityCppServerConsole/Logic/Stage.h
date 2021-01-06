#pragma once
#include "../Session/SessionManager.h"

class Stage
{
public:
	Stage() = default;
	Stage( const std::string& _stageId );

public:
	void Push( Session* _session );
	void Erase( const Session* _session );

	void Push( ServerActor* _actor );
	void Erase( const ServerActor* _actor );

	void Push( ServerNpc* _actor );

	void BroadCast( const UPACKET& _packet ) const;
	void BroadCastExceptSelf( const UPACKET& _packet, const Session* _session ) const;

	ServerActor* Find( SerialType serial ) const;
	
	ServerNpc* FindNpc( const std::string& _name ) const;

	const std::string& GetId() const;
	SessionContainer GetSessions() const;

private:
	std::string id;
	SessionContainer sessions;
	std::unordered_map<SerialType, ServerActor*> actors;
	std::unordered_map<std::string /* npc name */, ServerNpc*> npcs;
};
