#pragma once
#include "../Session/SessionManager.h"

class Stage
{
public:
	void BroadCast( const UPACKET& _packet ) const;
	void BroadCastExceptSelf( const UPACKET& _packet, const Session* _session ) const;

private:
	SessionContainer sessions;
	std::unordered_map<SerialType, ServerObject*> objects;
};
