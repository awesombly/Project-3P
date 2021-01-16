#pragma once
#include "../Session/SessionManager.h"

class Stage
{
public:
	Stage() = default;
	Stage( const std::string& _stageId );

public:
	void BroadCast( const UPACKET& _packet ) const;
	void BroadCastExceptSelf( const UPACKET& _packet, const Session* _session ) const;

	void LeaveStage( Session* _session, bool _removePlayer );

	void PushSession( Session* _session );
	void EraseSession( const Session* _session );

	void PushActor( ServerActor* _actor );
	void EraseActor( const ServerActor* _actor );
	ServerActor* FindActor( SerialType serial ) const;

	/* Npc */
	SOCKET GetHostSocket();
	void SetHostSocket( SOCKET _socket );

	ServerNpc* FindNpc( const std::string& _name ) const;
	ServerNpc* FindNpc( SerialType _serial ) const;
	void PushNpc( ServerNpc* _npc );

	const std::string& GetId() const;
	SessionContainer GetSessions() const;

private:
	std::string id;
	SessionContainer sessions;
	std::unordered_map<SerialType, ServerActor*> actors;
	
	SOCKET hostSocket;
	std::unordered_map<std::string /* name */, ServerNpc*> npcs;
};
