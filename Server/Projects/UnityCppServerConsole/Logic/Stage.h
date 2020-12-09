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

	void Push( ServerObject* _object );
	void Erase( const ServerObject* _object );

	void BroadCast( const UPACKET& _packet ) const;
	void BroadCastExceptSelf( const UPACKET& _packet, const Session* _session ) const;

	const std::string& GetId() const;

private:
	std::string id;

	SessionContainer sessions;
	std::unordered_map<SerialType, ServerObject*> objects;
};
